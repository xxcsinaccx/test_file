/**
 * @file eLSD_Lib_ComTypes.h
 * @brief Public vehicle CAN values and DBC state definitions.
 * @details Source: VTC_CAN_Implementation_Reference.html. State values retain
 * their DBC encoding. Unlisted state codes remain unlisted; do not treat them
 * as valid states. Physical values use the units stated below.
 */
#ifndef ELSD_LIB_COM_TYPES_H_
#define ELSD_LIB_COM_TYPES_H_

#include <stdbool.h>
#include <stdint.h>

/** @brief Numeric signal quality, independent of message expiration.
 * @details NOT_RECEIVED until the first reception, including when a DBC
 * startup raw code is available. Thereafter this describes the received code.
 */
typedef enum
{
    COM_SIGNAL_NOT_RECEIVED = 0,
    COM_SIGNAL_VALID,
    COM_SIGNAL_INITIAL,
    COM_SIGNAL_INVALID,
    COM_SIGNAL_RESERVED,
    COM_SIGNAL_OUT_OF_RANGE
} eLSD_Lib_Com_SignalStatusType;

/** @brief Invalid or valid, with explicit startup and reserved codes. */
typedef enum
{
    COM_VALIDITY_INVALID  = 0,
    COM_VALIDITY_VALID    = 1,
    COM_VALIDITY_INITIAL  = 2,
    COM_VALIDITY_RESERVED = 3
} eLSD_Lib_Com_ValidityType;

/** @brief Function or switch activity. */
typedef enum
{
    COM_INACTIVE = 0,
    COM_ACTIVE   = 1
} eLSD_Lib_Com_ActivityType;

/** @brief Request state. */
typedef enum
{
    COM_NO_REQUEST = 0,
    COM_REQUEST    = 1
} eLSD_Lib_Com_RequestType;

/** @brief Switch error state. */
typedef enum
{
    COM_NO_ERROR = 0,
    COM_ERROR    = 1
} eLSD_Lib_Com_ErrorType;

/** @brief External request acceptance in TOD1. */
typedef enum
{
    COM_REQUESTS_ACCEPTED = 0,
    COM_REQUESTS_IGNORED  = 1
} eLSD_Lib_Com_ExternalRequestType;

/** @brief Coupling thermal failsafe in TOD1. */
typedef enum
{
    COM_THERMAL_NORMAL   = 0,
    COM_THERMAL_FAILSAFE = 1
} eLSD_Lib_Com_ThermalType;

/** @brief Actuator failure in TOD1. */
typedef enum
{
    COM_ACTUATOR_NO_FAILURE = 0,
    COM_ACTUATOR_FAILURE    = 1
} eLSD_Lib_Com_ActuatorType;

/** @brief Wheel direction. */
typedef enum
{
    COM_DIRECTION_INVALID  = 0,
    COM_DIRECTION_FORWARD  = 1,
    COM_DIRECTION_BACKWARD = 2,
    COM_DIRECTION_STOP     = 3
} eLSD_Lib_Com_WheelDirectionType;

/** @brief Steering angle sign; the angle field remains a magnitude. */
typedef enum
{
    COM_STEER_LEFT  = 0,
    COM_STEER_RIGHT = 1
} eLSD_Lib_Com_SteerSignType;

/** @brief Steering sensor state. */
typedef enum
{
    COM_SAS_CORRECT            = 0,
    COM_SAS_NOT_CALIBRATED     = 1,
    COM_SAS_INTERMITTENT_ERROR = 2,
    COM_SAS_PERMANENT_ERROR    = 3
} eLSD_Lib_Com_SasStateType;

/** @brief Brake pedal state. */
typedef enum
{
    COM_BRAKE_NOT_PRESSED = 0,
    COM_BRAKE_PRESSED     = 1,
    COM_BRAKE_RESERVED    = 2,
    COM_BRAKE_ERROR       = 3
} eLSD_Lib_Com_BrakeStateType;

/** @brief Engine operating state. */
typedef enum
{
    COM_ENGINE_STOPPED  = 0,
    COM_ENGINE_CRANKING = 1,
    COM_ENGINE_RUNNING  = 2,
    COM_ENGINE_STALLING = 3
} eLSD_Lib_Com_EngineStateType;

/** @brief Transmission selector; codes 10 through 13 have no DBC label. */
typedef enum
{
    COM_LEVER_P            = 0,
    COM_LEVER_RESERVED_1   = 1,
    COM_LEVER_RESERVED_2   = 2,
    COM_LEVER_RESERVED_3   = 3,
    COM_LEVER_RESERVED_4   = 4,
    COM_LEVER_D            = 5,
    COM_LEVER_N            = 6,
    COM_LEVER_R            = 7,
    COM_LEVER_M            = 8,
    COM_LEVER_RESERVED_9   = 9,
    COM_LEVER_INTERMEDIATE = 14,
    COM_LEVER_INVALID      = 15
} eLSD_Lib_Com_LeverType;

