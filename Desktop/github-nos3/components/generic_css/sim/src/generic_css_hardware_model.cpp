#include <generic_css_hardware_model.hpp>

namespace Nos3
{
    REGISTER_HARDWARE_MODEL(Generic_cssHardwareModel,"GENERIC_CSS");

    extern ItcLogger::Logger *sim_logger;

    Generic_cssHardwareModel::Generic_cssHardwareModel(const boost::property_tree::ptree& config) : SimIHardwareModel(config), _enabled(0)
    {
        /* Get the NOS engine connection string */
        std::string connection_string = config.get("common.nos-connection-string", "tcp://192.168.100.11:12001"); 
        sim_logger->info("Generic_cssHardwareModel::Generic_cssHardwareModel:  NOS Engine connection string: %s.", connection_string.c_str());

        /* Get a data provider */
        std::string dp_name = config.get("simulator.hardware-model.data-provider.type", "GENERIC_CSS_42_PROVIDER");
        _generic_css_dp = SimDataProviderFactory::Instance().Create(dp_name, config);
        sim_logger->info("Generic_cssHardwareModel::Generic_cssHardwareModel:  Data provider %s created.", dp_name.c_str());

        /* Get on a protocol bus */
        /* Note: Initialized defaults in case value not found in config file */
       
        std::string bus_name = "i2c_2";
        int i2c_bus_address = 64; // 0x40
        bus_name = config.get("simulator.hardware-model.i2c.bus", "i2c_2");
        i2c_bus_address = config.get("simulator.hardware-model.i2c.address", 64);

        _i2c_slave_connection = new I2CSlaveConnection(this, i2c_bus_address, connection_string, bus_name);
        sim_logger->info("Generic_cssHardwareModel::Generic_cssHardwareModel:  Now on i2c bus name %s, address %d.", bus_name.c_str(), i2c_bus_address);
    
        // Get on the command bus
        std::string time_bus_name = "command";
        if (config.get_child_optional("hardware-model.connections")) 
        {
            // Loop through the connections for the hardware model
            BOOST_FOREACH(const boost::property_tree::ptree::value_type &v, config.get_child("hardware-model.connections"))
            {
                // v.first is the name of the child
                // v.second is the child tree
                if (v.second.get("type", "").compare("time") == 0) // 
                {
                    time_bus_name = v.second.get("bus-name", "command");
                    // Found it... don't need to go through any more items
                    break; 
                }
            }
        }
        _time_bus.reset(new NosEngine::Client::Bus(_hub, connection_string, time_bus_name));
        sim_logger->info("Generic_cssHardwareModel::Generic_cssHardwareModel:  Now on time bus named %s.", time_bus_name.c_str());
        /* Construction complete */
        sim_logger->info("Generic_cssHardwareModel::Generic_cssHardwareModel:  Construction complete.");
    }

    Generic_cssHardwareModel::~Generic_cssHardwareModel(void)
    {        
        /* Close the protocol bus */
        /* I2C cleanup currently unsupported */

        /* Clean up the data provider */
        delete _generic_css_dp;
        _generic_css_dp = nullptr;

        /* The bus will clean up the time node */
    }

    /* Automagically set up by the base class to be called */
    void Generic_cssHardwareModel::command_callback(NosEngine::Common::Message msg)
    {
        // Get the data out of the message
        NosEngine::Common::DataBufferOverlay dbf(const_cast<NosEngine::Utility::Buffer&>(msg.buffer));
        sim_logger->info("Generic_cssHardwareModel::command_callback:  Received command: %s.", dbf.data);

        // Do something with the data
        std::string command = dbf.data;
        std::string response = "Generic_cssHardwareModel::command_callback:  INVALID COMMAND! (Try HELP)";
        boost::to_upper(command);
        if (command.compare(0,4,"HELP") == 0) 
        {
            response = "Generic_cssHardwareModel::command_callback: Valid commands are HELP, ENABLE, DISABLE, or STOP";
        }
        else if (command.compare(0,6,"ENABLE") == 0) 
        {
            _enabled = GENERIC_CSS_SIM_SUCCESS;
            response = "Generic_cssHardwareModel::command_callback:  Enabled";
        }
        else if (command.compare(0,7,"DISABLE") == 0) 
        {
            _enabled = GENERIC_CSS_SIM_ERROR;
            response = "Generic_cssHardwareModel::command_callback:  Disabled";
        }
        else if (command.compare(0,4,"STOP") == 0) 
        {
            _keep_running = false;
            response = "Generic_cssHardwareModel::command_callback:  Stopping";
        }

        // Send a reply
        sim_logger->info("Generic_cssHardwareModel::command_callback:  Sending reply: %s.", response.c_str());
        _command_node->send_reply_message_async(msg, response.size(), response.c_str());
    }

