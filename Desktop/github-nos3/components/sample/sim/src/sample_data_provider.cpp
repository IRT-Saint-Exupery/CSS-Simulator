#include <sample_data_provider.hpp>

namespace Nos3
{
    REGISTER_DATA_PROVIDER(SampleDataProvider,"SAMPLE_PROVIDER");

    extern ItcLogger::Logger *sim_logger;

    SampleDataProvider::SampleDataProvider(const boost::property_tree::ptree& config) : SimIDataProvider(config)
    {
        sim_logger->trace("SampleDataProvider::SampleDataProvider:  Constructor executed");
        _request_count = 0;
    }

    boost::shared_ptr<SimIDataPoint> SampleDataProvider::get_data_point(void) const
    {
        sim_logger->trace("SampleDataProvider::get_data_point:  Executed");

        /* Prepare the provider data */
        _request_count++;

        /* Request a data point */
        SimIDataPoint *dp = new SampleDataPoint(_request_count);

        /* Return the data point */
        return boost::shared_ptr<SimIDataPoint>(dp);
    }
}
