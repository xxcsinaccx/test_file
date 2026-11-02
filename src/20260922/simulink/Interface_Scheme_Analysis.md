# VTC Software Interface: Scheme Analysis and Model Decision

## 1. Scope

This note compares two ways for the VTC application software (ASW, Simulink)
to exchange data with the basic software (BSW, supplied as `eLSD_Lib`).
It records the decision taken for the R2020b model and the open items that
remain. Reference headers: `eLSD_Lib.h` and `eLSD_Lib_ComTypes.h` in the
`20260922更新` folder.

## 2. Interface facts from the headers

- The BSW exposes functions only. There are no public global variables.
- Receive path: `eLSD_Lib_Com_GetRx<MSG>(&snapshot)` copies one message into
  the caller's structure. Each snapshot contains `data` and `meta` (named
  `state` in `eLSD_Lib.h`): `ageMs`, `received`, `expired`.
- Transmit path: `eLSD_Lib_Com_SetTx<MSG>(&data)` stores an application snapshot.
  The library transmits it periodically. Nothing is sent immediately.
- Motor path: `eLSD_Lib_mcIf_set*` / `get*` functions. The setters that
  command torque, speed, position and limit return `void`.
- Calls to Get and Set communication functions are allowed only in the
  customer 5 ms callback.
- Physical units are fixed by the header: wheel and vehicle speed in km/h,
  steering angle as a magnitude in degrees plus a sign, yaw rate in rad/s,
  clutch torque in Nm, accelerator position in %.

## 3. Scheme One: BSW calls the Simulink step

The 5 ms callback in the application copies the snapshots into the generated
input structure, calls the model step, and copies the outputs back to Set
functions and motor setters.

```text
eLSD_Lib (BSW) --Get--> adapter (hand-written C) --> model input bus
model step (ASW) --> model output bus --> adapter --Set / mcIf--> eLSD_Lib
```

Properties:

- The ASW is a pure function of its bus inputs. It can be generated, unit
  tested and reviewed without the BSW.
- Only the adapter depends on the library headers.
- The adapter contains field copies only. Unit conversion is inside the ASW,
  so the adapter stays free of control logic.
- Work needed: one bus definition per message group and one adapter file.

## 4. Scheme Two: Simulink calls BSW functions directly

The model contains wrapper blocks (S-Function, or MATLAB Function with
`coder.ceval`) that call the Get and Set functions. The generated code then
includes the library headers and links the library.

Properties:

- No hand-written adapter file, but the wrapper blocks still have to convert
  C structures into Simulink signals, so the mapping work is the same.
- The ASW cannot be built or simulated without the library. A stub is needed
  for MEX simulation, and stub behaviour can differ from the target library.
- Interface changes in the BSW force a model change and a regeneration.
- Requires Custom Code settings, a TLC file or `coder.updateBuildInfo`, and
  verification of the wrapper on the target.

## 5. Comparison

| Topic | Scheme one (adopted) | Scheme two |
|---|---|---|
| Where the struct-to-signal copy lives | Hand-written C adapter | Simulink wrapper blocks |
| ASW depends on BSW headers | No | Yes |
| ASW simulated without BSW | Yes | Needs stub |
| Configuration effort | Low | High |
| Effect of a BSW interface change | Adapter only | Wrapper and regeneration |
| Testability | High | Medium to low |

Decision: scheme one.

## 6. Model decision for R2020b

1. The model is split into two files.
   - `VTC_ASW_R2020b.slx`: the ASW boundary. Root inputs `RxBus` and
     `ExtInBus`; root outputs `TxBus` and `MotorCmdBus`. No plant, no
     scenario, no CAN stack. Intended for ERT code generation.
   - `VTC_TorqueVectoring_R2020b.slx`: the MIL harness. It simulates
     the scenario, the CAN reception path, the VTC-R ECU, the vehicle and the
     rear axle, and references the ASW model with a Model block.
2. Base sample time is 5 ms (`0.005` s), matching the BSW callback. The old
   model used 10 ms.
3. Interface buses (`Simulink.Bus`, all elements `single`) are defined by the
   build script and saved to `VTC_Interface_Buses.mat`.
   - `RxBus` (40 elements): fields used by the ASW from ECM2, CSA2, ESP2, ESP1,
     ABS3, ABM2, ABS1, ABS2, BCM6 and VTC_RR1, each with its `expired` flag.
   - `ExtInBus` (4 elements): clutch friction and clutch temperature. These
     are not in the CAN header (see section 7).
   - `TxBus` (16 elements): VTC_RL1, TOD1 and ELD1 fields. `ELD1_Checksum` is a
     placeholder for the adapter.
   - `MotorCmdBus` (4 elements): left motor torque, torque limit, enable, mode.