/** @brief Current gear; codes 12 through 15 have no DBC label. */
typedef enum
{
    COM_GEAR_N_OR_P   = 0,
    COM_GEAR_1        = 1,
    COM_GEAR_2        = 2,
    COM_GEAR_3        = 3,
    COM_GEAR_4        = 4,
    COM_GEAR_5        = 5,
    COM_GEAR_6        = 6,
    COM_GEAR_7        = 7,
    COM_GEAR_R        = 8,
    COM_GEAR_8        = 9,
    COM_GEAR_9        = 10,
    COM_GEAR_RESERVED = 11
} eLSD_Lib_Com_GearType;

/** @brief Parking brake state. */
typedef enum
{
    COM_EPB_RELEASED    = 0,
    COM_EPB_CLOSED      = 1,
    COM_EPB_IN_PROGRESS = 2,
    COM_EPB_UNKNOWN     = 3
} eLSD_Lib_Com_EpbStateType;

/** @brief ESP clutch opening request. */
typedef enum
{
    COM_CLUTCH_PASSIVE          = 0,
    COM_CLUTCH_FAST_OPEN        = 1,
    COM_CLUTCH_TORQUE_MAX_LIMIT = 2,
    COM_CLUTCH_ERROR            = 3
} eLSD_Lib_Com_ClutchRequestType;

/** @brief Right clutch command; codes 3 through 7 have no DBC label. */
typedef enum
{
    COM_RIGHT_CLUTCH_INITIAL   = 0,
    COM_RIGHT_CLUTCH_RUN       = 1,
    COM_RIGHT_CLUTCH_FAST_OPEN = 2
} eLSD_Lib_Com_RightClutchRequestType;

/** @brief ESP driving mode request. This encoding differs from the display. */
typedef enum
{
    COM_MODE_REQ_NO_TERRAIN   = 0,
    COM_MODE_REQ_STANDARD     = 1,
    COM_MODE_REQ_SPORT        = 2,
    COM_MODE_REQ_SNOW_NEX     = 3,
    COM_MODE_REQ_EXPERT       = 4,
    COM_MODE_REQ_DEEP_MUD     = 5,
    COM_MODE_REQ_2H           = 6,
    COM_MODE_REQ_SOFT_SAND    = 7,
    COM_MODE_REQ_4H           = 8,
    COM_MODE_REQ_4L           = 9,
    COM_MODE_REQ_ECONOMIC     = 10,
    COM_MODE_REQ_SNOW_ESOF    = 11,
    COM_MODE_REQ_SNOW_TOD     = 12,
    COM_MODE_REQ_OFF_ROAD     = 13,
    COM_MODE_REQ_SPORT_PLUS   = 14,
    COM_MODE_REQ_INVALID      = 15,
    COM_MODE_REQ_STAND_2H     = 16,
    COM_MODE_REQ_RESERVED     = 17,
    COM_MODE_REQ_STAND_4H     = 18,
    COM_MODE_REQ_SNOW_4H      = 19,
    COM_MODE_REQ_NORMAL_4L    = 20,
    COM_MODE_REQ_ROCK_4L      = 21,
    COM_MODE_REQ_POTHOLE_4L   = 22,
    COM_MODE_REQ_MUD_SAND_4L  = 23,
    COM_MODE_REQ_MOUNTAIN_4L  = 24,
    COM_MODE_REQ_TURBO        = 25,
    COM_MODE_REQ_RACE         = 26,
    COM_MODE_REQ_GRASS_GRAVEL = 27,
    COM_MODE_REQ_UNEVEN_NEX   = 28,
    COM_MODE_REQ_WADE_4H      = 29,
    COM_MODE_REQ_WADE_4L      = 30,
    COM_MODE_REQ_CLIMB_4L     = 31
} eLSD_Lib_Com_DrivingModeRequestType;

