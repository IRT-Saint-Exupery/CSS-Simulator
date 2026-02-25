#include <generic_mag_hardware_model.hpp>

namespace Nos3
{
    REGISTER_HARDWARE_MODEL(Generic_magHardwareModel,"GENERIC_MAG");

    extern ItcLogger::Logger *sim_logger;

    Generic_magHardwareModel::Generic_magHardwareModel(const boost::property_tree::ptree& config) : SimIHardwareModel(config), _enabled(0)
    {
        /* Get the NOS engine connection string */
        std::string connection_string = config.get("common.nos-connection-string", "tcp://192.168.100.11:12001"); 
        sim_logger->info("Generic_magHardwareModel::Generic_magHardwareModel:  NOS Engine connection string: %s.", connection_string.c_str());
        /* Get a data provider */
        std::string dp_name = config.get("simulator.hardware-model.data-provider.type", "GENERIC_MAG_42_PROVIDER");
        _generic_mag_dp = SimDataProviderFactory::Instance().Create(dp_name, config);
        sim_logger->info("Generic_magHardwareModel::Generic_magHardwareModel:  Data provider %s created.", dp_name.c_str());

        /* Get on a protocol bus */
        /* Note: Initialized defaults in case value not found in config file */
        std::string bus_name = "spi_2";
        int chip_select = 2;
        if (config.get_child_optional("simulator.hardware-model.connections")) 
        {
            /* Loop through the connections for hardware model */
            BOOST_FOREACH(const boost::property_tree::ptree::value_type &v, config.get_child("simulator.hardware-model.connections"))
            {
                /* v.second is the child tree (v.first is the name of the child) */
                if (v.second.get("type", "").compare("spi") == 0)
                {
                    /* Configuration found */
                    bus_name = v.second.get("bus-name", bus_name);
                    chip_select = v.second.get("chip-select", chip_select);
                    break;
                }
            }
        }
        _spi_slave_connection = new SpiSlaveConnection(this, chip_select, connection_string, bus_name);
        sim_logger->info("Generic_magHardwareModel::Generic_magHardwareModel:  Now on SPI bus name %s, chip select %d.", bus_name.c_str(), chip_select);
    
        /* Get on the command bus*/
        std::string time_bus_name = "command";
        if (config.get_child_optional("hardware-model.connections")) 
        {
            /* Loop through the connections for the hardware model */
            BOOST_FOREACH(const boost::property_tree::ptree::value_type &v, config.get_child("hardware-model.connections"))
            {
                /* v.first is the name of the child */
                /* v.second is the child tree */
                if (v.second.get("type", "").compare("time") == 0) // 
                {
                    time_bus_name = v.second.get("bus-name", "command");
                    /* Found it... don't need to go through any more items*/
                    break; 
                }
            }
        }
        _time_bus.reset(new NosEngine::Client::Bus(_hub, connection_string, time_bus_name));
        sim_logger->info("Generic_magHardwareModel::Generic_magHardwareModel:  Now on time bus named %s.", time_bus_name.c_str());

        /* Construction complete */
        sim_logger->info("Generic_magHardwareModel::Generic_magHardwareModel:  Construction complete.");
    }


    Generic_magHardwareModel::~Generic_magHardwareModel(void)
    {        
        /* Close the protocol bus */
        delete _spi_slave_connection;
        _spi_slave_connection = nullptr;

        /* Clean up the data provider */
        delete _generic_mag_dp;
        _generic_mag_dp = nullptr;

        /* The bus will clean up the time node */
    }


    /* Automagically set up by the base class to be called */
    void Generic_magHardwareModel::command_callback(NosEngine::Common::Message msg)
    {
        /* Get the data out of the message */
        NosEngine::Common::DataBufferOverlay dbf(const_cast<NosEngine::Utility::Buffer&>(msg.buffer));
        sim_logger->info("Generic_magHardwareModel::command_callback:  Received command: %s.", dbf.data);

        /* Do something with the data */
        std::string command = dbf.data;
        std::string response = "Generic_magHardwareModel::command_callback:  INVALID COMMAND! (Try HELP)";
        boost::to_upper(command);
        if (command.compare(0, 4, "HELP") == 0) 
        {
            response = "Generic_magHardwareModel::command_callback: Valid commands are HELP, ENABLE, DISABLE, or STOP";
        }
        else if (command.compare(0, 6, "ENABLE") == 0) 
        {
            _enabled = GENERIC_MAG_SIM_SUCCESS;
            response = "Generic_magHardwareModel::command_callback:  Enabled";
        }
        else if (command.compare(0, 7, "DISABLE") == 0) 
        {
            _enabled = GENERIC_MAG_SIM_ERROR;
            response = "Generic_magHardwareModel::command_callback:  Disabled";
        }
        else if (command.compare(0, 4, "STOP") == 0) 
        {
            _keep_running = false;
            response = "Generic_magHardwareModel::command_callback:  Stopping";
        }

        /* Send a reply */
        sim_logger->info("Generic_magHardwareModel::command_callback:  Sending reply: %s.", response.c_str());
        _command_node->send_reply_message_async(msg, response.size(), response.c_str());
    }

