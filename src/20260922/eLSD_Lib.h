/**
 * @file eLSD_Lib.h
 * @brief Public interface for the eLSD actuator library.
 */

#ifndef ELSD_LIB_H_
#define ELSD_LIB_H_

#include "eLSD_Lib_ComTypes.h"

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * @brief Type definition for the customer 5 ms task callback.
 */
typedef void (*eLSD_Lib_Task5msCallbackType)(void);

/**
 * @brief Angle value in rad.
 */
typedef struct
{
    float value; /**< Value in rad. */
} eLSD_Lib_RadType;

/**
 * @brief Angular-speed value in rad/s.
 */
typedef struct
{
    float value; /**< Value in rad/s. */
} eLSD_Lib_RadPerSecType;

/**
 * @brief Angular-acceleration value in rad/s^2.
 */
typedef struct
{
    float value; /**< Value in rad/s^2. */
} eLSD_Lib_RadPerSecSquaredType;

/**
 * @brief Torque value in Nm.
 */
typedef struct
{
    float value; /**< Value in Nm. */
} eLSD_Lib_NmType;

/**
 * @brief Identifies the result of a library operation.
 */
typedef enum
{
    ELSD_LIB_STATUS_OK,            /**< The operation succeeded. */
    ELSD_LIB_STATUS_INIT_FAILED,   /**< Platform initialization failed. */
    ELSD_LIB_STATUS_NOT_SUPPORTED, /**< The operation does not apply to this library artifact. */
    ELSD_LIB_STATUS_ERROR          /**< The operation failed because of an error. */
} eLSD_Lib_StatusType;

/** @brief Fixed vehicle transfer case role of a delivered library artifact. */
typedef enum
{
    ELSD_LIB_VTC_ROLE_LEFT,  /**< VTC-L actuator library. */
    ELSD_LIB_VTC_ROLE_RIGHT  /**< VTC-R actuator library. */
} eLSD_Lib_VtcRoleType;

/** @brief ECM4 RX data and reception state snapshot. */
typedef struct
{
    eLSD_Lib_Com_ECM4DataType data;
    eLSD_Lib_Com_RxStateType  state;
} eLSD_Lib_Com_ECM4RxType;

/** @brief ECM7 RX data and reception state snapshot. */
typedef struct
{
    eLSD_Lib_Com_ECM7DataType data;
    eLSD_Lib_Com_RxStateType  state;
} eLSD_Lib_Com_ECM7RxType;

/** @brief CSA2 RX data and reception state snapshot. */
typedef struct
{
    eLSD_Lib_Com_CSA2DataType data;
    eLSD_Lib_Com_RxStateType  state;
} eLSD_Lib_Com_CSA2RxType;

/** @brief ECM1 RX data and reception state snapshot. */
typedef struct
{
    eLSD_Lib_Com_ECM1DataType data;
    eLSD_Lib_Com_RxStateType  state;
} eLSD_Lib_Com_ECM1RxType;

/** @brief ESP2 RX data and reception state snapshot. */
typedef struct
{
    eLSD_Lib_Com_ESP2DataType data;
    eLSD_Lib_Com_RxStateType  state;
} eLSD_Lib_Com_ESP2RxType;

/** @brief ESP1 RX data and reception state snapshot. */
typedef struct
{
    eLSD_Lib_Com_ESP1DataType data;
    eLSD_Lib_Com_RxStateType  state;
} eLSD_Lib_Com_ESP1RxType;

/** @brief VTC_RL1 RX data and reception state snapshot. */
typedef struct
{
    eLSD_Lib_Com_VTC_RL1DataType data;
    eLSD_Lib_Com_RxStateType     state;
} eLSD_Lib_Com_VTC_RL1RxType;

/** @brief VTC_RR1 RX data and reception state snapshot. */
typedef struct
{
    eLSD_Lib_Com_VTC_RR1DataType data;
    eLSD_Lib_Com_RxStateType     state;
} eLSD_Lib_Com_VTC_RR1RxType;