/** @brief Displayed driving mode; not interchangeable with the request enum. */
typedef enum
{
    COM_MODE_DIS_STANDARD     = 0,
    COM_MODE_DIS_SPORT        = 1,
    COM_MODE_DIS_SNOW         = 2,
    COM_MODE_DIS_MUD          = 3,
    COM_MODE_DIS_SAND         = 4,
    COM_MODE_DIS_4L           = 5,
    COM_MODE_DIS_ECONOMIC     = 6,
    COM_MODE_DIS_UNKNOWN      = 7,
    COM_MODE_DIS_AUTO         = 8,
    COM_MODE_DIS_SPORT_PLUS   = 9,
    COM_MODE_DIS_OFF_ROAD     = 10,
    COM_MODE_DIS_STAND_2H     = 11,
    COM_MODE_DIS_EXPERT       = 12,
    COM_MODE_DIS_STAND_4H     = 13,
    COM_MODE_DIS_FAILED       = 14,
    COM_MODE_DIS_INVALID      = 15,
    COM_MODE_DIS_SNOW_4H      = 16,
    COM_MODE_DIS_NORMAL_4L    = 17,
    COM_MODE_DIS_ROCK_4L      = 18,
    COM_MODE_DIS_POTHOLE_4L   = 19,
    COM_MODE_DIS_MUD_SAND_4L  = 20,
    COM_MODE_DIS_MOUNTAIN_4L  = 21,
    COM_MODE_DIS_TURBO        = 22,
    COM_MODE_DIS_RACE         = 23,
    COM_MODE_DIS_GRASS_GRAVEL = 24,
    COM_MODE_DIS_UNEVEN_NEX   = 25,
    COM_MODE_DIS_WADE_4H      = 26,
    COM_MODE_DIS_WADE_4L      = 27,
    COM_MODE_DIS_2H           = 28,
    COM_MODE_DIS_4H           = 29,
    COM_MODE_DIS_NOVICIATE    = 30,
    COM_MODE_DIS_RESERVED     = 31
} eLSD_Lib_Com_DrivingModeDisplayType;

/** @brief TAB function state. */
typedef enum
{
    COM_TAB_OFF     = 0,
    COM_TAB_STANDBY = 1,
    COM_TAB_ACTIVE  = 2,
    COM_TAB_FAILURE = 3
} eLSD_Lib_Com_TabStateType;

/** @brief Reverse gear indication. */
typedef enum
{
    COM_REVERSE_OFF           = 0,
    COM_REVERSE_ON            = 1,
    COM_REVERSE_RESERVED      = 2,
    COM_REVERSE_NOT_AVAILABLE = 3
} eLSD_Lib_Com_ReverseStateType;

/** @brief Vehicle power mode. */
typedef enum
{
    COM_POWER_OFF          = 0,
    COM_POWER_ACC_RESERVED = 1,
    COM_POWER_ON           = 2,
    COM_POWER_CRANK        = 3
} eLSD_Lib_Com_PowerModeType;

/** @brief Clock format. */
typedef enum
{
    COM_TIME_24_HOUR = 0,
    COM_TIME_12_HOUR = 1
} eLSD_Lib_Com_TimeModeType;

/** @brief Clock AM or PM indication. */
typedef enum
{
    COM_TIME_AM = 0,
    COM_TIME_PM = 1
} eLSD_Lib_Com_TimeIndicatorType;

/** @brief Coupling fault state in TOD1. */
typedef enum
{
    COM_COUPLING_NORMAL            = 0,
    COM_COUPLING_REDUCED           = 1,
    COM_COUPLING_TEMPORARY_FAILURE = 2,
    COM_COUPLING_PERMANENT_FAILURE = 3
} eLSD_Lib_Com_CouplingFaultType;

/** @brief AWD operating mode in TOD1; code 7 has no DBC label. */
typedef enum
{
    COM_AWD_RESERVED     = 0,
    COM_AWD_SYSTEM_FAULT = 1,
    COM_AWD_AUTO1        = 2,
    COM_AWD_AUTO2        = 3,
    COM_AWD_AUTO3        = 4,
    COM_AWD_AUTO4        = 5,
    COM_AWD_2H           = 6
} eLSD_Lib_Com_AwdModeType;

/** @brief Right ECU system type; codes 6 and 7 have no DBC label. */
typedef enum
{
    COM_SYSTEM_INITIAL                 = 0,
    COM_SYSTEM_PART_TIME_2_SPEED_RWD   = 1,
    COM_SYSTEM_PASSIVE_2_SPEED_AWD     = 2,
    COM_SYSTEM_ACTIVE_SINGLE_SPEED_FWD = 3,
    COM_SYSTEM_ACTIVE_SINGLE_SPEED_RWD = 4,
    COM_SYSTEM_ACTIVE_2_SPEED_RWD      = 5
} eLSD_Lib_Com_SystemType;

/** @brief Right coupling fault state; encoding differs from TOD1. */
typedef enum
{
    COM_RIGHT_COUPLING_INITIAL           = 0,
    COM_RIGHT_COUPLING_NORMAL            = 1,
    COM_RIGHT_COUPLING_REDUCED           = 2,
    COM_RIGHT_COUPLING_TEMPORARY_FAILURE = 3,
    COM_RIGHT_COUPLING_PERMANENT_FAILURE = 4
} eLSD_Lib_Com_RightCouplingFaultType;

