/*******************************************************************************
** Purpose:
**   This file implements the functions to output messages to the actuator applications.
**
*******************************************************************************/

#include "generic_adcs_app.h"
#include "generic_adcs_utilities.h"
#include "generic_adcs_output.h"

static void send_mtb_commands(double Mcmd[3], Generic_ADCS_DO_Trq_TlmPayload_t *DO, GENERIC_TORQUER_All_Percent_On_cmd_t *MtbPctOnCmd);
static void mcmd_to_percent_direction(double Mcmd, uint8 *percent, uint8 *direction);
static void send_rw_commands(double Tcmd[3], Generic_ADCS_DO_Rw_TlmPayload_t *DO, GENERIC_RW_Cmd_t *RwCmd);

void Generic_ADCS_output_init(FILE *in, Generic_ADCS_DO_Tlm_Payload_t *DO)
{
    char junk[512], newline;
    fscanf(in, "%[^\n]%[\n]", junk, &newline);
    fscanf(in, "%lf %lf %lf %lf%[^\n]%[\n]", &DO->Trq.qba[0], &DO->Trq.qba[1], &DO->Trq.qba[2], &DO->Trq.qba[3], junk, &newline);
    fscanf(in, "%[^\n]%[\n]", junk, &newline);
    fscanf(in, "%lf %lf %lf%[^\n]%[\n]", &DO->Rw.axis[0][0], &DO->Rw.axis[0][1], &DO->Rw.axis[0][2], junk, &newline);
    fscanf(in, "%lf %lf %lf%[^\n]%[\n]", &DO->Rw.axis[1][0], &DO->Rw.axis[1][1], &DO->Rw.axis[1][2], junk, &newline);
    fscanf(in, "%lf %lf %lf%[^\n]%[\n]", &DO->Rw.axis[2][0], &DO->Rw.axis[2][1], &DO->Rw.axis[2][2], junk, &newline);
}

void Generic_ADCS_output_to_actuators(const Generic_ADCS_GNC_Tlm_Payload_t *GNC, Generic_ADCS_DO_Tlm_Payload_t *DO, GENERIC_TORQUER_All_Percent_On_cmd_t *MtbPctOnCmd, GENERIC_RW_Cmd_t *RwCmd)
{
    send_mtb_commands(GNC->Mcmd, &DO->Trq, MtbPctOnCmd);
    send_rw_commands(GNC->Tcmd, &DO->Rw, RwCmd);
}

static struct{uint8 Direction; uint8 PercentOn;} CurrentMtb[] = {{1,0}, {1,0}, {1,0}}; // Keep current state so commands are only sent on change
static void send_mtb_commands(double Mcmd[3], Generic_ADCS_DO_Trq_TlmPayload_t *DO, GENERIC_TORQUER_All_Percent_On_cmd_t *MtbPctOnCmd)
{
    QTxV(DO->qba, Mcmd, DO->Mcmd);
    mcmd_to_percent_direction(DO->Mcmd[0], &MtbPctOnCmd->PercentOn_0, &MtbPctOnCmd->Direction_0);
    mcmd_to_percent_direction(DO->Mcmd[1], &MtbPctOnCmd->PercentOn_1, &MtbPctOnCmd->Direction_1);
    mcmd_to_percent_direction(DO->Mcmd[2], &MtbPctOnCmd->PercentOn_2, &MtbPctOnCmd->Direction_2);
    if ((MtbPctOnCmd->Direction_0 != CurrentMtb[0].Direction) || (MtbPctOnCmd->PercentOn_0 != CurrentMtb[0].PercentOn) || 
        (MtbPctOnCmd->Direction_1 != CurrentMtb[1].Direction) || (MtbPctOnCmd->PercentOn_1 != CurrentMtb[1].PercentOn) || 
        (MtbPctOnCmd->Direction_2 != CurrentMtb[2].Direction) || (MtbPctOnCmd->PercentOn_2 != CurrentMtb[2].PercentOn)) {
        CFE_SB_TimeStampMsg((CFE_MSG_Message_t *)MtbPctOnCmd);
        CFE_SB_TransmitMsg((CFE_MSG_Message_t *)MtbPctOnCmd, true);
        CurrentMtb[0].Direction = MtbPctOnCmd->Direction_0;
        CurrentMtb[0].PercentOn = MtbPctOnCmd->PercentOn_0;
        CurrentMtb[1].Direction = MtbPctOnCmd->Direction_1;
        CurrentMtb[1].PercentOn = MtbPctOnCmd->PercentOn_1;
        CurrentMtb[2].Direction = MtbPctOnCmd->Direction_2;
        CurrentMtb[2].PercentOn = MtbPctOnCmd->PercentOn_2;
    }
}

static void mcmd_to_percent_direction(double Mcmd, uint8 *percent, uint8 *direction)
{
    double pct = 100.0 * Mcmd / Generic_ADCS_AppData.GNCPacket.Payload.MaxMcmd;
    *direction = 1;
    if (pct < 0) {
        pct *= -1.0;
        *direction = 0;
    }
    if (pct > 100) pct = 100;
    *percent = pct;
}

static int16 CurrentRw[] = {0, 0, 0}; // Keep current state so commands are only sent on change
static void send_rw_commands(double Tcmd[3], Generic_ADCS_DO_Rw_TlmPayload_t *DO, GENERIC_RW_Cmd_t *RwCmd)
{
    int16 torque;
    for (uint8 i = 0; i < 3; i++) {
        DO->Tcmd[i] = Tcmd[i];
        torque = 10000.0 * VoV(Tcmd, DO->axis[i]); // cmd is in 10^-4 Nm
        if (torque != CurrentRw[i]) {
            RwCmd->data = torque;
            RwCmd->wheel_number = i;
            CFE_SB_TimeStampMsg((CFE_MSG_Message_t *)RwCmd);
            CFE_SB_TransmitMsg((CFE_MSG_Message_t *)RwCmd, true);
            CurrentRw[i] = torque;
        }
    }
}