/** @brief EPB1 RX data and reception state snapshot. */
typedef struct
{
    eLSD_Lib_Com_EPB1DataType data;
    eLSD_Lib_Com_RxStateType  state;
} eLSD_Lib_Com_EPB1RxType;

/** @brief DCT5 RX data and reception state snapshot. */
typedef struct
{
    eLSD_Lib_Com_DCT5DataType data;
    eLSD_Lib_Com_RxStateType  state;
} eLSD_Lib_Com_DCT5RxType;

/** @brief ABS1 RX data and reception state snapshot. */
typedef struct
{
    eLSD_Lib_Com_ABS1DataType data;
    eLSD_Lib_Com_RxStateType  state;
} eLSD_Lib_Com_ABS1RxType;

/** @brief ABM2 RX data and reception state snapshot. */
typedef struct
{
    eLSD_Lib_Com_ABM2DataType data;
    eLSD_Lib_Com_RxStateType  state;
} eLSD_Lib_Com_ABM2RxType;

/** @brief ABS2 RX data and reception state snapshot. */
typedef struct
{
    eLSD_Lib_Com_ABS2DataType data;
    eLSD_Lib_Com_RxStateType  state;
} eLSD_Lib_Com_ABS2RxType;

/** @brief TOD1 RX data and reception state snapshot. */
typedef struct
{
    eLSD_Lib_Com_TOD1DataType data;
    eLSD_Lib_Com_RxStateType  state;
} eLSD_Lib_Com_TOD1RxType;

/** @brief ABS3 RX data and reception state snapshot. */
typedef struct
{
    eLSD_Lib_Com_ABS3DataType data;
    eLSD_Lib_Com_RxStateType  state;
} eLSD_Lib_Com_ABS3RxType;

/** @brief ECM2 RX data and reception state snapshot. */
typedef struct
{
    eLSD_Lib_Com_ECM2DataType data;
    eLSD_Lib_Com_RxStateType  state;
} eLSD_Lib_Com_ECM2RxType;

/** @brief IP2 RX data and reception state snapshot. */
typedef struct
{
    eLSD_Lib_Com_IP2DataType data;
    eLSD_Lib_Com_RxStateType state;
} eLSD_Lib_Com_IP2RxType;

/** @brief BCM6 RX data and reception state snapshot. */
typedef struct
{
    eLSD_Lib_Com_BCM6DataType data;
    eLSD_Lib_Com_RxStateType  state;
} eLSD_Lib_Com_BCM6RxType;

/** @brief PEPS2 RX data and reception state snapshot. */
typedef struct
{
    eLSD_Lib_Com_PEPS2DataType data;
    eLSD_Lib_Com_RxStateType   state;
} eLSD_Lib_Com_PEPS2RxType;

/** @brief BCM1 RX data and reception state snapshot. */
typedef struct
{
    eLSD_Lib_Com_BCM1DataType data;
    eLSD_Lib_Com_RxStateType  state;
} eLSD_Lib_Com_BCM1RxType;

/** @brief BCM3 RX data and reception state snapshot. */
typedef struct
{
    eLSD_Lib_Com_BCM3DataType data;
    eLSD_Lib_Com_RxStateType  state;
} eLSD_Lib_Com_BCM3RxType;

/** @brief HUT34 RX data and reception state snapshot. */
typedef struct
{
    eLSD_Lib_Com_HUT34DataType data;
    eLSD_Lib_Com_RxStateType   state;
} eLSD_Lib_Com_HUT34RxType;

/** @brief ECM3 RX data and reception state snapshot. */
typedef struct
{
    eLSD_Lib_Com_ECM3DataType data;
    eLSD_Lib_Com_RxStateType  state;
} eLSD_Lib_Com_ECM3RxType;

