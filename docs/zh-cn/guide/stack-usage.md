# 编译器栈证据

发布构建可以随预编译库导出编译器栈帧与调用图。源码维护者也可以按需采集：

```sh
GSP_STACK_EVIDENCE=1 bash ci/build_prebuilt_library.sh esp32s31
```

先启用匹配的 ESP-IDF／工具链环境。普通构建继续使用编译缓存；证据构建禁用缓存命中，
确保生成 `.su` 和 `.ci`。带发布 Tag 的构建自动采集。

`artifacts/stack-evidence/<target>/` 包含 `status.json`、`compiler-stack.json.gz`
和 `synchronous-entry-points.json.gz`。状态文件记录是否执行采集；证据绑定 archive
SHA-256、编译器、SDK、配置、源码和对象身份。使用不同 SDK 得到的产物，在证明机器码
等价之前属于参考构建。

分析器报告逐调用点的已知栈贡献和未闭合项：顺序调用取最大值，嵌套调用叠加仍存活的
前缀。汇编／ROM 栈帧、间接目标、弱导入、动态栈帧以及未证明上界的递归组件会明确保留，
不会当作零。输入域假设必须绑定对应输入哈希，并注明证据。
每个入口的 `call_sites` 按编译器调用点汇总目标集合、目标集完整性、被调用方最大和、
调用方存活栈帧加被调用方的最大和，以及递归组件编号。即使直接目标集合已完整，
下游仍有开放项时，数值最大和也保持 null。源码位置来自编译器，最终指令地址须结合链接 ELF。

`open_frontiers` 为每个未闭合项给出编译器图上达到最大已知前缀的一条路径，包含调用方、
源码位置、候选目标和递归组件编号。前缀不包含未知子树；调用图中的环不等于实际无界递归。
可同时导出 CSV：

```sh
python3 ci/stack_usage.py analyze --input compiler-stack.json.gz \
  --root esp_gsp_stop --output analysis.json.gz --frontiers-csv frontiers.csv
```

输入、假设文件与 JSON／CSV 输出分别使用不同路径。

`transitive_bounds` 注解中的 `bytes: null` 将整个函数子树保留为符号项，便于把
调用方前缀与单独证明的分配器上界组合。它不表示零，也不会产生完整数值上界；
填入数值时仍须提供证据。

任务级核算还需要最终应用 ELF／map、任务入口、应用回调目标和适用的端口／中断预留。
FreeType、presenter、IDF、ROM 和 libc 属于哪些产物，应根据实际链接结果确定；
GSP 中的未定义导入不代表实现已经包含在 GSP archive 内。

已观察或已解析的最深路径不等于整个任务的最坏上界。应用与外部开放前沿尚未闭合时，
`task_stack_bound_bytes` 保持 null。应用任务预算与 GSP 渲染工作任务栈策略分别处理；
运行时栈水位可以补充证据，但不能证明未执行路径。
