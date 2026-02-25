/* Copyright (C) 2015 - 2015 National Aeronautics and Space Administration. All Foreign Rights are Reserved to the U.S. Government.

   This software is provided "as is" without any warranty of any, kind either express, implied, or statutory, including, but not
   limited to, any warranty that the software will conform to, specifications any implied warranties of merchantability, fitness
   for a particular purpose, and freedom from infringement, and any warranty that the documentation will conform to the program, or
   any warranty that the software will be error free.

   In no event shall NASA be liable for any damages, including, but not limited to direct, indirect, special or consequential damages,
   arising out of, resulting from, or in any way connected with the software or its documentation.  Whether or not based upon warranty,
   contract, tort or otherwise, and whether or not loss was sustained from, or arose out of the results of, or use of, the software,
   documentation or services provided hereunder

   ITC Team
   NASA IV&V
   ivv-itc@lists.nasa.gov
*/

#include <cmath>

#include <ItcLogger/Logger.hpp>

#include <sim_coordinate_transformations.hpp>

namespace Nos3
{

    extern ItcLogger::Logger *sim_logger;

    /*************************************************************************
     * Constructors
     *************************************************************************/
    const SimCoordinateTransformations::SimConstants SimCoordinateTransformations::SIM_CONSTANTS;

    /*************************************************************************
     * Static Methods
     *************************************************************************/
    /* This function is agnostic to the TT-to-UTC offset.  You get out    */
    /* what you put in. */
    void SimCoordinateTransformations::AbsTime2YMDHMS(double abs_time, int32_t& year, int32_t& month, int32_t& day,
                                 int32_t& hour, int32_t& minute, double& second)
    {
        JD2YMDHMS(AbsTimeToJD(abs_time), year, month, day, hour, minute, second);
    }

    /**********************************************************************/
    /* AbsTime is elapsed seconds since J2000 epoch                       */
    /* This function is agnostic to the TT-to-UTC offset.  You get out    */
    /* what you put in. */
    double SimCoordinateTransformations::AbsTimeToJD(double abs_time)
    {
        return (abs_time/86400.0 + 2451545.0);
    }

    /**********************************************************************/
    /* AbsTime is elapsed seconds since J2000 epoch                       */
    /* This function is agnostic to the TT-to-UTC offset.  You get out    */
    /* what you put in. */
    double SimCoordinateTransformations::JDToAbsTime(double jd)
    {
        return ((jd - 2451545.0) * 86400.0);
    }

    /**********************************************************************/
    /* Convert Julian Day to Year, Month, Day, Hour, Minute, and Second   */
    /* Ref. Jean Meeus, 'Astronomical Algorithms', QB51.3.E43M42, 1991.   */
    /* This function is agnostic to the TT-to-UTC offset.  You get out    */
    /* what you put in. */

    void SimCoordinateTransformations::JD2YMDHMS(double jd, int32_t& year, int32_t& month, int32_t& day,
                                 int32_t& hour, int32_t& minute, double& second)
    {
          double Z,F,A,B,C,D,E,alpha;
            double FD;

          Z= floor(jd+0.5);
          F=(jd+0.5)-Z;

          if (Z < 2299161.0) 
          {
             A = Z;
          }
          else 
          {
             alpha = floor((Z-1867216.25)/36524.25);
             A = Z+1.0+alpha - floor(alpha/4.0);
          }

          B = A + 1524.0;
          C = floor((B-122.1)/365.25);
          D = floor(365.25*C);
          E = floor((B-D)/30.6001);

          FD = B - D - floor(30.6001*E) + F;
          day = (int32_t) FD;

          if (E < 14.0) 
          {
             month = (int32_t) (E - 1.0);
             year = (int32_t) (C - 4716.0);
          }
          else 
          {
             month = (int32_t) (E - 13.0);
             year = (int32_t) (C - 4715.0);
          }

            FD = FD - floor(FD);
            FD = FD * 24.0;
            hour = (int32_t) FD;
            FD = FD - floor(FD);
            FD = FD * 60.0;
            minute = (int32_t) FD;
            FD = FD - floor(FD);
            second = FD * 60.0;

    }