/** @brief Right coupling thermal state. */
typedef enum
{
    COM_RIGHT_THERMAL_INITIAL  = 0,
    COM_RIGHT_THERMAL_NORMAL   = 1,
    COM_RIGHT_THERMAL_FAILSAFE = 2
} eLSD_Lib_Com_RightThermalType;

/** @brief Right ECU lamp request. */
typedef enum
{
    COM_RIGHT_LAMP_INITIAL       = 0,
    COM_RIGHT_LAMP_NOT_REQUESTED = 1,
    COM_RIGHT_LAMP_REQUESTED     = 2
} eLSD_Lib_Com_RightLampType;

/** @brief Right ECU external request acceptance. */
typedef enum
{
    COM_RIGHT_REQUEST_INITIAL  = 0,
    COM_RIGHT_REQUEST_ACCEPTED = 1,
    COM_RIGHT_REQUEST_IGNORED  = 2
} eLSD_Lib_Com_RightExternalRequestType;

/** @brief Right actuator failure state. */
typedef enum
{
    COM_RIGHT_ACTUATOR_INITIAL    = 0,
    COM_RIGHT_ACTUATOR_NO_FAILURE = 1,
    COM_RIGHT_ACTUATOR_FAILURE    = 2
} eLSD_Lib_Com_RightActuatorType;

/** @brief Right AWD mode; zero is initial, not TOD1 reserved. */
typedef enum
{
    COM_RIGHT_AWD_INITIAL      = 0,
    COM_RIGHT_AWD_SYSTEM_FAULT = 1,
    COM_RIGHT_AWD_AUTO1        = 2,
    COM_RIGHT_AWD_AUTO2        = 3,
    COM_RIGHT_AWD_AUTO3        = 4,
    COM_RIGHT_AWD_AUTO4        = 5,
    COM_RIGHT_AWD_2H           = 6
} eLSD_Lib_Com_RightAwdModeType;

/** @brief Reception state, age in milliseconds, and message timeout state.
 * @details Age saturates at UINT32_MAX. Received becomes true after the first
 * structurally valid frame. Expired is true until the first reception. A
 * message expires when age is strictly greater than its configured timeout.
 */
typedef struct
{
    uint32_t  ageMs;
    bool received;
    bool expired;
} eLSD_Lib_Com_RxStateType;

/** @brief ECM4 physical signals [%], raw special codes, and quality. */
typedef struct
{
    float                       NetEngTrq;
    float                       DrvEngTrqReq;
    bool                       DrvEngTrqReqVld;
    uint32_t                        NetEngTrqRaw;
    eLSD_Lib_Com_SignalStatusType NetEngTrqStatus;
} eLSD_Lib_Com_ECM4DataType;

/** @brief ECM7 signals; torque is in percent. */
typedef struct
{
    float EngTrqEstimdActGear;
    bool EngTrqEstimdActGear_Qf;
} eLSD_Lib_Com_ECM7DataType;

/** @brief CSA2 signals; angle magnitude is in degrees. */
typedef struct
{
    float                       SteerWheelAng;
    uint32_t                        SteerWheelAngRaw;
    eLSD_Lib_Com_SignalStatusType SteerWheelAngStatus;
    eLSD_Lib_Com_SteerSignType    SteerWheelAngSign;
    eLSD_Lib_Com_SasStateType     SAS_Sts;
} eLSD_Lib_Com_CSA2DataType;

/** @brief ECM1 signals; engine speed is in rpm. */
typedef struct
{
    float                     EngSpd;
    eLSD_Lib_Com_ValidityType   EngSpdVldty;
    eLSD_Lib_Com_BrakeStateType BrkPedalSts;
    uint8_t                       Checksum_ECM1;
    uint8_t                       RollingCounter_ECM1;
} eLSD_Lib_Com_ECM1DataType;

/** @brief ESP2 state signals. */
typedef struct
{
    eLSD_Lib_Com_DrivingModeRequestType DrivingModReq_ESP;
    eLSD_Lib_Com_ActivityType           ESP_DiagActv;
    eLSD_Lib_Com_DrivingModeDisplayType DrivingModDis;
    eLSD_Lib_Com_TabStateType           TAB_Sts;
} eLSD_Lib_Com_ESP2DataType;

/** @brief ESP1 clutch request. */
typedef struct
{
    eLSD_Lib_Com_ClutchRequestType ClutOpenReq;
} eLSD_Lib_Com_ESP1DataType;

