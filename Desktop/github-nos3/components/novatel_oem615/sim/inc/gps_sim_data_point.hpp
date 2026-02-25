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

#ifndef NOS3_GPSSIMDATAPOINT_HPP
#define NOS3_GPSSIMDATAPOINT_HPP

#include <cstdint>
#include <string>

#include <boost/shared_ptr.hpp>

#include <sim_42data_point.hpp>
#include <sim_i_data_point.hpp>

namespace Nos3
{

    /** \brief Class to contain a file entry of GPS simulation data for a specific time.
     *
     *  The GPS data is actually for a specific orbit location and attitude for the spacecraft also.
     *
     *  Notes on symbols/abbreviations:
     *  - v is for vector
     *  - n is the standard symbol used for inertial reference frame (independent of spacecraft attitude),
     *    b is the standard symbol used for body reference frame (depends on spacecraft attitude)
     *  - x, y, z... are for the x, y, z components
     *
     *  !!! UNITS !!!:
     *    absolute time is in seconds, with an epoch of ??
     *    inertial reference frame has center at ??, x pointing in ?? direction, y pointing in ?? direction, z pointing in ?? direction
     *    body reference frame has center at ?? (mag center?  spacecraft center?), x pointing in ?? direction, y pointing in ?? direction, z pointing in ?? direction
     *
     *  TBD:  Specify units of GPS data.  Specify exactly what
     *    inertial reference frame we are talking about.  Specify exactly how the body reference frame is set up, e.g. do
     *    we need to worry about a rotation matrix to reference the magnetometer body reference frame to the cubesat body
     *    reference frame.
     */
    class GPSSimDataPoint : public Sim42DataPoint
    {
    public:
        /// @name Constructors
        //@{
        /** \brief Default constructor
         *  Just has no data
         */
        GPSSimDataPoint() : _not_parsed(false) /* nothing to parse */ {_ECEF.resize(3); _ECEF_vel.resize(3); _ECI.resize(3); _ECI_vel.resize(3); };
        /** \brief Constructor explicit data values.
         *  Sets all the data values.
         */
        GPSSimDataPoint(double abs_time, int16_t leap_seconds, int16_t gps_week, int32_t gps_sec_week, double gps_frac_sec, 
            const std::vector<double>& ECEF, const std::vector<double>& ECEF_vel, const std::vector<double>& ECI, const std::vector<double>& ECI_vel); 
        /** \brief Constructor from a 42 data point
         *  Just sets the data point... parsing done on demand later.  This is for efficiency so if no accessors are called, no parsing is done.
         */
        GPSSimDataPoint(int16_t spacecraft, int16_t gps, int16_t leap_seconds, const boost::shared_ptr<Sim42DataPoint> dp);
        //@}

        /// @name Accessors
        //@{
        /// \brief Returns a block formatted string representation of the GPS simulation data point
        /// @return     A block formatted string representation of the GPS simulation data point
        std::string to_formatted_string(void) const;
        /// \brief Returns one long single string representation of the GPS simulation data point
        /// @return     A long single string representation of the GPS simulation data point
        std::string to_string(void) const;

        int16_t get_gps_rollover(void) const {parse_data_point(); return _gps_rollover;}
        int16_t get_gps_week(void) const {parse_data_point(); return _gps_week;}
        int32_t get_gps_sec_week(void) const {parse_data_point(); return _gps_sec_week;}
        double get_gps_frac_sec(void) const {parse_data_point(); return _gps_frac_sec;}
        double get_ECEF_x(void) const {parse_data_point(); return _ECEF[0];}
        double get_ECEF_y(void) const {parse_data_point(); return _ECEF[1];}
        double get_ECEF_z(void) const {parse_data_point(); return _ECEF[2];}
        double get_ECEF_vx(void) const {parse_data_point(); return _ECEF_vel[0];}
        double get_ECEF_vy(void) const {parse_data_point(); return _ECEF_vel[1];}
        double get_ECEF_vz(void) const {parse_data_point(); return _ECEF_vel[2];}
        double get_abs_time(void) const {parse_data_point(); return _abs_time;}
        //@}
    private:
        /// @name Private mutators
        //@{
        inline void parse_data_point(void) const {if (_not_parsed) do_parsing();}
        void do_parsing(void) const;
        //@}

        // Private data
        mutable Sim42DataPoint _dp;
        int16_t _sc;
        int16_t _gps;
        int16_t _leap_seconds;
        // mutable below so parsing can be on demand:
        mutable bool _not_parsed;
        mutable double _abs_time;
        mutable bool _gps_valid;
        mutable int16_t _gps_rollover;
        mutable int16_t _gps_week; // Unambiguous GPS Week
        mutable int32_t _gps_sec_week; // Integer seconds elapsed since the start of the GPS week
        mutable double _gps_frac_sec; // Fractions of a second beyond the integer seconds_of_week
        mutable std::vector<double> _ECEF, _ECEF_vel, _ECI, _ECI_vel; // m, m/s, m, m/s
        mutable double _gps_lat, _gps_lng, _gps_alt; // degrees, degrees, m above WGS-84 ellipsoid

    };

}

#endif

