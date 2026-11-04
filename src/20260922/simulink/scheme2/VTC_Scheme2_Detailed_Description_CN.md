# VTC 方案二 S-Function 模型详细说明

## 1. 文档范围

本文说明 `scheme2` 目录中的方案二原型，包括：

- `build_VTC_Scheme2_R2020b.m`；
- `build_sfun_eLSD_Scheme2.m`；
- `include/eLSD_Sfun_FlatApi.h`；
- `src/eLSD_Sfun_FlatApi.c`；
- `stub/eLSD_Lib_stub.c` 和 `include/eLSD_Lib_stub_host.h`；
- 生成模型 `VTC_Scheme2_Loopback_R2020b.slx`；
- 条件生成模型 `VTC_ASW_Scheme2_R2020b.slx`。

方案二的目标是让 Simulink 模型通过 C S-Function 直接调用 `eLSD_Lib` 的 Get/Set 和电机接口。它没有消除结构体与模型信号之间的映射，只是把映射从模型外部适配层移动到了 `eLSD_Sfun_FlatApi.c`。

本文基于源代码静态审查。当前环境没有 MATLAB R2020b、Legacy Code Tool 编译结果或供应商二进制库，因此 MEX 构建、模型调度和目标代码链接仍需实机验证。

当前 `generated/` 目录只有 `.gitkeep`，目录中也没有已生成的 Scheme2 `.slx`、MEX、TLC 或 `rtwmakecfg.m`。下文所述模型均是依据构建脚本推导的预期结构，不代表已经成功生成或运行。

## 2. 方案一与方案二的边界

### 2.1 方案一

```text
eLSD_Lib Get
  -> 模型外手写 C 适配层
  -> RxBus
  -> VTC_ASW
  -> TxBus / MotorCmdBus
  -> 模型外手写 C 适配层
  -> eLSD_Lib Set / mcIf
```

### 2.2 方案二

```text
模型内部 GetRx S-Function
  -> eLSD_Sfun_FlatApi.c
  -> eLSD_Lib Get
  -> 40项向量
  -> RxBus
  -> VTC_ASW
  -> TxBus / MotorCmdBus
  -> 16项/4项向量
  -> SetTxMot S-Function
  -> eLSD_Lib Set / mcIf
```

ASW 控制算法仍由父目录中的 `VTC_ASW_R2020b` 提供。方案二只改变 BSW 接口接入位置，不改变 ASW 内部算法、标定和物理计算。ASW 的完整算法解释见 `../VTC_TorqueVectoring_Detailed_Description_CN.md`。

## 3. 生成产物和前置条件

### 3.1 工具要求

- MATLAB R2020b；
- Simulink；
- Simulink Coder 的 Legacy Code Tool；
- 受支持的 C 编译器，需先执行 `mex -setup C`；
- 完整 ASW 模型还需要父目录先生成 `VTC_ASW_R2020b.slx` 和 `VTC_Interface_Buses.mat`。

### 3.2 `build_sfun_eLSD_Scheme2.m` 的产物

Legacy Code Tool 在 `generated/` 中为四个接口生成 S-Function 源码、MEX 和库块：

| S-Function | 入口函数 | 采样时间 | 用途 |
|---|---|---:|---|
| `sfun_eLSD_GetAswRx` | `eLSD_Sfun_GetAswRx` | 0.005 s | 从 BSW 读取接收快照并输出 40 项向量。 |
| `sfun_eLSD_SetAswTxMot` | `eLSD_Sfun_SetAswTxMot` | 0.005 s | 把 16 项发送量和 4 项电机命令写入 BSW。 |
| `sfun_eLSD_StubInjectRx` | `eLSD_Sfun_StubInjectRx` | 0.005 s | 仅主机仿真：把测试向量写入桩的 RX 存储。 |
| `sfun_eLSD_StubReadTxMot` | `eLSD_Sfun_StubReadTxMot` | 0.005 s | 仅主机仿真：读回桩中最后一次 TX 和电机命令。 |

主机 MEX 的公共源文件：

- `eLSD_Sfun_FlatApi.c`：真实扁平封装；
- `eLSD_Lib_stub.c`：桌面端 `eLSD_Lib` 替身。

头文件搜索路径：

- `scheme2/include`；
- `20260922` 根目录，提供 `eLSD_Lib.h` 和 `eLSD_Lib_ComTypes.h`；
- `scheme2/stub`。

源文件搜索路径：

- `scheme2/src`；
- `scheme2/stub`。

脚本实际只调用：

- `sfcn_cmex_generate`：生成 MEX S-Function 包装源码；
- `compile`：编译主机 MEX；
- `slblock_generate`：生成可用于放置块的库/示例块。

