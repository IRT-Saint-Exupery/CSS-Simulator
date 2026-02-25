#ifndef NOS3_ISL42DATAPROVIDER_HPP
#define NOS3_ISL42DATAPROVIDER_HPP

#include <boost/property_tree/ptree.hpp>
#include <ItcLogger/Logger.hpp>
#include <isl_data_point.hpp>
#include <sim_data_42socket_provider.hpp>

namespace Nos3
{
    /* Standard for a 42 data provider */
    class Isl42DataProvider : public SimData42SocketProvider
    {
    public:
        /* Constructors */
        Isl42DataProvider(const boost::property_tree::ptree& config);

        /* Accessors */
        boost::shared_ptr<SimIDataPoint> get_data_point(void) const;

    private:
        /* Disallow these */
        ~Isl42DataProvider(void) {};
        Isl42DataProvider& operator=(const Isl42DataProvider&) {return *this;};

        int16_t _sc;  /* Which spacecraft number to parse out of 42 data */
    };
}

#endif
