#include <generic_imu_hardware_model.hpp>

namespace Nos3
{
    REGISTER_HARDWARE_MODEL(Generic_imuHardwareModel,"GENERIC_IMU");

    extern ItcLogger::Logger *sim_logger;

    Generic_imuHardwareModel::Generic_imuHardwareModel(const boost::property_tree::ptree& config) : SimIHardwareModel(config), _enabled(0), _count(0), _status(0)
    {
        /* Get the NOS engine connection string */
        std::string connection_string = config.get("common.nos-connection-string", "tcp://192.168.100.11:12001");
        sim_logger->info("Generic_imuHardwareModel::Generic_imuHardwareModel:  NOS Engine connection string: %s.", connection_string.c_str());

        /* Get a data provider */
        std::string dp_name = config.get("simulator.hardware-model.data-provider.type", "GENERIC_IMU_PROVIDER");
        _generic_imu_dp = SimDataProviderFactory::Instance().Create(dp_name, config);
        sim_logger->info("Generic_imuHardwareModel::Generic_imuHardwareModel:  Data provider %s created.", dp_name.c_str());

        /* Get on a protocol bus */
        /* Note: Initialized defaults in case value not found in config file */
        std::string bus_name = "can_0";
        int node_port = 0;
        if (config.get_child_optional("simulator.hardware-model.connections")) 
        {
            /* Loop through the connections for hardware model */
            BOOST_FOREACH(const boost::property_tree::ptree::value_type &v, config.get_child("simulator.hardware-model.connections"))
            {
                /* v.second is the child tree (v.first is the name of the child) */
                if (v.second.get("type", "").compare("can") == 0)
                {
                    /* Configuration found */
                    bus_name = v.second.get("bus-name", bus_name);
                    node_port = v.second.get("node-port", node_port);
                    break;
                }
            }
        }
        _can_connection = new IMUCanSlaveConnection(this, node_port, connection_string, bus_name);
        sim_logger->info("Generic_imuHardwareModel::Generic_imuHardwareModel:  Now on CAN bus name %s, port %d.", bus_name.c_str(), node_port);
    
        /* Configure protocol callback */
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
        sim_logger->info("Generic_imuHardwareModel::Generic_imuHardwareModel:  Now on time bus named %s.", time_bus_name.c_str());

        /* Construction complete */
        sim_logger->info("Generic_imuHardwareModel::Generic_imuHardwareModel:  Construction complete.");
    }


    Generic_imuHardwareModel::~Generic_imuHardwareModel(void)
    {        
        /* Close the protocol bus */
        delete _can_connection;
        _can_connection = nullptr;

        /* Clean up the data provider */
        delete _generic_imu_dp;
        _generic_imu_dp = nullptr;

        /* The bus will clean up the time node */
    }


    /* Automagically set up by the base class to be called */
    void Generic_imuHardwareModel::command_callback(NosEngine::Common::Message msg)
    {
        /* Get the data out of the message */
        NosEngine::Common::DataBufferOverlay dbf(const_cast<NosEngine::Utility::Buffer&>(msg.buffer));
        sim_logger->info("Generic_imuHardwareModel::command_callback:  Received command: %s.", dbf.data);

        /* Do something with the data */
        std::string command = dbf.data;
        std::string response = "Generic_imuHardwareModel::command_callback:  INVALID COMMAND! (Try HELP)";
        boost::to_upper(command);
        if (command.compare("HELP") == 0) 
        {
            response = "Generic_imuHardwareModel::command_callback: Valid commands are HELP, ENABLE, DISABLE, STATUS=X, or STOP";
        }
        else if (command.compare("ENABLE") == 0) 
        {
            _enabled = GENERIC_IMU_SIM_SUCCESS;
            response = "Generic_imuHardwareModel::command_callback:  Enabled";
        }
        else if (command.compare("DISABLE") == 0) 
        {
            _enabled = GENERIC_IMU_SIM_ERROR;
            _count = 0;
            _status = 0;
            response = "Generic_imuHardwareModel::command_callback:  Disabled";
        }
        else if (command.substr(0,7).compare("STATUS=") == 0)
        {
            try
            {
                _status = std::stod(command.substr(7));
                response = "Generic_imuHardwareModel::command_callback:  Status set";
            }
            catch (...)
            {
                response = "Generic_imuHardwareModel::command_callback:  Status invalid";
            }            
        }
        else if (command.compare("STOP") == 0) 
        {
            _keep_running = false;
            response = "Generic_imuHardwareModel::command_callback:  Stopping";
        }

        /* Send a reply */
        sim_logger->info("Generic_imuHardwareModel::command_callback:  Sending reply: %s.", response.c_str());
        _command_node->send_reply_message_async(msg, response.size(), response.c_str());
    }