/** @brief Right clutch command; torque is in Nm. */
typedef struct
{
    float                             VTCClutchTrqReq_R;
    uint32_t                              VTCClutchTrqReq_RRaw;
    eLSD_Lib_Com_SignalStatusType       VTCClutchTrqReq_RStatus;
    eLSD_Lib_Com_RightClutchRequestType VTCClutchOpenReq_R;
} eLSD_Lib_Com_VTC_RL1DataType;

/** @brief Right ECU feedback; estimated torque is in Nm. */
typedef struct
{
    eLSD_Lib_Com_SystemType               VTCSystemType_R;
    eLSD_Lib_Com_RightCouplingFaultType   VTCCouplingFaultSts_R;
    eLSD_Lib_Com_RightThermalType         VTCCouplingThermWarning_R;
    eLSD_Lib_Com_RightLampType            VTCDiagLmpReq_R;
    eLSD_Lib_Com_RightExternalRequestType VTCExternalReqIgnored_R;
    eLSD_Lib_Com_RightActuatorType        VTCActuatorFailSts_R;
    eLSD_Lib_Com_RightAwdModeType         VTCAWDOperMod_R;
    float                               VTCEstimatedTorque_R;
    uint32_t                                VTCEstimatedTorque_RRaw;
    eLSD_Lib_Com_SignalStatusType         VTCEstimatedTorque_RStatus;
} eLSD_Lib_Com_VTC_RR1DataType;

/** @brief Parking brake indication. */
typedef struct
{
    eLSD_Lib_Com_EpbStateType EPBSts;
} eLSD_Lib_Com_EPB1DataType;

/** @brief Transmission state. */
typedef struct
{
    eLSD_Lib_Com_LeverType TGS_LEVER;
    bool                CurrentGearValid;
    eLSD_Lib_Com_GearType  CURRENT_GEAR;
} eLSD_Lib_Com_DCT5DataType;

/** @brief Front wheel signals; speeds are in km/h. */
typedef struct
{
    bool                         FLWheelSpdVld;
    float                         FLWheelSpd;
    uint32_t                          FLWheelSpdRaw;
    eLSD_Lib_Com_SignalStatusType   FLWheelSpdStatus;
    bool                         FRWheelSpdVld;
    float                         FRWheelSpd;
    uint32_t                          FRWheelSpdRaw;
    eLSD_Lib_Com_SignalStatusType   FRWheelSpdStatus;
    uint8_t                           Checksum_ABS1;
    eLSD_Lib_Com_WheelDirectionType FLWheelDriveDirection;
    eLSD_Lib_Com_WheelDirectionType FRWheelDriveDirection;
    uint8_t                           RollingCounter_ABS1;
} eLSD_Lib_Com_ABS1DataType;

/** @brief Inertial signals [m/s^2, m/s^2, rad/s]. */
typedef struct
{
    float                       VehLgtAccel;
    uint32_t                        VehLgtAccelRaw;
    eLSD_Lib_Com_SignalStatusType VehLgtAccelStatus;
    float                       VehLatAccel;
    uint32_t                        VehLatAccelRaw;
    eLSD_Lib_Com_SignalStatusType VehLatAccelStatus;
    float                       VehYawRate;
    uint32_t                        VehYawRateRaw;
    eLSD_Lib_Com_SignalStatusType VehYawRateStatus;
    bool                       VehLgtAccelVld;
    bool                       VehLatAccelVld;
    bool                       VehDynYawRateVld;
} eLSD_Lib_Com_ABM2DataType;

/** @brief Rear wheel signals; speeds are in km/h. */
typedef struct
{
    uint8_t                           Checksum_ABS2;
    bool                         RLWheelSpdVld;
    float                         RLWheelSpd;
    uint32_t                          RLWheelSpdRaw;
    eLSD_Lib_Com_SignalStatusType   RLWheelSpdStatus;
    bool                         RRWheelSpdVld;
    float                         RRWheelSpd;
    uint32_t                          RRWheelSpdRaw;
    eLSD_Lib_Com_SignalStatusType   RRWheelSpdStatus;
    eLSD_Lib_Com_WheelDirectionType RLWheelDriveDirection;
    eLSD_Lib_Com_WheelDirectionType RRWheelDriveDirection;
    uint8_t                           RollingCounter_ABS2;
} eLSD_Lib_Com_ABS2DataType;

/** @brief Left ECU coupling feedback; torque is in Nm. */
typedef struct
{
    eLSD_Lib_Com_ExternalRequestType ExternalReqIgnored;
    eLSD_Lib_Com_RequestType         DiagLmpReq;
    eLSD_Lib_Com_ThermalType         CouplingThermWarning;
    eLSD_Lib_Com_CouplingFaultType   CouplingFaultSts;
    eLSD_Lib_Com_ActuatorType        ActuatorFailSts;
    eLSD_Lib_Com_AwdModeType         AWDOperMod;
    float                          EstimatedTorque;
    uint32_t                           EstimatedTorqueRaw;
    eLSD_Lib_Com_SignalStatusType    EstimatedTorqueStatus;
} eLSD_Lib_Com_TOD1DataType;