    void Generic_cssHardwareModel::create_generic_css_data(std::vector<uint8_t>& out_data)
    {
        boost::shared_ptr<Generic_cssDataPoint> data_point = boost::dynamic_pointer_cast<Generic_cssDataPoint>(_generic_css_dp->get_data_point());
        std::vector<float> cssValues = data_point->getValues();

        /* Prepare data size */
        out_data.clear();
        out_data.resize(12, 0x00);

        sim_logger->debug("Generic_cssHardwareModel::create_generic_css_data:  Creating data, enabled=%d", _enabled);
        if (_enabled == GENERIC_CSS_SIM_SUCCESS) 
        {
            out_data[0] = ((uint16_t) (cssValues[0] * 1000) & 0xFF00) >> 8;
            out_data[1] = ((uint16_t) (cssValues[0] * 1000) & 0x00FF);
            out_data[2] = ((uint16_t) (cssValues[1] * 1000) & 0xFF00) >> 8;
            out_data[3] = ((uint16_t) (cssValues[1] * 1000) & 0x00FF);
            out_data[4] = ((uint16_t) (cssValues[2] * 1000) & 0xFF00) >> 8;
            out_data[5] = ((uint16_t) (cssValues[2] * 1000) & 0x00FF);
            out_data[6] = ((uint16_t) (cssValues[3] * 1000) & 0xFF00) >> 8;
            out_data[7] = ((uint16_t) (cssValues[3] * 1000) & 0x00FF);
            out_data[8] = ((uint16_t) (cssValues[4] * 1000) & 0xFF00) >> 8;
            out_data[9] = ((uint16_t) (cssValues[4] * 1000) & 0x00FF);
            out_data[10] = ((uint16_t) (cssValues[5] * 1000) & 0xFF00) >> 8;
            out_data[11] = ((uint16_t) (cssValues[5] * 1000) & 0x00FF);

            /* Debugging print
            sim_logger->debug("  css[0] = %f", cssValues[0]);
            sim_logger->debug("    Converted = 0x%02x, 0x%02x", out_data[0], out_data[1]);
            */ 
        }
    }

    I2CSlaveConnection::I2CSlaveConnection(Generic_cssHardwareModel* hm, 
        int bus_address, std::string connection_string, std::string bus_name)
        : NosEngine::I2C::I2CSlave(bus_address, connection_string, bus_name)
    {
        _hardware_model = hm;
    }

    size_t I2CSlaveConnection::i2c_read(uint8_t *rbuf, size_t rlen)
    {
        _hardware_model->create_generic_css_data(_i2c_out_data);
        sim_logger->debug("i2c_read: %s", SimIHardwareModel::uint8_vector_to_hex_string(_i2c_out_data).c_str());
        for (size_t i = 0; i < rlen; i++) 
        {
            if (i < sizeof(_i2c_out_data))
            {
                rbuf[i] = _i2c_out_data[i];
            }
            else
            {
                rbuf[i] = 0x00;
            }
        }
        return rlen;
    }

    size_t I2CSlaveConnection::i2c_write(const uint8_t *wbuf, size_t wlen)
    {
        std::vector<uint8_t> in_data(wbuf, wbuf + wlen);
        sim_logger->debug("i2c_write: %s", SimIHardwareModel::uint8_vector_to_hex_string(in_data).c_str()); // log data
        sim_logger->error("CSS sim does not support I2C write!");
        return wlen;
    }
}
