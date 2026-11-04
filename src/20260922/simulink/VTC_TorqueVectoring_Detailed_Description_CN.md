# VTC Torque Vectoring R2020b 模型详细说明

## 1. 文档范围与依据

本文说明 `build_VTC_TorqueVectoring_R2020b.m` 生成的两个模型：

- `VTC_ASW_R2020b.slx`：VTC-L 应用软件（ASW）边界模型；
- `VTC_TorqueVectoring_R2020b.slx`：包含场景、通信、左右执行器和简化车辆模型的 MIL 测试台。

本文以构建脚本、`eLSD_Lib.h` 和 `eLSD_Lib_ComTypes.h` 为唯一事实依据。脚本中的控制器和被控对象均为原型级简化实现，不能直接视为量产控制策略或高保真车辆模型。当前环境未安装 MATLAB/Simulink，因此本文完成的是静态逐项审查；模型能否在 R2020b 中成功构建、编译和仿真仍需在 MATLAB 环境中确认。

## 2. 总体架构

### 2.1 执行周期和求解器

两个模型均采用：

- 固定步长离散求解器 `FixedStepDiscrete`；
- 基本步长 `0.005 s`，即 5 ms；
- ASW 的 5 ms 周期与 `eLSD_Lib` 客户回调周期一致；
- ASW 和 MIL 测试台的默认仿真终止时间均为 20 s。

ASW 被配置为 ERT 代码生成目标；MIL 测试台不作为代码生成目标。

### 2.2 ASW 主信号流

```text
RxBus / ExtInBus
  -> Bus Selector + Mux
  -> InputConditioning
  -> ModeStateManager + ReferenceEstimator
  -> AWDBaseTorque / SlipController / YawController / ELDController
  -> SafetyThermalManager
  -> TorqueArbitration
  -> LeftActuator
  -> OutputPacking
  -> TxBus / MotorCmdBus
```

### 2.3 MIL 测试台主信号流

```text
TestScenarios
  -> VehiclePlant / RearAxlePlant / CanRxSim / ExtSignals
  -> RxBus / ExtInBus
  -> VTC_ASW（引用模型）
  -> 左执行器命令 + 右侧 CAN 20 ms 链路
  -> RightECU_Sim
  -> RearAxlePlant
  -> VehiclePlant
```

`RXDelay`、`VehicleFeedbackDelay` 和 `AxleFeedbackDelay` 是 5 ms 单位延时，用于打断闭环中的代数环。其初始值为 0。

## 3. 接口总线

所有 Bus 元素均定义为 `single`。枚举和布尔状态也以浮点数形式通过 Bus 传递，进入 C 接口时再转换为枚举或布尔值。

### 3.1 RxBus：40 个接收量

| 序号 | Bus 元素 | 单位/编码 | 物理与控制含义 |
|---:|---|---|---|
| 1 | `ECM2_AccelPedalPosn` | `%` | 加速踏板开度。输入调理后除以 100 并限幅为 0～1。 |
| 2 | `ECM2_AccelPedalVldty` | 1=VALID | 加速踏板有效性；仅等于 1 时采用踏板值。 |
| 3 | `ECM2_Expired` | 0/1 | ECM2 报文超时标志；超时会置关键通信故障。 |
| 4 | `CSA2_SteerWheelAng` | `deg`，幅值 | 方向盘转角绝对值。 |
| 5 | `CSA2_SteerWheelAngSign` | 0=左，1=右 | 方向盘转角符号。模型约定左转为负、右转为正。 |
| 6 | `CSA2_SteerWheelAngStatus` | 1=VALID | 方向盘转角质量状态。 |
| 7 | `CSA2_Expired` | 0/1 | CSA2 报文超时标志。 |
| 8 | `ESP2_DrivingModReq` | 枚举码 | ESP 请求的驾驶模式。 |
| 9 | `ESP2_DiagActv` | 1=激活 | ESP 诊断活动；激活时安全模块强制分离。 |
| 10 | `ESP2_TAB_Sts` | 1=待机，2=激活，3=故障 | TAB 对 VTC 的许可状态；只有 1 或 2 被视为允许。 |
| 11 | `ESP2_Expired` | 0/1 | ESP2 报文超时；超时时驾驶模式回退到 2H。 |
| 12 | `ESP1_ClutOpenReq` | 1=快速分离 | ESP 请求离合器快速打开。脚本未定义编码 2 的行为。 |
| 13 | `ESP1_Expired` | 0/1 | ESP1 报文超时。当前仅使快速分离请求失效，不进入综合关键故障。 |
| 14 | `ABS3_VehSpd` | `km/h` | 车辆速度。输入调理后转换为 `m/s`。 |
| 15 | `ABS3_VehSpdVld` | 0/1 | 车速独立有效标志。 |
| 16 | `ABS3_VehSpdStatus` | 1=VALID | 车速信号质量状态。 |
| 17 | `ABS3_ABSActv` | 1=激活 | ABS 活动状态；激活时强制打开离合器。 |
| 18 | `ABS3_Expired` | 0/1 | ABS3 报文超时。 |
| 19 | `ABM2_VehYawRate` | `rad/s` | 实际车辆横摆角速度。 |
| 20 | `ABM2_VehYawRateStatus` | 1=VALID | 横摆角速度质量状态。 |
| 21 | `ABM2_VehDynYawRateVld` | 0/1 | 横摆角速度独立有效标志。 |
| 22 | `ABM2_Expired` | 0/1 | ABM2 报文超时。 |
| 23 | `ABS1_FLWheelSpd` | `km/h` | 左前轮轮缘线速度。输入调理后转换为车轮角速度。 |
| 24 | `ABS1_FRWheelSpd` | `km/h` | 右前轮轮缘线速度。 |
| 25 | `ABS1_FLWheelSpdStatus` | 1=VALID | 左前轮速质量状态。 |
| 26 | `ABS1_FRWheelSpdStatus` | 1=VALID | 右前轮速质量状态。 |
| 27 | `ABS1_Expired` | 0/1 | ABS1 报文超时。 |
| 28 | `ABS2_RLWheelSpd` | `km/h` | 左后轮轮缘线速度。 |
| 29 | `ABS2_RRWheelSpd` | `km/h` | 右后轮轮缘线速度。 |
| 30 | `ABS2_RLWheelSpdStatus` | 1=VALID | 左后轮速质量状态。 |
| 31 | `ABS2_RRWheelSpdStatus` | 1=VALID | 右后轮速质量状态。 |
| 32 | `ABS2_Expired` | 0/1 | ABS2 报文超时。 |
| 33 | `BCM6_RearELDLckSwtReq` | 1=请求 | 驾驶员后桥差速锁开关请求。 |
| 34 | `BCM6_Expired` | 0/1 | BCM6 报文超时；超时时不接受锁止请求。 |
| 35 | `VTC_RR1_EstimatedTorque` | `Nm` | 右侧 ECU 反馈的右离合器估算扭矩。 |
| 36 | `VTC_RR1_EstimatedTorqueStatus` | 1=VALID | 右侧估算扭矩质量状态。 |
| 37 | `VTC_RR1_ActuatorFailSts` | 2=故障 | 右侧执行器故障状态。 |
| 38 | `VTC_RR1_CouplingFaultSts` | 3=临时故障，4=永久故障 | 右侧耦合器故障状态。 |
| 39 | `VTC_RR1_CouplingThermWarning` | 2=失效保护 | 右侧热状态。当前 ASW 未使用该元素。 |
| 40 | `VTC_RR1_Expired` | 0/1 | 右侧反馈报文超时；超时会使右侧反馈无效并强制分离。 |