    /* Custom function to prepare the Generic_imu HK telemetry */
    void Generic_imuHardwareModel::create_generic_imu_hk(std::vector<uint8_t>& out_data)
    {
        /* Prepare data size */
        out_data.resize(8, 0x00);

        // I removed both header and trailer from this, since CAN does not use the same kind
        // as other protocols do.

        /* Sequence count */
        out_data[0] = (_count >> 24) & 0x000000FF; 
        out_data[1] = (_count >> 16) & 0x000000FF; 
        out_data[2] = (_count >>  8) & 0x000000FF; 
        out_data[3] =  _count & 0x000000FF;
        
        /* Device Status */
        out_data[4] = (_status >> 24) & 0x000000FF; 
        out_data[5] = (_status >> 16) & 0x000000FF; 
        out_data[6] = (_status >>  8) & 0x000000FF; 
        out_data[7] =  _status & 0x000000FF;
    }


    /* Custom function to prepare the Generic_imu Data */
    void Generic_imuHardwareModel::create_generic_imu_data(std::vector<uint8_t>& out_data, uint8_t axis)
    {
        boost::shared_ptr<Generic_imuDataPoint> data_point = boost::dynamic_pointer_cast<Generic_imuDataPoint>(_generic_imu_dp->get_data_point());

        /* Prepare data size */
        out_data.resize(8, 0x00); 
        
        /* 
        ** Payload 
        ** 
        ** Device is big endian (most significant byte first)
        ** Assuming data is valid regardless of dynamic / environmental data
        ** Floating poing numbers are extremely problematic 
        **   (https://docs.oracle.com/cd/E19957-01/806-3568/ncg_goldberg.html)
        ** Most hardware transmits some type of unsigned integer (e.g. from an ADC), so that's what we've done
        */
        switch (axis)
        {
            case 2:
                {
                    uint32_t x_linear = (uint32_t)((data_point->get_generic_imu_acc_x())*_LIN_CONV_CONST + _LIN_CONV_CONST*10);
                    uint32_t x_angular = (uint32_t)(data_point->get_generic_imu_gyro_x()*_ANG_CONV_CONST + _ANG_CONV_CONST*400);
                    sim_logger->debug("X linear acceleration: %f, converted to %u", data_point->get_generic_imu_acc_x(), x_linear);
                    sim_logger->debug("X rotational rate: %f, converted to %u", data_point->get_generic_imu_gyro_x(), x_angular);
                    out_data[0] = (x_linear >> 24);
                    out_data[1] = (x_linear >> 16);
                    out_data[2] = (x_linear >> 8 );
                    out_data[3] =  x_linear;
                    out_data[4] = (x_angular >> 24);
                    out_data[5] = (x_angular >> 16);
                    out_data[6] = (x_angular >> 8 );
                    out_data[7] =  x_angular;
                    break;
                }

            case 3:
                {
                    uint32_t y_linear = (uint32_t)(data_point->get_generic_imu_acc_y()*_LIN_CONV_CONST + _LIN_CONV_CONST*10);
                    uint32_t y_angular = (uint32_t)(data_point->get_generic_imu_gyro_y()*_ANG_CONV_CONST + _ANG_CONV_CONST*400);
                    sim_logger->debug("Y linear acceleration: %f, converted to %u", data_point->get_generic_imu_acc_y(), y_linear);
                    sim_logger->debug("Y rotational rate: %f, converted to %u", data_point->get_generic_imu_gyro_y(), y_angular);
                    out_data[0] = (y_linear >> 24);
                    out_data[1] = (y_linear >> 16);
                    out_data[2] = (y_linear >> 8 );
                    out_data[3] =  y_linear;
                    out_data[4] = (y_angular >> 24);
                    out_data[5] = (y_angular >> 16);
                    out_data[6] = (y_angular >> 8 );
                    out_data[7] =  y_angular;
                    break;
                }
       
            case 4:
                {
                    uint32_t z_linear = (uint32_t)(data_point->get_generic_imu_acc_z()*_LIN_CONV_CONST + _LIN_CONV_CONST*10);
                    uint32_t z_angular = (uint32_t)(data_point->get_generic_imu_gyro_z()*_ANG_CONV_CONST + _ANG_CONV_CONST*400); 
                    sim_logger->debug("Z linear acceleration: %f, converted to %u", data_point->get_generic_imu_acc_z(), z_linear);
                    sim_logger->debug("Z rotational rate: %f, converted to %u", data_point->get_generic_imu_gyro_z(), z_angular);
                    out_data[0] = (z_linear >> 24);
                    out_data[1] = (z_linear >> 16);
                    out_data[2] = (z_linear >> 8 );
                    out_data[3] =  z_linear;
                    out_data[4] = (z_angular >> 24);
                    out_data[5] = (z_angular >> 16);
                    out_data[6] = (z_angular >> 8 );
                    out_data[7] =  z_angular;
                    break;
                }

            default:
                /* Unused command code */
                sim_logger->debug("Generic_imuHardwareModel::create_generic_imu_data:  Fake axis %d received!", axis);
                break;

        }
    }


