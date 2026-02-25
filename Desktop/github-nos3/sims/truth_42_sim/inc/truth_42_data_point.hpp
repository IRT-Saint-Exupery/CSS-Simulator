#ifndef NOS3_TRUTH42DATAPOINT_HPP
#define NOS3_TRUTH42DATAPOINT_HPP

#include <boost/shared_ptr.hpp>

#include <sim_42data_point.hpp>

namespace Nos3
{
    // vvv This is pretty standard for a data point (if one is needed to transfer data between a data provider and a hardware model for your sim)
    class Truth42DataPoint : public SimIDataPoint
    {
    public:
        // Constructor
        Truth42DataPoint(int16_t orbit, int16_t spacecraft, const boost::shared_ptr<Sim42DataPoint> dp);

        // Accessors
        std::string to_string(void) const;

        int16_t get_year(void) const {parse_data_point(); return _year;}
        int16_t get_doy(void) const {parse_data_point(); return _doy;}
        int16_t get_month(void) const {parse_data_point(); return _month;}
        int16_t get_day(void) const {parse_data_point(); return _day;}
        int16_t get_utc_hh(void) const {parse_data_point(); return _utc_hh;}
        int16_t get_utc_mm(void) const {parse_data_point(); return _utc_mm;}
        double  get_utc_ss(void) const {parse_data_point(); return _utc_ss;}
        std::vector<double> get_pos(void) const {parse_data_point(); return _pos;} // ECI position (meters=m)
        std::vector<double> get_vel(void) const {parse_data_point(); return _vel;} // ECI velocity (meters/second=m/s)
        std::vector<double> get_svb(void)  const {parse_data_point(); return _svb;}  // sun unit vector in body frame (unitless)
        std::vector<double> get_bvb(void)  const {parse_data_point(); return _bvb;}  // magnetic field vector in body frame (Teslas=T)
        std::vector<double> get_Hvb(void)  const {parse_data_point(); return _Hvb;}  // angular momentum in body frame (Newton-meter-seconds=Nms)
        std::vector<double> get_wn(void)   const {parse_data_point(); return _wn;}   // body 0 angular velocity of body in body frame (radians/second=1/s)
        std::vector<double> get_qn(void)   const {parse_data_point(); return _qn;}   // body 0 quaternion in inertial frame (unitless)

    private:
        // Disallow the big 4
        Truth42DataPoint(void) {};
        Truth42DataPoint(const Truth42DataPoint&) {};
        Truth42DataPoint& operator=(const Truth42DataPoint&) {return *this;};
        ~Truth42DataPoint(void) {};

        /// @name Private mutators
        //@{
        inline void parse_data_point(void) const {if (_not_parsed) do_parsing();}
        void do_parsing(void) const;
        //@}

        // Private data
        Sim42DataPoint _dp;
        int16_t _orb;
        int16_t _sc;
        // mutable below so parsing can be on demand:
        mutable bool _not_parsed;
        mutable int16_t _year;
        mutable int16_t _doy; // day of year
        mutable int16_t _month;
        mutable int16_t _day;
        mutable int16_t _utc_hh;
        mutable int16_t _utc_mm;
        mutable double  _utc_ss;
        mutable std::vector<double> _pos;
        mutable std::vector<double> _vel;
        mutable std::vector<double> _svb;
        mutable std::vector<double> _bvb;
        mutable std::vector<double> _Hvb;
        mutable std::vector<double> _wn;
        mutable std::vector<double> _qn;
    };
}

#endif