/** @brief AC2 RX data and reception state snapshot. */
typedef struct
{
    eLSD_Lib_Com_AC2DataType data;
    eLSD_Lib_Com_RxStateType state;
} eLSD_Lib_Com_AC2RxType;

/**
 * @brief Motor-control command modes.
 */
typedef enum
{
    ELSD_LIB_MCIF_MCTRL_MODE_IDLE,      /**< No motor command is active. */
    ELSD_LIB_MCIF_MCTRL_MODE_POSITION,  /**< Position command mode. */
    ELSD_LIB_MCIF_MCTRL_MODE_SPEED,     /**< Speed command mode. */
    ELSD_LIB_MCIF_MCTRL_MODE_TORQUE     /**< Torque command mode. */
} eLSD_Lib_mcIf_mCtrlModeType;

/**
 * @brief Initializes the eLSD actuator library.
 * @return eLSD_Lib_StatusType Initialization result.
 */
eLSD_Lib_StatusType eLSD_Lib_Init(void);

/**
 * @brief Gets the fixed role of this library artifact.
 * @return VTC-L or VTC-R role.
 */
eLSD_Lib_VtcRoleType eLSD_Lib_GetVtcRole(void);

/**
 * @brief Sets the customer task called every 5 ms.
 * @param callback eLSD_Lib_Task5msCallbackType customer task callback, or
 *                 NULL to disable the task.
 * @details Call this function before eLSD_Lib_Init.
 */
void eLSD_Lib_SetTask5msCallback(eLSD_Lib_Task5msCallbackType callback);

/**
 * @brief Runs the eLSD actuator library.
 * @details This function does not return.
 */
void eLSD_Lib_Run(void);

/**
 * @brief Gets the current ECM4 RX snapshot.
 * @param snapshot Destination for message data and reception state.
 * @return eLSD_Lib_StatusType Operation result.
 * @details Call communication access functions only from the customer 5 ms
 * callback. The operation fails before successful library initialization or
 * returns NOT_SUPPORTED when the message does not apply to this library role.
 */
eLSD_Lib_StatusType eLSD_Lib_Com_GetRxECM4(eLSD_Lib_Com_ECM4RxType *snapshot);

/**
 * @brief Gets the current ECM7 RX snapshot.
 * @param snapshot Destination for message data and reception state.
 * @return eLSD_Lib_StatusType Operation result.
 * @details Call from the customer 5 ms callback after initialization.
 */
eLSD_Lib_StatusType eLSD_Lib_Com_GetRxECM7(eLSD_Lib_Com_ECM7RxType *snapshot);

/**
 * @brief Gets the current CSA2 RX snapshot.
 * @param snapshot Destination for message data and reception state.
 * @return eLSD_Lib_StatusType Operation result.
 * @details Call from the customer 5 ms callback after initialization.
 */
eLSD_Lib_StatusType eLSD_Lib_Com_GetRxCSA2(eLSD_Lib_Com_CSA2RxType *snapshot);

/**
 * @brief Gets the current ECM1 RX snapshot.
 * @param snapshot Destination for message data and reception state.
 * @return eLSD_Lib_StatusType Operation result.
 * @details Call from the customer 5 ms callback after initialization.
 */
eLSD_Lib_StatusType eLSD_Lib_Com_GetRxECM1(eLSD_Lib_Com_ECM1RxType *snapshot);

/**
 * @brief Gets the current ESP2 RX snapshot.
 * @param snapshot Destination for message data and reception state.
 * @return eLSD_Lib_StatusType Operation result.
 * @details Call from the customer 5 ms callback after initialization.
 */
eLSD_Lib_StatusType eLSD_Lib_Com_GetRxESP2(eLSD_Lib_Com_ESP2RxType *snapshot);

/**
 * @brief Gets the current ESP1 RX snapshot.
 * @param snapshot Destination for message data and reception state.
 * @return eLSD_Lib_StatusType Operation result.
 * @details Call from the customer 5 ms callback after initialization.
 */