脚本没有调用 `sfcn_tlc_generate` 和 `rtwmakecfg_generate`。因此当前流程只足以尝试主机 MEX，不能据此认定这些 S-Function 已支持 ERT 目标代码生成。并且最终模型的 `addSfun` 创建的是通用 S-Function 块，只设置 `FunctionName`，没有复制 `slblock_generate` 生成的掩码块。

### 3.3 构建脚本变量

`build_sfun_eLSD_Scheme2.m` 中：

- `here`：`scheme2` 目录；
- `genDir`：生成文件目录 `generated`；
- `libHdr`：父级 `20260922` 目录；
- `incDir`、`srcDir`、`stubDir`：封装头文件、封装源码和桩目录；
- `incPaths`、`srcPaths`：传给 Legacy Code Tool 的搜索路径集合；
- `commonSrc`：每个 S-Function 共同编译的两个 C 文件；
- `old`：切换到生成目录前的工作目录；
- `cleanup`：函数退出时恢复工作目录；
- `defs`：四个 Legacy Code Tool 定义的 cell 数组；
- `def`：循环中当前 S-Function 定义；
- `name`、`fcnSpec`、`srcFiles`、`hdr`：`localDef` 的名称、函数签名、源文件和头文件参数。

每个 `def` 的关键属性：

- `SFunctionName`：生成的 S-Function 名；
- `OutputFcnSpec`：C 函数原型和端口维度；
- `HeaderFiles`、`SourceFiles`、`IncPaths`、`SrcPaths`：编译依赖；
- `Options.language='C'`：按 C 语言生成；
- `SampleTime=0.005`：5 ms 调用。

## 4. 模型一：VTC_Scheme2_Loopback_R2020b

### 4.1 目的

该模型不包含 VTC 控制器，只验证：

1. 40 项接收向量能否写入主机桩；
2. Get S-Function 能否按相同顺序读回；
3. 16 项发送量和 4 项电机命令能否写入桩；
4. Readback S-Function 能否按相同顺序读回。

固定步长为 5 ms，默认终止时间为 1 s。

### 4.2 根端口

| 端口 | 方向 | 宽度 | 类型 | 含义 |
|---|---|---:|---|---|
| `RxIn` | 输入 | 40 | `single` | 待注入桩的 RX 扁平向量。 |
| `TxIn` | 输入 | 16 | `single` | 待写入桩的发送报文向量。 |
| `MotIn` | 输入 | 4 | `single` | 待写入桩的电机命令向量。 |
| `RxOut` | 输出 | 40（继承） | 继承 | Get 接口从桩读出的 RX 向量。 |
| `TxOut` | 输出 | 16（继承） | 继承 | 桩中最后保存的 TX 向量。 |
| `MotOut` | 输出 | 4（继承） | 继承 | 桩中最后保存的电机命令。 |

### 4.3 模块和计算意义

#### `Inject`

- S-Function：`sfun_eLSD_StubInjectRx`；
- 输入：`RxIn[40]`；
- 输出：`sync[1]`，固定为 1；
- 动作：调用 `eLSD_Stub_InjectAswRx`，把 40 项数据拆包到桩的 10 个报文快照中。

`sync` 本身没有物理含义，是同一仿真步内的执行顺序依赖信号。

#### `GetRx`

- S-Function：`sfun_eLSD_GetAswRx`；
- 输入：`sync`，函数体忽略数值；
- 输出：`RxOut[40]`；
- 动作：调用十个 `eLSD_Lib_Com_GetRx*`，再按接口顺序打包。

由于 `GetRx` 的输入来自 `Inject` 输出，Simulink 数据依赖保证先注入、后读取。

#### `SetTxMot`

- S-Function：`sfun_eLSD_SetAswTxMot`；
- 输入 1：`TxIn[16]`；
- 输入 2：`MotIn[4]`；
- 无输出；
- 动作：拆包成 VTC_RL1、TOD1、ELD1 三个发送结构，并调用电机 setter。

模块优先级设置为 1。

#### `Readback`

- S-Function：`sfun_eLSD_StubReadTxMot`；
- 无输入；
- 输出：`TxOut[16]` 和 `MotOut[4]`；
- 动作：读取桩中最后保存的数据。

模块优先级设置为 2。由于它与 `SetTxMot` 之间没有信号线，优先级用于保证同一步内先写后读；必须在 R2020b 的排序结果中确认该优先级确实生效。

更重要的是，四个 S-Function 由四次独立的 Legacy Code Tool 编译产生，并且每个定义都把 `eLSD_Lib_stub.c` 静态编入自己的 MEX。通常每个 MEX 模块会拥有一份独立的 `g_*` 静态变量：

