/**
 * @file eLSD_Lib_stub.c
 * @brief Desktop stub of eLSD_Lib for MEX / MIL simulation of Scheme Two.
 * @details Stores the ASW-relevant RX snapshots and the last TX / motor
 * commands. It does not implement CAN timing. Replace this file with the
 * vendor library when building for the target.
 */

#include "eLSD_Lib.h"
#include "eLSD_Sfun_FlatApi.h"
#include "eLSD_Lib_stub_host.h"

#include <string.h>

static eLSD_Lib_Task5msCallbackType g_cb5ms = 0;

static eLSD_Lib_Com_ECM2RxType    g_ecm2;
static eLSD_Lib_Com_CSA2RxType    g_csa2;
static eLSD_Lib_Com_ESP2RxType    g_esp2;
static eLSD_Lib_Com_ESP1RxType    g_esp1;
static eLSD_Lib_Com_ABS3RxType    g_abs3;
static eLSD_Lib_Com_ABM2RxType    g_abm2;
static eLSD_Lib_Com_ABS1RxType    g_abs1;
static eLSD_Lib_Com_ABS2RxType    g_abs2;
static eLSD_Lib_Com_BCM6RxType    g_bcm6;
static eLSD_Lib_Com_VTC_RR1RxType g_rr1;

static eLSD_Lib_Com_VTC_RL1TxDataType g_tx_rl1;
static eLSD_Lib_Com_TOD1TxDataType    g_tx_tod1;
static eLSD_Lib_Com_ELD1TxDataType    g_tx_eld1;
static eLSD_Lib_Com_VTC_RR1TxDataType g_tx_rr1;

static eLSD_Lib_NmType                g_cmd_tq;
static eLSD_Lib_NmType                g_cmd_lim;
static eLSD_Lib_mcIf_mCtrlModeType    g_mode = ELSD_LIB_MCIF_MCTRL_MODE_IDLE;
static int                            g_enable = 0;

static void markFresh(eLSD_Lib_Com_RxStateType *state, int expired)
{
    state->ageMs    = 0u;
    state->received = !expired;
    state->expired  = expired ? true : false;
}

void eLSD_Stub_Reset(void)
{
    memset(&g_ecm2, 0, sizeof(g_ecm2));
    memset(&g_csa2, 0, sizeof(g_csa2));
    memset(&g_esp2, 0, sizeof(g_esp2));
    memset(&g_esp1, 0, sizeof(g_esp1));
    memset(&g_abs3, 0, sizeof(g_abs3));
    memset(&g_abm2, 0, sizeof(g_abm2));
    memset(&g_abs1, 0, sizeof(g_abs1));
    memset(&g_abs2, 0, sizeof(g_abs2));
    memset(&g_bcm6, 0, sizeof(g_bcm6));
    memset(&g_rr1, 0, sizeof(g_rr1));
    memset(&g_tx_rl1, 0, sizeof(g_tx_rl1));
    memset(&g_tx_tod1, 0, sizeof(g_tx_tod1));
    memset(&g_tx_eld1, 0, sizeof(g_tx_eld1));
    memset(&g_tx_rr1, 0, sizeof(g_tx_rr1));
    memset(&g_cmd_tq, 0, sizeof(g_cmd_tq));
    memset(&g_cmd_lim, 0, sizeof(g_cmd_lim));
    g_mode   = ELSD_LIB_MCIF_MCTRL_MODE_IDLE;
    g_enable = 0;

    markFresh(&g_ecm2.state, 1);
    markFresh(&g_csa2.state, 1);
    markFresh(&g_esp2.state, 1);
    markFresh(&g_esp1.state, 1);
    markFresh(&g_abs3.state, 1);
    markFresh(&g_abm2.state, 1);
    markFresh(&g_abs1.state, 1);
    markFresh(&g_abs2.state, 1);
    markFresh(&g_bcm6.state, 1);
    markFresh(&g_rr1.state, 1);
}

