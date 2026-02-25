#ifndef NOS3_IDS42DATAPROVIDER_HPP
#define NOS3_IDS42DATAPROVIDER_HPP

#include <boost/property_tree/ptree.hpp>
#include <ItcLogger/Logger.hpp>
#include <ids_data_point.hpp>
#include <sim_data_42socket_provider.hpp>

namespace Nos3
{
    /* Standard for a 42 data provider */
    class Ids42DataProvider : public SimData42SocketProvider
    {
    public:
        /* Constructors */
        Ids42DataProvider(const boost::property_tree::ptree& config);

        /* Accessors */
        boost::shared_ptr<SimIDataPoint> get_data_point(void) const;

    private:
        /* Disallow these */
        ~Ids42DataProvider(void) {};
        Ids42DataProvider& operator=(const Ids42DataProvider&) {return *this;};

        int16_t _sc;  /* Which spacecraft number to parse out of 42 data */
    };
}

#endif