    /* Custom function to prepare the Generic_mag Data */
    void Generic_magHardwareModel::prepare_generic_mag_data_from_42(std::vector<uint8_t>& out_data)
    {
        boost::shared_ptr<Generic_magDataPoint> data_point = boost::dynamic_pointer_cast<Generic_magDataPoint>(_generic_mag_dp->get_data_point());
        std::vector<float> magValues = data_point->getValues();

        /* Prepare data size */
        out_data.clear();
        out_data.resize(16, 0x00);

        /* Streaming data header - 0xDEAD */
        out_data[0] = 0xDE;
        out_data[1] = 0xAD;
        out_data[2] = 0xBE;
        out_data[3] = 0xEF;

        /* sim_logger->debug("Generic_magHardwareModel::prepare_generic_mag_data_from_42:  Creating data, enabled=%d", _enabled); */
        if (_enabled == GENERIC_MAG_SIM_SUCCESS) 
        {
            out_data[4] = ((uint32_t) ((magValues[0] * _nano_conversion) * _mag_conv + _mag_conv * _mag_range) & 0xFF000000) >> 24;
            out_data[5] = ((uint32_t) ((magValues[0] * _nano_conversion) * _mag_conv + _mag_conv * _mag_range) & 0x00FF0000) >> 16;
            out_data[6] = ((uint32_t) ((magValues[0] * _nano_conversion) * _mag_conv + _mag_conv * _mag_range) & 0x0000FF00) >> 8;
            out_data[7] = ((uint32_t) ((magValues[0] * _nano_conversion) * _mag_conv + _mag_conv * _mag_range) & 0x000000FF);
            out_data[8] = ((uint32_t) ((magValues[1] * _nano_conversion) * _mag_conv + _mag_conv * _mag_range) & 0xFF000000) >> 24;
            out_data[9] = ((uint32_t) ((magValues[1] * _nano_conversion) * _mag_conv + _mag_conv * _mag_range) & 0x00FF0000) >> 16;
            out_data[10] = ((uint32_t) ((magValues[1] * _nano_conversion) * _mag_conv + _mag_conv * _mag_range) & 0x0000FF00) >> 8;
            out_data[11] = ((uint32_t) ((magValues[1] * _nano_conversion) * _mag_conv + _mag_conv * _mag_range) & 0x000000FF);
            out_data[12] = ((uint32_t) ((magValues[2] * _nano_conversion) * _mag_conv + _mag_conv * _mag_range) & 0xFF000000) >> 24;
            out_data[13] = ((uint32_t) ((magValues[2] * _nano_conversion) * _mag_conv + _mag_conv * _mag_range) & 0x00FF0000) >> 16;
            out_data[14] = ((uint32_t) ((magValues[2] * _nano_conversion) * _mag_conv + _mag_conv * _mag_range) & 0x0000FF00) >> 8;
            out_data[15] = ((uint32_t) ((magValues[2] * _nano_conversion) * _mag_conv + _mag_conv * _mag_range) & 0x000000FF);
            /*
            sim_logger->debug("Generic_magHardwareModel::prepare_generic_mag_data_from_42:  Creating data, data is below\nout_data[0]=0x%02x\tout_data[1]=0x%02x\tout_data[2]=0x%02x\tout_data[3]=0x%02x\nout_data[4]=0x%02x\tout_data[5]=0x%02x\tout_data[6]=0x%02x\tout_data[7]=0x%02x\nout_data[8]=0x%02x\tout_data[9]=0x%02x\tout_data[10]=0x%02x\tout_data[11]=0x%02x\nout_data[12]=0x%02x\tout_data[13]=0x%02x\tout_data[14]=0x%02x\tout_data[15]=0x%02x\n",
                out_data[0], out_data[1], out_data[2], out_data[3], 
                out_data[4], out_data[5], out_data[6], out_data[7], 
                out_data[8], out_data[9], out_data[10], out_data[11], 
                out_data[12], out_data[13], out_data[14], out_data[15]);
            */
        }
    }

    SpiSlaveConnection::SpiSlaveConnection(Generic_magHardwareModel* mag,
        int chip_select, std::string connection_string, std::string bus_name)
        : NosEngine::Spi::SpiSlave(chip_select, connection_string, bus_name)
    {
        _mag = mag;
    }

    size_t SpiSlaveConnection::spi_read(uint8_t *rbuf, size_t rlen) 
    {
        _mag->prepare_generic_mag_data_from_42(_spi_out_data);
        sim_logger->debug("spi_read: %s", SimIHardwareModel::uint8_vector_to_hex_string(_spi_out_data).c_str()); // log data

        if (_spi_out_data.size() < rlen) rlen = _spi_out_data.size();

        for (size_t i = 0; i < rlen; i++) {
            rbuf[i] = _spi_out_data[i];
        }
        return rlen;
    }

    size_t SpiSlaveConnection::spi_write(const uint8_t *wbuf, size_t wlen) {
        std::vector<uint8_t> in_data(wbuf, wbuf + wlen);
        sim_logger->debug("spi_write: %s", SimIHardwareModel::uint8_vector_to_hex_string(in_data).c_str()); // log data
        sim_logger->error("MAG sim does not support SPI write!");
        return wlen;

    }
}
