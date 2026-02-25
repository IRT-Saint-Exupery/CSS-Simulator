/*******************************************************************************
** Purpose:
**  Define GENERIC_ADCS application commands and telemetry messages
**
*******************************************************************************/
#ifndef _GENERIC_ADCS_MSG_H_
#define _GENERIC_ADCS_MSG_H_

#include "cfe.h"

/*
** Ground Command Codes
*/
#define GENERIC_ADCS_NOOP_CC                    0
#define GENERIC_ADCS_RESET_COUNTERS_CC          1
#define GENERIC_ADCS_SET_MODE_CC                2
#define GENERIC_ADCS_SEND_DI_CMD_CC             3
#define GENERIC_ADCS_SEND_AD_CMD_CC             4
#define GENERIC_ADCS_SEND_GNC_CMD_CC            5
#define GENERIC_ADCS_SEND_AC_CMD_CC             6
#define GENERIC_ADCS_SEND_DO_CMD_CC             7
#define GENERIC_ADCS_SET_MOMENTUM_MANAGEMENT_CC 8
#define GENERIC_ADCS_SET_TARGET_CC              9

/* 
** Telemetry Request Command Codes
*/
#define GENERIC_ADCS_REQ_HK_TLM              0

/*
** Generic "no arguments" command type definition
*/
typedef struct
{
    /* Every command requires a header used to identify it */
    CFE_MSG_CommandHeader_t CmdHeader;

} Generic_ADCS_NoArgs_cmd_t;

typedef struct
{
    /* Every command requires a header used to identify it */
    CFE_MSG_CommandHeader_t CmdHeader;
    uint8    Mode;
    char     MAC[CFE_CRITICAL_TC_MAC_LEN]; //IRT CSS
} Generic_ADCS_Mode_cmd_t;

typedef struct
{
    CFE_MSG_CommandHeader_t CmdHeader;
    uint8    MomentumManagement;
} Generic_ADCS_MomentumManagement_cmd_t;

typedef struct
{
    /* Every command requires a header used to identify it */
    CFE_MSG_CommandHeader_t CmdHeader;
    float latitude;
    float longitude;
} __attribute__((packed)) Generic_ADCS_SetTarget_cmd_t;

/*
** Generic_ADCS housekeeping type definition
*/
typedef struct 
{
    CFE_MSG_TelemetryHeader_t TlmHeader;
    uint8   CommandErrorCount;
    uint8   CommandCount;
} __attribute__((packed)) Generic_ADCS_Hk_tlm_t;
#define GENERIC_ADCS_HK_TLM_LNGTH sizeof ( Generic_ADCS_Hk_tlm_t )

/*
** Generic_ADCS DI type definition
*/
typedef struct
{
    double qbs[4]; // quaternion from sensor to body
    double bvb[3]; // magnetic field measurement by sensor in body frame
} __attribute__((packed)) Generic_ADCS_DI_Mag_Tlm_Payload_t;

typedef struct
{
    double qbs[4]; // quaternion from sensor to body
    uint8  valid;
    double svb[3]; // sun vector from sensor in body frame
} __attribute__((packed)) Generic_ADCS_DI_Fss_Tlm_Payload_t;

typedef struct
{
    double axis[3]; // CSS axis in body frame
    double scale;   // scale factor
    double percenton;
} __attribute__((packed)) Generic_ADCS_DI_Css_Sensor_Payload_t;

typedef struct
{
    Generic_ADCS_DI_Css_Sensor_Payload_t Sensor[6];
    uint8 valid;
    double svb[3]; // sun vector from sensors in body frame
} __attribute__((packed)) Generic_ADCS_DI_Css_Tlm_Payload_t;

typedef struct
{
    double qbs[4]; // quaternion from sensor to body
    double pos[3]; // position of sensor in body
    uint8  valid;
    double wbn[3]; // angular rate
    double acc[3]; // acceleration
} __attribute__((packed)) Generic_ADCS_DI_Imu_Tlm_Payload_t;

typedef struct
{
    double whl_axis[3][3];
    double H_maxB[3];
    double HwhlB[3];
} __attribute__((packed)) Generic_ADCS_DI_Rw_Tlm_Payload_t;

typedef struct
{
    double Weeks;
    double SecondsIntoWeek;
    double Fractions;
    double ECEFX;
    double ECEFY;
    double ECEFZ;
    double VelX;
    double VelY;
    double VelZ;
} __attribute__((packed)) Generic_ADCS_DI_Gps_Tlm_Payload_t; // IRT CSS

typedef struct
{
    double    Q0;
    double    Q1;
    double    Q2;
    double    Q3;
    uint8_t   IsValid;

} __attribute__((packed)) Generic_ADCS_DI_Star_Tracker_Tlm_Payload_t; //IRT CSS

typedef struct
{
    Generic_ADCS_DI_Mag_Tlm_Payload_t Mag;
    Generic_ADCS_DI_Fss_Tlm_Payload_t Fss;
    Generic_ADCS_DI_Css_Tlm_Payload_t Css;
    Generic_ADCS_DI_Imu_Tlm_Payload_t Imu;
    Generic_ADCS_DI_Rw_Tlm_Payload_t  Rw;
    Generic_ADCS_DI_Gps_Tlm_Payload_t  Gps; // IRT CSS
    Generic_ADCS_DI_Star_Tracker_Tlm_Payload_t Star_Tracker[2]; //IRT CSS
} __attribute__((packed)) Generic_ADCS_DI_Tlm_Payload_t;