### 3.2 ExtInBus：4 个非 CAN 外部量

| 序号 | Bus 元素 | 单位 | 含义 |
|---:|---|---|---|
| 1 | `ExtIn_MuLeft` | `-` | 左侧轮胎/路面摩擦系数估计。用于横摆参考限幅和左轮滑移相关计算。 |
| 2 | `ExtIn_MuRight` | `-` | 右侧摩擦系数估计。 |
| 3 | `ExtIn_TempLeft` | `degC` | 左离合器温度。用于统一热降额。 |
| 4 | `ExtIn_TempRight` | `degC` | 右离合器温度。用于统一热降额。 |

这四个量不来自当前 CAN 头文件。实际项目必须明确其来源、诊断和失效替代值。

### 3.3 TxBus：16 个发送量

| 序号 | Bus 元素 | 单位/编码 | 含义 |
|---:|---|---|---|
| 1 | `RL1_TrqReq_R` | `Nm` | 发给 VTC-R 的右离合器目标扭矩。 |
| 2 | `RL1_TrqReq_RStatus` | 1=VALID | 右扭矩请求状态。脚本固定输出 1。 |
| 3 | `RL1_OpenReq_R` | 1=RUN，2=FAST_OPEN | 右离合器运行/快速分离命令。 |
| 4 | `TOD1_AWDOperMod` | 1=故障，2～5=Auto1～Auto4，6=2H | 当前 AWD 工作模式。 |
| 5 | `TOD1_EstimatedTorque` | `Nm` | 左执行器经过动态处理后的估算扭矩 `TL`。 |
| 6 | `TOD1_EstimatedTorqueStatus` | 1=VALID | 左估算扭矩状态，脚本固定输出 1。 |
| 7 | `TOD1_CouplingFaultSts` | 0=正常，3=永久故障 | 综合故障时输出 3，否则 0。 |
| 8 | `TOD1_CouplingThermWarning` | 0=正常，1=失效保护 | 任一离合器温度达到警告阈值时输出 1。 |
| 9 | `TOD1_ActuatorFailSts` | 0/1 | 综合故障映射出的执行器故障。 |
| 10 | `TOD1_ExternalReqIgnored` | 0=接受 | 当前未赋值，保持 0。 |
| 11 | `TOD1_DiagLmpReq` | 0/1 | 综合故障时请求诊断灯。 |
| 12 | `ELD1_RearELDLckSts` | 0、1、2、3、4、14 | 后桥差速锁状态。 |
| 13 | `ELD1_ELDFaultSts` | 0/1 | 综合故障时输出关键故障 1。 |
| 14 | `ELD1_ELDFault` | 0 | 当前未赋值，始终为“正常”代码 0。 |
| 15 | `ELD1_RollingCounter` | 0～15 | 每个 5 ms ASW 步进加 1 并模 16。 |
| 16 | `ELD1_Checksum` | 0 | 占位值；模型未计算 ELD1 校验和。 |

### 3.4 MotorCmdBus：4 个左电机命令

| 序号 | Bus 元素 | 单位/编码 | 含义 |
|---:|---|---|---|
| 1 | `MotTorqueCmd` | `Nm` | 左执行器扭矩命令 `TL`。 |
| 2 | `MotTorqueLimit` | `Nm` | 电机扭矩上限，固定为 `TMAX`。 |
| 3 | `MotCtrlEnable` | 0/1 | 左电机控制使能。 |
| 4 | `MotCtrlMode` | 0=IDLE，3=TORQUE | 使能时使用扭矩控制模式 3，否则为 0。 |

## 4. 标定量 CAL：逐项说明

`CAL` 子系统将 41 个 Constant 块按下表顺序组成向量。ASW 和测试台各有一套同值常量，二者不是同一数据对象；修改时必须同步，否则被测控制器与测试环境可能使用不同参数。

