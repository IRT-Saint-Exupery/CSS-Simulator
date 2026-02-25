/* Copyright (C) 2015 - 2019 National Aeronautics and Space Administration. All Foreign Rights are Reserved to the U.S. Government.

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

#include <iomanip>
#include <string>
#include <cmath>

#include <ItcLogger/Logger.hpp>

#include <sim_coordinate_transformations.hpp>
#include <gps_sim_data_point.hpp>

namespace Nos3
{
    extern ItcLogger::Logger *sim_logger;

    /*************************************************************************
     * Constructors
     *************************************************************************/

    GPSSimDataPoint::GPSSimDataPoint(double abs_time, int16_t leap_seconds, int16_t gps_week, int32_t gps_sec_week, double gps_frac_sec, 
        const std::vector<double>& ECEF, const std::vector<double>& ECEF_vel, const std::vector<double>& ECI, const std::vector<double>& ECI_vel) : 
        _leap_seconds(leap_seconds), _not_parsed(false), _abs_time(abs_time), _gps_week(gps_week), _gps_sec_week(gps_sec_week), _gps_frac_sec(gps_frac_sec), 
        _ECEF(ECEF), _ECEF_vel(ECEF_vel), _ECI(ECI), _ECI_vel(ECI_vel)
    {
    }

    GPSSimDataPoint::GPSSimDataPoint(int16_t spacecraft, int16_t gps, int16_t leap_seconds, const boost::shared_ptr<Sim42DataPoint> dp) : 
        _dp(*dp), _sc(spacecraft), _gps(gps), _leap_seconds(leap_seconds), _not_parsed(true) 
    {
        _ECEF.resize(3); _ECEF_vel.resize(3); _ECI.resize(3); _ECI_vel.resize(3);
        sim_logger->trace("GPSSimDataPoint::GPSSimDataPoint:  Created instance using _sc=%d, _gps=%d, _dp=%s", 
            _sc, _gps, _dp.to_string().c_str());
    }

    /*************************************************************************
     * Mutators
     *************************************************************************/

    void GPSSimDataPoint::do_parsing(void) const
    {
        try {
            std::string valid_key;
            valid_key.append("SC[").append(std::to_string(_sc)).append("].AC.GPS[").append(std::to_string(_gps)).append("].");
            std::string rollover_key(valid_key), week_key(valid_key), sec_key(valid_key), posn_key(valid_key), veln_key(valid_key), posw_key(valid_key), velw_key(valid_key), lng_key(valid_key), lat_key(valid_key), alt_key(valid_key);
            valid_key.append("Valid");
            rollover_key.append("Rollover");
            week_key.append("Week");
            sec_key.append("Sec");
            posn_key.append("PosN");
            veln_key.append("VelN");
            posw_key.append("PosW");
            velw_key.append("VelW");
            lng_key.append("Lng");
            lat_key.append("Lat");
            alt_key.append("Alt");

            _gps_valid = (_dp.get_value_for_key(valid_key) == "1");
            _gps_rollover = strtol(_dp.get_value_for_key(rollover_key).c_str(), NULL, 10);
            _gps_week = strtol(_dp.get_value_for_key(week_key).c_str(), NULL, 10);
            double seconds = strtod(_dp.get_value_for_key(sec_key).c_str(), NULL);
            _gps_sec_week = (int32_t)seconds;
            _gps_frac_sec = seconds - (double)_gps_sec_week;
            parse_double_vector(_dp.get_value_for_key(posn_key), _ECI);
            parse_double_vector(_dp.get_value_for_key(veln_key), _ECI_vel);
            parse_double_vector(_dp.get_value_for_key(posw_key), _ECEF);
            parse_double_vector(_dp.get_value_for_key(velw_key), _ECEF_vel);
            _gps_lng = strtod(_dp.get_value_for_key(lng_key).c_str(), NULL);
            _gps_lat = strtod(_dp.get_value_for_key(lat_key).c_str(), NULL);
            _gps_alt = strtod(_dp.get_value_for_key(alt_key).c_str(), NULL);

            sim_logger->trace("GPSSimDataPoint::do_parsing:  Valid=%s, Rollover=%d, Week=%d, Seconds=%d, FracSeconds=%f, ECI=%f, %f, %f, ECI_vel=%f, %f, %f",
                _gps_valid?"TRUE ":"FALSE", _gps_rollover, _gps_week, _gps_sec_week, _gps_frac_sec, _ECI[0], _ECI[1], _ECI[2], _ECI_vel[0], _ECI_vel[1], _ECI_vel[2]);
            sim_logger->trace("GPSSimDataPoint::do_parsing: ECEF=%f, %f, %f, ECEF_vel=%f, %f, %f, Lat=%f, Lng=%f, Alt=%f",
                _ECEF[0], _ECEF[1], _ECEF[2], _ECEF_vel[0], _ECEF_vel[1], _ECEF_vel[2], _gps_lat, _gps_lng, _gps_alt);

            _not_parsed = false;

        } catch(const std::exception& e) {
            sim_logger->error("GPSSimDataPoint::do_parsing:  Parsing exception:  %s", e.what());
        }
        
        double JD;
        SimCoordinateTransformations::GpsTimeToJD(_gps_rollover, _gps_week, ((double)_gps_sec_week) + _gps_frac_sec, JD);
        _abs_time = SimCoordinateTransformations::JDToAbsTime(JD) - 32.184 - _leap_seconds;
        
        sim_logger->debug("GPSSimDataPoint::do_parsing:  Parsed data point:\n%s", to_string().c_str());

    }

    /*************************************************************************
     * Accessors
     *************************************************************************/

    std::string GPSSimDataPoint::to_formatted_string(void) const
    {
        parse_data_point();
        
        std::stringstream ss;

        ss << std::fixed << std::setprecision(4) << std::setfill(' ');
        ss << "GPS Data Point: " << std::endl;
        ss << "  Absolute Time                    : " << std::setw(15) << _abs_time << std::endl;
        ss << "  Leap Seconds                     : " << _leap_seconds << std::endl;
        ss << "  GPS Rollover, Week, Second, Fractional Second: "
           << std::setw(6) << _gps_rollover << ","
           << std::setw(6) << _gps_week << ","
           << std::setw(7) << _gps_sec_week << ","
           << std::setw(7) << _gps_frac_sec << std::endl;
        ss << std::setprecision(2);
        ss << "  ECEF                                         : "
           << std::setw(12) << _ECEF[0] << ","
           << std::setw(12) << _ECEF[1] << ","
           << std::setw(12) << _ECEF[2] << std::endl;
        ss << "  ECEF Velocity                                : "
           << std::setw(12) << _ECEF_vel[0] << ","
           << std::setw(12) << _ECEF_vel[1] << ","
           << std::setw(12) << _ECEF_vel[2] << std::endl;
        ss << "  ECI                                          : "
           << std::setw(12) << _ECI[0] << ","
           << std::setw(12) << _ECI[1] << ","
           << std::setw(12) << _ECI[2] << std::endl;
        ss << "  ECI Velocity                                 : "
           << std::setw(12) << _ECI_vel[0] << ","
           << std::setw(12) << _ECI_vel[1] << ","
           << std::setw(12) << _ECI_vel[2] << std::endl;
        ss << "  Geodetic Lat/Lng/Alt(m above WGS-84)         : "
           << std::setw(12) << _gps_lat << ","
           << std::setw(12) << _gps_lng << ","
           << std::setw(12) << _gps_alt << std::endl;

        return ss.str();
    }

    std::string GPSSimDataPoint::to_string(void) const
    {
        parse_data_point();
        
        std::stringstream ss;

        ss << std::fixed << std::setfill(' ');
        ss << "GPS Data Point: ";
        ss << std::setprecision(std::numeric_limits<double>::digits10); // Full double precision
        ss << " AbsTime: " << _abs_time;
        ss << " Leap Seconds: " << _leap_seconds;
        ss << std::setprecision(std::numeric_limits<int32_t>::digits10); // Full int32_t precision
        ss << " GPS Time: "
           << _gps_rollover << "/"
           << _gps_week << "/"
           << _gps_sec_week << "/";
        ss << std::setprecision(std::numeric_limits<double>::digits10); // Full double precision
        ss << _gps_frac_sec ;
        ss << " ECEF: "
           << _ECEF[0] << ","
           << _ECEF[1] << ","
           << _ECEF[2] ;
        ss << " ECEF Velocity"
           << _ECEF_vel[0] << ","
           << _ECEF_vel[1] << ","
           << _ECEF_vel[2] ;
        ss << " ECI "
           << _ECI[0] << ","
           << _ECI[1] << ","
           << _ECI[2] ;
        ss << " ECI Velocity"
           << _ECI_vel[0] << ","
           << _ECI_vel[1] << ","
           << _ECI_vel[2] ;
        ss << "  Geodetic Lat/Lng/Alt(m above WGS-84): "
           << _gps_lat << ","
           << _gps_lng << ","
           << _gps_alt ;

        return ss.str();
    }
    
}