- `Inject` 写入的是 Inject MEX 自己的 RX 存储；
- `GetRx` 读取的是 GetRx MEX 自己的另一份 RX 存储；
- `SetTxMot` 与 `Readback` 也很可能各自操作不同的 TX/电机存储。

因此，即使 Simulink 调度顺序完全正确，当前四 MEX 结构也很可能无法实现本节所述回环。必须通过共享动态库、单一多功能 S-Function、共享 Simulink 数据存储或其他明确的跨 MEX 状态机制重新设计。

### 4.4 回环判据

在没有非有限值和非法枚举的正常输入范围内，预期：

- `RxOut == RxIn`；
- `TxOut == TxIn`，但枚举项会经过浮点到整数转换后再转回浮点；
- `MotOut(1:2) == MotIn(1:2)`；
- `MotOut(3)` 根据 `MotIn(3)>0.5` 变为严格的 0 或 1；
- `MotOut(4)` 经整数舍入和枚举转换后读回。

## 5. 模型二：VTC_ASW_Scheme2_R2020b

### 5.1 生成条件和目的

只有父目录同时存在以下文件时才生成：

- `VTC_ASW_R2020b.slx`；
- `VTC_Interface_Buses.mat`。

构建脚本加载 `RxBus`、`ExtInBus`、`TxBus`、`MotorCmdBus` 并赋给 MATLAB base workspace。模型默认运行 10 s，固定步长 5 ms，并尝试设置 `ert.tlc`。

### 5.2 根端口

| 端口 | 方向 | 类型/宽度 | 含义 |
|---|---|---|---|
| `Sync` | 输入 | `single[1]` | 调度依赖输入。主机 MIL 中连接 Inject 的同步脉冲；目标端连接 Constant 1。 |
| `ExtInBus` | 输入 | `Bus: ExtInBus` | 摩擦系数和左右离合器温度。 |
| `TxBus` | 输出 | `Bus: TxBus` | ASW 的 16 项发送输出，同时供记录。 |
| `MotorCmdBus` | 输出 | `Bus: MotorCmdBus` | ASW 的 4 项左电机命令，同时供记录。 |

### 5.3 模块数据流

#### `GetRx`

读取 BSW 并输出 40 项向量。`Sync` 建立调用时序，不参与数值计算。

#### `RX_Demux`

把 40 项向量拆成 40 条标量信号。每个输出端口的序号必须与扁平 C 接口一致。

#### `RX_Bus`

把 40 条命名信号重建为非虚拟 `RxBus`。信号线名称决定 Bus 元素对应关系。

#### `VTC_ASW`

Model Reference，引用父目录的 `VTC_ASW_R2020b`：

- 输入 1：`RxBus`；
- 输入 2：`ExtInBus`；
- 输出 1：`TxBus`；
- 输出 2：`MotorCmdBus`。

该模块包含全部控制和物理意义计算；方案二模型本身不重复实现控制算法。

#### `TX_Select` / `TX_Mux`

按固定名称从 `TxBus` 选择 16 个元素并重新组成向量。

#### `MC_Select` / `MC_Mux`

按固定名称从 `MotorCmdBus` 选择 4 个元素并组成向量。

#### `SetTxMot`

将 16/4 项向量写入 `eLSD_Lib`。优先级设置为 2。由于其输入依赖 ASW 输出，正常排序应在 ASW 步进之后执行。

### 5.4 同一 5 ms 回调中的目标执行语义

目标设计意图是：

1. BSW 已更新当前 CAN 接收快照；
2. `GetRx` 调用 Get API；
3. ASW 用当前输入计算；
4. `SetTxMot` 写入发送快照和电机命令；
5. BSW 在其通信调度中发送快照。

`eLSD_Lib.h` 要求通信 Get/Set 只能从客户 5 ms 回调调用。因此生成模型的 step 必须只由该回调调用，不能由其他任务并发调用。

## 6. 40 项 RX 扁平向量与 C 字段映射

下表中的向量序号为 MATLAB/文档的 1 基索引；C 数组下标为 0 基索引。

