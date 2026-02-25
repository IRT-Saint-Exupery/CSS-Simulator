/*******************************************************************************
** Purpose:
**   This file contains the source code for the attitude determination and
**   attitude control routines of the Generic ADCS application.
**
*******************************************************************************/

#include <stdio.h>
#include <math.h>
#include "generic_adcs_app.h"
#include "generic_adcs_utilities.h"
#include "generic_adcs_events.h"
#include "generic_adcs_adac.h"
#include "novatel_oem615_app.h" //IRT CSS


static void AD_imu(const Generic_ADCS_DI_Imu_Tlm_Payload_t *DI_IMU, Generic_ADCS_AD_Imu_Tlm_Payload_t *AD_IMU);
static void AD_mag(const Generic_ADCS_DI_Mag_Tlm_Payload_t *DI_Mag, Generic_ADCS_AD_Mag_Tlm_Payload_t *AD_Mag);
static void AD_sol(const Generic_ADCS_DI_Fss_Tlm_Payload_t *DI_FSS, const Generic_ADCS_DI_Css_Tlm_Payload_t *DI_CSS, 
    Generic_ADCS_AD_Sol_Tlm_Payload_t *AD_Sol);
static void AD_to_GNC(const Generic_ADCS_AD_Tlm_Payload_t *AD, Generic_ADCS_GNC_Tlm_Payload_t *GNC);
static void AC_bdot(Generic_ADCS_GNC_Tlm_Payload_t *GNC, Generic_ADCS_AC_Bdot_Tlm_t *AC_bdot);
static void AC_sunsafe(Generic_ADCS_GNC_Tlm_Payload_t *GNC, Generic_ADCS_AC_Sunsafe_Tlm_t *ACS);
static void AC_normal(Generic_ADCS_GNC_Tlm_Payload_t *GNC, Generic_ADCS_AC_Normal_Tlm_t *ACS);
static void AC_h_mgmt(Generic_ADCS_GNC_Tlm_Payload_t *GNC);
static int counterCoord = 0;
static int counterMSE = 0;
static int counterCTRL = 0;
static int counterStarKO = 0;
static int counterGPSKO = 0;
static int counterGPSEst = 0;
static int counterSTEst = 0;
static double prev_werr[3] = {0,0,0};
static double integ_err[3] = {0,0,0};
static double werr[3] = {0,0,0};
static double prev_therr[3] = {0,0,0};
static double Sat_GPS_PosPrev[3] = {0,0,0};
static double Sat_GPS_PosEst[3] = {0,0,0};
static double Sat_Sun_PosPrev[3] = {1,0,0};
static double Sat_traj[3] = {1,1,1};
static double Sat_vel[3] = {0,0,0};
static double Tcmd_traj[3] = {0,0,0};
static double q_prev[4] = {1,0,0,0};
static double q_est[4] = {1,0,0,0};
static int GPSvalid = 0;
static int GPSEst = 0;
static int STEst = 0;

void Generic_ADCS_init_attitude_determination_and_attitude_control(FILE *in, Generic_ADCS_AD_Tlm_Payload_t *AD, 
    Generic_ADCS_GNC_Tlm_Payload_t *GNC, Generic_ADCS_AC_Tlm_Payload_t *ACS)
{
    char junk[512], newline;
    // AD
    fscanf(in, "%[^\n]%[\n]", junk, &newline);
    fscanf(in, "%lf%[^\n]%[\n]", &AD->Imu.alpha, junk, &newline);
    AD->Imu.init = 0;
    // GNC
    fscanf(in, "%[^\n]%[\n]", junk, &newline);
    fscanf(in, "%lf%[^\n]%[\n]", &GNC->DT, junk, &newline);
    fscanf(in, "%lf%[^\n]%[\n]", &GNC->MaxMcmd, junk, &newline);
    // AC Bdot
    fscanf(in, "%[^\n]%[\n]", junk, &newline);
    fscanf(in, "%lf %lf%[^\n]%[\n]", &ACS->Bdot.b_range, &ACS->Bdot.Kb, junk, &newline);
    // AC Sunsafe
    fscanf(in, "%[^\n]%[\n]", junk, &newline);
    fscanf(in, "%lf %lf %lf %lf %lf %lf%[^\n]%[\n]", &ACS->Sunsafe.Kp[0], &ACS->Sunsafe.Kp[1], &ACS->Sunsafe.Kp[2], 
        &ACS->Sunsafe.Kr[0], &ACS->Sunsafe.Kr[1], &ACS->Sunsafe.Kr[2], junk, &newline);
    fscanf(in, "%lf %lf %lf %lf %lf %lf %lf%[^\n]%[\n]", &ACS->Sunsafe.sside[0], &ACS->Sunsafe.sside[1], &ACS->Sunsafe.sside[2], &ACS->Sunsafe.vmax, 
        &ACS->Sunsafe.cmd_wbn[0], &ACS->Sunsafe.cmd_wbn[1], &ACS->Sunsafe.cmd_wbn[2], junk, &newline);
    for (int i = 0; i < 3; i++) {
        ACS->Sunsafe.therr[i] = ACS->Sunsafe.werr[i] = ACS->Sunsafe.Tcmd[i] = 0;
    }
    // AC normal
    fscanf(in, "%[^\n]%[\n]", junk, &newline);
    fscanf(in, "%lf %lf %lf %lf %lf %lf%[^\n]%[\n]", &ACS->Normal.Kp[0], &ACS->Normal.Kp[1], &ACS->Normal.Kp[2], 
        &ACS->Normal.Kr[0], &ACS->Normal.Kr[1], &ACS->Normal.Kr[2], junk, &newline);
    fscanf(in, "%lf %lf %lf %lf %lf %lf %lf%[^\n]%[\n]", &ACS->Normal.sside[0], &ACS->Normal.sside[1], &ACS->Normal.sside[2], &ACS->Normal.vmax, 
        &ACS->Normal.cmd_wbn[0], &ACS->Normal.cmd_wbn[1], &ACS->Normal.cmd_wbn[2], junk, &newline);
    for (int i = 0; i < 3; i++) {
        ACS->Normal.therr[i] = ACS->Normal.werr[i] = ACS->Normal.Tcmd[i] = 0;
    }
    // AC Momentum management
    fscanf(in, "%[^\n]%[\n]", junk, &newline);
    fscanf(in, "%lf %lf %lf %lf%[^\n]%[\n]", &GNC->Hmgmt.Kb, &GNC->Hmgmt.b_range, &GNC->Hmgmt.loFrac, &GNC->Hmgmt.hiFrac, junk, &newline);
}