/** @brief Vehicle speed [km/h] and ABS state. */
typedef struct
{
    uint8_t                         Checksum_ABS3;
    bool                       VehSpdVld;
    float                       VehSpd;
    uint32_t                        VehSpdRaw;
    eLSD_Lib_Com_SignalStatusType VehSpdStatus;
    eLSD_Lib_Com_ActivityType     ABSActv;
    uint8_t                         RollingCounter_ABS3;
} eLSD_Lib_Com_ABS3DataType;

/** @brief Engine state and accelerator position [%]. */
typedef struct
{
    eLSD_Lib_Com_EngineStateType EngState;
    float                      AccelPedalPosn;
    eLSD_Lib_Com_ValidityType    AccelPedalPosnVldty;
} eLSD_Lib_Com_ECM2DataType;

/** @brief Odometer [km] and validity. */
typedef struct
{
    float                       IP_VehTotDistance;
    uint32_t                        IP_VehTotDistanceRaw;
    eLSD_Lib_Com_SignalStatusType IP_VehTotDistanceStatus;
    bool                       IP_VehTotDistanceValid;
} eLSD_Lib_Com_IP2DataType;

/** @brief Rear differential lock switch signals. */
typedef struct
{
    eLSD_Lib_Com_RequestType RearELDLckSwtReq;
    eLSD_Lib_Com_ErrorType   RearELDLckSwtErrSts;
} eLSD_Lib_Com_BCM6DataType;

/** @brief Power mode and brake or clutch switch. */
typedef struct
{
    eLSD_Lib_Com_PowerModeType SysPowerMod;
    bool                    SysPowerModVld;
    eLSD_Lib_Com_ActivityType  BrkOrClutSwt;
} eLSD_Lib_Com_PEPS2DataType;

/** @brief Reverse gear indication. */
typedef struct
{
    eLSD_Lib_Com_ReverseStateType ReversGearSts;
} eLSD_Lib_Com_BCM1DataType;

/** @brief Vehicle soak time [minutes]. */
typedef struct
{
    float                       VehicleSoakTime;
    uint32_t                        VehicleSoakTimeRaw;
    eLSD_Lib_Com_SignalStatusType VehicleSoakTimeStatus;
} eLSD_Lib_Com_BCM3DataType;

/** @brief Calendar values; year digits are not converted to a year epoch. */
typedef struct
{
    eLSD_Lib_Com_TimeModeType      TimeMod;
    eLSD_Lib_Com_TimeIndicatorType TimeIndcn;
    float                        Time_Hour;
    uint32_t                         Time_HourRaw;
    eLSD_Lib_Com_SignalStatusType  Time_HourStatus;
    float                        Time_Minutes;
    uint32_t                         Time_MinutesRaw;
    eLSD_Lib_Com_SignalStatusType  Time_MinutesStatus;
    float                        Time_Second;
    uint32_t                         Time_SecondRaw;
    eLSD_Lib_Com_SignalStatusType  Time_SecondStatus;
    float                        Time_Year_Left;
    uint32_t                         Time_Year_LeftRaw;
    eLSD_Lib_Com_SignalStatusType  Time_Year_LeftStatus;
    float                        Time_Year_Right;
    uint32_t                         Time_Year_RightRaw;
    eLSD_Lib_Com_SignalStatusType  Time_Year_RightStatus;
    float                        Time_Month;
    uint32_t                         Time_MonthRaw;
    eLSD_Lib_Com_SignalStatusType  Time_MonthStatus;
    float                        Time_Day;
    uint32_t                         Time_DayRaw;
    eLSD_Lib_Com_SignalStatusType  Time_DayStatus;
} eLSD_Lib_Com_HUT34DataType;

/** @brief Engine torque normalization [Nm]. */
typedef struct
{
    float                       MaxEngTrqNorm;
    uint32_t                        MaxEngTrqNormRaw;
    eLSD_Lib_Com_SignalStatusType MaxEngTrqNormStatus;
} eLSD_Lib_Com_ECM3DataType;

/** @brief Ambient temperature [degrees Celsius]. */
typedef struct
{
    float                       ACAmbTemp;
    uint32_t                        ACAmbTempRaw;
    eLSD_Lib_Com_SignalStatusType ACAmbTempStatus;
} eLSD_Lib_Com_AC2DataType;