void eLSD_Stub_InjectAswRx(const float u_rx[ELSD_SFUN_RX_WIDTH])
{
    int exp;

    g_ecm2.data.AccelPedalPosn       = u_rx[0];
    g_ecm2.data.AccelPedalPosnVldty  = (eLSD_Lib_Com_ValidityType)(int)(u_rx[1] + 0.5f);
    exp = (u_rx[2] > 0.5f);
    markFresh(&g_ecm2.state, exp);

    g_csa2.data.SteerWheelAng       = u_rx[3];
    g_csa2.data.SteerWheelAngSign   = (eLSD_Lib_Com_SteerSignType)(int)(u_rx[4] + 0.5f);
    g_csa2.data.SteerWheelAngStatus = (eLSD_Lib_Com_SignalStatusType)(int)(u_rx[5] + 0.5f);
    exp = (u_rx[6] > 0.5f);
    markFresh(&g_csa2.state, exp);

    g_esp2.data.DrivingModReq_ESP = (eLSD_Lib_Com_DrivingModeRequestType)(int)(u_rx[7] + 0.5f);
    g_esp2.data.ESP_DiagActv      = (eLSD_Lib_Com_ActivityType)(int)(u_rx[8] + 0.5f);
    g_esp2.data.TAB_Sts           = (eLSD_Lib_Com_TabStateType)(int)(u_rx[9] + 0.5f);
    exp = (u_rx[10] > 0.5f);
    markFresh(&g_esp2.state, exp);

    g_esp1.data.ClutOpenReq = (eLSD_Lib_Com_ClutchRequestType)(int)(u_rx[11] + 0.5f);
    exp = (u_rx[12] > 0.5f);
    markFresh(&g_esp1.state, exp);

    g_abs3.data.VehSpd       = u_rx[13];
    g_abs3.data.VehSpdVld    = (u_rx[14] > 0.5f);
    g_abs3.data.VehSpdStatus = (eLSD_Lib_Com_SignalStatusType)(int)(u_rx[15] + 0.5f);
    g_abs3.data.ABSActv      = (eLSD_Lib_Com_ActivityType)(int)(u_rx[16] + 0.5f);
    exp = (u_rx[17] > 0.5f);
    markFresh(&g_abs3.state, exp);

    g_abm2.data.VehYawRate       = u_rx[18];
    g_abm2.data.VehYawRateStatus = (eLSD_Lib_Com_SignalStatusType)(int)(u_rx[19] + 0.5f);
    g_abm2.data.VehDynYawRateVld = (u_rx[20] > 0.5f);
    exp = (u_rx[21] > 0.5f);
    markFresh(&g_abm2.state, exp);

    g_abs1.data.FLWheelSpd       = u_rx[22];
    g_abs1.data.FRWheelSpd       = u_rx[23];
    g_abs1.data.FLWheelSpdStatus = (eLSD_Lib_Com_SignalStatusType)(int)(u_rx[24] + 0.5f);
    g_abs1.data.FRWheelSpdStatus = (eLSD_Lib_Com_SignalStatusType)(int)(u_rx[25] + 0.5f);
    exp = (u_rx[26] > 0.5f);
    markFresh(&g_abs1.state, exp);

    g_abs2.data.RLWheelSpd       = u_rx[27];
    g_abs2.data.RRWheelSpd       = u_rx[28];
    g_abs2.data.RLWheelSpdStatus = (eLSD_Lib_Com_SignalStatusType)(int)(u_rx[29] + 0.5f);
    g_abs2.data.RRWheelSpdStatus = (eLSD_Lib_Com_SignalStatusType)(int)(u_rx[30] + 0.5f);
    exp = (u_rx[31] > 0.5f);
    markFresh(&g_abs2.state, exp);

    g_bcm6.data.RearELDLckSwtReq = (eLSD_Lib_Com_RequestType)(int)(u_rx[32] + 0.5f);
    exp = (u_rx[33] > 0.5f);
    markFresh(&g_bcm6.state, exp);

    g_rr1.data.VTCEstimatedTorque_R       = u_rx[34];
    g_rr1.data.VTCEstimatedTorque_RStatus = (eLSD_Lib_Com_SignalStatusType)(int)(u_rx[35] + 0.5f);
    g_rr1.data.VTCActuatorFailSts_R       = (eLSD_Lib_Com_RightActuatorType)(int)(u_rx[36] + 0.5f);
    g_rr1.data.VTCCouplingFaultSts_R      = (eLSD_Lib_Com_RightCouplingFaultType)(int)(u_rx[37] + 0.5f);
    g_rr1.data.VTCCouplingThermWarning_R  = (eLSD_Lib_Com_RightThermalType)(int)(u_rx[38] + 0.5f);
    exp = (u_rx[39] > 0.5f);
    markFresh(&g_rr1.state, exp);
}