void Generic_ADCS_execute_attitude_determination_and_attitude_control(const Generic_ADCS_DI_Tlm_Payload_t *DI, Generic_ADCS_AD_Tlm_Payload_t *AD, 
    Generic_ADCS_GNC_Tlm_Payload_t *GNC, Generic_ADCS_AC_Tlm_Payload_t *ACS)
{
    AD_imu(&DI->Imu, &AD->Imu);
    AD_mag(&DI->Mag, &AD->Mag);
    AD_sol(&DI->Fss, &DI->Css, &AD->Sol);

    AD_to_GNC(AD, GNC);
    for (int i = 0; i < 3; i++) GNC->HwhlB[i] = DI->Rw.HwhlB[i];
    for (int i = 0; i < 3; i++) GNC->HwhlMaxB[i] = DI->Rw.H_maxB[i];

    switch(GNC->Mode) {
    case BDOT_MODE:
        AC_bdot(GNC, &ACS->Bdot);
        break;
    
    case SUNSAFE_MODE:
        AC_sunsafe(GNC, &ACS->Sunsafe);
        break;
    
    case NORMAL_MODE:                   // IRT CSS       
        AC_normal(GNC, &ACS->Normal);
        break;    
        
    case PASSIVE_MODE:
    default:
        for (int i = 0; i < 3; i++) {
            GNC->Mcmd[i] = 0.0;
            GNC->Tcmd[i] = 0.0;
        }
        break;
    }
}

static void AD_imu(const Generic_ADCS_DI_Imu_Tlm_Payload_t *DI_IMU, Generic_ADCS_AD_Imu_Tlm_Payload_t *AD_IMU)
{
    if (DI_IMU->valid) {
        AD_IMU->valid = 1;
        for (int i = 0; i < 3; i++) {
            AD_IMU->acc[i] = DI_IMU->acc[i];
        }

        if (AD_IMU->init == 0) {
            for (int i = 0; i < 3; i++) {
                AD_IMU->wbn[i] = DI_IMU->wbn[i];
            }
            AD_IMU->init = 1;
        } else {
            for (int i = 0; i < 3; i++) {
                AD_IMU->wbn[i] = AD_IMU->alpha * AD_IMU->wbn_prev[i] + (1 - AD_IMU->alpha) * DI_IMU->wbn[i];
            }
        }
        for (int i = 0; i < 3; i++) {
            AD_IMU->wbn_prev[i] = AD_IMU->wbn[i];
        }
        // IRT CSS
        for (int i = 0; i < 4; i++) {
            AD_IMU->qbs[i] = DI_IMU->qbs[i];
        }
    } else {
        AD_IMU->valid = 0;
    }
}

static void AD_mag(const Generic_ADCS_DI_Mag_Tlm_Payload_t *DI_Mag, Generic_ADCS_AD_Mag_Tlm_Payload_t *AD_Mag)
{
    /* AD very simple for magnetometer... there is only one mag and no fusion with anything else */
    for (int i = 0; i < 3; i++) {
        AD_Mag->bvb[i] = DI_Mag->bvb[i];
    }
}