typedef struct
{
    CFE_MSG_TelemetryHeader_t TlmHeader;
    Generic_ADCS_DI_Tlm_Payload_t Payload;
} __attribute__((packed)) Generic_ADCS_DI_Tlm_t;
#define GENERIC_ADCS_DI_LNGTH sizeof ( Generic_ADCS_DI_Tlm_t )

/*
** Generic_ADCS AD type definition
*/
typedef struct 
{
    double bvb[3];
} __attribute__((packed)) Generic_ADCS_AD_Mag_Tlm_Payload_t;

typedef struct
{
    uint8 SunValid;
    uint8 FssValid;
    double svb[3];
} __attribute__((packed)) Generic_ADCS_AD_Sol_Tlm_Payload_t;

typedef struct
{
    uint8  init;
    double alpha;
    uint8  valid;
    double wbn_prev[3];
    double wbn[3];
    double acc[3];
    double qbs[4];
} __attribute__((packed)) Generic_ADCS_AD_Imu_Tlm_Payload_t;

typedef struct
{
    Generic_ADCS_AD_Mag_Tlm_Payload_t Mag;
    Generic_ADCS_AD_Sol_Tlm_Payload_t Sol;
    Generic_ADCS_AD_Imu_Tlm_Payload_t Imu;
} __attribute__((packed)) Generic_ADCS_AD_Tlm_Payload_t;

typedef struct
{
    CFE_MSG_TelemetryHeader_t TlmHeader;
    Generic_ADCS_AD_Tlm_Payload_t Payload;
} __attribute__((packed)) Generic_ADCS_AD_Tlm_t;
#define GENERIC_ADCS_AD_LNGTH sizeof ( Generic_ADCS_AD_Tlm_t )

/*
** Generic_ADCS GNC type definition
*/
typedef struct {
    double Kb;
    double b_range;
    double loFrac;
    double hiFrac;
    uint8  mm_active[3];
    double Mcmd[3];
} __attribute__((packed)) Generic_ADCS_GNC_Hmgmt_t;

typedef struct {
    double DT;
    double MaxMcmd;
    uint8  Mode;
    uint8  HmgmtOn;
    Generic_ADCS_GNC_Hmgmt_t Hmgmt;
    double bvb[3];
    double svb[3];
    uint8  SunValid;
    double wbn[3];
    double HwhlMaxB[3];
    double HwhlB[3];
    double Mcmd[3];
    double Tcmd[3];
    double qbs[4];
    float  latitude;
    float  longitude;
} __attribute__((packed)) Generic_ADCS_GNC_Tlm_Payload_t;

typedef struct
{
    CFE_MSG_TelemetryHeader_t TlmHeader;
    Generic_ADCS_GNC_Tlm_Payload_t Payload;
} __attribute__((packed)) Generic_ADCS_GNC_Tlm_t;
#define GENERIC_ADCS_GNC_LNGTH sizeof ( Generic_ADCS_GNC_Tlm_t )

/*
** Generic_ADCS AC type definition
*/
typedef struct {
    double b_range;
    double Kb;
    double bold[3];
    double bdot[3];
} __attribute__((packed)) Generic_ADCS_AC_Bdot_Tlm_t;

typedef struct {
   /* Inputs*/
   double Kp[3];
   double Kr[3];
   double sside[3];
   double vmax;
   double cmd_wbn[3];
   uint8 h_mgmt;

   /* Internal Variables */
   double therr[3];
   double werr[3];
   double Tcmd[3];
   double err_t;
} __attribute__((packed)) Generic_ADCS_AC_Sunsafe_Tlm_t;

typedef struct {
   /* Inputs*/
   double Kp[3];
   double Kr[3];
   double sside[3];
   double vmax;
   double cmd_wbn[3];
   uint8 h_mgmt;

   /* Internal Variables */
   double therr[3];
   double werr[3];
   double Tcmd[3];
   double err_t;
} __attribute__((packed)) Generic_ADCS_AC_Normal_Tlm_t; //IRT CSS

typedef struct {
    Generic_ADCS_AC_Bdot_Tlm_t    Bdot;
    Generic_ADCS_AC_Sunsafe_Tlm_t Sunsafe;
    Generic_ADCS_AC_Normal_Tlm_t Normal; //IRT CSS
} __attribute__((packed)) Generic_ADCS_AC_Tlm_Payload_t;

typedef struct {
    CFE_MSG_TelemetryHeader_t TlmHeader;
    Generic_ADCS_AC_Tlm_Payload_t Payload;
} __attribute__((packed)) Generic_ADCS_AC_Tlm_t;
#define GENERIC_ADCS_AC_LNGTH sizeof ( Generic_ADCS_AC_Tlm_t )

/*
** Generic_ADCS DO type definition
*/
typedef struct
{
    double qba[4]; // quaternion from actuator to body
    double Mcmd[3];
} __attribute__((packed)) Generic_ADCS_DO_Trq_TlmPayload_t;

typedef struct
{
    double axis[3][3];
    double Tcmd[3];
} __attribute__((packed)) Generic_ADCS_DO_Rw_TlmPayload_t;

typedef struct
{
    Generic_ADCS_DO_Trq_TlmPayload_t Trq;
    Generic_ADCS_DO_Rw_TlmPayload_t Rw;
} __attribute__((packed)) Generic_ADCS_DO_Tlm_Payload_t;

typedef struct
{
    CFE_MSG_TelemetryHeader_t TlmHeader;
    Generic_ADCS_DO_Tlm_Payload_t Payload;
} __attribute__((packed)) Generic_ADCS_DO_Tlm_t;
#define GENERIC_ADCS_DO_LNGTH sizeof ( Generic_ADCS_DO_Tlm_t )


#endif
