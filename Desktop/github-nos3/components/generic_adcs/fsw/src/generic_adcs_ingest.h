/*******************************************************************************
** Purpose:
**   This file has the functions to ingest messages from the sensor applications.
**
*******************************************************************************/
#ifndef _GENERIC_ADCS_INGEST_H_
#define _GENERIC_ADCS_INGEST_H_

#include "cfe.h"
#include "generic_adcs_msg.h"

void Generic_ADCS_ingest_init(FILE *in, Generic_ADCS_DI_Tlm_Payload_t *DI);
void Generic_ADCS_ingest_generic_mag(CFE_MSG_Message_t * Msg, Generic_ADCS_DI_Mag_Tlm_Payload_t *Mag);
void Generic_ADCS_ingest_generic_fss(CFE_MSG_Message_t * Msg, Generic_ADCS_DI_Fss_Tlm_Payload_t *Fss);
void Generic_ADCS_ingest_generic_css(CFE_MSG_Message_t * Msg, Generic_ADCS_DI_Css_Tlm_Payload_t *Css);
void Generic_ADCS_ingest_generic_imu(CFE_MSG_Message_t * Msg, Generic_ADCS_DI_Imu_Tlm_Payload_t *Imu);
void Generic_ADCS_ingest_generic_rw(CFE_MSG_Message_t * Msg, Generic_ADCS_DI_Rw_Tlm_Payload_t *Rw);

void Generic_ADCS_ingest_generic_Gps(CFE_MSG_Message_t * Msg, Generic_ADCS_DI_Gps_Tlm_Payload_t *Gps); //IRT CSS
void Generic_ADCS_ingest_generic_Star_Tracker(CFE_MSG_Message_t * Msg, Generic_ADCS_DI_Star_Tracker_Tlm_Payload_t (*Star_Tracker)[]); //IRT CSS
#endif