| CAL 索引 | 名称 | 默认值 | 单位 | 使用位置及物理意义 |
|---:|---|---:|---|---|
| 1 | `TS` | 0.005 | s | 基础采样时间。用于滑移退出计时、横摆控制进入/退出计时、积分器、执行器动态和被控对象积分。应与模型固定步长一致。 |
| 2 | `TCAN` | 0.02 | s | 左到右 ECU 的 CAN 发送周期。`CAN_20ms` 计算 `round(TCAN/TS)=4`，每 4 个基本步更新一次。仅测试台使用。 |
| 3 | `WB` | 2.70 | m | 轴距。用于稳态横摆角速度参考分母 `WB + KUS*vx²`。 |
| 4 | `TRW` | 1.60 | m | 后轮轮距。用于左右轮目标线速度、差动扭矩与横摆力矩互换，以及简化车辆横摆力矩。 |
| 5 | `RE` | 0.34 | m | 轮胎有效滚动半径。用于线速度/角速度、扭矩/纵向力及横摆力矩换算。 |
| 6 | `KUS` | 0.0020 | s²/m | 欠转向系数。速度升高时增大参考横摆率分母，使同一转角下的期望横摆率降低。 |
| 7 | `MU` | 0.90 | - | 场景默认左右摩擦系数。仅用于测试台初始化 `SC(8)`、`SC(9)`；ASW 实际使用 ExtInBus 输入。 |
| 8 | `G` | 9.81 | m/s² | 重力加速度。用于摩擦圆横摆率上限 `mu*g/vx` 和每轮静态法向载荷 `m*g/4`。 |
| 9 | `KPY` | 850 | Nm/(rad/s) | 横摆误差比例增益。`KPY*(yawRef-yaw)` 形成目标附加横摆力矩。 |
| 10 | `KIY` | 130 | Nm/rad | 横摆误差积分增益；横摆率误差积分后的量纲为 rad。 |
| 11 | `SON` | 0.15 | - | 驱动滑移控制进入阈值。后轮滑移率大于该值时对应侧进入限扭。 |
| 12 | `SOFF` | 0.10 | - | 滑移控制退出阈值。与 `SON` 形成回差。 |
| 13 | `TSLP` | 0.20 | s | 滑移退出消抖时间；滑移持续低于 `SOFF` 达到该时间后退出。 |
| 14 | `TPRE` | 120 | Nm/侧 | 低速起步预紧扭矩。低于 5 m/s 且踏板大于 0.05 时，总基础扭矩至少为 `2*TPRE`。 |
| 15 | `TMAX` | 1500 | Nm/侧 | 单侧离合器最大容量。用于基础扭矩、滑移限值、横摆力矩、仲裁、执行器和电机限值。 |
| 16 | `SRU` | 2500 | Nm/s | 扭矩上升斜率。每 5 ms 最大上升量为 `SRU*TS=12.5 Nm`。 |
| 17 | `SRD` | 4000 | Nm/s | 扭矩下降斜率。每 5 ms 最大下降量为 `SRD*TS=20 Nm`。 |
| 18 | `CTO` | 0.06 | s | 右 ECU 接收左侧命令的通信超时。仅测试台使用。代码以 `age < CTO` 判有效，因此恰好等于 60 ms 时已无效。 |
| 19 | `TACT` | 0.08 | s | 执行器一阶时间常数。左执行器和右 ECU 模型均在斜率限制后再应用一阶逼近。 |
| 20 | `TWRN` | 140 | degC | 离合器热警告/开始降额温度。 |
| 21 | `TTRP` | 160 | degC | 离合器热跳闸温度；达到该值时降额为 0 并强制分离。 |
| 22 | `RTH` | 0.040 | degC/W | 离合器对环境热阻。散热功率为 `(temp-25)/RTH`。仅测试台后桥热模型使用。 |
| 23 | `CTH` | 30000 | J/degC | 离合器等效热容。温升率等于净热功率除以热容。 |
| 24 | `ODR` | 0.020 | - | 后桥输入相对车轮的超速比。离合器输入角速度近似为 `(1+ODR)*vx/RE`。 |
| 25 | `SLMX` | 0.25 | - | 注释为“最大命令滑移”，但脚本中没有任何计算引用该标定；当前不生效。 |
| 26 | `VMAX` | 22.2222 | m/s | 横摆扭矩矢量控制最高允许车速，约 80 km/h。 |
| 27 | `EON` | 0.030 | rad/s | 横摆控制进入误差阈值。 |
| 28 | `EOFF` | 0.020 | rad/s | 横摆控制退出误差阈值，与 `EON` 构成回差。 |
| 29 | `TYON` | 0.10 | s | 横摆控制进入消抖时间。 |
| 30 | `TYOFF` | 0.20 | s | 横摆控制退出消抖时间。 |
| 31 | `ELDT` | 800 | Nm/侧 | ELD 锁止时左右离合器目标扭矩。 |
| 32 | `ELDV` | 11.1111 | m/s | ELD 释放/拒绝速度，约 40 km/h。请求速度不低于该值时拒绝锁止。 |
| 33 | `ELDW` | 8.3333 | m/s | ELD 超速警告阈值，约 30 km/h；在 `ELDW ≤ vx < ELDV` 时仍输出锁止扭矩，但状态为 2。 |
| 34 | `MASS` | 1800 | kg | 车辆质量。用于纵向加速度和静态单轮法向载荷。 |
| 35 | `IZZ` | 2800 | kg·m² | 整车绕垂直轴转动惯量。用于差动驱动力矩导致的横摆角加速度。 |
| 36 | `LF` | 1.20 | m | 质心到前轴距离。当前脚本未使用。 |
| 37 | `LR` | 1.50 | m | 质心到后轴距离。当前脚本未使用；且 `LF+LR=WB` 仅作为参数一致性信息。 |
| 38 | `CF` | 90000 | N/rad | 前轴侧偏刚度。当前脚本未使用。 |
| 39 | `CR` | 100000 | N/rad | 后轴侧偏刚度。当前脚本未使用。 |
| 40 | `KSLP` | 3200 | Nm | 滑移超限到扭矩容量削减的增益。`TMAX-KSLP*(slip-SOFF)`。 |
| 41 | `SCN` | 3 | 整数码 | 测试场景选择 1～6。仅测试台使用，经过 `floor(SCN+0.5)` 取整。 |

## 5. ASW 内部向量和变量

### 5.1 CI：InputConditioning 输出的 26 元控制输入

