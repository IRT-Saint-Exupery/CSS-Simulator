#include <ids_data_provider.hpp>

namespace Nos3
{
    REGISTER_DATA_PROVIDER(IdsDataProvider,"IDS_PROVIDER");

    extern ItcLogger::Logger *sim_logger;

    IdsDataProvider::IdsDataProvider(const boost::property_tree::ptree& config) : SimIDataProvider(config)
    {
        sim_logger->trace("IdsDataProvider::IdsDataProvider:  Constructor executed");
        _request_count = 0;
    }

    boost::shared_ptr<SimIDataPoint> IdsDataProvider::get_data_point(void) const
    {
        sim_logger->trace("IdsDataProvider::get_data_point:  Executed");

        /* Prepare the provider data */
        _request_count++;

        /* Request a data point */
        SimIDataPoint *dp = new IdsDataPoint(_request_count);

        /* Return the data point */
        return boost::shared_ptr<SimIDataPoint>(dp);
    }
}
