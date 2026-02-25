#ifndef NOS3_GENERIC_EPSDATAPOINT_HPP
#define NOS3_GENERIC_EPSDATAPOINT_HPP

#include <boost/shared_ptr.hpp>
#include <sim_42data_point.hpp>

namespace Nos3
{
    /* Standard for a data point used transfer data between a data provider and a hardware model */
    class Generic_epsDataPoint : public SimIDataPoint
    {
    public:
        /* Constructors */
        Generic_epsDataPoint(double count);
        Generic_epsDataPoint(int16_t spacecraft, const boost::shared_ptr<Sim42DataPoint> dp);

        /* Accessors */
        /* Provide the hardware model a way to get the specific data out of the data point */
        std::string to_string(void) const;
        double      get_sun_vector_x(void) const {return _sun_vector[0];}
        double      get_sun_vector_y(void) const {return _sun_vector[1];}
        double      get_sun_vector_z(void) const {return _sun_vector[2];}
        bool        is_generic_eps_data_valid(void) const {return _generic_eps_data_is_valid;}
    
    private:
        /* Disallow these */
        Generic_epsDataPoint(void) {};
        Generic_epsDataPoint(const Generic_epsDataPoint&) {};
        ~Generic_epsDataPoint(void) {};

        /* Specific data you need to get from the data provider to the hardware model */
        /* You only get to this data through the accessors above */
        mutable bool   _generic_eps_data_is_valid;
        mutable double _sun_vector[3];
    };
}

#endif