    /* Protocol callback */
    std::vector<uint8_t> Generic_imuHardwareModel::determine_can_response(const std::vector<uint8_t>& in_data)
    {
        std::vector<uint8_t> out_data;
        std::vector<uint8_t> imu_data; 
        std::uint8_t valid = GENERIC_IMU_SIM_SUCCESS;

        /* Retrieve data and log in man readable format */
        sim_logger->debug("Generic_imuHardwareModel::determine_can_response:  REQUEST %s",
            SimIHardwareModel::uint8_vector_to_hex_string(in_data).c_str());

        /* Check simulator is enabled */
        if (_enabled != GENERIC_IMU_SIM_SUCCESS)
        {
            sim_logger->debug("Generic_imuHardwareModel::determine_can_response:  Generic_imu sim disabled!");
            valid = GENERIC_IMU_SIM_ERROR;
        }
        else
        {
            /* Check if message is incorrect size */
            if (in_data.size() != 14) 
            {
                sim_logger->debug("Generic_imuHardwareModel::determine_can_response:  Invalid command size of %d received!", in_data.size());
                valid = GENERIC_IMU_SIM_ERROR;
            }
            else
            {
                if (in_data[4] != _IMU_CAN_CMD_SIZE)
                {
                    sim_logger->debug("Generic_imuHardwareModel::determine_can_response:  Invalid command length of %d received!", in_data[4]);
                    valid = GENERIC_IMU_SIM_ERROR;
                }
                // If I were to check the header, this is where I would want to do it.
                // Ditto for the trailer. The header (first chunk, at least) should typically
                // be 8, and the second chunk should be 0. The first 8 bits should be 80.
            }

            if (valid == GENERIC_IMU_SIM_SUCCESS)
            {   
                /* Process command */
                switch (in_data[9]) 
                {
                    case 0:
                        /* NOOP */
                        sim_logger->debug("Generic_imuHardwareModel::determine_can_response:  NOOP command received!");
                        break;

                    case 1:
                        /* Request HK */
                        sim_logger->debug("Generic_imuHardwareModel::determine_can_response:  Send HK command received!");
                        create_generic_imu_hk(imu_data);
                        out_data.clear();
                        out_data.push_back(0x00); // CAN_ID[0]
                        out_data.push_back(0x00); // CAN_ID[1]
                        out_data.push_back(0x00); // CAN_ID[2]
                        out_data.push_back(0x00); // CAN_ID[3]
                        out_data.push_back(imu_data.size());
                        out_data.push_back(0x00); // Pad
                        out_data.push_back(0x00); // Pad
                        out_data.push_back(0x00); // Pad
                        out_data.insert(out_data.end(), imu_data.begin(), imu_data.end());
                        break;

                    case 2:
                        /* Request data */
                        sim_logger->debug("Generic_imuHardwareModel::determine_can_response:  Send data X command received!");
                        create_generic_imu_data(imu_data, 2);
                        out_data.clear();
                        out_data.push_back(0x00); // CAN_ID[0]
                        out_data.push_back(0x00); // CAN_ID[1]
                        out_data.push_back(0x00); // CAN_ID[2]
                        out_data.push_back(0x00); // CAN_ID[3]
                        out_data.push_back(imu_data.size());
                        out_data.push_back(0x00); // Pad
                        out_data.push_back(0x00); // Pad
                        out_data.push_back(0x00); // Pad
                        out_data.insert(out_data.end(), imu_data.begin(), imu_data.end());
                        break;

                    case 3:
                        /* Request data */
                        sim_logger->debug("Generic_imuHardwareModel::determine_can_response:  Send data Y command received!");
                        create_generic_imu_data(imu_data, 3);
                        out_data.clear();
                        out_data.push_back(0x00); // CAN_ID[0]
                        out_data.push_back(0x00); // CAN_ID[1]
                        out_data.push_back(0x00); // CAN_ID[2]
                        out_data.push_back(0x00); // CAN_ID[3]
                        out_data.push_back(imu_data.size());
                        out_data.push_back(0x00); // Pad
                        out_data.push_back(0x00); // Pad
                        out_data.push_back(0x00); // Pad
                        out_data.insert(out_data.end(), imu_data.begin(), imu_data.end());
                        break;

                    case 4:
                        /* Request data */
                        sim_logger->debug("Generic_imuHardwareModel::determine_can_response:  Send data Z command received!");
                        create_generic_imu_data(imu_data, 4);
                        out_data.clear();
                        out_data.push_back(0x00); // CAN_ID[0]
                        out_data.push_back(0x00); // CAN_ID[1]
                        out_data.push_back(0x00); // CAN_ID[2]
                        out_data.push_back(0x00); // CAN_ID[3]
                        out_data.push_back(imu_data.size());
                        out_data.push_back(0x00); // Pad
                        out_data.push_back(0x00); // Pad
                        out_data.push_back(0x00); // Pad
                        out_data.insert(out_data.end(), imu_data.begin(), imu_data.end());
                        break;
                    
                    default:
                        /* Unused command code */
                        valid = GENERIC_IMU_SIM_ERROR;
                        sim_logger->debug("Generic_imuHardwareModel::determine_can_response:  Unused command %d received!", in_data[1]);
                        break;
                }
            }
        }

        /* Increment count and echo command since format valid */
        if (valid == GENERIC_IMU_SIM_SUCCESS)
        {
            _count++;

            /* Send response if existing */
            if (out_data.size() > 0)
            {
                sim_logger->debug("Generic_imuHardwareModel::determine_can_response:  REPLY %s",
                    SimIHardwareModel::uint8_vector_to_hex_string(out_data).c_str());
            }
        }
        return out_data;
    }

