#ifndef NOS3_GENERIC_CSSDATAPROVIDER_HPP
#define NOS3_GENERIC_CSSDATAPROVIDER_HPP

#include <boost/property_tree/xml_parser.hpp>
#include <ItcLogger/Logger.hpp>
#include <generic_css_data_point.hpp>
#include <sim_i_data_provider.hpp>

namespace Nos3
{
    class Generic_cssDataProvider : public SimIDataProvider
    {
    public:
        /* Constructors */
        Generic_cssDataProvider(const boost::property_tree::ptree& config);

        /* Accessors */
        boost::shared_ptr<SimIDataPoint> get_data_point(void) const;

    private:
        /* Disallow these */
        ~Generic_cssDataProvider(void) {};
        Generic_cssDataProvider& operator=(const Generic_cssDataProvider&) {return *this;};

        mutable double _request_count;
    };
}

#endif