| 序号 | C 下标 | 模型名 | C 快照字段 | 单位/编码 | 含义 |
|---:|---:|---|---|---|---|
| 1 | 0 | `ECM2_AccelPedalPosn` | `ecm2.data.AccelPedalPosn` | % | 加速踏板开度。 |
| 2 | 1 | `ECM2_AccelPedalVldty` | `ecm2.data.AccelPedalPosnVldty` | 0～3 | 踏板有效性枚举。 |
| 3 | 2 | `ECM2_Expired` | `ecm2.state.expired` | 0/1 | ECM2 超时。 |
| 4 | 3 | `CSA2_SteerWheelAng` | `csa2.data.SteerWheelAng` | deg | 转角幅值。 |
| 5 | 4 | `CSA2_SteerWheelAngSign` | `csa2.data.SteerWheelAngSign` | 0左/1右 | 转角符号。 |
| 6 | 5 | `CSA2_SteerWheelAngStatus` | `csa2.data.SteerWheelAngStatus` | 状态码 | 转角质量。 |
| 7 | 6 | `CSA2_Expired` | `csa2.state.expired` | 0/1 | CSA2 超时。 |
| 8 | 7 | `ESP2_DrivingModReq` | `esp2.data.DrivingModReq_ESP` | 模式码 | 驾驶模式请求。 |
| 9 | 8 | `ESP2_DiagActv` | `esp2.data.ESP_DiagActv` | 0/1 | ESP 诊断活动。 |
| 10 | 9 | `ESP2_TAB_Sts` | `esp2.data.TAB_Sts` | 0～3 | TAB 状态。 |
| 11 | 10 | `ESP2_Expired` | `esp2.state.expired` | 0/1 | ESP2 超时。 |
| 12 | 11 | `ESP1_ClutOpenReq` | `esp1.data.ClutOpenReq` | 0～3 | ESP 离合器请求。 |
| 13 | 12 | `ESP1_Expired` | `esp1.state.expired` | 0/1 | ESP1 超时。 |
| 14 | 13 | `ABS3_VehSpd` | `abs3.data.VehSpd` | km/h | 车辆速度。 |
| 15 | 14 | `ABS3_VehSpdVld` | `abs3.data.VehSpdVld` | 0/1 | 车速有效位。 |
| 16 | 15 | `ABS3_VehSpdStatus` | `abs3.data.VehSpdStatus` | 状态码 | 车速质量。 |
| 17 | 16 | `ABS3_ABSActv` | `abs3.data.ABSActv` | 0/1 | ABS 活动。 |
| 18 | 17 | `ABS3_Expired` | `abs3.state.expired` | 0/1 | ABS3 超时。 |
| 19 | 18 | `ABM2_VehYawRate` | `abm2.data.VehYawRate` | rad/s | 横摆角速度。 |
| 20 | 19 | `ABM2_VehYawRateStatus` | `abm2.data.VehYawRateStatus` | 状态码 | 横摆率质量。 |
| 21 | 20 | `ABM2_VehDynYawRateVld` | `abm2.data.VehDynYawRateVld` | 0/1 | 横摆率有效位。 |
| 22 | 21 | `ABM2_Expired` | `abm2.state.expired` | 0/1 | ABM2 超时。 |
| 23 | 22 | `ABS1_FLWheelSpd` | `abs1.data.FLWheelSpd` | km/h | 左前轮速。 |
| 24 | 23 | `ABS1_FRWheelSpd` | `abs1.data.FRWheelSpd` | km/h | 右前轮速。 |
| 25 | 24 | `ABS1_FLWheelSpdStatus` | `abs1.data.FLWheelSpdStatus` | 状态码 | 左前轮速质量。 |
| 26 | 25 | `ABS1_FRWheelSpdStatus` | `abs1.data.FRWheelSpdStatus` | 状态码 | 右前轮速质量。 |
| 27 | 26 | `ABS1_Expired` | `abs1.state.expired` | 0/1 | ABS1 超时。 |
| 28 | 27 | `ABS2_RLWheelSpd` | `abs2.data.RLWheelSpd` | km/h | 左后轮速。 |
| 29 | 28 | `ABS2_RRWheelSpd` | `abs2.data.RRWheelSpd` | km/h | 右后轮速。 |
| 30 | 29 | `ABS2_RLWheelSpdStatus` | `abs2.data.RLWheelSpdStatus` | 状态码 | 左后轮速质量。 |
| 31 | 30 | `ABS2_RRWheelSpdStatus` | `abs2.data.RRWheelSpdStatus` | 状态码 | 右后轮速质量。 |
| 32 | 31 | `ABS2_Expired` | `abs2.state.expired` | 0/1 | ABS2 超时。 |
| 33 | 32 | `BCM6_RearELDLckSwtReq` | `bcm6.data.RearELDLckSwtReq` | 0/1 | ELD 开关请求。 |
| 34 | 33 | `BCM6_Expired` | `bcm6.state.expired` | 0/1 | BCM6 超时。 |
| 35 | 34 | `VTC_RR1_EstimatedTorque` | `rr1.data.VTCEstimatedTorque_R` | Nm | 右侧估算扭矩。 |
| 36 | 35 | `VTC_RR1_EstimatedTorqueStatus` | `rr1.data.VTCEstimatedTorque_RStatus` | 状态码 | 右扭矩质量。 |
| 37 | 36 | `VTC_RR1_ActuatorFailSts` | `rr1.data.VTCActuatorFailSts_R` | 右侧枚举 | 右执行器故障。 |
| 38 | 37 | `VTC_RR1_CouplingFaultSts` | `rr1.data.VTCCouplingFaultSts_R` | 右侧枚举 | 右耦合故障。 |
| 39 | 38 | `VTC_RR1_CouplingThermWarning` | `rr1.data.VTCCouplingThermWarning_R` | 右侧枚举 | 右热状态。 |
| 40 | 39 | `VTC_RR1_Expired` | `rr1.state.expired` | 0/1 | VTC_RR1 超时。 |