void eLSD_Stub_ReadAswTx(float y_tx[ELSD_SFUN_TX_WIDTH])
{
    y_tx[0]  = g_tx_rl1.VTCClutchTrqReq_R;
    y_tx[1]  = (float)g_tx_rl1.VTCClutchTrqReq_RStatus;
    y_tx[2]  = (float)g_tx_rl1.VTCClutchOpenReq_R;
    y_tx[3]  = (float)g_tx_tod1.AWDOperMod;
    y_tx[4]  = g_tx_tod1.EstimatedTorque;
    y_tx[5]  = (float)g_tx_tod1.EstimatedTorqueStatus;
    y_tx[6]  = (float)g_tx_tod1.CouplingFaultSts;
    y_tx[7]  = (float)g_tx_tod1.CouplingThermWarning;
    y_tx[8]  = (float)g_tx_tod1.ActuatorFailSts;
    y_tx[9]  = (float)g_tx_tod1.ExternalReqIgnored;
    y_tx[10] = (float)g_tx_tod1.DiagLmpReq;
    y_tx[11] = (float)g_tx_eld1.RearELDLckSts;
    y_tx[12] = (float)g_tx_eld1.ELDFaultSts;
    y_tx[13] = (float)g_tx_eld1.ELDFault;
    y_tx[14] = (float)g_tx_eld1.RollingCounter_ELD1;
    y_tx[15] = (float)g_tx_eld1.Checksum_ELD1;
}

void eLSD_Stub_ReadMotorCmd(float y_mot[ELSD_SFUN_MOT_WIDTH])
{
    y_mot[0] = g_cmd_tq.value;
    y_mot[1] = g_cmd_lim.value;
    y_mot[2] = g_enable ? 1.0f : 0.0f;
    y_mot[3] = (float)g_mode;
}

void eLSD_Sfun_StubInjectRx(const float u_rx[ELSD_SFUN_RX_WIDTH], float y_sync[1])
{
    eLSD_Stub_InjectAswRx(u_rx);
    if (y_sync != 0)
    {
        y_sync[0] = 1.0f;
    }
}

void eLSD_Sfun_StubReadTxMot(float y_tx[ELSD_SFUN_TX_WIDTH],
                             float y_mot[ELSD_SFUN_MOT_WIDTH])
{
    eLSD_Stub_ReadAswTx(y_tx);
    eLSD_Stub_ReadMotorCmd(y_mot);
}

eLSD_Lib_StatusType eLSD_Lib_Init(void)
{
    eLSD_Stub_Reset();
    return ELSD_LIB_STATUS_OK;
}

eLSD_Lib_VtcRoleType eLSD_Lib_GetVtcRole(void)
{
    return ELSD_LIB_VTC_ROLE_LEFT;
}

void eLSD_Lib_SetTask5msCallback(eLSD_Lib_Task5msCallbackType callback)
{
    g_cb5ms = callback;
}