    /**********************************************************************/
    /*  Find Month, Day, given Day of Year                                */
    /*  Ref. Jean Meeus, 'Astronomical Algorithms', QB51.3.E43M42, 1991.  */
    /*  Chapter 7, pp. 62, 66                                             */

    void SimCoordinateTransformations::DOY2MD(int16_t Year, int16_t DayOfYear, int16_t &Month, int16_t &Day)
    {
          int16_t K;

          if (Year % 4 == 0) 
          {
              if (Year % 100 == 0) 
              {
                  if (Year % 400 == 0)
                  {
                      K = 1;
                  }
                  else 
                  {
                      K = 2;
                  }
              }
              else
              {
                  K = 1;
              }
          }
          else 
          {
             K = 2;
          }

          if (DayOfYear < 32) 
          {
             Month = 1;
          }
          else 
          {
             Month = (int16_t) (9.0*(K+DayOfYear)/275.0+0.98);
          }

          Day = DayOfYear - 275*(Month)/9 + K*(((Month)+9)/12) + 30;

    }

    /**********************************************************************/
    /* Convert Year, Month, Day, Hour, Minute and Second to               */
    /* "Absolute Time", i.e. seconds elapsed since J2000 epoch.           */
    /* J2000 = 2451545.0 TT  =  01 Jan 2000 12:00:00.00 TT                */
    /* Year, Month, Day assumed in Gregorian calendar. (Not true < 1582)  */
    /* Ref. Jean Meeus, 'Astronomical Algorithms', QB51.3.E43M42, 1991.   */
    /* This function is agnostic to the TT-to-UTC offset.  You get out    */
    /* what you put in. */

    double SimCoordinateTransformations::DateToAbsTime(int32_t Year, int32_t Month, int32_t Day, int32_t Hour,
       int32_t Minute, double Second)
    {
          int32_t A,B;
          double Days;

          if (Month < 3) 
          {
             Year--;
             Month+=12;
          }

          A = Year/100;
          B = 2 - A + A/4;

          /* Days since J2000 Epoch (01 Jan 2000 12:00:00.0) */
          Days = floor(365.25*(Year+4716))
                      + floor(30.6001*(Month+1))
                      + Day + B - 1524.5 - 2451545.0;

          /* Add fractional day */
          return(86400.0*Days + 3600.0*((double) Hour)
             + 60.0*((double) Minute) + Second);
    }

    /**********************************************************************/
    /* GPS Epoch is 6 Jan 1980 00:00:00.0 which is JD = 2444244.5         */
    /* GPS Time is expressed in weeks and seconds                         */
    /* GPS Time rolls over every 1024 weeks                               */
    /* This function requires JD in TT                                    */
    void SimCoordinateTransformations::JDToGpsTime(double JD, int32_t &GpsRollover, int16_t &GpsWeek, double &GpsSecond)
    {
          double DaysSinceEpoch, DaysSinceRollover, DaysSinceWeek;

          DaysSinceEpoch = JD - 2444244.5;
          GpsRollover = (int32_t) (DaysSinceEpoch/7168.0);
          DaysSinceRollover = DaysSinceEpoch - 7168.0*((double) GpsRollover);
          GpsWeek = (int32_t) (DaysSinceRollover/7.0);
          DaysSinceWeek = DaysSinceRollover - 7.0*((double) GpsWeek);
          GpsSecond = DaysSinceWeek*86400.0;
    }

    /* This function yields JD in TT                                      */
    void SimCoordinateTransformations::GpsTimeToJD(int32_t GpsRollover, int16_t GpsWeek, double GpsSecond, double &JD)
    {
        JD = GpsRollover * 7168.0 + GpsWeek * 7.0 + GpsSecond/86400.0 + 2444244.5;
    }

