#ifndef NOS3_IDSDATAPROVIDER_HPP
#define NOS3_IDSDATAPROVIDER_HPP

#include <boost/property_tree/xml_parser.hpp>
#include <ItcLogger/Logger.hpp>
#include <ids_data_point.hpp>
#include <sim_i_data_provider.hpp>

namespace Nos3
{
    class IdsDataProvider : public SimIDataProvider
    {
    public:
        /* Constructors */
        IdsDataProvider(const boost::property_tree::ptree& config);

        /* Accessors */
        boost::shared_ptr<SimIDataPoint> get_data_point(void) const;

    private:
        /* Disallow these */
        ~IdsDataProvider(void) {};
        IdsDataProvider& operator=(const IdsDataProvider&) {return *this;};

        mutable double _request_count;
    };
}

#endif
