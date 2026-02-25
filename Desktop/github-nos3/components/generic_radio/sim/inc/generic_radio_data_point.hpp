#ifndef NOS3_GENERIC_RADIODATAPOINT_HPP
#define NOS3_GENERIC_RADIODATAPOINT_HPP

#include <boost/shared_ptr.hpp>
#include <sim_42data_point.hpp>

namespace Nos3
{
    /* Standard for a data point used transfer data between a data provider and a hardware model */
    class Generic_radioDataPoint : public SimIDataPoint
    {
    public:
        /* Constructors */
        Generic_radioDataPoint(double count);
        Generic_radioDataPoint(int16_t spacecraft, const boost::shared_ptr<Sim42DataPoint> dp);

        /* Accessors */
        /* Provide the hardware model a way to get the specific data out of the data point */
        std::string to_string(void) const;
        double      get_generic_radio_data_x(void) const {return _generic_radio_data[0];}
        double      get_generic_radio_data_y(void) const {return _generic_radio_data[1];}
        double      get_generic_radio_data_z(void) const {return _generic_radio_data[2];}
        bool        is_generic_radio_data_valid(void) const {return _generic_radio_data_is_valid;}
    
    private:
        /* Disallow these */
        Generic_radioDataPoint(void) {};
        Generic_radioDataPoint(const Generic_radioDataPoint&) {};
        ~Generic_radioDataPoint(void) {};

        /* Specific data you need to get from the data provider to the hardware model */
        /* You only get to this data through the accessors above */
        mutable bool   _generic_radio_data_is_valid;
        mutable double _generic_radio_data[3];
    };
}

#endif
