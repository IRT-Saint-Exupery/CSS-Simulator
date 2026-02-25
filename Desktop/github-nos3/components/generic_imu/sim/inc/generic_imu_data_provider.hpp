#ifndef NOS3_GENERIC_IMUDATAPROVIDER_HPP
#define NOS3_GENERIC_IMUDATAPROVIDER_HPP

#include <boost/property_tree/xml_parser.hpp>
#include <ItcLogger/Logger.hpp>
#include <generic_imu_data_point.hpp>
#include <sim_i_data_provider.hpp>

namespace Nos3
{
    class Generic_imuDataProvider : public SimIDataProvider
    {
    public:
        /* Constructors */
        Generic_imuDataProvider(const boost::property_tree::ptree& config);

        /* Accessors */
        boost::shared_ptr<SimIDataPoint> get_data_point(void) const;

    private:
        /* Disallow these */
        ~Generic_imuDataProvider(void) {};
        Generic_imuDataProvider& operator=(const Generic_imuDataProvider&) {return *this;};

        mutable double _request_count;
    };
}

#endif