void eLSD_Lib_Run(void)
{
    /* Host stub never enters the infinite run loop. */
    if (g_cb5ms != 0)
    {
        g_cb5ms();
    }
}

#define STUB_GET_EMPTY(Name, Type)                                             \
    eLSD_Lib_StatusType eLSD_Lib_Com_GetRx##Name(Type *snapshot)               \
    {                                                                          \
        if (snapshot == 0)                                                     \
        {                                                                      \
            return ELSD_LIB_STATUS_ERROR;                                      \
        }                                                                      \
        memset(snapshot, 0, sizeof(*snapshot));                                \
        snapshot->state.expired  = true;                                       \
        snapshot->state.received = false;                                      \
        snapshot->state.ageMs    = 0u;                                         \
        return ELSD_LIB_STATUS_OK;                                             \
    }

STUB_GET_EMPTY(ECM4, eLSD_Lib_Com_ECM4RxType)
STUB_GET_EMPTY(ECM7, eLSD_Lib_Com_ECM7RxType)
STUB_GET_EMPTY(ECM1, eLSD_Lib_Com_ECM1RxType)
STUB_GET_EMPTY(VTC_RL1, eLSD_Lib_Com_VTC_RL1RxType)
STUB_GET_EMPTY(EPB1, eLSD_Lib_Com_EPB1RxType)
STUB_GET_EMPTY(DCT5, eLSD_Lib_Com_DCT5RxType)
STUB_GET_EMPTY(TOD1, eLSD_Lib_Com_TOD1RxType)
STUB_GET_EMPTY(IP2, eLSD_Lib_Com_IP2RxType)
STUB_GET_EMPTY(PEPS2, eLSD_Lib_Com_PEPS2RxType)
STUB_GET_EMPTY(BCM1, eLSD_Lib_Com_BCM1RxType)
STUB_GET_EMPTY(BCM3, eLSD_Lib_Com_BCM3RxType)
STUB_GET_EMPTY(HUT34, eLSD_Lib_Com_HUT34RxType)
STUB_GET_EMPTY(ECM3, eLSD_Lib_Com_ECM3RxType)
STUB_GET_EMPTY(AC2, eLSD_Lib_Com_AC2RxType)

eLSD_Lib_StatusType eLSD_Lib_Com_GetRxECM2(eLSD_Lib_Com_ECM2RxType *snapshot)
{
    if (snapshot == 0) { return ELSD_LIB_STATUS_ERROR; }
    *snapshot = g_ecm2;
    return ELSD_LIB_STATUS_OK;
}

eLSD_Lib_StatusType eLSD_Lib_Com_GetRxCSA2(eLSD_Lib_Com_CSA2RxType *snapshot)
{
    if (snapshot == 0) { return ELSD_LIB_STATUS_ERROR; }
    *snapshot = g_csa2;
    return ELSD_LIB_STATUS_OK;
}

eLSD_Lib_StatusType eLSD_Lib_Com_GetRxESP2(eLSD_Lib_Com_ESP2RxType *snapshot)
{
    if (snapshot == 0) { return ELSD_LIB_STATUS_ERROR; }
    *snapshot = g_esp2;
    return ELSD_LIB_STATUS_OK;
}

eLSD_Lib_StatusType eLSD_Lib_Com_GetRxESP1(eLSD_Lib_Com_ESP1RxType *snapshot)
{
    if (snapshot == 0) { return ELSD_LIB_STATUS_ERROR; }
    *snapshot = g_esp1;
    return ELSD_LIB_STATUS_OK;
}

eLSD_Lib_StatusType eLSD_Lib_Com_GetRxABS3(eLSD_Lib_Com_ABS3RxType *snapshot)
{
    if (snapshot == 0) { return ELSD_LIB_STATUS_ERROR; }
    *snapshot = g_abs3;
    return ELSD_LIB_STATUS_OK;
}