/** @brief Complete RX message snapshots. Numeric special codes retain the last
 * valid physical value or the startup value. Normal DBC startup codes are
 * converted; special startup codes use zero as an unavailable placeholder.
 * Raw and Status fields identify special encodings. Explicit validity flags
 * remain separate. Consumers must check meta.expired before using a message.
 */
typedef struct
{
    struct
    {
        eLSD_Lib_Com_ECM4DataType data;
        eLSD_Lib_Com_RxStateType  meta;
    } ECM4;
    struct
    {
        eLSD_Lib_Com_ECM7DataType data;
        eLSD_Lib_Com_RxStateType  meta;
    } ECM7;
    struct
    {
        eLSD_Lib_Com_CSA2DataType data;
        eLSD_Lib_Com_RxStateType  meta;
    } CSA2;
    struct
    {
        eLSD_Lib_Com_ECM1DataType data;
        eLSD_Lib_Com_RxStateType  meta;
    } ECM1;
    struct
    {
        eLSD_Lib_Com_ESP2DataType data;
        eLSD_Lib_Com_RxStateType  meta;
    } ESP2;
    struct
    {
        eLSD_Lib_Com_ESP1DataType data;
        eLSD_Lib_Com_RxStateType  meta;
    } ESP1;
    struct
    {
        eLSD_Lib_Com_VTC_RL1DataType data;
        eLSD_Lib_Com_RxStateType     meta;
    } VTC_RL1;
    struct
    {
        eLSD_Lib_Com_VTC_RR1DataType data;
        eLSD_Lib_Com_RxStateType     meta;
    } VTC_RR1;
    struct
    {
        eLSD_Lib_Com_EPB1DataType data;
        eLSD_Lib_Com_RxStateType  meta;
    } EPB1;
    struct
    {
        eLSD_Lib_Com_DCT5DataType data;
        eLSD_Lib_Com_RxStateType  meta;
    } DCT5;
    struct
    {
        eLSD_Lib_Com_ABS1DataType data;
        eLSD_Lib_Com_RxStateType  meta;
    } ABS1;
    struct
    {
        eLSD_Lib_Com_ABM2DataType data;
        eLSD_Lib_Com_RxStateType  meta;
    } ABM2;
    struct
    {
        eLSD_Lib_Com_ABS2DataType data;
        eLSD_Lib_Com_RxStateType  meta;
    } ABS2;
    struct
    {
        eLSD_Lib_Com_TOD1DataType data;
        eLSD_Lib_Com_RxStateType  meta;
    } TOD1;
    struct
    {
        eLSD_Lib_Com_ABS3DataType data;
        eLSD_Lib_Com_RxStateType  meta;
    } ABS3;
    struct
    {
        eLSD_Lib_Com_ECM2DataType data;
        eLSD_Lib_Com_RxStateType  meta;
    } ECM2;
    struct
    {
        eLSD_Lib_Com_IP2DataType data;
        eLSD_Lib_Com_RxStateType meta;
    } IP2;
    struct
    {
        eLSD_Lib_Com_BCM6DataType data;
        eLSD_Lib_Com_RxStateType  meta;
    } BCM6;
    struct
    {
        eLSD_Lib_Com_PEPS2DataType data;
        eLSD_Lib_Com_RxStateType   meta;
    } PEPS2;
    struct
    {
        eLSD_Lib_Com_BCM1DataType data;
        eLSD_Lib_Com_RxStateType  meta;
    } BCM1;
    struct
    {
        eLSD_Lib_Com_BCM3DataType data;
        eLSD_Lib_Com_RxStateType  meta;
    } BCM3;
    struct
    {
        eLSD_Lib_Com_HUT34DataType data;
        eLSD_Lib_Com_RxStateType   meta;
    } HUT34;
    struct
    {
        eLSD_Lib_Com_ECM3DataType data;
        eLSD_Lib_Com_RxStateType  meta;
    } ECM3;
    struct
    {
        eLSD_Lib_Com_AC2DataType data;
        eLSD_Lib_Com_RxStateType meta;
    } AC2;
} eLSD_Lib_Com_RxMessagesType;

/** @brief ELD fault classification. Unlisted codes have no DBC label. */
typedef enum
{
    COM_ELD_NORMAL           = 0,
    COM_ELD_CRITICAL_FAULT   = 1,
    COM_ELD_NO_FRONT_CONTROL = 2,
    COM_ELD_RESERVED_3       = 3,
    COM_ELD_RESERVED_4       = 4,
    COM_ELD_NO_DEGRADATION   = 5,
    COM_ELD_RESERVED_6       = 6,
    COM_ELD_INVALID          = 15
} eLSD_Lib_Com_EldFaultStateType;

