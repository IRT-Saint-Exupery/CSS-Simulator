#include <generic_fss_data_point.hpp>
#include <generic_fss_hardware_model.hpp>

namespace Nos3
{
    REGISTER_HARDWARE_MODEL(Generic_fssHardwareModel,"GENERIC_FSS");

    extern ItcLogger::Logger *sim_logger;

    Generic_fssHardwareModel::Generic_fssHardwareModel(const boost::property_tree::ptree& config) : SimIHardwareModel(config), _enabled(0)
    {
        /* Get the NOS engine connection string */
        std::string connection_string = config.get("common.nos-connection-string", "tcp://192.168.100.11:12001"); 
        sim_logger->info("Generic_fssHardwareModel::Generic_fssHardwareModel:  NOS Engine connection string: %s.", connection_string.c_str());

        /* Get a data provider */
        std::string dp_name = config.get("simulator.hardware-model.data-provider.type", "GENERIC_FSS_PROVIDER");
        _generic_fss_dp = SimDataProviderFactory::Instance().Create(dp_name, config);
        sim_logger->info("Generic_fssHardwareModel::Generic_fssHardwareModel:  Data provider %s created.", dp_name.c_str());

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
        sim_logger->info("Generic_fssHardwareModel::Generic_fssHardwareModel:  Now on time bus named %s.", time_bus_name.c_str());

        /* Get on a protocol bus */
        /* Note: Initialized defaults in case value not found in config file */
        std::string spi_name = "NULL"; // Initialize to default in case value not found in config file
        int chip_select = 0;
        if (config.get_child_optional("simulator.hardware-model.connections")) 
        {
            /* Loop through the connections for hardware model */
            BOOST_FOREACH(const boost::property_tree::ptree::value_type &v, config.get_child("simulator.hardware-model.connections"))
            {
                /* v.second is the child tree (v.first is the name of the child) */
                if (v.second.get("type", "").compare("spi") == 0)
                {
                    /* Configuration found */
                    spi_name = v.second.get("bus-name", spi_name);
                    chip_select = v.second.get("chip-select", chip_select);
                    break;
                }
            }
        }
        _spi = new SpiSlaveConnection(this, chip_select, connection_string, spi_name);
        sim_logger->info("Generic_fssHardwareModel::Generic_fssHardwareModel:  Now on SPI bus name %s, chip select %d.", spi_name.c_str(), chip_select);
    
        /* Construction complete */
        sim_logger->info("Generic_fssHardwareModel::Generic_fssHardwareModel:  Construction complete.");
    }


    Generic_fssHardwareModel::~Generic_fssHardwareModel(void)
    {        
        /* Close the protocol bus */
        delete _spi;
        _spi = nullptr;

        /* Clean up the data provider */
        delete _generic_fss_dp;
        _generic_fss_dp = nullptr;

        /* The bus will clean up the time node */
    }

    /* Automagically set up by the base class to be called */
    void Generic_fssHardwareModel::command_callback(NosEngine::Common::Message msg)
    {
        /* Get the data out of the message */
        NosEngine::Common::DataBufferOverlay dbf(const_cast<NosEngine::Utility::Buffer&>(msg.buffer));
        sim_logger->info("Generic_fssHardwareModel::command_callback:  Received command: %s.", dbf.data);

        /* Do something with the data */
        std::string command = dbf.data;
        std::string response = "Generic_fssHardwareModel::command_callback:  INVALID COMMAND! (Try HELP)";
        boost::to_upper(command);
        if (command.compare(0, 4, "HELP") == 0) 
        {
            response = "Generic_fssHardwareModel::command_callback: Valid commands are HELP, ENABLE, DISABLE, or STOP";
        }
        else if (command.compare(0, 6, "ENABLE") == 0) 
        {
            _enabled = GENERIC_FSS_SIM_SUCCESS;
            response = "Generic_fssHardwareModel::command_callback:  Enabled";
        }
        else if (command.compare(0, 7, "DISABLE") == 0) 
        {
            _enabled = GENERIC_FSS_SIM_ERROR;
            response = "Generic_fssHardwareModel::command_callback:  Disabled";
        }
        else if (command.compare(0, 4, "STOP") == 0) 
        {
            _keep_running = false;
            response = "Generic_fssHardwareModel::command_callback:  Stopping";
        }
        /* TODO: Add anything additional commands here */

        /* Send a reply */
        sim_logger->info("Generic_fssHardwareModel::command_callback:  Sending reply: %s.", response.c_str());
        _command_node->send_reply_message_async(msg, response.size(), response.c_str());
    }