eLSD_Lib_StatusType eLSD_Lib_Com_GetRxABM2(eLSD_Lib_Com_ABM2RxType *snapshot)
{
    if (snapshot == 0) { return ELSD_LIB_STATUS_ERROR; }
    *snapshot = g_abm2;
    return ELSD_LIB_STATUS_OK;
}

eLSD_Lib_StatusType eLSD_Lib_Com_GetRxABS1(eLSD_Lib_Com_ABS1RxType *snapshot)
{
    if (snapshot == 0) { return ELSD_LIB_STATUS_ERROR; }
    *snapshot = g_abs1;
    return ELSD_LIB_STATUS_OK;
}

eLSD_Lib_StatusType eLSD_Lib_Com_GetRxABS2(eLSD_Lib_Com_ABS2RxType *snapshot)
{
    if (snapshot == 0) { return ELSD_LIB_STATUS_ERROR; }
    *snapshot = g_abs2;
    return ELSD_LIB_STATUS_OK;
}

eLSD_Lib_StatusType eLSD_Lib_Com_GetRxBCM6(eLSD_Lib_Com_BCM6RxType *snapshot)
{
    if (snapshot == 0) { return ELSD_LIB_STATUS_ERROR; }
    *snapshot = g_bcm6;
    return ELSD_LIB_STATUS_OK;
}

eLSD_Lib_StatusType eLSD_Lib_Com_GetRxVTC_RR1(eLSD_Lib_Com_VTC_RR1RxType *snapshot)
{
    if (snapshot == 0) { return ELSD_LIB_STATUS_ERROR; }
    *snapshot = g_rr1;
    return ELSD_LIB_STATUS_OK;
}

eLSD_Lib_StatusType eLSD_Lib_Com_GetTxELD1(eLSD_Lib_Com_ELD1TxDataType *data)
{
    if (data == 0) { return ELSD_LIB_STATUS_ERROR; }
    *data = g_tx_eld1;
    return ELSD_LIB_STATUS_OK;
}

eLSD_Lib_StatusType eLSD_Lib_Com_GetTxTOD1(eLSD_Lib_Com_TOD1TxDataType *data)
{
    if (data == 0) { return ELSD_LIB_STATUS_ERROR; }
    *data = g_tx_tod1;
    return ELSD_LIB_STATUS_OK;
}

eLSD_Lib_StatusType eLSD_Lib_Com_GetTxVTC_RL1(eLSD_Lib_Com_VTC_RL1TxDataType *data)
{
    if (data == 0) { return ELSD_LIB_STATUS_ERROR; }
    *data = g_tx_rl1;
    return ELSD_LIB_STATUS_OK;
}

eLSD_Lib_StatusType eLSD_Lib_Com_GetTxVTC_RR1(eLSD_Lib_Com_VTC_RR1TxDataType *data)
{
    if (data == 0) { return ELSD_LIB_STATUS_ERROR; }
    *data = g_tx_rr1;
    return ELSD_LIB_STATUS_OK;
}

eLSD_Lib_StatusType eLSD_Lib_Com_SetTxELD1(const eLSD_Lib_Com_ELD1TxDataType *data)
{
    if (data == 0) { return ELSD_LIB_STATUS_ERROR; }
    g_tx_eld1 = *data;
    return ELSD_LIB_STATUS_OK;
}

eLSD_Lib_StatusType eLSD_Lib_Com_SetTxTOD1(const eLSD_Lib_Com_TOD1TxDataType *data)
{
    if (data == 0) { return ELSD_LIB_STATUS_ERROR; }
    g_tx_tod1 = *data;
    return ELSD_LIB_STATUS_OK;
}

eLSD_Lib_StatusType eLSD_Lib_Com_SetTxVTC_RL1(const eLSD_Lib_Com_VTC_RL1TxDataType *data)
{
    if (data == 0) { return ELSD_LIB_STATUS_ERROR; }
    g_tx_rl1 = *data;
    return ELSD_LIB_STATUS_OK;
}