| 索引 | 含义 | 单位/编码 | 生成规则 |
|---:|---|---|---|
| 1 | 归一化加速踏板 | 0～1 | ECM2 未超时且有效性为 1时，`clamp(pedal/100,0,1)`；否则 0。 |
| 2 | 带符号方向盘转角 | rad | 角度转弧度；左转取负，右转取正。 |
| 3 | 驾驶模式请求 | 枚举码 | ESP2 正常时复制模式；ESP2 超时时强制为 6（2H）。 |
| 4 | 有效 ELD 锁止请求 | 0/1 | BCM6 未超时且请求值为 1。 |
| 5 | 有效 ABS 活动 | 0/1 | ABS3 未超时且 `ABSActv=1`。 |
| 6 | 有效 ESP 诊断活动 | 0/1 | ESP2 未超时且 `ESP_DiagActv=1`。 |
| 7 | 有效离合器快速分离请求 | 0/1 | ESP1 未超时且 `ClutOpenReq=1`。 |
| 8 | 左摩擦系数 | - | `ExtIn_MuLeft`，未在输入调理中限幅。 |
| 9 | 右摩擦系数 | - | `ExtIn_MuRight`。 |
| 10 | 保留 | - | 未赋值，始终为 0。 |
| 11 | 关键报文超时故障 | 0/1 | ECM2、CSA2、ESP2、ABS3、ABM2、ABS1、ABS2 中任一超时。 |
| 12 | 信号质量故障 | 0/1 | 无关键超时时，车速有效位/状态、横摆状态/有效位或转角状态不正确。 |
| 13 | 保留 | - | 未赋值。 |
| 14 | 保留 | - | 未赋值。 |
| 15 | ESP/TAB 许可 | 0/1 | ESP2 未超时且 TAB 状态为待机 1 或激活 2。 |
| 16 | 保留 | - | 未赋值。 |
| 17 | 车辆纵向速度 `vx` | m/s | ABS3 未超时、有效位为真且状态为 VALID 时，`max(km/h/3.6,0)`。 |
| 18 | 实际横摆角速度 `yaw` | rad/s | ABM2 未超时且动态横摆有效位为真时复制。 |
| 19 | 左前轮角速度 | rad/s | 有效时为 `(km/h)/3.6/RE`；已计算，但当前下游控制未使用。 |
| 20 | 右前轮角速度 | rad/s | 同上；已计算，但当前下游控制未使用。 |
| 21 | 左后轮角速度 | rad/s | 同上。 |
| 22 | 右后轮角速度 | rad/s | 同上。 |
| 23 | 保留 | - | 未赋值。 |
| 24 | 保留 | - | 未赋值。 |
| 25 | 左离合器温度 | degC | `ExtIn_TempLeft`。 |
| 26 | 右离合器温度 | degC | `ExtIn_TempRight`。 |

### 5.2 RRF：右侧 ECU 反馈摘要

| 索引 | 含义 | 生成规则 |
|---:|---|---|
| 1 | 有效右侧估算扭矩 `Nm` | VTC_RR1 未超时且扭矩状态为 VALID 时复制，否则 0。当前后续控制未使用。 |
| 2 | 右侧故障 | 执行器状态为 2，或耦合故障状态为 3/4 时为 1。 |
| 3 | 右离合器温度 `degC` | 直接复制 `EXV(4)`。当前后续控制未使用。 |
| 4 | 右侧反馈有效 | VTC_RR1 未超时且扭矩状态为 VALID 时为 1。 |
| 5 | 保留 | 未赋值，始终为 0。 |

注意：`VTC_RR1_CouplingThermWarning` 没有进入 `RRF(2)`，因此右 ECU 报告热失效保护时不会单独触发本模型故障；统一热保护只看 ExtInBus 的温度值。

### 5.3 MOD：模式管理输出

| 索引 | 含义 | 编码/规则 |
|---:|---|---|
| 1 | AWD 工作模式 | 2=Auto1/ELD，3=Auto2，4=Auto3，5=Auto4，6=2H。 |
| 2 | 后桥连接许可 | 模式不为 6 且无综合故障时为 1。 |
| 3 | `mode==2` 标志 | 当前下游逻辑没有使用。 |
| 4 | 综合故障 | `CI(11)`、`CI(12)` 或 `RRF(2)` 任一为真。 |

模式映射：

- 请求 10（Economic）、6（2H）或 TAB 不许可：模式 6；
- 有效 ELD 开关请求：模式 2；
- 请求 2、25、26、27：模式 4；
- 请求 5、7、13、28：模式 5；
- 其余请求：模式 3。

请求 8（4H）和 9（4L）没有专门策略，会落入默认模式 3。

### 5.4 REF：参考估算量

| 索引 | 含义 | 单位 |
|---:|---|---|
| 1 | `vx`，车辆速度 | m/s |
| 2 | `yawRef`，限幅后的目标横摆角速度 | rad/s；向量中保留，但下游只使用已经形成的 `REF(3)` |
| 3 | `yawRef-yaw`，横摆率误差 | rad/s |
| 4 | 左后轮归一化驱动滑移率 | - |
| 5 | 右后轮归一化驱动滑移率 | - |

关键计算：

1. 前轮等效转角：`delta = steeringWheelAngle / 15`，其中 15 是硬编码转向传动比。
2. 稳态参考横摆率：

   `yawRef = vx * delta / max(WB + KUS*vx^2, 0.1)`

3. 路面附着限制：

   `yawLim = mu * G / max(vx,1)`

   `mu` 取左右摩擦系数的较小值，并限幅到 0.05～1.2。

4. 左右后轮中心目标线速度：

   `vL = max(vx - yaw*TRW/2, 0)`

   `vR = max(vx + yaw*TRW/2, 0)`

5. 轮胎实际轮缘速度：`wL = rearWheelOmegaL*RE`，`wR = rearWheelOmegaR*RE`。
6. 归一化滑移率：

   `s = (wheelPeripheralSpeed - localVehicleSpeed) / max(wheelPeripheralSpeed, localVehicleSpeed, 0.5)`

正值代表驱动轮转得比地面运动更快，即驱动滑移。

## 6. ASW 各计算模块

### 6.1 `RX_Select`、`RX_Mux`、`EXT_Select`、`EXT_Mux`

这些模块不实施控制计算，只将命名 Bus 元素按固定顺序转换为向量。顺序是接口契约的一部分；改变 Bus 或 Mux 顺序而不同时修改 MATLAB Function 索引会导致字段错配。

### 6.2 `InputConditioning`

物理职责：

- 检查报文超时、有效位和质量状态；
- 在无效时使用安全默认值 0；
- 完成 `% -> 0～1`、`deg -> rad`、`km/h -> m/s`、线速度到轮速 `rad/s` 的换算；
- 形成控制器内部输入 `CI` 和右侧反馈摘要 `RRF`。

局部变量：

- `crit`：关键报文超时的逻辑或；
- `a`：带符号方向盘转角，rad；
- `ok`：右侧扭矩反馈是否同时满足未超时和状态 VALID。

### 6.3 `ModeStateManager`

物理职责是把大量驾驶模式请求压缩为 VTC 的 Auto1～Auto4/2H 工作模式，并在通信、信号质量或右执行器故障时断开后桥。

局部变量：

- `modeReq`：对 `CI(3)` 四舍五入后的整数模式码；
- `fault`：综合故障；
- `mode`：输出 AWD 模式码；
- `rearConnected`：后桥是否允许传递扭矩。

该模块名称包含“State”，但实现没有 `persistent` 状态，实质是纯组合逻辑，不是状态机。

