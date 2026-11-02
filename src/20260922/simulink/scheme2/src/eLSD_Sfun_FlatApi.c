/**
 * @file eLSD_Sfun_FlatApi.c
 * @brief Pack/unpack between flat ASW vectors and eLSD_Lib snapshot structs.
 */

#include "eLSD_Sfun_FlatApi.h"

#include "eLSD_Lib.h"

#include <string.h>

static float asFloat(float v)
{
    return v;
}

static int asInt(float v)
{
    if (v >= 0.0f)
    {
        return (int)(v + 0.5f);
    }
    return (int)(v - 0.5f);
}

static float expiredFlag(const eLSD_Lib_Com_RxStateType *state)
{
    return (state->expired) ? 1.0f : 0.0f;
}

void eLSD_Sfun_GetAswRx(float u_sync, float y[ELSD_SFUN_RX_WIDTH])
{
    eLSD_Lib_Com_ECM2RxType   ecm2;
    (void)u_sync;
    eLSD_Lib_Com_CSA2RxType   csa2;
    eLSD_Lib_Com_ESP2RxType   esp2;
    eLSD_Lib_Com_ESP1RxType   esp1;
    eLSD_Lib_Com_ABS3RxType   abs3;
    eLSD_Lib_Com_ABM2RxType   abm2;
    eLSD_Lib_Com_ABS1RxType   abs1;
    eLSD_Lib_Com_ABS2RxType   abs2;
    eLSD_Lib_Com_BCM6RxType   bcm6;
    eLSD_Lib_Com_VTC_RR1RxType rr1;

    memset(y, 0, sizeof(float) * (size_t)ELSD_SFUN_RX_WIDTH);

    (void)eLSD_Lib_Com_GetRxECM2(&ecm2);
    (void)eLSD_Lib_Com_GetRxCSA2(&csa2);
    (void)eLSD_Lib_Com_GetRxESP2(&esp2);
    (void)eLSD_Lib_Com_GetRxESP1(&esp1);
    (void)eLSD_Lib_Com_GetRxABS3(&abs3);
    (void)eLSD_Lib_Com_GetRxABM2(&abm2);
    (void)eLSD_Lib_Com_GetRxABS1(&abs1);
    (void)eLSD_Lib_Com_GetRxABS2(&abs2);
    (void)eLSD_Lib_Com_GetRxBCM6(&bcm6);
    (void)eLSD_Lib_Com_GetRxVTC_RR1(&rr1);

    y[0]  = asFloat(ecm2.data.AccelPedalPosn);
    y[1]  = asFloat((float)ecm2.data.AccelPedalPosnVldty);
    y[2]  = expiredFlag(&ecm2.state);

    y[3]  = asFloat(csa2.data.SteerWheelAng);
    y[4]  = asFloat((float)csa2.data.SteerWheelAngSign);
    y[5]  = asFloat((float)csa2.data.SteerWheelAngStatus);
    y[6]  = expiredFlag(&csa2.state);

    y[7]  = asFloat((float)esp2.data.DrivingModReq_ESP);
    y[8]  = asFloat((float)esp2.data.ESP_DiagActv);
    y[9]  = asFloat((float)esp2.data.TAB_Sts);
    y[10] = expiredFlag(&esp2.state);

    y[11] = asFloat((float)esp1.data.ClutOpenReq);
    y[12] = expiredFlag(&esp1.state);

    y[13] = asFloat(abs3.data.VehSpd);
    y[14] = abs3.data.VehSpdVld ? 1.0f : 0.0f;
    y[15] = asFloat((float)abs3.data.VehSpdStatus);
    y[16] = asFloat((float)abs3.data.ABSActv);
    y[17] = expiredFlag(&abs3.state);

    y[18] = asFloat(abm2.data.VehYawRate);
    y[19] = asFloat((float)abm2.data.VehYawRateStatus);
    y[20] = abm2.data.VehDynYawRateVld ? 1.0f : 0.0f;
    y[21] = expiredFlag(&abm2.state);

    y[22] = asFloat(abs1.data.FLWheelSpd);
    y[23] = asFloat(abs1.data.FRWheelSpd);
    y[24] = asFloat((float)abs1.data.FLWheelSpdStatus);
    y[25] = asFloat((float)abs1.data.FRWheelSpdStatus);
    y[26] = expiredFlag(&abs1.state);

    y[27] = asFloat(abs2.data.RLWheelSpd);
    y[28] = asFloat(abs2.data.RRWheelSpd);
    y[29] = asFloat((float)abs2.data.RLWheelSpdStatus);
    y[30] = asFloat((float)abs2.data.RRWheelSpdStatus);
    y[31] = expiredFlag(&abs2.state);

    y[32] = asFloat((float)bcm6.data.RearELDLckSwtReq);
    y[33] = expiredFlag(&bcm6.state);

    y[34] = asFloat(rr1.data.VTCEstimatedTorque_R);
    y[35] = asFloat((float)rr1.data.VTCEstimatedTorque_RStatus);
    y[36] = asFloat((float)rr1.data.VTCActuatorFailSts_R);
    y[37] = asFloat((float)rr1.data.VTCCouplingFaultSts_R);
    y[38] = asFloat((float)rr1.data.VTCCouplingThermWarning_R);
    y[39] = expiredFlag(&rr1.state);
}