`eLSD_Sfun_GetAswRx` 中的局部快照变量为 `ecm2`、`csa2`、`esp2`、`esp1`、`abs3`、`abm2`、`abs1`、`abs2`、`bcm6`、`rr1`；`y` 是 40 项输出，`u_sync` 仅建立排序依赖并被显式忽略。

## 7. 16 项 TX 扁平向量与 C 字段映射

| 序号 | C 下标 | 模型名 | C 结构字段 | 单位/编码 |
|---:|---:|---|---|---|
| 1 | 0 | `RL1_TrqReq_R` | `rl1.VTCClutchTrqReq_R` | Nm |
| 2 | 1 | `RL1_TrqReq_RStatus` | `rl1.VTCClutchTrqReq_RStatus` | 信号状态枚举 |
| 3 | 2 | `RL1_OpenReq_R` | `rl1.VTCClutchOpenReq_R` | 右离合器请求枚举 |
| 4 | 3 | `TOD1_AWDOperMod` | `tod1.AWDOperMod` | AWD 模式枚举 |
| 5 | 4 | `TOD1_EstimatedTorque` | `tod1.EstimatedTorque` | Nm |
| 6 | 5 | `TOD1_EstimatedTorqueStatus` | `tod1.EstimatedTorqueStatus` | 信号状态枚举 |
| 7 | 6 | `TOD1_CouplingFaultSts` | `tod1.CouplingFaultSts` | 耦合故障枚举 |
| 8 | 7 | `TOD1_CouplingThermWarning` | `tod1.CouplingThermWarning` | 热状态枚举 |
| 9 | 8 | `TOD1_ActuatorFailSts` | `tod1.ActuatorFailSts` | 执行器状态枚举 |
| 10 | 9 | `TOD1_ExternalReqIgnored` | `tod1.ExternalReqIgnored` | 请求接受枚举 |
| 11 | 10 | `TOD1_DiagLmpReq` | `tod1.DiagLmpReq` | 请求枚举 |
| 12 | 11 | `ELD1_RearELDLckSts` | `eld1.RearELDLckSts` | 后桥锁止状态 |
| 13 | 12 | `ELD1_ELDFaultSts` | `eld1.ELDFaultSts` | ELD 故障分类 |
| 14 | 13 | `ELD1_ELDFault` | `eld1.ELDFault` | ELD 故障码 |
| 15 | 14 | `ELD1_RollingCounter` | `eld1.RollingCounter_ELD1` | `uint8_t` |
| 16 | 15 | `ELD1_Checksum` | `eld1.Checksum_ELD1` | `uint8_t` |

`rl1`、`tod1`、`eld1` 在填充前全部清零，因此没有映射的字段保持 0。随后依次调用：

- `eLSD_Lib_Com_SetTxVTC_RL1(&rl1)`；
- `eLSD_Lib_Com_SetTxTOD1(&tod1)`；
- `eLSD_Lib_Com_SetTxELD1(&eld1)`。

Set API 保存发送快照，不代表函数调用时立刻发送 CAN 报文。

## 8. 4 项电机命令映射

| 序号 | C 下标 | 模型名 | 调用 | 含义 |
|---:|---:|---|---|---|
| 1 | 0 | `MotTorqueCmd` | `eLSD_Lib_mcIf_setCmdMotTq(tq)` | 左电机扭矩命令，Nm。 |
| 2 | 1 | `MotTorqueLimit` | `eLSD_Lib_mcIf_setReqMotTorqLimit(lim)` | 左电机请求扭矩限值，Nm。 |
| 3 | 2 | `MotCtrlEnable` | `>0.5` 调 Enable，否则 Disable | 电机控制使能。 |
| 4 | 3 | `MotCtrlMode` | `eLSD_Lib_mcIf_setMotCtrlMode(...)` | 0空闲、1位置、2速度、3扭矩。 |

局部变量 `tq` 和 `lim` 是只含 `.value` 的 `eLSD_Lib_NmType` 包装结构。

## 9. FlatApi 辅助函数和转换规则

### 9.1 `asFloat`

输入和输出均为 `float`，直接返回原值。该函数主要表达“物理浮点值直接通过”的意图，没有数值变换。