static void AD_sol(const Generic_ADCS_DI_Fss_Tlm_Payload_t *DI_Fss, const Generic_ADCS_DI_Css_Tlm_Payload_t *DI_Css, Generic_ADCS_AD_Sol_Tlm_Payload_t *AD_Sol)
{
    if (DI_Fss->valid == 1) {
        AD_Sol->SunValid = 1;
        AD_Sol->FssValid = 1;
        AD_Sol->svb[0] = DI_Fss->svb[0];
        AD_Sol->svb[1] = DI_Fss->svb[1];
        AD_Sol->svb[2] = DI_Fss->svb[2];
    } else if (DI_Css->valid == 1) {
        AD_Sol->SunValid = 1;
        AD_Sol->FssValid = 0;
        AD_Sol->svb[0] = DI_Css->svb[0];
        AD_Sol->svb[1] = DI_Css->svb[1];
        AD_Sol->svb[2] = DI_Css->svb[2];
    } else {// IRT CSS
        AD_Sol->SunValid = 0;
        AD_Sol->FssValid = 0;
        //AD_Sol->svb[0] = 0.0;
        //AD_Sol->svb[1] = 0.0;
        //AD_Sol->svb[2] = 0.0;
    }
}

static void AD_to_GNC(const Generic_ADCS_AD_Tlm_Payload_t *AD, Generic_ADCS_GNC_Tlm_Payload_t *GNC)
{
    for (int i = 0; i < 3; i++) {
        GNC->bvb[i] = AD->Mag.bvb[i];
        GNC->svb[i] = AD->Sol.svb[i];
        GNC->wbn[i] = AD->Imu.wbn[i];
    }
    //IRT CSS
    for (int i = 0; i < 3; i++) {
        GNC->qbs[i] = AD->Imu.qbs[i];
    }
    GNC->SunValid = AD->Sol.SunValid;
}

static void AC_bdot(Generic_ADCS_GNC_Tlm_Payload_t *GNC, Generic_ADCS_AC_Bdot_Tlm_t *ACS)
{
    /* apply control only if b-field is in range */
    if (MAGV(GNC->bvb) > ACS->b_range) {
        for(int i = 0; i < 3; i++) {
            /* backward difference b-field derivative */
            ACS->bdot[i] = (GNC->bvb[i] - ACS->bold[i]) / GNC->DT;
            /* store old b-field */
            ACS->bold[i] = GNC->bvb[i];
            /* traditional b-dot algorithm */
            GNC->Mcmd[i] = -ACS->Kb * ACS->bdot[i] / MAGV(GNC->bvb);
            /* ensure wheels disabled */
            GNC->Tcmd[i] = 0.0;
        }
    } else {
        for (int i = 0; i < 3; i++) {
            GNC->Mcmd[i] = 0.0;
            GNC->Tcmd[i] = 0.0;
        }
    }
}

#define EPS 1.0E-6
static void AC_sunsafe(Generic_ADCS_GNC_Tlm_Payload_t *GNC, Generic_ADCS_AC_Sunsafe_Tlm_t *ACS)
{
   int i;
   double u1[3] = {0.0, 0.0, 0.0}, err_b[3] = {0.0, 0.0, 0.0};      /* angle error calculation parameteres */
   double temp_sside[3] = {0.0, 0.0, 0.0};
   double SoS = 0.0;

/* .. Check that SS Vector is valid */
   if (GNC->SunValid) {

/* .. Form attitude error signals */
      SoS = VoV(GNC->svb, ACS->sside);
      if ((SoS > (EPS - 1.0)) && (SoS < (1.0 - EPS))) {
         VxV(GNC->svb, ACS->sside, ACS->therr);
      }
      else if (SoS >= (1.0 - EPS)) {
         ACS->therr[0] = 0.0;
         ACS->therr[1] = 0.0;
         ACS->therr[2] = 0.0;
         }
      else {
         err_b[0] = ACS->sside[1];
         err_b[1] = ACS->sside[2];
         err_b[2] = ACS->sside[0];
         if (fabs(err_b[0] - err_b[1]) < EPS && fabs(err_b[0] - err_b[2]) < EPS) {
            err_b[0] = -err_b[0];
         }
         VxV(ACS->sside, err_b, temp_sside);
         VxV(GNC->svb, temp_sside, ACS->therr);
      }
      
/* .. Closed-loop attitude control - PD Method */
      for(i = 0; i < 3; i++) {
         /* Clip attitude slew rates */
         u1[i] = Limit(ACS->Kp[i] / ACS->Kr[i] * ACS->therr[i], -ACS->vmax,ACS->vmax);
         ACS->werr[i] = GNC->wbn[i] - ACS->cmd_wbn[i];
         ACS->Tcmd[i] = -ACS->Kr[i] * (u1[i] + ACS->werr[i]);
      }

/* .. Apply Torque Command */
      for(i = 0; i < 3; i++) {
         GNC->Tcmd[i] = -ACS->Tcmd[i];
      }
   }

   else { /* during eclipse, reduce attitude rates only */

      for(i = 0; i < 3; i++) {
         ACS->werr[i] = GNC->wbn[i];
         ACS->Tcmd[i] = -ACS->Kr[i]* ACS->werr[i];
      }
      /* .. Apply Torque Command  */
      for (i = 0; i < 3; i++) {
         GNC->Tcmd[i] = -ACS->Tcmd[i];
      }
   }

   if (GNC->HmgmtOn) {
      AC_h_mgmt(GNC);
      for(i = 0; i < 3; i++) {
        GNC->Mcmd[i] = GNC->Hmgmt.Mcmd[i];
      }
   }
   else {
      for(i = 0; i < 3; i++) {
         GNC->Mcmd[i] = 0.0;
      }
   }

}