4. Units stay as in the header at the boundary. The ASW input conditioning
   converts them: km/h to m/s, degrees to radians, wheel speed to rad/s.
5. The input conditioning derives the signals the controller used before:
   - throttle from the accelerator pedal (ECM2, percent);
   - steering wheel angle from CSA2 (degrees and sign to radians);
   - driving mode request from ESP2 (header codes);
   - rear ELD request from BCM6;
   - ABS and ESP activity from ABS3 and ESP2;
   - fast-open request from ESP1 (`ClutOpenReq = 1`);
   - ESP permission from ESP2 `TAB_Sts` (standby or active), with
     2H as the fallback when ESP2 is expired;
   - fault flags from message timeouts and invalid statuses;
   - right clutch state from VTC_RR1.
6. Output mapping was corrected.
   - Right clutch open request: `RUN = 1` normally, `FAST_OPEN = 2` when the
     safety manager forces open or the rear axle is disconnected. The old
     model sent 1 in the forced-open case, which is `RUN` in the header.
   - AWD mode is sent in TOD1 (`AWDOperMod`), not in VTC_RL1. The old model
     placed the mode in VTC_RL1.
   - Mode codes 2 to 6 match the header `AwdModeType` (Auto1 to Auto4, 2H).
     Fault maps to `SYSTEM_FAULT = 1`.
   - ELD lock state maps to the header `RearLockStateType`.
7. The VTC-R ECU is simulated in the harness, not in the ASW under test.
   The harness uses a one-cycle delay on the RX path to avoid an algebraic loop
   through the model boundary.
8. Bug fixed: the scenario subsystem input was wired to port 2 of a
   one-input subsystem. It is now wired to port 1.

## 7. Gaps in the header (open items)

| # | Gap | Effect | Proposed handling |
|---|---|---|---|
| G1 | No clutch friction signal | Reference and slip limits use a nominal value | `ExtIn_MuLeft/Right` from supplier or calibration |
| G2 | No clutch temperature value (only warnings) | Thermal manager cannot run on CAN data | `ExtIn_TempLeft/Right`; ASW estimator or supplier signal to be decided |
| G3 | No motor feedback in the ASW input | Slip and thermal estimators need speed and torque feedback | Add a motor feedback bus when the estimator is designed |
| G4 | ELD1 checksum needs an algorithm and an owner | Frame rejected if wrong | ASW supplies counter; adapter computes checksum |
| G5 | RX timeouts are not published | Timeout values are CAL assumptions | Obtain values from DBC |
| G6 | Cross-message snapshot consistency not specified | Signals may come from different cycles | Document the assumption; keep to one cycle |
| G7 | `meta` vs `state` field name differs between the two headers | Documentation or code errors | Align names in the header |
| G8 | TAB failure encoded as 3 in header, 0x4 in 0922 spec | ESP permission could be wrong | Resolve with DBC owner |
| G9 | Wheel direction Stop encoded 3 in header, 0x4 in spec | Not used by the ASW | Resolve with DBC owner |
| G10 | ClutOpenReq value 2 (TorqueMaxLimit) has no defined behaviour | Not handled by the ASW | Define behaviour with the supplier |
| G11 | Modes 4H (8), 4L (9) have no VTC behaviour | Mapped to Auto2 by the ASW | Confirm the strategy |
| G12 | Self-learning and NVM interfaces are absent | Not modelled | Separate task after interface review |
| G13 | Hardware inputs (ignition, supply voltage) have no interface | Not modelled | Confirm whether needed |

## 8. Scheme Two S-Function prototype

A prototype lives in `simulink/scheme2/`. It uses the Legacy Code Tool to build
four S-Functions on top of a flat C wrapper (`eLSD_Sfun_FlatApi.c`) and a host
stub of `eLSD_Lib`. The wrapper still packs nested snapshot structs into the
same 40/16/4 float vectors as Scheme One. See `scheme2/README.md`.

This does not change the decision in section 5. Scheme One remains the
recommended product path. The prototype exists to show that Scheme Two is
implementable once the struct mapping is accepted inside the S-Function sources.

## 9. Verification status

- The Scheme One build script and the models were not executed. This environment
  has no MATLAB. Verification is limited to static review of the script.
- The Scheme Two S-Function sources and build scripts were not mexed or
  simulated here for the same reason.
- Menu and property names for Embedded Coder must be checked in the local
  R2020b installation.
- Recommended first run: build the Scheme One model, open the Model Advisor on
  `VTC_ASW_R2020b`, run a short simulation of the harness, and check that the
  ASW outputs match the expected torque and mode for each scenario.
- For Scheme Two: run `scheme2/build_sfun_eLSD_Scheme2`, then the loopback
  model, before wiring the full ASW.