eLSD_Lib_StatusType eLSD_Lib_Com_GetRxESP1(eLSD_Lib_Com_ESP1RxType *snapshot);

/**
 * @brief Gets the current VTC_RL1 RX snapshot.
 * @param snapshot Destination for message data and reception state.
 * @return eLSD_Lib_StatusType Operation result.
 * @details Call from the customer 5 ms callback after initialization.
 */
eLSD_Lib_StatusType eLSD_Lib_Com_GetRxVTC_RL1(eLSD_Lib_Com_VTC_RL1RxType *snapshot);

/**
 * @brief Gets the current VTC_RR1 RX snapshot.
 * @param snapshot Destination for message data and reception state.
 * @return eLSD_Lib_StatusType Operation result.
 * @details Call from the customer 5 ms callback after initialization.
 */
eLSD_Lib_StatusType eLSD_Lib_Com_GetRxVTC_RR1(eLSD_Lib_Com_VTC_RR1RxType *snapshot);

/**
 * @brief Gets the current EPB1 RX snapshot.
 * @param snapshot Destination for message data and reception state.
 * @return eLSD_Lib_StatusType Operation result.
 * @details Call from the customer 5 ms callback after initialization.
 */
eLSD_Lib_StatusType eLSD_Lib_Com_GetRxEPB1(eLSD_Lib_Com_EPB1RxType *snapshot);

/**
 * @brief Gets the current DCT5 RX snapshot.
 * @param snapshot Destination for message data and reception state.
 * @return eLSD_Lib_StatusType Operation result.
 * @details Call from the customer 5 ms callback after initialization.
 */
eLSD_Lib_StatusType eLSD_Lib_Com_GetRxDCT5(eLSD_Lib_Com_DCT5RxType *snapshot);

/**
 * @brief Gets the current ABS1 RX snapshot.
 * @param snapshot Destination for message data and reception state.
 * @return eLSD_Lib_StatusType Operation result.
 * @details Call from the customer 5 ms callback after initialization.
 */
eLSD_Lib_StatusType eLSD_Lib_Com_GetRxABS1(eLSD_Lib_Com_ABS1RxType *snapshot);

/**
 * @brief Gets the current ABM2 RX snapshot.
 * @param snapshot Destination for message data and reception state.
 * @return eLSD_Lib_StatusType Operation result.
 * @details Call from the customer 5 ms callback after initialization.
 */
eLSD_Lib_StatusType eLSD_Lib_Com_GetRxABM2(eLSD_Lib_Com_ABM2RxType *snapshot);

/**
 * @brief Gets the current ABS2 RX snapshot.
 * @param snapshot Destination for message data and reception state.
 * @return eLSD_Lib_StatusType Operation result.
 * @details Call from the customer 5 ms callback after initialization.
 */
eLSD_Lib_StatusType eLSD_Lib_Com_GetRxABS2(eLSD_Lib_Com_ABS2RxType *snapshot);

/**
 * @brief Gets the current TOD1 RX snapshot.
 * @param snapshot Destination for message data and reception state.
 * @return eLSD_Lib_StatusType Operation result.
 * @details Call from the customer 5 ms callback after initialization.
 */
eLSD_Lib_StatusType eLSD_Lib_Com_GetRxTOD1(eLSD_Lib_Com_TOD1RxType *snapshot);

/**
 * @brief Gets the current ABS3 RX snapshot.
 * @param snapshot Destination for message data and reception state.
 * @return eLSD_Lib_StatusType Operation result.
 * @details Call from the customer 5 ms callback after initialization.
 */
eLSD_Lib_StatusType eLSD_Lib_Com_GetRxABS3(eLSD_Lib_Com_ABS3RxType *snapshot);

