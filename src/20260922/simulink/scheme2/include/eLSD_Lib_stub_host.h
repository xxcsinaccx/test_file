/**
 * @file eLSD_Lib_stub_host.h
 * @brief Host-simulation helpers for the eLSD_Lib stub.
 */

#ifndef ELSD_LIB_STUB_HOST_H_
#define ELSD_LIB_STUB_HOST_H_

#include "eLSD_Sfun_FlatApi.h"

#ifdef __cplusplus
extern "C" {
#endif

/** Reset all stub RX/TX/motor storage to zero / expired. */
void eLSD_Stub_Reset(void);

#ifdef __cplusplus
}
#endif

#endif /* ELSD_LIB_STUB_HOST_H_ */
