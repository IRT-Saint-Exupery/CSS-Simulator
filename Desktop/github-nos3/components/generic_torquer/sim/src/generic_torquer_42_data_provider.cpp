#include <generic_torquer_42_data_provider.hpp>

namespace Nos3
{
    REGISTER_DATA_PROVIDER(Generic_torquer42DataProvider,"GENERIC_TORQUER_42_PROVIDER");

    extern ItcLogger::Logger *sim_logger;

    Generic_torquer42DataProvider::Generic_torquer42DataProvider(const boost::property_tree::ptree& config) : SimData42SocketProvider(config)
    {
        sim_logger->trace("Generic_torquer42DataProvider::Generic_torquer42DataProvider:  Constructor executed");
        _sc = config.get("simulator.hardware-model.data-provider.spacecraft", 0);
    }

    void Generic_torquer42DataProvider::cmd_torque(int trq_num, double trq_value)
    {
        char buffer [56];
        sprintf (buffer, "SC[%i].AC.MTB[%i].Mcmd = %lf\n[EOF]\n", _sc, trq_num, trq_value);
        sim_logger->debug("Generic_torquer42DataProvider::cmd_torque:  buffer = %s\n", buffer);

        send_command_to_socket(std::string(buffer));
    }
}