static void AC_normal(Generic_ADCS_GNC_Tlm_Payload_t *GNC, Generic_ADCS_AC_Normal_Tlm_t *ACS)   // IRT CSS 
{

   // this mode is for pointing a target (lat,long) on Earth
   // please note that this mode is just a prototype ! // use at your own risk! //TODO
   
   int i;
   double u1[3] = {0.0, 0.0, 0.0}, err_b[3] = {0.0, 0.0, 0.0};      /* angle error calculation parameteres */
   double temp_sside[3] = {0.0, 0.0, 0.0};
   double SoS = 0.0;
   double target_vectorE[3] = {0.0,0.0,0.0}; 
   double MSE = 0;
   double dt = GNC->DT; 
   // printf(" ***** GNC->svb = %lf, %lf, %lf. \n", GNC->svb[1], GNC->svb[2], GNC->svb[0]); // 0.086756, -0.095483, 0.991624. // example of svb
   
   
   // get satellite position from GPS (from NASA 42)
   double XE = Generic_ADCS_AppData.DIPacket.Payload.Gps.ECEFX; //NB: GPS data from 42 is freezing sometimes // to be investigated //TODO 
   double YE = Generic_ADCS_AppData.DIPacket.Payload.Gps.ECEFY;
   double ZE = Generic_ADCS_AppData.DIPacket.Payload.Gps.ECEFZ;
   double Sat_GPS_Pos[3] = {XE, YE, ZE};
  
   Sat_traj[0] = Sat_GPS_Pos[0]-Sat_GPS_PosPrev[0];
   Sat_traj[1] = Sat_GPS_Pos[1]-Sat_GPS_PosPrev[1];
   Sat_traj[2] = Sat_GPS_Pos[2]-Sat_GPS_PosPrev[2];
   Sat_vel[0] = Sat_traj[0]/dt;
   Sat_vel[1] = Sat_traj[1]/dt;
   Sat_vel[2] = Sat_traj[2]/dt;
   
   // check gps not zero, not nan, not fixed
   if ( (!isnan(XE)) && (!isnan(YE)) && (!isnan(ZE)) ) {
   	if ( ( fabs(XE-Sat_GPS_PosPrev[0]) > EPS ) && ( fabs(YE-Sat_GPS_PosPrev[1]) > EPS ) && ( fabs(ZE-Sat_GPS_PosPrev[2]) > EPS )) {
   		if (( fabs(XE) > EPS ) && ( fabs(YE) > EPS ) && ( fabs(ZE) > EPS )) {                 
                   GPSvalid = 1;
		   Sat_GPS_PosPrev[0] = Sat_GPS_Pos[0];
		   Sat_GPS_PosPrev[1] = Sat_GPS_Pos[1];
		   Sat_GPS_PosPrev[2] = Sat_GPS_Pos[2];
		}
	}
   }
   else {
	   GPSEst = 1;
	   estimatedPos(dt, Sat_GPS_Pos[0], Sat_GPS_Pos[1], Sat_GPS_Pos[2], &Sat_GPS_PosEst[0], &Sat_GPS_PosEst[0], &Sat_GPS_PosEst[0]);  //TODO //this is only a proto! 
	   Sat_GPS_Pos[0] = Sat_GPS_PosEst[0];
	   Sat_GPS_Pos[1] = Sat_GPS_PosEst[1];
	   Sat_GPS_Pos[2] = Sat_GPS_PosEst[2];	   
	   Sat_GPS_PosPrev[0] = Sat_GPS_Pos[0];
	   Sat_GPS_PosPrev[1] = Sat_GPS_Pos[1];
	   Sat_GPS_PosPrev[2] = Sat_GPS_Pos[2];	   		   
   }
   
   
   if (GPSEst && !GPSvalid) {
      counterGPSEst++;
      if(counterGPSEst == 10) {
        CFE_EVS_SendEvent(GENERIC_ADCS_SET_TARGET_INF_EID, CFE_EVS_EventType_INFORMATION, "***ADCS*** Warning: GPS position temporary based on estimation only."); //TODO adapt event param.
      	//printf(" ********* ADCS Warning: GPS position based on estimation only ... \n");   //TODO  system print	
      	counterGPSEst = 0;
      }
   }	
  
   // get GMST from satellite data 
   double weeks = Generic_ADCS_AppData.DIPacket.Payload.Gps.Weeks;
   double secondsIntoWeek = Generic_ADCS_AppData.DIPacket.Payload.Gps.SecondsIntoWeek;
   double fractions = Generic_ADCS_AppData.DIPacket.Payload.Gps.Fractions;
   double GMST = gps_mod1024_to_gmst(weeks, secondsIntoWeek, fractions, 3); //deg // Greenwich mean sideral time // epoch = 3 
   
   // define a target direction in ECEF frame 
   double XE_target = 4687031.59; // Toulouse in ECEF frame // init
   double YE_target = 118073.85;
   double ZE_target = 4379073.37;
   double Earth_Pos[3] = {XE_target, YE_target, ZE_target};
   double Earth_Pos_body[3] = {0,0,0};

   // take the latitude / longitude of the target on ground and convert to ECEF target
   latLonToEcef(GNC->latitude, GNC->longitude, 0.0, &XE_target, &YE_target, &ZE_target); // lat,long is zero by default; it can be set by TC
      
   target_vectorE[0] = (Sat_GPS_Pos[0]-XE_target); // sat position - earth target // target vector in ECEF
   target_vectorE[1] = (Sat_GPS_Pos[1]-YE_target);
   target_vectorE[2] = (Sat_GPS_Pos[2]-ZE_target);
   
   if ((GNC->latitude < 1e-6) && (GNC->latitude > -1e-6) && (GNC->longitude < 1e-6) && (GNC->longitude > -1e-6)) {  // pointing to NADIR if LAT,LONG = 0,0 !!!!!!
   target_vectorE[0] = Sat_GPS_Pos[0]/2; // this is only a test to point to NADIR instead of target ! //decomment to test
   target_vectorE[1] = Sat_GPS_Pos[1]/2;
   target_vectorE[2] = Sat_GPS_Pos[2]/2;
   }
      
   UNITV(target_vectorE); // create a unit target vector in ECEF frame 
   
   //double cam_side[3] = {0.0, -1.0, 0.0}; //cam side vector in body frame : -Y
   double cam_side[3] = {0.0, 1.0, 0.0}; //cam side vector in body frame : -Y //TODO
   double svb[3] = {1, 0, 0};
   
   if (GNC->SunValid) {
   	svb[0] = GNC->svb[0];
   	svb[1] = GNC->svb[1]; 
   	svb[2] = GNC->svb[2]; // sun vector in body frame
        Sat_Sun_PosPrev[0] =  svb[0];
        Sat_Sun_PosPrev[1] =  svb[1];
        Sat_Sun_PosPrev[2] =  svb[2];
   }
   else {
   	svb[0] = Sat_Sun_PosPrev[0];
   	svb[1] = Sat_Sun_PosPrev[1]; 
   	svb[2] = Sat_Sun_PosPrev[2]; // sun vector in body frame keep previous 	
   }
   
   // get quaternion in ECI to body frame from Start Trackers (from NASA 42) : SC[].AC.ST[].qn
   // get quaternion in ECEF to body frame from Start Trackers (from NASA 42) : SC[].AC.ST[].qbw // we have added a socket 
   // HP: for star trackers axes see SC_NOS3_satX.txt in sims/cfg/InOut : 2 star trackers by default  
   double STisValid[2] = {0,0}; // init 
   //double qn[4]; //qn, quaternion of body orientation in ECI frame 
   double qbw[4]; //qbw, quaternion of body orientation in ECEF frame 
   double qn_st0[4]; //qn_st0, quaternion of ST0 orientation in ECEF frame 
   double qn_st1[4]; //qn_st1, quaternion of ST1 orientation in ECEF frame 
   
   STisValid[0] = Generic_ADCS_AppData.DIPacket.Payload.Star_Tracker[0].IsValid; // validity ST1
   STisValid[1] = Generic_ADCS_AppData.DIPacket.Payload.Star_Tracker[1].IsValid; // validity ST2
         
   if ((int) STisValid[0] == 1) { // XYZW quaternion from 42!
   	qn_st0[0] = Generic_ADCS_AppData.DIPacket.Payload.Star_Tracker[0].Q3;  
   	qn_st0[1]  = Generic_ADCS_AppData.DIPacket.Payload.Star_Tracker[0].Q0;
   	qn_st0[2]  = Generic_ADCS_AppData.DIPacket.Payload.Star_Tracker[0].Q1;
   	qn_st0[3]  = Generic_ADCS_AppData.DIPacket.Payload.Star_Tracker[0].Q2;
   	rotate_quaternion(qn_st0,0,0,0,qbw); // turn if necessary // 0,0,0 deg = no XYZ rotations
   	assignQuaternion(qbw,q_prev);
   	
   }
   // if first star tracker data are not valid, use the other  
   if ((int) STisValid[1] == 1) { // XYZW quaternion from 42!
   	qn_st1[0] = Generic_ADCS_AppData.DIPacket.Payload.Star_Tracker[1].Q3;  
   	qn_st1[1]  = Generic_ADCS_AppData.DIPacket.Payload.Star_Tracker[1].Q0;
   	qn_st1[2]  = Generic_ADCS_AppData.DIPacket.Payload.Star_Tracker[1].Q1;
   	qn_st1[3]  = Generic_ADCS_AppData.DIPacket.Payload.Star_Tracker[1].Q2;
   	rotate_quaternion(qn_st1,0,0,0,qbw); //turn if necessary
   	assignQuaternion(qbw,q_prev);
   }
   
   if (((int) STisValid[0] == 0 && (int) STisValid[1] == 0)) {
	   STEst = 1;
	   estimateQuaternion(q_prev,q_est);
	   assignQuaternion(q_est,qbw);
	   assignQuaternion(qbw,q_prev);
   }
   
   if (((int) STisValid[0] == 0 && (int) STisValid[1] == 0 && STEst)) {
      counterSTEst++;
      if(counterSTEst == 10) {
        CFE_EVS_SendEvent(GENERIC_ADCS_SET_TARGET_INF_EID, CFE_EVS_EventType_INFORMATION, "***ADCS*** Warning: ST quaternion temporary based on estimation only."); //TODO adapt event param.	
      	counterSTEst = 0;
      }
   }
        
   normalize_quaternion(qbw);  
        
   // tuning param for PID	// this is a prototype ! //TODO	   
   double alpha = 1.0; // [0-1] // 1 = no filter // filter on rates // tuning TODO
   double intLim = 0.0001; // tune lim on integral //TODO
   double csi = 0.9; // desired damping //TODO
   double T0 = 15; //default 15? // desired resp time [s]// main tunining param // tuning stability? //TODO
   double omega0 = 2 * M_PI / T0;
   double omega1 = omega0*0.01; // tune integral TODO // 0 for no control // tuning stability? //TODO
   double Jeq[3] = {0.0067,0.033,0.033}; // Ixx, Iyy, Izz, see SC_NOS3_satX.txt
   // Gains
   double Kp = omega0 * (2 * csi * omega1 + omega0);
   double Kd = (2 * csi * omega0 + omega1);
   double Ki = omega1 * omega0 * omega0;
     
    
   if (((int) STisValid[0] == 1 || (int) STisValid[1] == 1 || STEst) && (GPSvalid || GPSEst)) {  // if star trackers data and gps are valid compute the normal mode
   	   	   	 
   	   // data from 42 are freezing sometimes !!! // to be investigated //TODO  	  	   
	   if (qbw[0] > 0.9999999999 ) { // if the quaternion is {1,0,0,0} after norm then exit 
	   	return;
	   }
	   	    
	   // Init
	   double Xref[3] = {1.0,0.0,0.0};
	   double Yref[3] = {0.0,1.0,0.0};
	   double Zref[3] = {0.0,0.0,1.0};
	   
	   // convert quaternion in rotation matrix from body to ECEF // Xref is the first column of R
	   Q2Rn(qbw, Xref, Yref, Zref);  // Rbw 
	   	   
	   //target vector from ECEF to body 
	   double target_vector[3] = {0.0,0.0,0.0};
	   RTxVn(Xref, Yref, Zref, target_vectorE, target_vector); 	   	   
	   UNITV(target_vector);	   
	   	      
	   // sun vector from body to ECEF frame 
	   double svb_w[3] = {0.0,0.0,0.0};
	   RxVn(Xref, Yref, Zref, svb, svb_w); 
	   UNITV(svb_w);
	   
	   // estimate RAAN for the imager 
	   //double RAAN = 0.383972; //4.989 //rad //init
	   //RAAN = computeRAAN(GMST, XE, YE); //rad  // TODO RAAN computation is not correct!
	   double sig = 1.0; // change axis sign
	       
           counterCoord++;  
	   if(counterCoord == 10) { // 1 time per second! (1 every 10 steps of 0.1s) // SEE SCH conf in sch_def_schtbl // it depends also on FSW frequency ! see cfe_psp_start.c !
	        // this file is for the imager (Imager Input)
	   	FILE* fp = fopen("/tmp/coord1.txt","w");
	   	fprintf(fp, "%f %f %f\n", XE, YE, ZE); // ECEF Sat position
	   	fprintf(fp, "%f %f %f\n", sig*Xref[0], sig*Xref[1], sig*Xref[2]); // Rotation Matrix column in ECEF frame
		fprintf(fp, "%f %f %f\n", sig*Yref[0], sig*Yref[1], sig*Yref[2]);
		fprintf(fp, "%f %f %f\n", sig*Zref[0], sig*Zref[1], sig*Zref[2]);
		fprintf(fp, "%f %f %f\n", XE_target, YE_target, ZE_target);// add ECEF target on ground
	        fprintf(fp, "%f %f %f\n", svb_w[0], svb_w[1], svb_w[2]); // Sun vector in ECEF frame // 42 svb freezing sometimes // to be investigated //TODO
	   	//fprintf(fp, "%f %f %f\n", 1.0, 0.0, 0.0); // Sun vector in ECEF frame //TODO
	   	//fprintf(fp, "%f \n", RAAN); //old
	   	fprintf(fp, "%f \n", GMST*(M_PI / 180.0));	   	
	   	fclose(fp);
	   	system("mv /tmp/coord1.txt /tmp/coord.txt");

		//debug prints
	   	//printf(" **** Quaternion = %f, %f, %f, %f \n", qbw[0], qbw[1],qbw[2], qbw[3]); // check quaternion 
	   	//printf(" **** ST Validity 1 and 2 = %f, %f \n", STisValid[0],STisValid[1]); // check validity from star tracker  
		//printf(" **** GMST = %lf \n", GMST);
	        //printf(" **** RAAN = %lf \n", RAAN);
		//printf(" **** target  = %f %f %f \n",target_vector[0], target_vector[1], target_vector[2]);
	   	//printf(" **** Sun vector ECEF = %f, %f, %f \n", svb_w[0], svb_w[1], svb_w[2]);
	   	//printf(" **** Sun vector Body = %f, %f, %f \n", svb[0], svb[1], svb[2]);
	   	//printf(" **** Latitude / Longitude (target) = %f, %f \n", GNC->latitude, GNC->longitude);
	   	//printf(" **** X,Y,Z target ECEF = %f %f %f \n", XE_target, YE_target, ZE_target);
	   	//printf(" **** GPS data, SATPOS ECEFX,Y,Z = %lf, %lf, %lf \n", XE, YE, ZE);	   			
	   	//printf(" **** weeks, secondsIntoWeek, fractions = %f %f %f \n", weeks, secondsIntoWeek, fractions);
	   	counterCoord = 0;
	   }
	      	      
	   /* .. Form attitude error signals */
	   SoS = VoV(target_vector, cam_side); // compare vectors in body frame 

	      if ((SoS > (EPS - 1.0)) && (SoS < (1.0 - EPS))) {
		 VxV(target_vector, cam_side, ACS->therr);
	      }
	      else if (SoS >= (1.0 - EPS)) {
		 ACS->therr[0] = 0.0;
		 ACS->therr[1] = 0.0;
		 ACS->therr[2] = 0.0;
		 }
	      else {
		 err_b[0] = cam_side[1]; // anti parallel case ?
		 err_b[1] = cam_side[2];
		 err_b[2] = cam_side[0];
		 if (fabs(err_b[0] - err_b[1]) < EPS && fabs(err_b[0] - err_b[2]) < EPS) {
		    err_b[0] = -err_b[0];
		 }
		 VxV(cam_side, err_b, temp_sside);
		 VxV(target_vector, temp_sside, ACS->therr);
	      }
	      	      
	      counterMSE++;
	      // check MSE for control convergence 
	      if(counterMSE == 10) { // SEE SCH conf in sch_def_schtbl // it depends also on FSW frequency ! see cfe_psp_start.c !
		      MSE = (pow(ACS->therr[0],2)+pow(ACS->therr[1],2)+pow(ACS->therr[2],2))/3;
		      // debug prints
		      //printf("*************** ThetaErr X,Y,Z = %f %f %f \n", ACS->therr[0], ACS->therr[1], ACS->therr[2]);
		      //printf("*************** MSE = %f \n", MSE);
		      counterMSE = 0;
		      FILE* fperr = fopen("/tmp/ctrlErr.txt","a");
		      fprintf(fperr, "%f %f %f %f \n", ACS->therr[0], ACS->therr[1], ACS->therr[2], MSE); // Errors // python3 checkResp_NormalMode.py to plot time response! 
		      fclose(fperr);
	      }
	      		   	      
	/* .. Closed-loop attitude control - PID Method */
	      for(i = 0; i < 3; i++) {

		 u1[i] = Limit(Kp / Kd * ACS->therr[i], -ACS->vmax,ACS->vmax);
		 ACS->werr[i] = GNC->wbn[i] - ACS->cmd_wbn[i];
		 		 
		 // test low pass filter on werr
		 ACS->werr[i] = alpha*ACS->werr[i] + (1 - alpha)*prev_werr[i];
		 prev_werr[i] = ACS->werr[i];

		 // test integral action 
		 integ_err[i] += Limit(0.5*(ACS->therr[i] + prev_therr[i])*dt, -intLim, intLim);
		 prev_therr[i] = ACS->therr[i];
		 	
		 // PID control		 		 
		 ACS->Tcmd[i] = -Jeq[i]*(Kd*u1[i] + Kd*ACS->werr[i] + Ki*integ_err[i]);
		 			 		 	 
	      }
	      	 
	      //debug prints	      
	      /*counterCTRL++;
	      if(counterCTRL == 10) { 
	      		printf("********** Kp, Kd, Ki = %f %f %f \n", Kp, Kd, Ki);
		 	printf("********** u1[0], u1[1], u1[2], vmax = %f %f %f %f \n", u1[0], u1[1], u1[2], ACS->vmax);
		 	printf("********** werr[0], werr[1], werr[2] = %f %f %f \n", ACS->werr[0], ACS->werr[1], ACS->werr[2]);
		 	printf("********** ACS->Tcmd[i] = %f %f %f \n", ACS->Tcmd[0], ACS->Tcmd[1], ACS->Tcmd[2]);
		 	counterCTRL = 0;
		 	}*/
		 	
	      // this is an attempt to control also X axis to be close to the Satellite trajectory (X axis expected on top for imager photos). //this is a simple prototype //TODO
	      UNITV(Sat_traj);
	      double targetN = VoV(target_vector,target_vector);
	      double trajN = VoV(Sat_traj,target_vector);
	      double coeff1 = trajN / targetN;
	      double Sat_traj_N[3] = {coeff1 * target_vector[0],coeff1 * target_vector[1],coeff1 * target_vector[2]};
	      double Sat_traj_P[3] = {Sat_traj[0]-Sat_traj_N[0],Sat_traj[1]-Sat_traj_N[1],Sat_traj[2]-Sat_traj_N[2]};
	      double therr_traj[3] = {0,0,0};
	      double coeff2 = 0.0; // [0 - 1] // 0 for no control // tuning // tuning stability/perfo ? //TODO  
	      
	      if ((SoS > (EPS - 1.0)) && (SoS < (1.0 - EPS))) {
		 VxV(Sat_traj_P, ACS->sside, therr_traj);
	      }
	      else if (SoS >= (1.0 - EPS)) {
		 therr_traj[0] = 0.0;
		 therr_traj[1] = 0.0;
		 therr_traj[2] = 0.0;
		 }
	
	      for(i = 0; i < 3; i++) {
	      u1[i] = Limit(Kp / Kd * therr_traj[i], -ACS->vmax,ACS->vmax);		 		 
	      Tcmd_traj[i] = -Jeq[i]*coeff2*(Kd*u1[i] + Kd*ACS->werr[i]);	// to be refined //TODO	 			 		 	 
	      }
	      		 
	/* .. Apply Torque Command */
	      for(i = 0; i < 3; i++) {
		 GNC->Tcmd[i] = -(ACS->Tcmd[i]+Tcmd_traj[i]); 
		 GNC->Mcmd[i] = 0.0;
	      }
	            	      
   }
    
   else if (((int) STisValid[0] == 0 && (int) STisValid[1] == 0 && !STEst)) { /* during star tracker not valid, adapt the controller  TBD */ // work in progress // TODO 

      counterStarKO++;
      if(counterStarKO == 10) {
      	//printf(" ********* ADCS Warning: Star Tracker input data temporary KO \n");  //TODO  system print 
      	CFE_EVS_SendEvent(GENERIC_ADCS_SET_TARGET_INF_EID, CFE_EVS_EventType_INFORMATION, "***ADCS*** Warning: Star Tracker input data temporary KO."); //TODO adapt event param.	
      	counterStarKO = 0;
      }
      return;
      
      /*double coeff3 = 1/10; // reduce kd?? //TODO
      for(i = 0; i < 3; i++) { 
         ACS->werr[i] = GNC->wbn[i];
         ACS->Tcmd[i] = -Jeq[i] * Kd * coeff3 * ACS->werr[i];
         //ACS->werr[i] = 0;
         //ACS->Tcmd[i] = 0;
      }     
      // Apply Torque Command
      for (i = 0; i < 3; i++) {
         GNC->Tcmd[i] = -ACS->Tcmd[i];
         GNC->Mcmd[i] = 0.0;
         //GNC->Tcmd[i] = 0;
      }*/
   }
     
   else if (!GPSvalid && !GPSEst) {
      counterGPSKO++;
      if(counterGPSKO == 10) {
      	//printf(" ********* ADCS Warning: GPS input data temporary KO \n");   //TODO  system print	
      	CFE_EVS_SendEvent(GENERIC_ADCS_SET_TARGET_INF_EID, CFE_EVS_EventType_INFORMATION, "***ADCS*** Warning: GPS input data temporary KO."); //TODO adapt event param.
      	counterGPSKO = 0;
      }
      return;
   }	
      
	if (GNC->HmgmtOn) {
	      AC_h_mgmt(GNC);
	      for(i = 0; i < 3; i++) {
		GNC->Mcmd[i] = GNC->Hmgmt.Mcmd[i];
	      }
	   }
	   else {
	      for(i = 0; i < 3; i++) {
		 GNC->Mcmd[i] = 0.0;
	      }
	   }
   
}