/**
 * @brief Gets the current ECM2 RX snapshot.
 * @param snapshot Destination for message data and reception state.
 * @return eLSD_Lib_StatusType Operation result.
 * @details Call from the customer 5 ms callback after initialization.
 */
eLSD_Lib_StatusType eLSD_Lib_Com_GetRxECM2(eLSD_Lib_Com_ECM2RxType *snapshot);

/**
 * @brief Gets the current IP2 RX snapshot.
 * @param snapshot Destination for message data and reception state.
 * @return eLSD_Lib_StatusType Operation result.
 * @details Call from the customer 5 ms callback after initialization.
 */
eLSD_Lib_StatusType eLSD_Lib_Com_GetRxIP2(eLSD_Lib_Com_IP2RxType *snapshot);

/**
 * @brief Gets the current BCM6 RX snapshot.
 * @param snapshot Destination for message data and reception state.
 * @return eLSD_Lib_StatusType Operation result.
 * @details Call from the customer 5 ms callback after initialization.
 */
eLSD_Lib_StatusType eLSD_Lib_Com_GetRxBCM6(eLSD_Lib_Com_BCM6RxType *snapshot);

/**
 * @brief Gets the current PEPS2 RX snapshot.
 * @param snapshot Destination for message data and reception state.
 * @return eLSD_Lib_StatusType Operation result.
 * @details Call from the customer 5 ms callback after initialization.
 */
eLSD_Lib_StatusType eLSD_Lib_Com_GetRxPEPS2(eLSD_Lib_Com_PEPS2RxType *snapshot);

/**
 * @brief Gets the current BCM1 RX snapshot.
 * @param snapshot Destination for message data and reception state.
 * @return eLSD_Lib_StatusType Operation result.
 * @details Call from the customer 5 ms callback after initialization.
 */
eLSD_Lib_StatusType eLSD_Lib_Com_GetRxBCM1(eLSD_Lib_Com_BCM1RxType *snapshot);

/**
 * @brief Gets the current BCM3 RX snapshot.
 * @param snapshot Destination for message data and reception state.
 * @return eLSD_Lib_StatusType Operation result.
 * @details Call from the customer 5 ms callback after initialization.
 */
eLSD_Lib_StatusType eLSD_Lib_Com_GetRxBCM3(eLSD_Lib_Com_BCM3RxType *snapshot);

/**
 * @brief Gets the current HUT34 RX snapshot.
 * @param snapshot Destination for message data and reception state.
 * @return eLSD_Lib_StatusType Operation result.
 * @details Call from the customer 5 ms callback after initialization.
 */
eLSD_Lib_StatusType eLSD_Lib_Com_GetRxHUT34(eLSD_Lib_Com_HUT34RxType *snapshot);

/**
 * @brief Gets the current ECM3 RX snapshot.
 * @param snapshot Destination for message data and reception state.
 * @return eLSD_Lib_StatusType Operation result.
 * @details Call from the customer 5 ms callback after initialization.
 */
eLSD_Lib_StatusType eLSD_Lib_Com_GetRxECM3(eLSD_Lib_Com_ECM3RxType *snapshot);

/**
 * @brief Gets the current AC2 RX snapshot.
 * @param snapshot Destination for message data and reception state.
 * @return eLSD_Lib_StatusType Operation result.
 * @details Call from the customer 5 ms callback after initialization.
 */
eLSD_Lib_StatusType eLSD_Lib_Com_GetRxAC2(eLSD_Lib_Com_AC2RxType *snapshot);

/**
 * @brief Gets the current ELD1 TX snapshot.
 * @param data Destination for the message data.
 * @return eLSD_Lib_StatusType Operation result.
 * @details The operation returns NOT_SUPPORTED when the message does not apply
 * to this library role.
 */
eLSD_Lib_StatusType eLSD_Lib_Com_GetTxELD1(eLSD_Lib_Com_ELD1TxDataType *data);

