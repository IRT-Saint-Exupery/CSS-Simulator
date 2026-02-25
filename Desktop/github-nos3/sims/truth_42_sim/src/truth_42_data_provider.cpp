#include <ItcLogger/Logger.hpp>

#include <truth_42_data_point.hpp>

#include <truth_42_data_provider.hpp>

namespace Nos3
{
    REGISTER_DATA_PROVIDER(Truth42DataProvider,"TRUTH42PROVIDER");

    extern ItcLogger::Logger *sim_logger;

    Truth42DataProvider::Truth42DataProvider(const boost::property_tree::ptree& config) : SimData42SocketProvider(config)
    {
        sim_logger->trace("Truth42DataProvider::Truth42DataProvider:  Constructor executed");

        connect_reader_thread_as_42_socket_client(
            config.get("simulator.hardware-model.data-provider.hostname", "192.168.100.42"),
            config.get("simulator.hardware-model.data-provider.port", 4242) );

        _orb = config.get("simulator.hardware-model.data-provider.orbit", 0);
        _sc  = config.get("simulator.hardware-model.data-provider.spacecraft", 0);
    }

    boost::shared_ptr<SimIDataPoint> Truth42DataProvider::get_data_point(void) const
    {
        sim_logger->trace("Truth42DataProvider::get_data_point:  Executed");

        // Get the 42 data
        const boost::shared_ptr<Sim42DataPoint> dp42 =
            boost::dynamic_pointer_cast<Sim42DataPoint>(SimData42SocketProvider::get_data_point());

        // vvv Prepare the specific data... this may need changed in your use case
        SimIDataPoint *dp = new Truth42DataPoint(_orb, _sc, dp42);

        return boost::shared_ptr<SimIDataPoint>(dp);
    }
}