### 9.2 `asInt`

把浮点编码转换为整数：

- 非负值：`(int)(v+0.5)`；
- 负值：`(int)(v-0.5)`。

这是最接近整数、半值远离零的舍入。枚举应本来就是整数值；如果输入为非整数，该函数会静默量化，没有范围检查。

### 9.3 `expiredFlag`

输入 `eLSD_Lib_Com_RxStateType* state`，把布尔 `expired` 转成严格的 0.0 或 1.0。`ageMs` 和 `received` 没有暴露给模型。

### 9.4 宽度宏

- `ELSD_SFUN_RX_WIDTH=40`；
- `ELSD_SFUN_TX_WIDTH=16`；
- `ELSD_SFUN_MOT_WIDTH=4`。

三个宏必须与 Bus 定义、构建脚本端口宽度和所有 C 数组访问保持一致。

## 10. 主机 Stub 的状态与变量

### 10.1 静态全局状态

| 变量 | 类型/组 | 含义 |
|---|---|---|
| `g_cb5ms` | 5 ms 回调函数指针 | 保存客户回调；主机桩的 `Run` 只调用一次，不进入无限循环。 |
| `g_ecm2` | ECM2 RX 快照 | 踏板报文数据和接收状态。 |
| `g_csa2` | CSA2 RX 快照 | 转向报文数据和接收状态。 |
| `g_esp2` | ESP2 RX 快照 | 驾驶模式、诊断和 TAB。 |
| `g_esp1` | ESP1 RX 快照 | 离合器打开请求。 |
| `g_abs3` | ABS3 RX 快照 | 车速和 ABS 活动。 |
| `g_abm2` | ABM2 RX 快照 | 横摆角速度。 |
| `g_abs1` | ABS1 RX 快照 | 前轮速。 |
| `g_abs2` | ABS2 RX 快照 | 后轮速。 |
| `g_bcm6` | BCM6 RX 快照 | ELD 开关请求。 |
| `g_rr1` | VTC_RR1 RX 快照 | 右 ECU 反馈。 |
| `g_tx_rl1` | VTC_RL1 TX 数据 | 最后一次写入的右离合器命令。 |
| `g_tx_tod1` | TOD1 TX 数据 | 最后一次写入的左 ECU 状态。 |
| `g_tx_eld1` | ELD1 TX 数据 | 最后一次写入的 ELD 状态。 |
| `g_tx_rr1` | VTC_RR1 TX 数据 | 桩为接口完整性保留；方案二 ASW 不写它。 |
| `g_cmd_tq` | `eLSD_Lib_NmType` | 最后一次电机扭矩命令。 |
| `g_cmd_lim` | `eLSD_Lib_NmType` | 最后一次电机扭矩限值。 |
| `g_mode` | 电机模式枚举 | 初始为 IDLE。 |
| `g_enable` | int | 电机控制使能状态。 |

这些变量模拟 BSW 的“当前快照存储”，不是 CAN 网络、异步任务或真实电机。

### 10.2 `markFresh`

输入：

- `state`：要修改的接收状态；
- `expired`：是否标为超时。

动作：

- `ageMs=0`；
- `received=!expired`；
- `state.expired=expired`。

桩不随仿真时间增加 `ageMs`，也不自行产生超时。

### 10.3 `eLSD_Stub_Reset`

将所有 RX/TX/电机存储清零，将电机模式设为 IDLE、使能设为 0，并把十个 RX 快照标为：

- `ageMs=0`；
- `received=false`；
- `expired=true`。

该函数由桩的 `eLSD_Lib_Init` 调用，但构建出的四个 S-Function 本身没有显式 Init S-Function。若测试流程不调用 `eLSD_Lib_Init` 且在 Inject 前先 Get，C 静态零初始化会使 `expired=false`，与预期启动状态不一致。

### 10.4 `eLSD_Stub_InjectAswRx`

输入 `u_rx[40]`，按第 6 节映射写入十个 RX 全局快照。每个报文的 expired 元素传给 `markFresh`。

局部变量 `exp` 是当前报文的超时布尔值。枚举转换采用 `(int)(u+0.5f)`，适合预期的非负枚举，但没有范围检查。

未包含在 40 项向量中的结构字段保持之前的值；由于静态存储初始为 0，通常保持 0。

### 10.5 `eLSD_Stub_ReadAswTx`

把 `g_tx_rl1`、`g_tx_tod1`、`g_tx_eld1` 按 16 项顺序打包到 `y_tx`。用于回环测试，不调用真实 BSW。

### 10.6 `eLSD_Stub_ReadMotorCmd`

输出：

- `g_cmd_tq.value`；
- `g_cmd_lim.value`；
- `g_enable` 转成 0/1；
- `g_mode` 转成浮点枚举码。

