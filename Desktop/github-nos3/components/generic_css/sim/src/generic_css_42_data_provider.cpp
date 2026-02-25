#include <generic_css_42_data_provider.hpp>

namespace Nos3
{
    REGISTER_DATA_PROVIDER(Generic_css42DataProvider,"GENERIC_CSS_42_PROVIDER");

    extern ItcLogger::Logger *sim_logger;

    Generic_css42DataProvider::Generic_css42DataProvider(const boost::property_tree::ptree& config) : SimData42SocketProvider(config)
    {
        sim_logger->trace("Generic_css42DataProvider::Generic_css42DataProvider:  Constructor executed");

        connect_reader_thread_as_42_socket_client(
            config.get("simulator.hardware-model.data-provider.hostname", "192.168.100.42"),
            config.get("simulator.hardware-model.data-provider.port", 4227) );

        _sc = config.get("simulator.hardware-model.data-provider.spacecraft", 0);
        _scale_factor = config.get("simulator.hardware-model.data-provider.42-css-scale-factor", 1.0);
    }

    boost::shared_ptr<SimIDataPoint> Generic_css42DataProvider::get_data_point(void) const
    {
        sim_logger->trace("Generic_css42DataProvider::get_data_point:  Executed");

        /* Get the 42 data */
        const boost::shared_ptr<Sim42DataPoint> dp42 = boost::dynamic_pointer_cast<Sim42DataPoint>(SimData42SocketProvider::get_data_point());

        /* Prepare the specific data */
        SimIDataPoint *dp = new Generic_cssDataPoint(_scale_factor, _sc, dp42);

        return boost::shared_ptr<SimIDataPoint>(dp);
    }
}