/** @brief ELD fault code. Detailed nonzero fault meanings are not supplied. */
typedef enum
{
    COM_ELD_FAULT_NORMAL  = 0,
    COM_ELD_FAULT_VALID   = 1,
    COM_ELD_FAULT_INVALID = 255
} eLSD_Lib_Com_EldFaultCodeType;

/** @brief Rear differential lock state. Unlisted codes have no DBC label. */
typedef enum
{
    COM_REAR_LOCK_UNLOCKED       = 0,
    COM_REAR_LOCK_LOCKED         = 1,
    COM_REAR_LOCK_OVER_SPEED     = 2,
    COM_REAR_LOCK_MODE_ERROR     = 3,
    COM_REAR_LOCK_REQUEST_DENIED = 4,
    COM_REAR_LOCK_RESERVED       = 5,
    COM_REAR_LOCK_FAULT          = 14,
    COM_REAR_LOCK_INVALID        = 15
} eLSD_Lib_Com_RearLockStateType;

/** @brief Outgoing ELD1 signals. The application supplies checksum and counter.
 * @details App_Com does not calculate the checksum or advance the counter.
 */
typedef struct
{
    uint8_t                          Checksum_ELD1;
    eLSD_Lib_Com_EldFaultStateType ELDFaultSts;
    eLSD_Lib_Com_EldFaultCodeType  ELDFault;
    eLSD_Lib_Com_RearLockStateType RearELDLckSts;
    uint8_t                          RollingCounter_ELD1;
} eLSD_Lib_Com_ELD1TxDataType;

/** @brief Outgoing TOD1 signals; torque is in Nm. */
typedef struct
{
    eLSD_Lib_Com_ExternalRequestType ExternalReqIgnored;
    eLSD_Lib_Com_RequestType         DiagLmpReq;
    eLSD_Lib_Com_ThermalType         CouplingThermWarning;
    eLSD_Lib_Com_CouplingFaultType   CouplingFaultSts;
    eLSD_Lib_Com_ActuatorType        ActuatorFailSts;
    eLSD_Lib_Com_AwdModeType         AWDOperMod;
    float                          EstimatedTorque;
    eLSD_Lib_Com_SignalStatusType    EstimatedTorqueStatus;
} eLSD_Lib_Com_TOD1TxDataType;

/** @brief Outgoing right clutch command; torque is in Nm. */
typedef struct
{
    float                             VTCClutchTrqReq_R;
    eLSD_Lib_Com_SignalStatusType       VTCClutchTrqReq_RStatus;
    eLSD_Lib_Com_RightClutchRequestType VTCClutchOpenReq_R;
} eLSD_Lib_Com_VTC_RL1TxDataType;

/** @brief Outgoing right ECU feedback; torque is in Nm. */
typedef struct
{
    eLSD_Lib_Com_SystemType               VTCSystemType_R;
    eLSD_Lib_Com_RightCouplingFaultType   VTCCouplingFaultSts_R;
    eLSD_Lib_Com_RightThermalType         VTCCouplingThermWarning_R;
    eLSD_Lib_Com_RightLampType            VTCDiagLmpReq_R;
    eLSD_Lib_Com_RightExternalRequestType VTCExternalReqIgnored_R;
    eLSD_Lib_Com_RightActuatorType        VTCActuatorFailSts_R;
    eLSD_Lib_Com_RightAwdModeType         VTCAWDOperMod_R;
    float                               VTCEstimatedTorque_R;
    eLSD_Lib_Com_SignalStatusType         VTCEstimatedTorque_RStatus;
} eLSD_Lib_Com_VTC_RR1TxDataType;

/** @brief Outgoing snapshots, separate from received messages.
 * @details The application writes these values in foreground context. For
 * numeric signals set Status to VALID to encode the physical value, or request
 * an explicitly defined INITIAL, INVALID, or RESERVED code. Other statuses,
 * non-finite values, out-of-range values, and states wider than their field
 * suppress the complete frame. Raw quantization rounds halfway upward.
 * Unused payload bits are zero. App_Com initializes DBC startup values once;
 * later transmissions never change the application-owned snapshots.
 */
typedef struct
{
    struct
    {
        eLSD_Lib_Com_ELD1TxDataType data;
    } ELD1;
    struct
    {
        eLSD_Lib_Com_TOD1TxDataType data;
    } TOD1;
    struct
    {
        eLSD_Lib_Com_VTC_RL1TxDataType data;
    } VTC_RL1;
    struct
    {
        eLSD_Lib_Com_VTC_RR1TxDataType data;
    } VTC_RR1;
} eLSD_Lib_Com_TxMessagesType;

#endif /* ELSD_LIB_COM_TYPES_H_ */