### 6.4 `ReferenceEstimator`

物理职责是根据车速、方向盘转角和欠转向特性生成参考横摆率，同时估算左右后轮驱动滑移。

局部变量：

- `vx`：非负车速；
- `yaw`：实测横摆率；
- `delta`：前轮等效转角；
- `den`：稳态横摆率模型分母；
- `yawRef`：目标横摆率；
- `mu`：左右路面中较低的摩擦系数；
- `yawLim`：摩擦限制下的最大横摆率；
- `vL`、`vR`：左右后轮位置处的车辆纵向速度；
- `wL`、`wR`：左右后轮轮缘速度；
- `sL`、`sR`：左右后轮滑移率。

### 6.5 `AWDBaseTorque`

输出 `TB` 是左右离合器合计基础扭矩，不是单侧扭矩。

1. 后桥未连接时 `TB=0`。
2. 踏板限幅为 0～1。
3. 模式增益：
   - 模式 2 或 3：0.45；
   - 模式 4：0.65；
   - 模式 5：0.80。
4. 基础扭矩：`TB = 2*TMAX*modeGain*throttle`。
5. 低于 5 m/s 且踏板大于 0.05 时，至少施加 `2*TPRE`。
6. 最终限幅为 `0～2*TMAX`。

局部变量 `thr` 为归一化踏板，`vx` 为车速，`modeGain` 为模式增益。

### 6.6 `SlipController`

模块分别监控左右后轮滑移，并输出单侧允许扭矩上限：

- `SL(1)`：左侧上限 `limL`，Nm；
- `SL(2)`：右侧上限 `limR`，Nm；
- `SL(3)`：任一侧滑移控制激活标志。

持久变量：

- `actL`、`actR`：左右滑移控制是否激活；
- `offL`、`offR`：滑移低于退出阈值的累计时间，s。

进入和退出逻辑：

- `slip > SON`：立即激活并清零退出计时；
- 激活期间 `slip < SOFF`：每周期累加 `TS`；
- 低滑移持续达到 `TSLP`：退出；
- 后桥断开：立即清除全部状态。

激活时的容量：

`limit = max(TMAX - KSLP*max(slip-SOFF,0), 0)`

该公式从 `SOFF` 而不是 `SON` 开始计算削减，因此刚进入控制时已经存在 `KSLP*(SON-SOFF)` 的扭矩削减。

### 6.7 `YawController`

模块输出 `DT`，即左右扭矩差命令，定义为后续仲裁中的 `right-left`：

- `left = 0.5*(TB-DT)`；
- `right = 0.5*(TB+DT)`。

因此 `DT>0` 增加右侧扭矩并减少左侧扭矩。

允许条件：

- 后桥已连接；
- 当前不是模式 2（ELD/Auto1）；
- 车速低于 `VMAX`。

持久变量：

- `ie`：横摆率误差积分，rad；
- `active`：横摆控制激活状态；
- `onTime`：误差超过 `EON` 的持续时间；
- `offTime`：误差低于 `EOFF` 的持续时间。

计算过程：

1. `e = yawRef-yaw`。
2. 允许且 `|e|>EON` 持续 `TYON` 后激活。
3. 激活后 `|e|<EOFF` 持续 `TYOFF`，或允许条件消失时退出。
4. 激活时 `ie = ie + TS*e`；未激活时每周期 `ie=0.98*ie`，属于指数泄漏而非立即清零。
5. 目标横摆力矩：`mz = KPY*e + KIY*ie`。
6. `mz` 限幅到 `±0.5*TMAX*TRW/RE`。
7. 激活时转换为扭矩差：`DT = 2*RE*mz/TRW`；未激活时为 0。

此处 PI 没有单独的积分限幅或反饱和反馈；只有总横摆力矩限幅。

### 6.8 `ELDController`

输入锁止请求 `req=CI(4)`，允许驾驶模式直接检查原始请求码：

- 3：SNOW_NEX；
- 5：DEEP_MUD；
- 7：SOFT_SAND；
- 28：UNEVEN_NEX。

输出 `ELD=[leftTorque; rightTorque; state]`：

| `state` | 含义 | 扭矩 |
|---:|---|---|
| 0 | 未锁止 | 0 |
| 1 | 已锁止，速度低于警告阈值 | 左右各 `ELDT` |
| 2 | 已锁止但速度达到 `ELDW` | 左右各 `ELDT` |
| 3 | 当前驾驶模式不允许 | 0 |
| 4 | 车速达到或超过 `ELDV`，请求被拒绝 | 0 |
| 14 | 综合故障 | 0 |

局部变量 `t` 是单侧锁止扭矩，`allowed` 是模式许可。该模块没有锁止状态记忆：请求消失后在下一周期立即解锁。

### 6.9 `SafetyThermalManager`

输出：

- `SAFE(1)=enable`：扭矩使能；
- `SAFE(2)=derate`：热降额系数 0～1；
- `SAFE(3)=forceOpen`：强制分离标志。

计算：

1. `temp=max(leftTemp,rightTemp)`，任一侧过热会同时降低两侧扭矩。
2. `temp<=TWRN` 时 `derate=1`。
3. `TWRN<temp<TTRP` 时线性降额：

   `derate=(TTRP-temp)/(TTRP-TWRN)`

4. 以下任一条件强制分离：
   - ABS 激活；
   - ESP 诊断激活；
   - ESP 快速分离请求；
   - 综合故障；
   - 右侧反馈无效；
   - 温度达到 `TTRP`。
5. 后桥连接且没有强制分离时 `enable=1`。

### 6.10 `TorqueArbitration`

输出 `TRQ=[leftRequest;rightRequest]`，单位 Nm。

仲裁顺序：

1. ELD 状态为 1 或 2 时，ELD 左右目标直接取代基础扭矩和横摆差动命令。
2. 非 ELD 时：
   - `left=0.5*(TB-DT)`；
   - `right=0.5*(TB+DT)`。
3. 各侧先限幅到 `0～SL(side)`。
4. 再限幅到 `TMAX`。
5. 最后乘以 `SAFE.enable*SAFE.derate`。

这意味着安全关闭的优先级最高，滑移容量限制高于普通基础/横摆命令，ELD 只覆盖请求源但仍受滑移和安全限制。

### 6.11 `LeftActuator`

该模块将左侧目标 `TRQ(1)` 变为动态响应 `TL`。

