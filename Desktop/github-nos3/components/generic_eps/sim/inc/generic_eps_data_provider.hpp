#ifndef NOS3_GENERIC_EPSDATAPROVIDER_HPP
#define NOS3_GENERIC_EPSDATAPROVIDER_HPP

#include <boost/property_tree/xml_parser.hpp>
#include <ItcLogger/Logger.hpp>
#include <generic_eps_data_point.hpp>
#include <sim_i_data_provider.hpp>

namespace Nos3
{
    class Generic_epsDataProvider : public SimIDataProvider
    {
    public:
        /* Constructors */
        Generic_epsDataProvider(const boost::property_tree::ptree& config);

        /* Accessors */
        boost::shared_ptr<SimIDataPoint> get_data_point(void) const;

    private:
        /* Disallow these */
        ~Generic_epsDataProvider(void) {};
        Generic_epsDataProvider& operator=(const Generic_epsDataProvider&) {return *this;};

        mutable double _request_count;
    };
}

#endif