### 10.7 `eLSD_Sfun_StubInjectRx`

先调用 Inject，再在 `y_sync` 非空时输出 1。`y_sync` 仅用于 Simulink 排序。

### 10.8 `eLSD_Sfun_StubReadTxMot`

依次调用 TX 读回和电机读回，形成两个输出端口。

### 10.9 桩的 `eLSD_Lib` 公共接口

- `eLSD_Lib_Init`：Reset 后返回 OK；
- `eLSD_Lib_GetVtcRole`：固定返回 VTC-L；
- `eLSD_Lib_SetTask5msCallback`：保存函数指针到 `g_cb5ms`；
- `eLSD_Lib_Run`：若回调非空，只调用一次；与真实库“不返回”的语义不同；
- 十个被使用的 Get：复制对应全局快照；
- 其他 `STUB_GET_EMPTY` Get：清零目标，标记 expired=true、received=false；
- 四个 GetTx：返回当前 TX 存储；
- 四个 SetTx：覆盖对应 TX 存储；
- Motor Enable/Disable/Mode/Torque/Limit：更新静态状态；
- 未被方案二使用的位置、速度、速度斜率 setter：接受但不保存或返回零；
- 电机速度、位置和扭矩反馈 getter：返回零。

## 11. 数据类型和枚举含义

### 11.1 信号质量

`eLSD_Lib_Com_SignalStatusType`：

- 0：NOT_RECEIVED；
- 1：VALID；
- 2：INITIAL；
- 3：INVALID；
- 4：RESERVED；
- 5：OUT_OF_RANGE。

### 11.2 常用请求和活动

- Activity：0 不活动，1 活动；
- Request：0 无请求，1 请求；
- Validity：0 无效，1 有效，2 初始，3 保留；
- 右离合器命令：0 初始，1 RUN，2 FAST_OPEN；
- 电机模式：0 IDLE，1 POSITION，2 SPEED，3 TORQUE。

### 11.3 右侧反馈枚举

- 右执行器：0 初始，1 无故障，2 故障；
- 右耦合：0 初始，1 正常，2 降额，3 临时故障，4 永久故障；
- 右热状态：0 初始，1 正常，2 失效保护。

这些右侧枚举不能与 TOD1 的左侧枚举直接互换，因为编码不同。

## 12. 目标端集成步骤

1. 保留 `sfun_eLSD_GetAswRx` 和 `sfun_eLSD_SetAswTxMot`。
2. 删除主机专用 `StubInjectRx` 和 `StubReadTxMot`。
3. `Sync` 接 Constant 1，或接能保证当前周期 BSW 数据已更新的调度信号。
4. 目标构建中不要链接 `eLSD_Lib_stub.c`。
5. 链接供应商真实 `eLSD_Lib` 源码或库，并包含真实头文件路径。
6. 继续编译 `eLSD_Sfun_FlatApi.c`。
7. 确保模型 step 只在客户 5 ms 回调中调用。
8. 确认目标生成代码支持 Legacy Code Tool 生成的 S-Function；必要时配置 Custom Code、`coder.updateBuildInfo` 或 S-Function 的目标支持文件。
9. 对所有 Get/Set 返回值定义错误处理策略。
10. 在目标上验证任务并发、快照一致性、报文周期和 BSW 初始化顺序。
11. 为目标 S-Function 生成并验证 TLC 和 `rtwmakecfg.m`，或提供等价的非内联/自定义代码生成支持；仅设置模型的 `SystemTargetFile='ert.tlc'` 不足以让外部 S-Function 可部署。

## 13. 静态审查发现的风险

### 13.1 四个独立 MEX 很可能不共享 Stub 状态

每个 S-Function 定义都独立编译 `eLSD_Lib_stub.c`。MEX 是独立动态模块时，文件静态变量不会自动跨模块共享。因此当前 Loopback 的 Inject→Get 和 Set→Readback 很可能在不同存储副本上运行。这是阻止主机回环成立的首要结构性风险。

### 13.2 ERT 代码生成链路不完整

构建脚本没有执行 `sfcn_tlc_generate` 或 `rtwmakecfg_generate`，也没有提供手写 TLC、`rtwmakecfg.m`、目标库配置或其他目标代码生成支持。`VTC_ASW_Scheme2_R2020b` 即使设置为 `ert.tlc`，也不能因此证明其中的 S-Function 可生成和链接目标代码。

### 13.3 Get 返回值被忽略

`eLSD_Sfun_GetAswRx` 对十个 `eLSD_Lib_Com_GetRx*` 的返回值全部使用 `(void)` 丢弃，而且局部快照结构在调用前没有清零。若真实库返回 ERROR 或 NOT_SUPPORTED 且未写目标结构，随后读取的是未初始化内存。输出向量虽然先清零，但会被未初始化字段覆盖。

