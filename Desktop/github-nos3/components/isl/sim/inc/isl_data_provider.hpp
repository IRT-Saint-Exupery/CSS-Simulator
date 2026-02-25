#ifndef NOS3_ISLDATAPROVIDER_HPP
#define NOS3_ISLDATAPROVIDER_HPP

#include <boost/property_tree/xml_parser.hpp>
#include <ItcLogger/Logger.hpp>
#include <isl_data_point.hpp>
#include <sim_i_data_provider.hpp>

namespace Nos3
{
    class IslDataProvider : public SimIDataProvider
    {
    public:
        /* Constructors */
        IslDataProvider(const boost::property_tree::ptree& config);

        /* Accessors */
        boost::shared_ptr<SimIDataPoint> get_data_point(void) const;

    private:
        /* Disallow these */
        ~IslDataProvider(void) {};
        IslDataProvider& operator=(const IslDataProvider&) {return *this;};

        mutable double _request_count;
    };
}

#endif
