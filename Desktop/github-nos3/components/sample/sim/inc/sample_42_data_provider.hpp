#ifndef NOS3_SAMPLE42DATAPROVIDER_HPP
#define NOS3_SAMPLE42DATAPROVIDER_HPP

#include <boost/property_tree/ptree.hpp>
#include <ItcLogger/Logger.hpp>
#include <sample_data_point.hpp>
#include <sim_data_42socket_provider.hpp>

namespace Nos3
{
    /* Standard for a 42 data provider */
    class Sample42DataProvider : public SimData42SocketProvider
    {
    public:
        /* Constructors */
        Sample42DataProvider(const boost::property_tree::ptree& config);

        /* Accessors */
        boost::shared_ptr<SimIDataPoint> get_data_point(void) const;

    private:
        /* Disallow these */
        ~Sample42DataProvider(void) {};
        Sample42DataProvider& operator=(const Sample42DataProvider&) {return *this;};

        int16_t _sc;  /* Which spacecraft number to parse out of 42 data */
    };
}

#endif