eLSD_Lib_StatusType eLSD_Lib_Com_SetTxVTC_RR1(const eLSD_Lib_Com_VTC_RR1TxDataType *data)
{
    if (data == 0) { return ELSD_LIB_STATUS_ERROR; }
    g_tx_rr1 = *data;
    return ELSD_LIB_STATUS_OK;
}

eLSD_Lib_StatusType eLSD_Lib_mcIf_setMotCtrlEnable(void)
{
    g_enable = 1;
    return ELSD_LIB_STATUS_OK;
}

eLSD_Lib_StatusType eLSD_Lib_mcIf_setMotCtrlDisable(void)
{
    g_enable = 0;
    return ELSD_LIB_STATUS_OK;
}

eLSD_Lib_StatusType eLSD_Lib_mcIf_setClearFaults(void)
{
    return ELSD_LIB_STATUS_OK;
}

eLSD_Lib_StatusType eLSD_Lib_mcIf_setPosSensCalibStart(void)
{
    return ELSD_LIB_STATUS_OK;
}

eLSD_Lib_StatusType eLSD_Lib_mcIf_setMotCtrlMode(eLSD_Lib_mcIf_mCtrlModeType mode)
{
    g_mode = mode;
    return ELSD_LIB_STATUS_OK;
}

eLSD_Lib_mcIf_mCtrlModeType eLSD_Lib_mcIf_getMotCtrlMode(void)
{
    return g_mode;
}

void eLSD_Lib_mcIf_setCmdMotPos(eLSD_Lib_RadType pos)
{
    (void)pos;
}

eLSD_Lib_RadType eLSD_Lib_mcIf_getCmdMotPos(void)
{
    eLSD_Lib_RadType z;
    z.value = 0.0f;
    return z;
}

void eLSD_Lib_mcIf_setCmdMotSpeed(eLSD_Lib_RadPerSecType speed)
{
    (void)speed;
}

eLSD_Lib_RadPerSecType eLSD_Lib_mcIf_getCmdMotSpeed(void)
{
    eLSD_Lib_RadPerSecType z;
    z.value = 0.0f;
    return z;
}

void eLSD_Lib_mcIf_setReqSpeedRamp(eLSD_Lib_RadPerSecSquaredType speedRamp)
{
    (void)speedRamp;
}

eLSD_Lib_RadPerSecSquaredType eLSD_Lib_mcIf_getCmdMotSpeedRamp(void)
{
    eLSD_Lib_RadPerSecSquaredType z;
    z.value = 0.0f;
    return z;
}

void eLSD_Lib_mcIf_setCmdMotTq(eLSD_Lib_NmType torque)
{
    g_cmd_tq = torque;
}

eLSD_Lib_NmType eLSD_Lib_mcIf_getCmdMotTq(void)
{
    return g_cmd_tq;
}

void eLSD_Lib_mcIf_setReqMotTorqLimit(eLSD_Lib_NmType torqueLimit)
{
    g_cmd_lim = torqueLimit;
}

eLSD_Lib_NmType eLSD_Lib_mcIf_getReqMotTorqLimit(void)
{
    return g_cmd_lim;
}

eLSD_Lib_RadPerSecType eLSD_Lib_mcIf_getOmegaMeFiltFbck(void)
{
    eLSD_Lib_RadPerSecType z;
    z.value = 0.0f;
    return z;
}

eLSD_Lib_RadType eLSD_Lib_mcIf_getPosMeAbsFbck(void)
{
    eLSD_Lib_RadType z;
    z.value = 0.0f;
    return z;
}

eLSD_Lib_NmType eLSD_Lib_mcIf_getTorqFbck(void)
{
    eLSD_Lib_NmType z;
    z.value = 0.0f;
    return z;
}