    IMUCanSlaveConnection::IMUCanSlaveConnection(Generic_imuHardwareModel* hm, int bus_address, std::string connection_string, std::string bus_name)
        : NosEngine::Can::CanSlave(bus_address, connection_string, bus_name)
    {
        _hardware_model = hm;
        _can_out_data = {0,0,0,0,0,0,0,0,0,0,0,0,0,0};
        sim_logger->debug("IMUCanSlaveConnection::IMUCanSlaveConnection:  IMUCanSlaveConnection constructor complete");
    }

    size_t IMUCanSlaveConnection::can_read(uint8_t *rbuf, size_t rlen)
    {
        // copy data to buffer
        _can_out_data.resize(rlen);
        std::copy(_can_out_data.begin(), _can_out_data.begin() + _can_out_data.size(), rbuf);
        //sim_logger->debug("Generic_imuHardwareModel::can_read[%d]: %s", rlen, SimIHardwareModel::uint8_vector_to_hex_string(_can_out_data).c_str());
        return rlen;
    }

    size_t IMUCanSlaveConnection::can_write(const uint8_t *wbuf, size_t wlen)
    {
        std::vector<uint8_t> in_data(wbuf, wbuf + _IMU_CAN_FRAME_SIZE);
        //sim_logger->debug("Generic_imuHardwareModel::can_write[%d]: %s", wlen, SimIHardwareModel::uint8_vector_to_hex_string(in_data).c_str());
        _can_out_data = _hardware_model->determine_can_response(in_data);
        return wlen;
    }

}