void eLSD_Sfun_SetAswTxMot(const float u_tx[ELSD_SFUN_TX_WIDTH],
                           const float u_mot[ELSD_SFUN_MOT_WIDTH])
{
    eLSD_Lib_Com_VTC_RL1TxDataType rl1;
    eLSD_Lib_Com_TOD1TxDataType    tod1;
    eLSD_Lib_Com_ELD1TxDataType    eld1;
    eLSD_Lib_NmType                tq;
    eLSD_Lib_NmType                lim;

    memset(&rl1, 0, sizeof(rl1));
    memset(&tod1, 0, sizeof(tod1));
    memset(&eld1, 0, sizeof(eld1));

    rl1.VTCClutchTrqReq_R       = u_tx[0];
    rl1.VTCClutchTrqReq_RStatus = (eLSD_Lib_Com_SignalStatusType)asInt(u_tx[1]);
    rl1.VTCClutchOpenReq_R      = (eLSD_Lib_Com_RightClutchRequestType)asInt(u_tx[2]);

    tod1.AWDOperMod            = (eLSD_Lib_Com_AwdModeType)asInt(u_tx[3]);
    tod1.EstimatedTorque       = u_tx[4];
    tod1.EstimatedTorqueStatus = (eLSD_Lib_Com_SignalStatusType)asInt(u_tx[5]);
    tod1.CouplingFaultSts      = (eLSD_Lib_Com_CouplingFaultType)asInt(u_tx[6]);
    tod1.CouplingThermWarning  = (eLSD_Lib_Com_ThermalType)asInt(u_tx[7]);
    tod1.ActuatorFailSts       = (eLSD_Lib_Com_ActuatorType)asInt(u_tx[8]);
    tod1.ExternalReqIgnored    = (eLSD_Lib_Com_ExternalRequestType)asInt(u_tx[9]);
    tod1.DiagLmpReq            = (eLSD_Lib_Com_RequestType)asInt(u_tx[10]);

    eld1.RearELDLckSts       = (eLSD_Lib_Com_RearLockStateType)asInt(u_tx[11]);
    eld1.ELDFaultSts         = (eLSD_Lib_Com_EldFaultStateType)asInt(u_tx[12]);
    eld1.ELDFault            = (eLSD_Lib_Com_EldFaultCodeType)asInt(u_tx[13]);
    eld1.RollingCounter_ELD1 = (uint8_t)asInt(u_tx[14]);
    eld1.Checksum_ELD1       = (uint8_t)asInt(u_tx[15]);

    (void)eLSD_Lib_Com_SetTxVTC_RL1(&rl1);
    (void)eLSD_Lib_Com_SetTxTOD1(&tod1);
    (void)eLSD_Lib_Com_SetTxELD1(&eld1);

    tq.value  = u_mot[0];
    lim.value = u_mot[1];
    eLSD_Lib_mcIf_setCmdMotTq(tq);
    eLSD_Lib_mcIf_setReqMotTorqLimit(lim);

    if (u_mot[2] > 0.5f)
    {
        (void)eLSD_Lib_mcIf_setMotCtrlEnable();
    }
    else
    {
        (void)eLSD_Lib_mcIf_setMotCtrlDisable();
    }

    (void)eLSD_Lib_mcIf_setMotCtrlMode((eLSD_Lib_mcIf_mCtrlModeType)asInt(u_mot[3]));
}
