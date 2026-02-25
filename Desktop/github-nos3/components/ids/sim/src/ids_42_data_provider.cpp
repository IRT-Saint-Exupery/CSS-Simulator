#include <ids_42_data_provider.hpp>

namespace Nos3
{
    REGISTER_DATA_PROVIDER(Ids42DataProvider,"IDS_42_PROVIDER");

    extern ItcLogger::Logger *sim_logger;

    Ids42DataProvider::Ids42DataProvider(const boost::property_tree::ptree& config) : SimData42SocketProvider(config)
    {
        sim_logger->trace("Ids42DataProvider::Ids42DataProvider:  Constructor executed");

        connect_reader_thread_as_42_socket_client(
            config.get("simulator.hardware-model.data-provider.hostname", "192.168.100.6"),
            config.get("simulator.hardware-model.data-provider.port", 4243) );

        _sc = config.get("simulator.hardware-model.data-provider.spacecraft", 0);
    }

    boost::shared_ptr<SimIDataPoint> Ids42DataProvider::get_data_point(void) const
    {
        sim_logger->trace("Ids42DataProvider::get_data_point:  Executed");

        /* Get the 42 data */
        const boost::shared_ptr<Sim42DataPoint> dp42 = boost::dynamic_pointer_cast<Sim42DataPoint>(SimData42SocketProvider::get_data_point());

        /* Prepare the specific data */
        SimIDataPoint *dp = new IdsDataPoint(_sc, dp42);

        return boost::shared_ptr<SimIDataPoint>(dp);
    }
}