static void AC_h_mgmt(Generic_ADCS_GNC_Tlm_Payload_t *GNC)
{

   double Herr[3] = {0.0, 0.0, 0.0};
   double bvb[3] = {0.0, 0.0, 0.0};
   double HxB[3] = {0.0, 0.0, 0.0};
   int i;

   if ( MAGV(GNC->bvb) > GNC->Hmgmt.b_range ) {
      /*Test if any axis needs to be momentum managed*/
      for(i=0;i<3;i++) {
         if (fabs(GNC->HwhlB[i]) > GNC->Hmgmt.hiFrac*fabs(GNC->HwhlMaxB[i])) {
            GNC->Hmgmt.mm_active[i] = 1;
         }
         if (fabs(GNC->HwhlB[i]) < GNC->Hmgmt.loFrac*fabs(GNC->HwhlMaxB[i])) {
            GNC->Hmgmt.mm_active[i] = 0;
         }
      }
      for(i = 0; i < 3; i++) {
         Herr[i] = 0.0;
         if (GNC->Hmgmt.mm_active[i] == 1) {
            Herr[i] = GNC->HwhlB[i];
         }
      }
      CopyUnitV(GNC->bvb, bvb);
      VxV(Herr,bvb,HxB);
      for(i = 0; i < 3; i++) {
         GNC->Hmgmt.Mcmd[i] = GNC->Hmgmt.Kb * HxB[i] / MAGV(GNC->bvb);
      }
   }
   else {
      for (i = 0; i < 3; i++) {
         GNC->Hmgmt.Mcmd[i] = 0.0;
      }
   }

}