建议：每个快照先 `memset`，检查返回状态；失败时输出 expired=1 和安全默认值。

### 13.4 Set 和电机接口错误未传播

三个 `SetTx` 和 Enable/Disable/Mode 的状态返回值被忽略，扭矩和限值 setter 本身返回 `void`。模型无法知道目标 BSW 是否接受了命令。

### 13.5 主机 Stub 不模拟时间

桩不增加 `ageMs`、不根据周期使报文超时、不模拟异步 CAN 发送，也不实现真实电机反馈。它只能验证字段映射和基本调用链，不能验证通信时序或执行器性能。

### 13.6 Init 调用不明确

S-Function 构建定义只有 OutputFcn，没有 Start/Initialize/Terminate 函数。模型中也没有 `eLSD_Lib_Init` 模块。主机回环依赖每步 Inject 覆盖 RX；独立 Get 测试可能看到静态零值且 expired=false。

### 13.7 回环模型的无数据依赖排序

Set 与 Readback 依靠块优先级 1/2 排序。需要查看 Sorted Order，确认 R2020b 中优先级方向和执行结果符合“先 Set 后 Readback”。

### 13.8 枚举和整数缺少范围检查

`asInt` 会把任意浮点值舍入后强制转换为枚举；计数器和校验和还会强制转换为 `uint8_t`。超范围值可能截断或形成未定义枚举状态。

### 13.9 接口顺序存在三份副本

40/16/4 顺序同时存在于：

- 父目录 Bus 定义；
- `build_VTC_Scheme2_R2020b.m` 的名称列表；
- `eLSD_Sfun_FlatApi.c` 和 Stub 的数组下标。

任一处单独修改都会静默错配。建议用单一接口描述生成 Bus、C 映射和测试向量。

### 13.10 Scheme2 没有解决外部量缺口

摩擦系数和温度仍通过根端口 `ExtInBus` 输入，不来自 `eLSD_Lib`；其目标端数据源仍需设计。

### 13.11 校验和仍未实现

`ELD1_Checksum` 只是 ASW 输出向量的第 16 项。FlatApi 原样转换为 `uint8_t`，不计算校验和。必须确认 BSW 是否计算；当前头文件说明应用负责提供校验和。

### 13.12 供应商库替换不是简单删除 Stub

Legacy Code Tool 定义的 `SourceFiles` 明确包含 `eLSD_Lib_stub.c`。目标端必须重新配置或重新生成 S-Function 构建信息，确保不会把桩同时链接进目标并与真实库产生重复符号。

### 13.13 其他构建和诊断风险

- `vectorToBus` 对信号命名失败使用空 `catch`，可能把非虚拟 Bus 的元素匹配错误隐藏到模型编译阶段；
- 设置 `SystemTargetFile='ert.tlc'` 也使用空 `catch`，失败时模型仍会保存；
- Stub 的 `Reset` 不清除 `g_cb5ms`；
- Stub 注入把 expired 报文同时标记为 `received=false`，不能表达“曾收到、后来超时”的真实历史；
- 十个 Get 分别调用，脚本没有跨报文原子快照或时间一致性保证。

## 14. 验证清单

### 14.1 MEX 和回环

- 四个 MEX 均成功生成并加载；
- 先验证四个 MEX 是否共享同一 Stub 状态；若不共享，应停止把当前模型作为有效回环测试；
- 40 项 RX 逐项单独置非零，确认仅对应元素回读；
- 16 项 TX 和 4 项电机量逐项回读；
- 检查所有枚举半值、负值和超范围输入的转换；
- 查看 Sorted Order，确认 Inject→Get 和 Set→Readback；
- 仿真重启后检查桩状态是否被正确复位。

### 14.2 完整 ASW

- 父 ASW 和 Bus MAT 正确加载；
- 40 项向量到 RxBus 的线名、顺序和类型全部一致；
- TxBus/MotorCmdBus 到向量的顺序全部一致；
- `Sync` 不改变数值结果，只改变排序；
- ASW 每 5 ms 只调用一次 Get 和 Set；
- ExtInBus 的目标端数据源有效。

### 14.3 目标端

- 确认未链接 Stub；
- 确认目标构建包含有效 TLC/`rtwmakecfg` 或等价支持；
- 确认真实库已初始化后才调用模型；
- 对 Get 的 ERROR、NOT_SUPPORTED 定义安全输出；
- 对 Set 返回错误建立诊断；
- 验证 5 ms 最坏执行时间；
- 验证跨报文快照一致性；
- 验证 ELD1 计数器和校验和；
- 与方案一适配层使用同一测试向量做结果对比。

