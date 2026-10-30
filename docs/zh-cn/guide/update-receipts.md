# 目标帧更新回执

需要确认一组必需更新时，包含 `esp_gsp_updates.h`。回执分别记录已经到达的最远阶段
和目标结果，避免把命令入队、渲染尝试或队列空闲当成显示完成。

| 阶段 | 含义 |
|---|---|
| `ACCEPTED` | 已复制命令并进入 UI 队列。 |
| `APPLIED` | 所有必需操作成功，包括异步图片真正发布。 |
| `RENDERED` | 成功提交的渲染帧包含目标状态。 |
| `TRANSFERRED` | 后端确认对应帧传输完成。 |
| `PRESENTED` | 与目标帧关联的硬件刷新反馈已确认。 |

提交前用 `esp_gsp_update_capabilities()` 查询完成能力。模拟器提供 `RENDERED`，
SPI／QSPI 的 GRAM 面板提供传输确认；有硬件 VSYNC 的帧缓冲后端可以提供 `PRESENTED`。
不支持的阶段在接收前返回错误，不会自动降级。

`PRESENTED` 同时关联显示绑定代次和帧 ID。RGB 和 MIPI 均等待切换票据及随后的两次
硬件 VSYNC。没有硬件 VSYNC 的 MIPI 驱动（包括 P4 rev < v3）最高报告 `TRANSFERRED`，
具体能力以当前 SDK 和目标配置的查询结果为准。确认期间，同一 presenter 保留目标帧，
并推迟下一帧，包括普通更新产生的帧。每个目标都请求 `PRESENTED` 会降低吞吐量，应按应用
需要选择最早满足要求的阶段。未请求跟踪时保留原有流水线，跟踪不增加任务或帧缓冲。
扫描时钟回调在首次请求显示确认时启用，保留至显示绑定销毁。

## 组合更新

标量 `PROPERTY` 命令使用生成的场景前缀组件键；`VALUE`、`COLOR`、`VISIBLE`、
`TEXT` 和 `IMAGE` 使用生成的 Bind ID。需要 Bind 常量时，在包含 Bundle 头文件前
定义 `GSP_BUNDLE_ENABLE_RAW_IDS`，并使用带场景前缀的常量。
`SCENE` 必须是第一条命令，之后的 Bind 属于目标场景，因为一个目标帧只属于一个场景。

```c
#include "esp_gsp_updates.h"
#include "bundle_gsp.h"

/* 使用应用生成头文件中的组件键替换以下名称。 */
static const esp_gsp_update_command_t changes[] = {
    {
        .kind = ESP_GSP_UPDATE_PROPERTY,
        .property = {
            .component = GSP_HOME_OBJ_KEY_LEVEL,
            .property = GSP_PROPERTY_KEY_VALUE,
            .value = {.type = GSP_VALUE_I32, .data.i32 = 75},
        },
    },
    {
        .kind = ESP_GSP_UPDATE_PROPERTY,
        .property = {
            .component = GSP_HOME_OBJ_KEY_PANEL,
            .property = GSP_PROPERTY_KEY_VISIBLE,
            .value = {.type = GSP_VALUE_BOOL, .data.boolean = true},
        },
    },
};

esp_gsp_update_receipt_t *receipt = NULL;
esp_gsp_update_options_t options = {
    .complete_at = ESP_GSP_UPDATE_RENDERED, .timeout_ms = 1000,
};
esp_gsp_err_t error = esp_gsp_update_submit(
    ui, changes, sizeof(changes) / sizeof(changes[0]), &options, &receipt);
if (error == ESP_GSP_OK) {
    /* 保留 receipt 查询，或通过 options 注册结束回调。 */
}
```

`complete_at` 为零时默认等待 `RENDERED`。纯 `PROPERTY` 组复用标量原子事务；
混合组按顺序执行，失败时可能已有部分状态生效，不回滚先前更新。
`applied_count` 表示已经确认的进度；`failed_command` 为失败命令下标，组级错误
使用 `UINT32_MAX`。原有属性编解码、范围、文字换行和裁剪规则继续适用。
同一种命令重复写入同一 Bind，或 `PROPERTY` 重复写入同一组件属性时，以最后一个值作为目标。

结束结果包括 `COMPLETE`、`FAILED`、`SUPERSEDED`、`CANCELLED` 和 `TIMED_OUT`。
应检查结果，不能仅凭提交成功或已经到达某个阶段认定目标完成。目标在对应帧生成前
被其它更新替换时，返回 `SUPERSEDED`。相同状态可以复用已经确认匹配的渲染帧；
队列空闲本身不是渲染证据。

