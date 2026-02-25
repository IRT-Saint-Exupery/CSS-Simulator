#include <generic_eps_data_provider.hpp>

namespace Nos3
{
    REGISTER_DATA_PROVIDER(Generic_epsDataProvider,"GENERIC_EPS_PROVIDER");

    extern ItcLogger::Logger *sim_logger;

    Generic_epsDataProvider::Generic_epsDataProvider(const boost::property_tree::ptree& config) : SimIDataProvider(config)
    {
        sim_logger->trace("Generic_epsDataProvider::Generic_epsDataProvider:  Constructor executed");
        _request_count = 0;
    }

    boost::shared_ptr<SimIDataPoint> Generic_epsDataProvider::get_data_point(void) const
    {
        sim_logger->trace("Generic_epsDataProvider::get_data_point:  Executed");

        /* Prepare the provider data */
        _request_count++;

        /* Request a data point */
        SimIDataPoint *dp = new Generic_epsDataPoint(_request_count);

        /* Return the data point */
        return boost::shared_ptr<SimIDataPoint>(dp);
    }
}