持久变量 `x` 是左执行器当前估算扭矩。每周期依次执行：

1. 目标变化量按上升 `SRU*TS` 和下降 `SRD*TS` 限制；
2. 在斜率限制后的 `x` 上再次执行一阶逼近：

   `a = clamp(TS/max(TACT,TS),0,1)`

   `x = x + a*(target-x)`

3. 输出限幅到 `0～TMAX`。

注意：斜率限制和一阶逼近在同一周期串联作用，但第二步仍朝原始 `target` 移动，因此最终单步变化可能超过第一步的纯斜率限制量。该实现不是严格的“斜率限制器后接一阶对象”标准离散结构。

### 6.12 `OutputPacking`

该模块把内部量映射为 TxBus 和 MotorCmdBus。

持久变量 `cnt` 是 4 位滚动计数器，每个 ASW 5 ms 步进先加 1 再模 16。若 ELD1 实际报文周期不是 5 ms，则计数器更新频率需要重新确认。

局部变量：

- `faultOn`：综合故障；
- `enable`：安全使能；
- `openReq`：强制分离或后桥未连接；
- `lockState`：ELD 内部状态到 CAN 状态的映射。

未显式赋值的 `TXV(10)`、`TXV(14)`、`TXV(16)` 保持 0，分别对应外部请求接受、ELD 故障码和校验和。

### 6.13 `TX_Demux` / `TX_Bus` / `MC_Demux` / `MC_Bus`

这些模块把固定顺序向量恢复为带名称的非结构化 Bus 信号。线名必须与 `Simulink.Bus` 元素名称完全一致。

## 7. MIL 测试台内部变量

### 7.1 SC：场景向量 16 项

| 索引 | 含义 | 单位/编码 |
|---:|---|---|
| 1 | 加速踏板归一化请求 | 0～1 |
| 2 | 方向盘转角 | rad |
| 3 | 驾驶模式请求 | ESP2 枚举码 |
| 4 | ELD 锁止开关请求 | 0/1 |
| 5 | ABS 活动注入 | 0/1 |
| 6 | ESP 诊断活动注入 | 0/1 |
| 7 | ESP 离合器快速分离请求 | 0/1 |
| 8 | 左侧摩擦系数 | - |
| 9 | 右侧摩擦系数 | - |
| 10 | 左到右 CAN 丢帧开关 | 0/1 |
| 11 | 右 ECU 故障注入 | 0/1 |
| 12 | 保留 | 固定 0，当前未使用 |
| 13 | 外加横摆角加速度扰动 | rad/s² |
| 14 | TAB 许可开关 | 默认 1；1 映射为 ACTIVE=2，0 映射为 FAILURE=3 |
| 15 | 保留 | 固定 1，当前未使用 |
| 16 | 保留 | 固定 0，当前未使用 |

### 7.2 六个场景

| SCN | 激励 |
|---:|---|
| 1 | 1 s 后踏板以 0.10/s 斜率上升，最高 0.55；直线、均一附着。 |
| 2 | 1 s 后踏板阶跃到 0.55；左侧 `mu=0.25`，右侧 `mu=0.80`。 |
| 3 | 0.5 s 后踏板 0.35；3 s 后方向盘 0.12 rad；模式请求 2（SPORT）。 |
| 4 | 0.5 s 后踏板 0.30；3 s 后方向盘 0.10 rad；5～6.5 s 加 0.25 rad/s² 横摆扰动。 |
| 5 | 1 s 后踏板 0.45；模式 5（DEEP_MUD）；1～12 s 请求 ELD 锁止。 |
| 6 | 0.5 s 后踏板 0.35；2 s 后方向盘 0.08 rad；5～8 s 阻断左到右 CAN 更新。 |

### 7.3 VF：简化整车状态

| 索引 | 含义 | 单位 |
|---:|---|---|
| 1 | 车辆纵向速度 `vx` | m/s |
| 2 | 横摆角速度 `yaw` | rad/s |
| 3 | 左前轮角速度 | rad/s |
| 4 | 右前轮角速度 | rad/s |

### 7.4 AF：简化后桥状态

| 索引 | 含义 | 单位 |
|---:|---|---|
| 1 | 左后轮角速度 `wL` | rad/s |
| 2 | 右后轮角速度 `wR` | rad/s |
| 3 | 左离合器命令/估算扭矩 `TL` | Nm |
| 4 | 右离合器命令/估算扭矩 `TR` | Nm |
| 5 | 左离合器温度 `tempL` | degC |
| 6 | 右离合器温度 `tempR` | degC |

### 7.5 RRFS：右 ECU 反馈状态

| 索引 | 含义 |
|---:|---|
| 1 | 右执行器动态扭矩 `TR`，Nm |
| 2 | 场景注入的右 ECU 故障标志 |
| 3 | 右 ECU 通信有效标志 |

### 7.6 RX3：右 ECU 接收的简化命令

| 索引 | 含义 |
|---:|---|
| 1 | 保持后的右扭矩请求，Nm |
| 2 | 保持后的右离合器打开请求 |
| 3 | 本周期是否收到新帧 |

原始 `RL1V` 为 `[torque; status; openRequest]`，但 `CAN_20ms` 没有把状态字段送入 `RX3`，右 ECU 仿真也不检查 `RL1_TrqReq_RStatus`。

## 8. MIL 测试台各模块

### 8.1 `TestScenarios`

`Clock` 提供仿真时间 `t`，`ScenarioLogic` 根据 `SCN` 生成 `SC`。除所选场景覆盖的元素外，其余量先设置为默认值。

局部变量 `scn=floor(CAL(41)+0.5)` 是场景号。超出 1～6 时只保留默认场景值，不报错。

### 8.2 `CanRxSim`

将 `SC`、车辆状态、后桥状态和右 ECU 反馈组装为 40 项 Rx 向量。

主要单位换算：

- 踏板：`100*SC(1)`，转换为 `%`；
- 方向盘：`abs(SC(2))*180/pi`，符号单独编码；
- 车速：`VF(1)*3.6`，转换为 `km/h`；
- 轮速：`wheelOmega*RE*3.6`，转换为轮缘 `km/h`。

局部变量：

- `fault=RRFS(2)>0.5`：右侧故障注入；
- `valid=RRFS(3)>0.5`：右侧通信有效。

测试台把多数状态固定为 VALID、expired=0。右热状态 `RXV(39)` 固定为 1（NORMAL），没有根据后桥温度生成。

