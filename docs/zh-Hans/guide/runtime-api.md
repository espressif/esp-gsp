# 运行时与生成 API

## 从生成配置启动

`gsp_<symbol>_config()` 提供 Bundle 数据、组件目录和 JSON 推导需求。将它与 `ESP_GSP_ESP_LCD_CONFIG_INIT()`、BSP 创建的显示目标组合，再调用 `esp_gsp_esp_lcd_start()`：

```c
esp_gsp_config_t app = gsp_bundle_config();
esp_gsp_esp_lcd_config_t lcd = ESP_GSP_ESP_LCD_CONFIG_INIT();
lcd.display = display;
lcd.touch = touch;

esp_gsp_handle_t ui;
ESP_ERROR_CHECK(esp_gsp_esp_lcd_start(&app, &lcd, &ui));
```

## 生成对象操作

只有控件类型和 JSON 动态属性支持时，GSPC 才生成对应操作。命名 Slider 可能生成 `get_value()`、`set_value()`；带运行时文字的命名 Label 可能生成文字 setter；`callback` 会生成事件判断函数。应查看生成头文件，或各[控件示例页](../components/index.md)展示的真实签名。

ESP-IDF 中 setter 是异步的。成功表示命令已被接受，不代表已经显示到屏幕。`esp_gsp_flush()` 只用于确定性测试、截图或明确同步边界，不要每次更新都调用。

Slider 和 Arc 拖动会更新保留的数值状态，但当前公共运行时不会自动产生应用层数值
变化回调。生成的 getter/setter 仍然有效；产品逻辑必须收到拖动通知时，应将其视为
能力缺口，不能假设 `callback` 会在每个数值步骤触发。

## 事件与任务上下文

事件、列表绑定、图片释放、定时器和 Canvas 回调运行在框架任务中。不要在其中阻塞、访问慢速存储或执行网络操作；只发送队列/任务通知并尽快返回。

应用负责的返回行为使用 `callback`，StackView 返回使用 `stack_pop`。JSON 的
`back` 动作为保留能力，当前公共运行时不会分发。

```text
UI 事件 → 短回调 → 应用任务 → 产品状态改变
                                  → 生成 setter → 渲染提交
```

## 动态内容

| 内容 | 推荐 API 路径 |
|---|---|
| 命名数值、文字、可见性、选择状态 | 生成对象辅助函数 |
| 应用持有的行或单元格 | List/Grid/Wheel/Message List 适配器 |
| 偶尔替换的编码图片 | 生成图片 setter 或 `esp_gsp_set_image*()` |
| 相机、视频、连续像素 | Canvas 帧或 Direct Draw 回调 |
| 页面与堆栈导航 | 生成导航/组件辅助函数 |

缓冲区必须遵循 COPY、BORROW 或 TAKE 所有权约定。`esp_gsp_stop()` 完成前，先停止外部生产者、解绑回调并释放借用资源。详细规则见[生命周期](lifecycle.md)和
[API 参考](../reference/api.md)。

## 正确停止

从应用任务调用 `esp_gsp_stop()`，不要在事件、列表、图片释放、定时器或 Canvas 回调
中停止。先停止新的媒体帧、网络结果和数据发布，再停止 Canvas 并在需要时执行一次
`esp_gsp_flush()`，最后停止 UI。返回 `ESP_GSP_ERR_TIMEOUT` 时 Handle 仍有效，可以
重试；成功后再释放 Bundle、字体、回调上下文、显示目标和 BSP 资源。

详细所有权规则见[生命周期](lifecycle.md)，完整签名见[函数级 API 参考](../reference/api-functions.md)。
