#include <isl_data_provider.hpp>

namespace Nos3
{
    REGISTER_DATA_PROVIDER(IslDataProvider,"ISL_PROVIDER");

    extern ItcLogger::Logger *sim_logger;

    IslDataProvider::IslDataProvider(const boost::property_tree::ptree& config) : SimIDataProvider(config)
    {
        sim_logger->trace("IslDataProvider::IslDataProvider:  Constructor executed");
        _request_count = 0;
    }

    boost::shared_ptr<SimIDataPoint> IslDataProvider::get_data_point(void) const
    {
        sim_logger->trace("IslDataProvider::get_data_point:  Executed");

        /* Prepare the provider data */
        _request_count++;

        /* Request a data point */
        SimIDataPoint *dp = new IslDataPoint(_request_count);

        /* Return the data point */
        return boost::shared_ptr<SimIDataPoint>(dp);
    }
}
