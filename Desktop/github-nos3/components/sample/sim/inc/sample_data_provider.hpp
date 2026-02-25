#ifndef NOS3_SAMPLEDATAPROVIDER_HPP
#define NOS3_SAMPLEDATAPROVIDER_HPP

#include <boost/property_tree/xml_parser.hpp>
#include <ItcLogger/Logger.hpp>
#include <sample_data_point.hpp>
#include <sim_i_data_provider.hpp>

namespace Nos3
{
    class SampleDataProvider : public SimIDataProvider
    {
    public:
        /* Constructors */
        SampleDataProvider(const boost::property_tree::ptree& config);

        /* Accessors */
        boost::shared_ptr<SimIDataPoint> get_data_point(void) const;

    private:
        /* Disallow these */
        ~SampleDataProvider(void) {};
        SampleDataProvider& operator=(const SampleDataProvider&) {return *this;};

        mutable double _request_count;
    };
}

#endif
