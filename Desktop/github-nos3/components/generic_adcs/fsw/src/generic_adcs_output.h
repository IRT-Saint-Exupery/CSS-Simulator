/*******************************************************************************
** Purpose:
**   This file has the functions to output messages to the actuator applications.
**
*******************************************************************************/
#ifndef _GENERIC_ADCS_OUTPUT_H_
#define _GENERIC_ADCS_OUTPUT_H_

#include "cfe.h"
#include "generic_adcs_msg.h"
#include "generic_torquer_msg.h"
#include "generic_reaction_wheel_msg.h"

void Generic_ADCS_output_init(FILE *in, Generic_ADCS_DO_Tlm_Payload_t *DO);
void Generic_ADCS_output_to_actuators(const Generic_ADCS_GNC_Tlm_Payload_t *GNC, Generic_ADCS_DO_Tlm_Payload_t *DO, GENERIC_TORQUER_All_Percent_On_cmd_t *MtbPctOnCmd, GENERIC_RW_Cmd_t *RwCmd);

#endif