### 8.3 `ExtSignals`

输出：

`EXV=[SC.muLeft; SC.muRight; AF.tempLeft; AF.tempRight]`

该模块只完成测试环境到 ExtInBus 的映射。

### 8.4 `CAN_20ms`

持久变量：

- `hold(3)`：最近一次成功发送的 `[torque,status,openRequest]`；
- `count`：5 ms 基本周期计数。

`nPer=max(round(TCAN/TS),1)` 给出 CAN 更新间隔。计数达到间隔时产生发送机会；若 `SC(10)` 为 0，则更新 `hold` 并使 `newFrame=1`。丢帧期间保持旧命令，但不产生新帧。

### 8.5 `RightECU_Sim`

持久变量：

- `age`：距离最近新帧的时间；
- `x`：右执行器当前扭矩。

逻辑：

1. 新帧到达时 `age=0`，否则每周期加 `TS`。
2. `age<CTO` 且未注入故障时通信有效。
3. 无效或收到 FAST_OPEN 时目标扭矩为 0。
4. 目标经过与左执行器相同的斜率限制和一阶逼近。
5. 输出限幅到 `0～TMAX`。

该模块是测试台桩，不是被测 ASW，也不是供应商 VTC-R ECU 的真实算法。

### 8.6 `VehiclePlant`

持久状态：

- `vx`：车辆纵向速度；
- `yaw`：横摆角速度。

纵向动力学：

`force = (AF(3)+AF(4))/RE - 0.40*vx²`

`vx = max(vx + TS*force/MASS, 0)`

其中 `0.40*vx²` 是硬编码二次阻力，系数没有独立标定。车辆驱动力直接由离合器扭矩换算，没有使用后桥模型算出的摩擦受限轮胎力，因此低附着场景主要通过轮速/滑移影响控制器，而不会正确限制整车纵向力。

横摆动力学：

- `delta=SC(2)/15`；
- `yawRef=vx*delta/(WB+KUS*vx²)`；
- 差动扭矩横摆力矩 `mz=TRW/(2*RE)*(rightTorque-leftTorque)`；
- `yaw += TS*((yawRef-yaw)/0.25 + mz/IZZ + disturbance)`。

`0.25 s` 是硬编码横摆跟随时间常数。

前轮角速度由刚体运动学直接得到：

- `vFL=max(vx-yaw*TRW/2,0)`；
- `vFR=max(vx+yaw*TRW/2,0)`；
- `omega=v/RE`。

### 8.7 `RearAxlePlant`

持久状态：

- `wL`、`wR`：左右后轮角速度；
- `tempL`、`tempR`：左右离合器温度，初始均为 25 degC。

局部变量和物理意义：

- `vL`、`vR`：左右后轮位置处的地面速度；
- `muL`、`muR`：左右附着系数，最小取 0.05；
- `fz=MASS*G*0.25`：假设静态载荷均匀分配时的单轮垂向载荷；
- `fL`、`fR`：由离合器扭矩和摩擦极限共同限制的纵向轮胎力；
- `J=1.8 kg·m²`：硬编码单轮等效转动惯量；
- `damp=8`：硬编码轮速相对地面速度的粘性阻尼系数；
- `win`：离合器输入侧角速度；
- `pL`、`pR`：离合器滑摩功率。

轮速动态：

`wDot = (clutchTorque - tireForce*RE - damp*(w-v/RE))/J`

热模型：

`p = abs(clutchTorque*(win-wheelOmega))`

`tempDot = (p - (temp-25)/RTH)/CTH`

环境温度硬编码为 25 degC。

### 8.8 三个 Unit Delay

- `RXDelay`：40 项 Rx 向量延迟 5 ms；
- `VehicleFeedbackDelay`：4 项 VF 延迟 5 ms；
- `AxleFeedbackDelay`：6 项 AF 延迟 5 ms。

构建函数 `addDelay` 的 `width` 参数没有被使用；实际宽度依靠 Simulink 信号维度传播。初始条件写为标量 0，需在 R2020b 中确认能否正确扩展到向量。

### 8.9 `Logging`

按 `SC`、`VF`、`AF`、`RRFS`、`RXV` 的顺序通过 Mux 拼接并保存到工作区变量 `VTC_LOG`，格式为 `Structure With Time`，同时送入 Scope。

若按向量拼接，数据区间为：

- 1～16：SC；
- 17～20：VF；
- 21～26：AF；
- 27～29：RRFS；
- 30～69：RXV。

## 9. 模型的重要符号和方向约定

- 方向盘左转：负；右转：正。
- 正横摆率的具体车辆坐标方向没有在脚本中明示，但计算上与“右侧局部速度更高”一致。
- `DT = rightTorque-leftTorque`；正 `DT` 增加右侧扭矩。
- 离合器扭矩均按非负传递建模，不支持反向扭矩。
- 后轮滑移率为正表示轮缘速度高于局部车速。
- 所有温度保护使用左右温度最大值，因此任一侧过热会共同降额。

## 10. 非 CAL 硬编码常量

下列数值不是 CAL 子系统中的 Constant 块，但会影响算法或测试台行为。若模型用于标定研究，应评估是否需要将其参数化。