    /* Fundamentals of Astrodynamics and Applications, 3rd edition, David A. Vallado,
     * Space Technology Library, Microcosm Press / Springer, Hawthorne, CA / New York, NY, 2007.
     * Section 3.4, Algorithm 12, p. 179 */
    void SimCoordinateTransformations::ECEF2LLA(double x, double y, double z, double& phi_gd, double& lambda, double& h_ellp)
    {
        sim_logger->trace("SimCoordinateTransformations::ECEF2LLA:  Inputs: x = %12.4f, y = %12.4f, z = %12.4f",
            x, y, z);
        double r_I = x / SIM_CONSTANTS.R_plus;
        double r_J = y / SIM_CONSTANTS.R_plus;
        double r_K_sat = z / SIM_CONSTANTS.R_plus;

        sim_logger->trace("SimCoordinateTransformations::ECEF2LLA:  Converted: r_I = %12.4f, r_J = %12.4f, r_K_sat = %12.4f",
            r_I, r_J, r_K_sat);

        double r_delta_sat, sin_alpha, cos_alpha, r, delta, r_delta, r_K, phi_gd_old, sin_phi_gd, C_plus, tan_phi_gd;
        double tolerance = 0.000000001;

        r = sqrt(r_I * r_I + r_J * r_J + r_K_sat * r_K_sat);
        r_delta_sat = sqrt(r_I * r_I + r_J * r_J);
        sin_alpha = r_J / r_delta_sat;
        cos_alpha = r_I / r_delta_sat;

        if (sin_alpha >= 0) 
        { // 1st or 2nd quadrant, 0 <= alpha <= PI
            lambda = acos(cos_alpha); // Result of acos is between 0 and PI
        } 
        else 
        { // 3rd or 4th quadrant, 0 -PI < alpha < PI
            lambda = asin(sin_alpha); // Result of asin is between -PI/2 and 0, so this is only correct if we are in 4th quadrant
            if (cos_alpha < 0) 
            { // 3rd quadrant, so we need to fix the result
                lambda = -1 * SIM_CONSTANTS.PI - lambda;
            }
        }
        lambda = lambda * 180.0 / SIM_CONSTANTS.PI; // convert to degrees for output

        delta = asin(r_K_sat / r);
        sim_logger->trace("SimCoordinateTransformations::ECEF2LLA:  Fixed Computations: r_delta_sat = %12.4f (%12.4f m), alpha = %12.8f, delta = %12.8f",
            r_delta_sat, r_delta_sat * SIM_CONSTANTS.R_plus, lambda, delta * 180.0 / SIM_CONSTANTS.PI);


        phi_gd = delta;
        r_delta = r_delta_sat;
        r_K = r_K_sat;

        do {
            sin_phi_gd = sin(phi_gd);

            C_plus = 1 / sqrt(1 - SIM_CONSTANTS.e_plus * SIM_CONSTANTS.e_plus * sin_phi_gd * sin_phi_gd);
            tan_phi_gd = (r_K + C_plus * SIM_CONSTANTS.e_plus * SIM_CONSTANTS.e_plus * sin_phi_gd) / r_delta;

            phi_gd_old = phi_gd;
            phi_gd = atan(tan_phi_gd);

            sim_logger->trace("SimCoordinateTransformations::ECEF2LLA:  Iteration: C_plus = %12.8f, phi_gd = %12.8f (phi_gd_old = %12.8f), phi_gd - phi_gd_old = %12.8f",
                C_plus, phi_gd * 180.0 / SIM_CONSTANTS.PI, phi_gd_old * 180.0 / SIM_CONSTANTS.PI, phi_gd - phi_gd_old);
        } while ((phi_gd - phi_gd_old) > tolerance);

        h_ellp = (r_delta / cos(phi_gd) - C_plus) * SIM_CONSTANTS.R_plus;

        phi_gd = phi_gd * 180.0 / SIM_CONSTANTS.PI; // convert to degrees for output

        sim_logger->trace("SimCoordinateTransformations::ECEF2LLA:  Outputs: lambda = %12.8f, phi_gd = %12.8f, h_ellp = %12.4f",
            lambda, phi_gd, h_ellp);
    }

    /*************************************************************************
     * Private helper methods
     *************************************************************************/


}