`failed_stage` 表示未能确认的阶段，与已经到达的 `stage` 分开：可为 `APPLIED`、
`RENDERED`、`TRANSFERRED` 或 `PRESENTED`，仅在 `FAILED` 或 `TIMED_OUT` 时非零。
一次渲染尝试失败即结束对应回执，普通渲染器仍可继续恢复，应用可重新提交目标。
`error` 在等待期间为 `GSP_OK`，结束后保留错误原因。

`timeout_ms` 限制从接收到观察到完成证据的时长，包含排队和暂停时间，为零时不设期限。
下一次 UI 服务发现到期后返回 `TIMED_OUT` 和 `GSP_ERR_TIMEOUT`；到期后才观察到的
证据不会把超时变为成功。它与调用方的等待超时不同：等待超时只结束本次等待。
两种超时都不回滚状态或提前释放仍在使用的输入。取消或到期使同一帧的最后一个目标
结束时，会撤销该帧的显示确认等待，保留正常的缓冲区所有权栅栏；迟到反馈不能改变终态。

## 所有权与寿命

原生服务首次使用时分配跟踪状态，最多同时处理 16 个未完成目标，每组最多 128 条命令。
提交不等待队列空间；队列或目标名额不足时返回 `ESP_GSP_ERR_TIMEOUT`。

命令和 UTF-8 字符串会复制，图片 `COPY` 输入只复制一次。成功接收的 `BORROW`、
`TAKE` 输入遵循原有图片归还契约；立即失败时输出回执为 NULL，输入仍归调用方。
帧回执完成不代表场景已经释放图片，归还上下文仍应保留到 `on_release` 执行。
终态回执保留状态；没有在途图片回调需要命令条目时，结束时即回收命令副本。

`on_complete` 在 UI／渲染任务通知结束结果；停止取消未完成目标时，也可能由停止的
调用线程通知。上下文应保留到通知结束。输出句柄先于通知写入，因此渲染任务即使在
提交函数返回前执行，回调也可以查询回执。回调不得阻塞、刷新或停止自己的 UI。

持有回执引用时，`esp_gsp_update_query()` 读取一致快照；竞争时返回 false，可稍后重试。
提交操作应与 UI 销毁互斥。UI 停止后仍可查询保留的回执；完成使用后调用
`esp_gsp_update_release()`。释放调用方引用不取消更新。
`esp_gsp_update_cancel()` 请求取消，不撤销已经生效的状态，也不会提前回收解码器或 DMA
仍引用的输入。

暂停保留未完成回执，但暂停处理和回调；恢复时先检查取消和已经到期的期限，再继续目标。
暂停失败并恢复同一 UI 时也遵循此规则。Quiesce 等待缓冲区所有权栅栏，不会将回执提升为
`PRESENTED`。更换显示绑定使旧帧代次失效，停止 UI 则取消未完成目标。
提交应与暂停、恢复及销毁互斥，不向暂停的 UI 提交；暂停期间仍可查询已有回执或请求取消。
结束回调在恢复服务后或停止时交付。原生头文件契约为 `ESP_GSP_UPDATE_API_VERSION`，
头文件与实现应来自同一组件构建。

## 模拟器与 AI 操作

命名 RPC 提供 `update_submit`、`query_update`、`wait_update`、`cancel_update`、
`release_update` 和 `update_capabilities`。例如，提交包含
`{"kind":"property","name":"level","property":"value","value":75}` 与
`{"kind":"text","name":"caption","text":"Ready"}` 的 `commands` 数组，
随后把返回的原样 `id` 传给查询或等待接口。
`update_submit` 同样接收可选的 `timeout_ms`；状态包含 `failed_stage` 和
`failed_stage_code`，便于结构化处理失败。

`wait_update` 只对 `COMPLETE` 返回成功；失败、取消和替换会返回包含观察结果的错误。
`max_frames` 限制实际推进的帧数，等待超时不取消目标。重载或重启后，旧 ID 失效。
只有原连接可以取消和释放目标，观察方可以只读查询；断连会取消并释放该连接保留的目标。
消费终态结果后调用 `release_update`，失败结果同样需要释放。模拟器在各连接之间最多
保留 64 个回执，包含已经完成的目标。释放尚未完成的回执不取消其工作。

真实 C Backend 通过 `gsp_sim_bridge` 提供相同 API，继续遵守 open／poll 线程契约；
空闲时也要执行 `gsp_sim_bridge_poll()`，以交付回执通知和图片归还回调。
模拟器回执确认渲染输出；画面验收结合参考图／指定区域比较，设备显示验收使用硬件反馈。
完成只确认一帧中的必需状态，动画结束或符合参考设计仍需单独检查。

`esp_gsp_flush()` 保持原来的队列与渲染尝试栅栏语义；目标组使用独立、明确的状态和帧证据。
