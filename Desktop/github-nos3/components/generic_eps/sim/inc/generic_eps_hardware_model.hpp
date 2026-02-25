#ifndef NOS3_GENERIC_EPSHARDWAREMODEL_HPP
#define NOS3_GENERIC_EPSHARDWAREMODEL_HPP

/*
** Includes
*/
#include <map>

#include <boost/tuple/tuple.hpp>
#include <boost/property_tree/ptree.hpp>

#include <Client/Bus.hpp>
#include <Client/DataNode.hpp>
#include <I2C/Client/I2CSlave.hpp>

#include <sim_i_data_provider.hpp>
#include <generic_eps_data_point.hpp>
#include <sim_i_hardware_model.hpp>

#include <string>


/*
** Defines
*/
#define GENERIC_EPS_SIM_SUCCESS 0
#define GENERIC_EPS_SIM_ERROR   1


/*
** Namespace
*/
namespace Nos3
{
    /* Standard for a hardware model */
    class Generic_epsHardwareModel : public SimIHardwareModel
    {
    public:
        /* Constructor and destructor */
        Generic_epsHardwareModel(const boost::property_tree::ptree& config);
        ~Generic_epsHardwareModel(void);
        std::uint8_t determine_i2c_response_for_request(const std::vector<uint8_t>& in_data, std::vector<uint8_t>& out_data); 

    private:
        /* Private helper methods */
        void command_callback(NosEngine::Common::Message msg); /* Handle backdoor commands and time tick to the simulator */
        void eps_switch_update(const std::uint8_t sw_num, uint8_t sw_status);
        std::uint8_t generic_eps_crc8(const std::vector<uint8_t>& crc_data, std::uint32_t crc_size);
        void create_generic_eps_data(std::vector<uint8_t>& out_data); 
        void update_battery_values(void);
        
        /* Private data members */
        class I2CSlaveConnection*                           _i2c_slave_connection;
        
        std::string                                         _command_bus_name;
        std::unique_ptr<NosEngine::Client::Bus>             _command_bus; /* Standard */

        SimIDataProvider*                                   _generic_eps_dp;

        /* Time Bus */
        std::unique_ptr<NosEngine::Client::Bus>             _time_bus;
        
        /* Internal switch data */
        struct Init_Switch_State
        {
            std::string   _node_name;
            std::string   _voltage;
            std::string   _current;
            std::string   _state;
        };

        struct EPS_Rail
        {
            std::uint16_t _voltage;
            std::uint16_t _current;
            std::uint16_t _status;
            std::uint16_t _temperature;
            double        _battery_watthrs;
        };

        Init_Switch_State                                   _init_switch[8];
        EPS_Rail                                            _switch[8];
        EPS_Rail                                            _bus[5];
                                                                /*
                                                                0 - Battery
                                                                1 - 3.3v
                                                                2 - 5.0v
                                                                3 - 12.0v
                                                                4 - Solar Array
                                                                */

        std::uint8_t                                        _enabled;
        std::uint8_t                                        _initialized_other_sims;
        
        double                                              _power_per_panel;
        double                                              _max_battery;
        double                                              _nominal_batt_voltage;
    };

    class I2CSlaveConnection : public NosEngine::I2C::I2CSlave
    {
    public:
        I2CSlaveConnection(Generic_epsHardwareModel* hm, int bus_address, std::string connection_string, std::string bus_name);
        size_t i2c_read(uint8_t *rbuf, size_t rlen);
        size_t i2c_write(const uint8_t *wbuf, size_t wlen);
    private:
        Generic_epsHardwareModel* _hardware_model;
        std::uint8_t _i2c_read_valid;
        std::vector<uint8_t> _i2c_out_data;
    };
}

#endif