| 数值/表达式 | 位置 | 含义 |
|---|---|---|
| `100` | InputConditioning、CanRxSim | 踏板百分数与 0～1 归一化值的换算系数。 |
| `pi/180`、`180/pi` | InputConditioning、CanRxSim | 方向盘角度与弧度换算。 |
| `3.6` | InputConditioning、CanRxSim | `m/s` 与 `km/h` 换算。 |
| `15` | ReferenceEstimator、VehiclePlant | 固定方向盘到前轮转角传动比。 |
| `0.1 m` | ReferenceEstimator、VehiclePlant | 横摆参考模型分母的数值下限，防止除零。 |
| `0.05～1.2` | ReferenceEstimator | 横摆参考限幅所用摩擦系数范围。 |
| `1 m/s` | ReferenceEstimator | `mu*g/vx` 中车速分母下限。 |
| `0.5 m/s` | ReferenceEstimator | 滑移率分母下限，抑制接近静止时的数值放大。 |
| `0.45/0.65/0.80` | AWDBaseTorque | 普通、运动和越野模式的基础扭矩增益。 |
| `5 m/s` | AWDBaseTorque | 启用低速预紧的车速边界。 |
| `0.05` | AWDBaseTorque | 启用低速预紧的最小踏板开度。 |
| `0.98` | YawController | 横摆控制未激活时积分状态每 5 ms 的泄漏系数。 |
| `0.5` | YawController | 单侧最大扭矩换算为最大差动横摆力矩时的系数。 |
| `1 degC` | SafetyThermalManager | 热降额分母的最小值，防止 `TTRP-TWRN` 为零。 |
| `16` | OutputPacking | ELD1 滚动计数器模数。 |
| `1/2/3/4/14/15` | OutputPacking | CAN 枚举状态码，不是连续物理量。 |
| `4` 个基本周期 | 默认 CAN 配置 | `TCAN/TS=0.02/0.005` 的结果，不是独立常量。 |
| `0.40` | VehiclePlant | 二次纵向阻力系数；按当前公式量纲应为 `N/(m/s)^2`。 |
| `0.25 s` | VehiclePlant | 横摆率向运动学参考值收敛的固定时间常数。 |
| `1.8 kg·m²` | RearAxlePlant | 单个后轮及等效传动系的转动惯量。 |
| `8` | RearAxlePlant | 轮速相对地面速度的粘性阻尼系数；按公式量纲为 `Nm/(rad/s)`。 |
| `25 degC` | RearAxlePlant | 离合器初始温度和热模型环境温度。 |
| `0.05` | RearAxlePlant | 路面摩擦系数下限。 |
| `0.25` | RearAxlePlant | 假定四轮静载均分时单轮垂向载荷比例。 |
| `0.5` | 多个布尔判断 | 把浮点 0/1 信号判为逻辑量的阈值。 |
| `0.5` 加到枚举后取整 | 多个模式选择 | 对非负浮点枚举执行四舍五入。 |

## 11. 静态审查发现和使用限制

以下内容是代码事实或由公式直接得出的限制，不能在未验证前忽略：

1. `SLMX`、`LF`、`LR`、`CF`、`CR` 五个标定当前未参与任何计算。
2. `CI(10/13/14/16/23/24)`、`RRF(5)`、`SC(12/15/16)` 为保留槽；`MOD(3)`、`RRF(1)`、`RRF(3)` 当前没有下游用途。
3. VTC_RR1 热失效状态未进入安全逻辑；安全逻辑仅使用 ExtInBus 温度。
4. 右侧 CAN 接收仿真忽略扭矩状态字段。
5. ELD 模式许可直接检查原始驾驶模式请求，而模式管理器又可能因 ELD 请求把模式改为 2；两处模式语义并不完全统一。
6. `TOD1_EstimatedTorqueStatus` 和 `RL1_TrqReq_RStatus` 始终为 VALID，即使系统故障或命令被强制置零。
7. `ELD1_RollingCounter` 每 5 ms 更新，而不是在实际 ELD1 报文发送时更新；必须根据通信设计确认。
8. `ELD1_Checksum` 始终为 0；真实接口必须由模型、适配层或 BSW 中明确的一方计算。
9. 车辆模型未使用 `LF/LR/CF/CR`，不是标准二自由度侧向动力学模型。
10. 车辆纵向力没有采用后桥中的附着限值，因此测试台不能用于绝对加速性能或轮胎力验证。
11. 左右执行器动态中的一阶补偿可能突破斜率限制器的单步限制。
12. 横摆 PI 没有显式积分限幅或反饱和。
13. 所有场景和被控对象均为确定性简化模型，没有传感器噪声、偏置、量化、真实 CAN 抖动或执行器死区。
14. 脚本没有检查 `TS` 与模型 FixedStep 是否一致；只修改其一会造成控制计时错误。
15. 两个 CAL 子系统是复制值，不是共享标定数据源。
16. `CI(19:20)` 前轮角速度、`RRF(1)` 右估算扭矩、`RRF(3)` 右温度副本、`MOD(3)`、`REF(2)` 和 `SL(3)` 均已计算但没有下游读取；测试台还只消费 TxBus 的 3 个字段和 MotorCmdBus 的 `MotTorqueCmd`。
17. 滑移退出计时不要求连续低于 `SOFF`：滑移回到 `SOFF～SON` 区间时，`offL/offR` 既不累加也不清零，因此多段低滑移时间可能累计后触发退出。
18. 转向与横摆符号存在需要实机确认的不一致：输入调理把左转定义为负角，但 Plant 的正横摆运动学使右侧局部速度高于左侧；脚本没有给出完整车辆坐标系定义，不能据此确认左右扭矩矢量方向正确。
19. VTC-R 通信无效只通过 `SAFE` 强制分离，不会置 `MOD(4)` 综合故障，因此 TOD1/ELD1 故障输出可能仍显示正常。
20. ELD 状态 2 被描述为超速状态，但仲裁仍施加左右各 `ELDT` 的满锁止扭矩；是否应在警告速度开始降扭需要需求确认。
21. 后轮轮速状态无效只会把对应轮速置零，不会置 `CI(12)`；这会产生较大负滑移，而当前滑移控制只处理正滑移。
22. ABM2 横摆率赋值检查超时和独立有效位，但不直接检查 `VehYawRateStatus`；状态异常虽会触发综合故障，异常数值仍会被写入 `CI(18)`。
23. `MotTorqueLimit` 固定为 `TMAX`，不反映实时热降额或滑移限扭；实际电机接口是否允许这种“命令已降额但限值未降额”的组合需要确认。

## 12. 建议验证项目

1. 在 MATLAB R2020b 中运行构建脚本，确认两个 SLX 和 Bus MAT 文件成功生成。
2. 使用 Model Advisor 检查维度传播、Unit Delay 初始条件、数据类型转换和代数环。
3. 对 40/4/16/4 个接口元素逐项做 Bus 顺序回环测试。
4. 对每个模式请求码、所有超时/状态组合做真值表测试。
5. 验证 `SON/SOFF/TSLP` 和 `EON/EOFF/TYON/TYOFF` 的边界等号行为。
6. 验证温度恰好等于 `TWRN`、`TTRP` 时的输出。
7. 验证 CAN 年龄恰好等于 `CTO` 时右 ECU 失效。
8. 比较左右执行器单步变化与 `SRU/SRD` 标称限制。
9. 明确 ELD1 的真实发送周期、计数器和校验和责任方。
10. 用高保真车辆模型或实车数据替代简化 Plant 后再进行控制性能结论。

