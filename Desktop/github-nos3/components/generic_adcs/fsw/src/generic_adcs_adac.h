/*******************************************************************************
** Purpose:
**   This file is the header file for the attitude determination and
**   attitude control routines of the Generic ADCS application.
**
*******************************************************************************/
#ifndef _GENERIC_ADCS_ADAC_H_
#define _GENERIC_ADCS_ADAC_H_

#define PASSIVE_MODE 0
#define BDOT_MODE    1
#define SUNSAFE_MODE 2
#define NORMAL_MODE  3

void Generic_ADCS_init_attitude_determination_and_attitude_control(FILE *in, Generic_ADCS_AD_Tlm_Payload_t *AD, Generic_ADCS_GNC_Tlm_Payload_t *GNC, Generic_ADCS_AC_Tlm_Payload_t *ACS);
void Generic_ADCS_execute_attitude_determination_and_attitude_control(const Generic_ADCS_DI_Tlm_Payload_t *DI, Generic_ADCS_AD_Tlm_Payload_t *AD, Generic_ADCS_GNC_Tlm_Payload_t *GNC, Generic_ADCS_AC_Tlm_Payload_t *ACS);

#endif