    std::vector<uint8_t> Generic_fssHardwareModel::determine_spi_response_for_request(const std::vector<uint8_t>& in_data)
    {
        // Remember... SPI sends and receives between the master and slave at the same time... so what goes back out depends on computations done based on the last in... not this one

        // Take care of creating out_data from previously queued data
        std::vector<uint8_t> out_data = _queued_data;
        out_data.resize(in_data.size(), 0xFF);

        // Create next queued data based on in_data
        if ((in_data.size() == 7) && 
            (in_data[0] == 0xDE) && (in_data[1] == 0xAD) && (in_data[2] == 0xBE) && (in_data[3] == 0xEF) &&
            (in_data[4] == 0x01) && (in_data[5] == 0x01) && (in_data[6] == 0x02)                            ) {
            create_generic_fss_data();
        } else {
            _queued_data.resize(0);
        }
        return out_data;
    }

    /* Custom function to prepare the Generic_fss Data */
    void Generic_fssHardwareModel::create_generic_fss_data()
    {
        uint8_t four_bytes[4];
        boost::shared_ptr<Generic_fssDataPoint> data_point = boost::dynamic_pointer_cast<Generic_fssDataPoint>(_generic_fss_dp->get_data_point());

        /* Prepare data size */
        _queued_data.resize(16, 0x00);

        sim_logger->debug("Generic_fssHardwareModel::create_generic_fss_data:  Creating data, enabled=%d", _enabled);
        if (_enabled == GENERIC_FSS_SIM_SUCCESS) {
            bool valid = data_point->get_generic_fss_valid();
            double alpha = data_point->get_generic_fss_alpha();
            double beta = data_point->get_generic_fss_beta();
            sim_logger->debug("Generic_fssHardwareModel::create_generic_fss_data:  data_point data:  valid=%s, alpha=%f, beta=%f", valid?"TRUE ":"FALSE", alpha, beta);

            /* Streaming data header - 0xDEADBEEF */
            _queued_data[0] = 0xDE;
            _queued_data[1] = 0xAD;
            _queued_data[2] = 0xBE;
            _queued_data[3] = 0xEF;

            _queued_data[4] = 0x01; // command code
            _queued_data[5] = 0x0A; // length
            
            // alpha
            double_to_4bytes_little_endian(alpha, four_bytes);
            _queued_data[6] = four_bytes[0];
            _queued_data[7] = four_bytes[1];
            _queued_data[8] = four_bytes[2];
            _queued_data[9] = four_bytes[3];

            // beta
            double_to_4bytes_little_endian(beta, four_bytes);
            _queued_data[10] = four_bytes[0];
            _queued_data[11] = four_bytes[1];
            _queued_data[12] = four_bytes[2];
            _queued_data[13] = four_bytes[3];

            // error code
            _queued_data[14] = 1; // error
            if (valid) _queued_data[14] = 0; // valid

            // checksum
            _queued_data[15] = compute_checksum(_queued_data, 4, 11);
        }
    }

    uint8_t Generic_fssHardwareModel::compute_checksum(std::vector<uint8_t>& in, int starting_byte, int number_of_bytes)
    {
        uint32_t sum = 0;
        uint8_t checksum;
        for (int i = starting_byte; i < starting_byte + number_of_bytes; i++) {
            sum += in[i];
        }
        checksum = (uint8_t)(sum & 0x000000FF);
        return checksum;
    }

    void Generic_fssHardwareModel::double_to_4bytes_little_endian(double in, uint8_t out[4])
    {
        union {float f; uint32_t u;} fu;
        fu.f = (float)in;
        uint32_t u = fu.u;
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
        out[0] = (uint8_t)((u      ) & 0xFF);
        out[1] = (uint8_t)((u >>  8) & 0xFF);
        out[2] = (uint8_t)((u >> 16) & 0xFF);
        out[3] = (uint8_t)((u >> 24) & 0xFF);
#elif __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
        out[0] = (uint8_t)((u >> 24) & 0xFF);
        out[1] = (uint8_t)((u >> 16) & 0xFF);
        out[2] = (uint8_t)((u >>  8) & 0xFF);
        out[3] = (uint8_t)((u      ) & 0xFF);
#else
    #error "__BYTE_ORDER__ is not defined"
#endif
    }

    SpiSlaveConnection::SpiSlaveConnection(Generic_fssHardwareModel* fss,
        int chip_select, std::string connection_string, std::string bus_name)
        : NosEngine::Spi::SpiSlave(chip_select, connection_string, bus_name)
    {
        _fss = fss;
    }

    size_t SpiSlaveConnection::spi_read(uint8_t *rbuf, size_t rlen) {
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
        _spi_out_data = _fss->determine_spi_response_for_request(in_data);
        return wlen;

    }
}
