/**
 * @file eLSD_Sfun_FlatApi.h
 * @brief Flat scalar/vector wrappers around eLSD_Lib for Simulink S-Functions.
 * @details Legacy Code Tool and Level-2 C S-Functions cannot pass nested
 * snapshot structs as ports. These wrappers call the real BSW Get/Set and
 * motor APIs and expose the ASW interface as float arrays that match the
 * Scheme One bus element order in build_VTC_TorqueVectoring_R2020b.m.
 */

#ifndef ELSD_SFUN_FLAT_API_H_
#define ELSD_SFUN_FLAT_API_H_

#ifdef __cplusplus
extern "C" {
#endif

/** Number of ASW RX flat elements (matches RxBus). */
#define ELSD_SFUN_RX_WIDTH 40

/** Number of ASW TX flat elements (matches TxBus). */
#define ELSD_SFUN_TX_WIDTH 16

/** Number of ASW motor-command flat elements (matches MotorCmdBus). */
#define ELSD_SFUN_MOT_WIDTH 4

/**
 * @brief Pull the ASW-relevant RX snapshots from eLSD_Lib into a flat vector.
 * @param u_sync Ordering input. Ignored by the function body. In MIL, wire the
 *               stub-inject sync here so Inject runs before Get in the same step.
 *               On target, wire a Constant 1.
 * @param y Length ELSD_SFUN_RX_WIDTH. Units match the header (km/h, deg, rad/s,
 *          Nm, percent, enum codes as floats). Expired flags are 0 or 1.
 */
void eLSD_Sfun_GetAswRx(float u_sync, float y[ELSD_SFUN_RX_WIDTH]);

/**
 * @brief Push ASW TX and motor commands into eLSD_Lib Set and mcIf setters.
 * @param u_tx Length ELSD_SFUN_TX_WIDTH.
 * @param u_mot Length ELSD_SFUN_MOT_WIDTH. MotCtrlEnable > 0.5 enables control.
 *              MotCtrlMode is cast to eLSD_Lib_mcIf_mCtrlModeType.
 */
void eLSD_Sfun_SetAswTxMot(const float u_tx[ELSD_SFUN_TX_WIDTH],
                           const float u_mot[ELSD_SFUN_MOT_WIDTH]);

/**
 * @brief Host-only S-Function entry: inject flat RX into the stub, then pulse sync.
 * @details Available only when linked with eLSD_Lib_stub.c. On target, omit this
 * S-Function; the real BSW fills RX from CAN.
 */
void eLSD_Sfun_StubInjectRx(const float u_rx[ELSD_SFUN_RX_WIDTH], float y_sync[1]);

/**
 * @brief Host-only: write flat RX into the stub message store before GetAswRx.
 */
void eLSD_Stub_InjectAswRx(const float u_rx[ELSD_SFUN_RX_WIDTH]);

/**
 * @brief Host-only: read back the last TX snapshot written by SetAswTxMot.
 */
void eLSD_Stub_ReadAswTx(float y_tx[ELSD_SFUN_TX_WIDTH]);

/**
 * @brief Host-only: read back the last motor commands written by SetAswTxMot.
 */
void eLSD_Stub_ReadMotorCmd(float y_mot[ELSD_SFUN_MOT_WIDTH]);

/**
 * @brief Host-only S-Function entry: expose last TX and motor commands.
 */
void eLSD_Sfun_StubReadTxMot(float y_tx[ELSD_SFUN_TX_WIDTH],
                             float y_mot[ELSD_SFUN_MOT_WIDTH]);

#ifdef __cplusplus
}
#endif

#endif /* ELSD_SFUN_FLAT_API_H_ */
