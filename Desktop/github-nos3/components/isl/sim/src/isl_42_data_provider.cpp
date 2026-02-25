#include <isl_42_data_provider.hpp>

namespace Nos3
{
    REGISTER_DATA_PROVIDER(Isl42DataProvider,"ISL_42_PROVIDER");

    extern ItcLogger::Logger *sim_logger;

    Isl42DataProvider::Isl42DataProvider(const boost::property_tree::ptree& config) : SimData42SocketProvider(config)
    {
        sim_logger->trace("Isl42DataProvider::Isl42DataProvider:  Constructor executed");

        connect_reader_thread_as_42_socket_client(
            config.get("simulator.hardware-model.data-provider.hostname", "192.168.100.42"),
            config.get("simulator.hardware-model.data-provider.port", 4240) );

        _sc = config.get("simulator.hardware-model.data-provider.spacecraft", 0);
    }

    boost::shared_ptr<SimIDataPoint> Isl42DataProvider::get_data_point(void) const
    {
        sim_logger->trace("Isl42DataProvider::get_data_point:  Executed");

        /* Get the 42 data */
        const boost::shared_ptr<Sim42DataPoint> dp42 = boost::dynamic_pointer_cast<Sim42DataPoint>(SimData42SocketProvider::get_data_point());

        /* Prepare the specific data */
        SimIDataPoint *dp = new IslDataPoint(_sc, dp42);

        return boost::shared_ptr<SimIDataPoint>(dp);
    }
}