/**
 * @brief Gets the current TOD1 TX snapshot.
 * @param data Destination for the message data.
 * @return eLSD_Lib_StatusType Operation result.
 * @details The operation returns NOT_SUPPORTED when the message does not apply
 * to this library role.
 */
eLSD_Lib_StatusType eLSD_Lib_Com_GetTxTOD1(eLSD_Lib_Com_TOD1TxDataType *data);

/**
 * @brief Gets the current VTC_RL1 TX snapshot.
 * @param data Destination for the message data.
 * @return eLSD_Lib_StatusType Operation result.
 * @details The operation returns NOT_SUPPORTED when the message does not apply
 * to this library role.
 */
eLSD_Lib_StatusType eLSD_Lib_Com_GetTxVTC_RL1(eLSD_Lib_Com_VTC_RL1TxDataType *data);

/**
 * @brief Gets the current VTC_RR1 TX snapshot.
 * @param data Destination for the message data.
 * @return eLSD_Lib_StatusType Operation result.
 * @details The operation returns NOT_SUPPORTED when the message does not apply
 * to this library role.
 */
eLSD_Lib_StatusType eLSD_Lib_Com_GetTxVTC_RR1(eLSD_Lib_Com_VTC_RR1TxDataType *data);

/**
 * @brief Sets the complete ELD1 TX snapshot.
 * @param data New message data.
 * @return eLSD_Lib_StatusType Operation result.
 * @details This function does not trigger immediate transmission.
 * The operation returns NOT_SUPPORTED when the message does not apply to this
 * library role.
 */
eLSD_Lib_StatusType eLSD_Lib_Com_SetTxELD1(const eLSD_Lib_Com_ELD1TxDataType *data);

/**
 * @brief Sets the complete TOD1 TX snapshot.
 * @param data New message data.
 * @return eLSD_Lib_StatusType Operation result.
 * @details This function does not trigger immediate transmission. The operation
 * returns NOT_SUPPORTED when the message does not apply to this library role.
 */
eLSD_Lib_StatusType eLSD_Lib_Com_SetTxTOD1(const eLSD_Lib_Com_TOD1TxDataType *data);

/**
 * @brief Sets the complete VTC_RL1 TX snapshot.
 * @param data New message data.
 * @return eLSD_Lib_StatusType Operation result.
 * @details This function does not trigger immediate transmission. The operation
 * returns NOT_SUPPORTED when the message does not apply to this library role.
 */
eLSD_Lib_StatusType eLSD_Lib_Com_SetTxVTC_RL1(const eLSD_Lib_Com_VTC_RL1TxDataType *data);

/**
 * @brief Sets the complete VTC_RR1 TX snapshot.
 * @param data New message data.
 * @return eLSD_Lib_StatusType Operation result.
 * @details This function does not trigger immediate transmission. The operation
 * returns NOT_SUPPORTED when the message does not apply to this library role.
 */
eLSD_Lib_StatusType eLSD_Lib_Com_SetTxVTC_RR1(const eLSD_Lib_Com_VTC_RR1TxDataType *data);

/**
 * @brief Enables motor control.
 * @return eLSD_Lib_StatusType Operation result.
 */
eLSD_Lib_StatusType eLSD_Lib_mcIf_setMotCtrlEnable(void);

/**
 * @brief Disables motor control.
 * @return eLSD_Lib_StatusType Operation result.
 */
eLSD_Lib_StatusType eLSD_Lib_mcIf_setMotCtrlDisable(void);

/**
 * @brief Clears motor-control faults.
 * @return eLSD_Lib_StatusType Operation result.
 */
eLSD_Lib_StatusType eLSD_Lib_mcIf_setClearFaults(void);

/**
 * @brief Starts position-sensor calibration.
 * @return eLSD_Lib_StatusType Operation result.
 */
eLSD_Lib_StatusType eLSD_Lib_mcIf_setPosSensCalibStart(void);

/**
 * @brief Sets the motor-control mode.
 * @param mode eLSD_Lib_mcIf_mCtrlModeType motor-control mode.
 * @return eLSD_Lib_StatusType Operation result.
 */
eLSD_Lib_StatusType eLSD_Lib_mcIf_setMotCtrlMode(eLSD_Lib_mcIf_mCtrlModeType mode);

/**
 * @brief Gets the motor-control mode.
 * @return eLSD_Lib_mcIf_mCtrlModeType Current motor-control mode.
 */
eLSD_Lib_mcIf_mCtrlModeType eLSD_Lib_mcIf_getMotCtrlMode(void);

/**
 * @brief Sets the commanded motor position.
 * @param pos eLSD_Lib_RadType Commanded motor position.
 */
void eLSD_Lib_mcIf_setCmdMotPos(eLSD_Lib_RadType pos);

/**
 * @brief Gets the commanded motor position.
 * @return eLSD_Lib_RadType Commanded motor position.
 */
eLSD_Lib_RadType eLSD_Lib_mcIf_getCmdMotPos(void);

/**
 * @brief Sets the commanded motor speed.
 * @param speed eLSD_Lib_RadPerSecType Commanded motor speed.
 */
void eLSD_Lib_mcIf_setCmdMotSpeed(eLSD_Lib_RadPerSecType speed);

/**
 * @brief Gets the commanded motor speed.
 * @return eLSD_Lib_RadPerSecType Commanded motor speed.
 */
eLSD_Lib_RadPerSecType eLSD_Lib_mcIf_getCmdMotSpeed(void);

/**
 * @brief Sets the requested speed ramp.
 * @param speedRamp eLSD_Lib_RadPerSecSquaredType Requested speed ramp.
 */
void eLSD_Lib_mcIf_setReqSpeedRamp(eLSD_Lib_RadPerSecSquaredType speedRamp);

/**
 * @brief Gets the commanded motor speed ramp.
 * @return eLSD_Lib_RadPerSecSquaredType Commanded motor speed ramp.
 */
eLSD_Lib_RadPerSecSquaredType eLSD_Lib_mcIf_getCmdMotSpeedRamp(void);

/**
 * @brief Sets the commanded motor torque.
 * @param torque eLSD_Lib_NmType Commanded motor torque.
 */
void eLSD_Lib_mcIf_setCmdMotTq(eLSD_Lib_NmType torque);

/**
 * @brief Gets the commanded motor torque.
 * @return eLSD_Lib_NmType Commanded motor torque.
 */
eLSD_Lib_NmType eLSD_Lib_mcIf_getCmdMotTq(void);

/**
 * @brief Sets the requested motor torque limit.
 * @param torqueLimit eLSD_Lib_NmType Requested motor torque limit.
 */
void eLSD_Lib_mcIf_setReqMotTorqLimit(eLSD_Lib_NmType torqueLimit);

/**
 * @brief Gets the requested motor torque limit.
 * @return eLSD_Lib_NmType Requested motor torque limit.
 */
eLSD_Lib_NmType eLSD_Lib_mcIf_getReqMotTorqLimit(void);

/**
 * @brief Gets the filtered motor angular speed feedback.
 * @return eLSD_Lib_RadPerSecType Filtered motor angular speed feedback.
 */
eLSD_Lib_RadPerSecType eLSD_Lib_mcIf_getOmegaMeFiltFbck(void);

/**
 * @brief Gets the absolute motor position feedback.
 * @return eLSD_Lib_RadType Absolute motor position feedback.
 */
eLSD_Lib_RadType eLSD_Lib_mcIf_getPosMeAbsFbck(void);

/**
 * @brief Gets the motor torque feedback.
 * @return eLSD_Lib_NmType Motor torque feedback.
 */
eLSD_Lib_NmType eLSD_Lib_mcIf_getTorqFbck(void);

#ifdef __cplusplus
}
#endif

#endif
