#include <generic_eps_hardware_model.hpp>

namespace Nos3
{
    REGISTER_HARDWARE_MODEL(Generic_epsHardwareModel,"GENERIC_EPS");

    extern ItcLogger::Logger *sim_logger;

    Generic_epsHardwareModel::Generic_epsHardwareModel(const boost::property_tree::ptree& config) : SimIHardwareModel(config), 
    _enabled(GENERIC_EPS_SIM_SUCCESS), _initialized_other_sims(GENERIC_EPS_SIM_ERROR)
    {
        /* Get the NOS engine connection string */
        std::string connection_string = config.get("common.nos-connection-string", "tcp://192.168.100.11:12001"); 
        sim_logger->info("Generic_epsHardwareModel::Generic_epsHardwareModel:  NOS Engine connection string: %s.", connection_string.c_str());

        // Set up the time node which is **required** for this model 
        std::string time_bus_name = "command";
        if(config.get_child_optional("hardware-model.connections")) {
          BOOST_FOREACH(const boost::property_tree::ptree::value_type &v, config.get_child("hardware-model.connections")) {       
               // v.first is the name of the child
               // v.second is the child tree 
               if(v.second.get("type","").compare("time") == 0) {
                  time_bus_name = v.second.get("bus-name","command");
                  break;   
               }
          }
        }
        _time_bus.reset(new NosEngine::Client::Bus(_hub, connection_string, time_bus_name));
        sim_logger->debug("Generic_epsHardwareModel::Generic_epsHardwareModel: Time bus %s now active.", time_bus_name.c_str());
                  
        /* Get a data provider */
        std::string dp_name = config.get("simulator.hardware-model.data-provider.type", "GENERIC_EPS_PROVIDER");
        _generic_eps_dp = SimDataProviderFactory::Instance().Create(dp_name, config);
        sim_logger->info("Generic_epsHardwareModel::Generic_epsHardwareModel:  Data provider %s created.", dp_name.c_str());

        /* Get on a protocol bus */
        /* Note: Initialized defaults in case value not found in config file */
        std::string bus_name = "i2c_0";
        int bus_address = 0x2B;
        if (config.get_child_optional("simulator.hardware-model.connections")) 
        {
            /* Loop through the connections for hardware model */
            BOOST_FOREACH(const boost::property_tree::ptree::value_type &v, config.get_child("simulator.hardware-model.connections"))
            {
                /* v.second is the child tree (v.first is the name of the child) */
                if (v.second.get("type", "").compare("i2c") == 0)
                {
                    /* Configuration found */
                    bus_name = v.second.get("bus-name", bus_name);
                    bus_address = v.second.get("bus-address", bus_address);
                    break;
                }
            }
        }
        _time_bus->add_time_tick_callback(std::bind(&Generic_epsHardwareModel::update_battery_values, this));
        
        _i2c_slave_connection = new I2CSlaveConnection(this, bus_address, connection_string, bus_name);
        sim_logger->info("Generic_epsHardwareModel::Generic_epsHardwareModel:  Now on I2C bus name %s as address 0x%02x.", bus_name.c_str(), bus_address);

        /* Get on the command bus */
        _command_bus_name = "command";
        if (config.get_child_optional("hardware-model.connections")) 
        {
            /* Loop through the connections for the hardware model connections */
            BOOST_FOREACH(const boost::property_tree::ptree::value_type &v, config.get_child("hardware-model.connections"))
            {
                /* v.first is the name of the child */
                /* v.second is the child tree */
                if (v.second.get("type", "").compare("time") == 0) // 
                {
                    _command_bus_name = v.second.get("bus-name", "command");
                    /* Found it... don't need to go through any more items*/
                    break; 
                }
            }
        }
        _command_bus.reset(new NosEngine::Client::Bus(_hub, connection_string, _command_bus_name));
        sim_logger->info("Generic_epsHardwareModel::Generic_epsHardwareModel:  Now on time bus named %s.", _command_bus_name.c_str());

        /* Initialize status for battery and bus */
        std::string battv, battv_temp, solararray, solararray_current, solararray_temp, batt_watt_hrs, always_on_v, always_on_a;
        
        // Below, the battery watt-hrs arbitrarily selected 
        // it could well do to be changed to be more in line with SAT true values
        // additionally, the current values (as indicated) are placeholders and 
        // should probably be changed to something more correct
                
        _power_per_panel = atof(config.get("simulator.hardware-model.physical.bus.solar-array-power-per-panel", "26.91").c_str()); //power generated, in Watts; data taken from GTOSat
        
        battv = config.get("simulator.hardware-model.physical.bus.battery-voltage", "24.0");
        battv_temp = config.get("simulator.hardware-model.physical.bus.battery-temperature", "25.0");
        batt_watt_hrs = config.get("simulator.hardware-model.physical.bus.battery-watt-hrs", "10.0");
        solararray = config.get("simulator.hardware-model.physical.bus.solar-array-voltage", "32.0");
        solararray_temp = config.get("simulator.hardware-model.physical.bus.solar-array-temperature", "80.0");
        solararray_current = config.get("simulator.hardware-model.physical.bus.solar-array-current", "4.0");
        
             
        /* Initialize status for buses */
        std::string bus_low_volt, bus_mid_volt, bus_high_volt, bus_low_current, bus_mid_current, bus_high_current;
        
        bus_low_volt =  config.get("simulator.hardware-model.physical.bus.bus-low-voltage", "3.3");
        bus_mid_volt =  config.get("simulator.hardware-model.physical.bus.bus-mid-voltage", "5.0");
        bus_high_volt =  config.get("simulator.hardware-model.physical.bus.bus-high-voltage", "12.0"); 
        
        bus_low_current =  config.get("simulator.hardware-model.physical.bus.bus-low-current", "1.0");
        bus_mid_current =  config.get("simulator.hardware-model.physical.bus.bus-mid-current", "1.0");
        bus_high_current =  config.get("simulator.hardware-model.physical.bus.bus-high-current", "1.0"); 
        
        _nominal_batt_voltage = atoi(battv.c_str());
        _max_battery = atof(batt_watt_hrs.c_str());       
        
        _bus[0]._voltage = atoi(battv.c_str()) * 1000;
        _bus[0]._temperature = (atoi(battv_temp.c_str()) + 60) * 100;
        _bus[0]._battery_watthrs = atof(batt_watt_hrs.c_str());
        _bus[1]._voltage = atof(bus_low_volt.c_str()) * 1000;
        _bus[2]._voltage = atof(bus_mid_volt.c_str()) * 1000;
        _bus[3]._voltage = atof(bus_high_volt.c_str()) * 1000;
        _bus[4]._voltage = atoi(solararray.c_str()) * 1000;
        _bus[4]._temperature = (atoi(solararray_temp.c_str()) + 60) * 100;
        _bus[4]._current = atof(solararray_current.c_str()) * 1000;
        _bus[1]._current = atof(bus_low_current.c_str()) * 1000;
        _bus[2]._current = atof(bus_low_current.c_str()) * 1000;
        _bus[3]._current = atof(bus_low_current.c_str()) * 1000;
        
        /*
        sim_logger->info("  Initial _bus[0]._voltage = 0x%04x", _bus[0]._voltage);
        sim_logger->info("  Initial _bus[0]._temperature = 0x%04x", _bus[0]._temperature);
        sim_logger->info("  Initial _bus[1]._voltage = 0x%04x", _bus[1]._voltage);
        sim_logger->info("  Initial _bus[2]._voltage = 0x%04x", _bus[2]._voltage);
        sim_logger->info("  Initial _bus[3]._voltage = 0x%04x", _bus[3]._voltage);
        sim_logger->info("  Initial _bus[4]._voltage = 0x%04x", _bus[4]._voltage);
        sim_logger->info("  Initial _bus[4]._temperature = 0x%04x", _bus[4]._temperature);
        */

        /* Initialize status for each switch */
        _init_switch[0]._node_name = config.get("simulator.hardware-model.physical.switch-0.node-name", "switch-0");
        _init_switch[0]._voltage = config.get("simulator.hardware-model.physical.switch-0.voltage", "3.30");
        _init_switch[0]._current = config.get("simulator.hardware-model.physical.switch-0.current", "0.25");
        _init_switch[0]._state = config.get("simulator.hardware-model.physical.switch-0.hex-status", "0000");

        _init_switch[1]._node_name = config.get("simulator.hardware-model.physical.switch-1.node-name", "switch-1");
        _init_switch[1]._voltage = config.get("simulator.hardware-model.physical.switch-1.voltage", "3.30");
        _init_switch[1]._current = config.get("simulator.hardware-model.physical.switch-1.current", "0.10");
        _init_switch[1]._state = config.get("simulator.hardware-model.physical.switch-1.hex-status", "0000");

        _init_switch[2]._node_name = config.get("simulator.hardware-model.physical.switch-2.node-name", "switch-2");
        _init_switch[2]._voltage = config.get("simulator.hardware-model.physical.switch-2.voltage", "5.00");
        _init_switch[2]._current = config.get("simulator.hardware-model.physical.switch-2.current", "0.20");
        _init_switch[2]._state = config.get("simulator.hardware-model.physical.switch-2.hex-status", "0000");

        _init_switch[3]._node_name = config.get("simulator.hardware-model.physical.switch-3.node-name", "switch-3");
        _init_switch[3]._voltage = config.get("simulator.hardware-model.physical.switch-3.voltage", "5.00");
        _init_switch[3]._current = config.get("simulator.hardware-model.physical.switch-3.current", "0.30");
        _init_switch[3]._state = config.get("simulator.hardware-model.physical.switch-3.hex-status", "0000");

        _init_switch[4]._node_name = config.get("simulator.hardware-model.physical.switch-4.node-name", "switch-4");
        _init_switch[4]._voltage = config.get("simulator.hardware-model.physical.switch-4.voltage", "12.00");
        _init_switch[4]._current = config.get("simulator.hardware-model.physical.switch-4.current", "0.40");
        _init_switch[4]._state = config.get("simulator.hardware-model.physical.switch-4.hex-status", "0000");

        _init_switch[5]._node_name = config.get("simulator.hardware-model.physical.switch-5.node-name", "switch-5");
        _init_switch[5]._voltage = config.get("simulator.hardware-model.physical.switch-5.voltage", "12.00");
        _init_switch[5]._current = config.get("simulator.hardware-model.physical.switch-5.current", "0.50");
        _init_switch[5]._state = config.get("simulator.hardware-model.physical.switch-5.hex-status", "0000");

        _init_switch[6]._node_name = config.get("simulator.hardware-model.physical.switch-6.node-name", "switch-6");
        _init_switch[6]._voltage = config.get("simulator.hardware-model.physical.switch-6.voltage", "3.30");
        _init_switch[6]._current = config.get("simulator.hardware-model.physical.switch-6.current", "0.60");
        _init_switch[6]._state = config.get("simulator.hardware-model.physical.switch-6.hex-status", "0000");

        _init_switch[7]._node_name = config.get("simulator.hardware-model.physical.switch-7.node-name", "switch-7");
        _init_switch[7]._voltage = config.get("simulator.hardware-model.physical.switch-7.voltage", "5.00"); 
        _init_switch[7]._current = config.get("simulator.hardware-model.physical.switch-7.current", "0.7"); 
        _init_switch[7]._state = config.get("simulator.hardware-model.physical.switch-7.hex-status", "0000");

        std::uint8_t i;
        for (i = 0; i < 8; i++)
        {
            _switch[i]._voltage = atof((_init_switch[i]._voltage).c_str()) * 1000;
            _switch[i]._current = atof((_init_switch[i]._current).c_str()) * 1000;
            _switch[i]._status = std::stoi((_init_switch[i]._state).c_str(), 0, 16);
        }

        sim_logger->info("    _switch[0]._voltage = %d", _switch[0]._voltage);
        sim_logger->info("    _switch[0]._current = %d", _switch[0]._current);
        sim_logger->info("    _switch[0]._status = 0x%04x", _switch[0]._status);

        /* Construction complete */
        sim_logger->info("Generic_epsHardwareModel::Generic_epsHardwareModel:  Construction complete.");
    }


    Generic_epsHardwareModel::~Generic_epsHardwareModel(void)
    {        
        /* Close the protocol bus */
       delete _i2c_slave_connection;
        _i2c_slave_connection = nullptr;

        /* Clean up the data provider */
        delete _generic_eps_dp;
        _generic_eps_dp = nullptr;

        /* The bus will clean up the time and sim nodes */
    }


    /* Automagically set up by the base class to be called */
    void Generic_epsHardwareModel::command_callback(NosEngine::Common::Message msg)
    {
        /* Get the data out of the message */
        NosEngine::Common::DataBufferOverlay dbf(const_cast<NosEngine::Utility::Buffer&>(msg.buffer));
        sim_logger->info("Generic_epsHardwareModel::command_callback:  Received command: %s.", dbf.data);

        /* Do something with the data */
        std::string command = dbf.data;
        std::string response = "Generic_epsHardwareModel::command_callback:  INVALID COMMAND! (Try HELP)";
        boost::to_upper(command);
        if (command.compare("HELP") == 0) 
        {
            response = "Generic_epsHardwareModel::command_callback: Valid commands are HELP, ENABLE, DISABLE, STATUS=X, or STOP";
        }
        else if (command.compare("ENABLE") == 0) 
        {
            _enabled = GENERIC_EPS_SIM_SUCCESS;
            response = "Generic_epsHardwareModel::command_callback:  Enabled";
        }
        else if (command.compare("DISABLE") == 0) 
        {
            _enabled = GENERIC_EPS_SIM_ERROR;
            response = "Generic_epsHardwareModel::command_callback:  Disabled";
        }
        else if (command.compare("STOP") == 0) 
        {
            _keep_running = false;
            response = "Generic_epsHardwareModel::command_callback:  Stopping";
        }

        /* Send a reply */
        sim_logger->info("Generic_epsHardwareModel::command_callback:  Sending reply: %s.", response.c_str());
        _command_node->send_reply_message_async(msg, response.size(), response.c_str());
    }

    void Generic_epsHardwareModel::eps_switch_update(const std::uint8_t sw_num, uint8_t sw_status)
    {
        /* Is the switch valid? */
        if (sw_num < 8)
        {
            /* Is the status valid? */
            if ((sw_status == 0x00) || (sw_status == 0xAA))
            {
                /* Prepare the simulator bus with the switch node */
                //_sim_bus.reset(new NosEngine::Client::Bus(_hub, _switch[sw_num], _time_bus_name));

                /* Use the simulator bus to set the state in other simulators */
                if (sw_status == 0x00)
                {
                    _command_node->send_non_confirmed_message_async(_init_switch[sw_num]._node_name, 7, "DISABLE");
                }
                else
                {
                    _command_node->send_non_confirmed_message_async(_init_switch[sw_num]._node_name, 6, "ENABLE");
                }
                
                /* Set the values internally */
                _switch[sw_num]._status = sw_status;
            }
            else
            {
                sim_logger->debug("Generic_epsHardwareModel::eps_switch_update:  Set state of 0x%02x invalid! Expected 0x00 or 0xAA", sw_status);
            }
        }
        else
        {
            sim_logger->debug("Generic_epsHardwareModel::eps_switch_update:  Switch number %d is invalid!", sw_num);
        }
    }

    std::uint8_t Generic_epsHardwareModel::generic_eps_crc8(const std::vector<uint8_t>& crc_data, std::uint32_t crc_size)
    {
        std::uint8_t crc = 0xFF;
        std::uint32_t i;
        std::uint32_t j;

        for (i = 0; i < crc_size; i++) 
        {
            crc ^= crc_data[i];
            for (j = 0; j < 8; j++) 
            {
                if ((crc & 0x80) != 0)
                {
                    crc = (uint8_t)((crc << 1) ^ 0x31);
                }
                else
                {
                    crc <<= 1;
                }
            }
        }
        return crc;
    }

    /* Custom function to prepare the Generic_eps Data */
    void Generic_epsHardwareModel::create_generic_eps_data(std::vector<uint8_t>& out_data)
    {
        boost::shared_ptr<Generic_epsDataPoint> data_point = boost::dynamic_pointer_cast<Generic_epsDataPoint>(_generic_eps_dp->get_data_point());
        
        /* Iniitalize if not yet done */
        if(_initialized_other_sims == GENERIC_EPS_SIM_ERROR)
        {    
            std::uint8_t i, j;
            for (i = 0; i < 8; i++)
            {
                j = std::uint8_t (_switch[i]._status & 0x00AA);
                if(j == 0xAA)
                {
                    eps_switch_update(i, j);
                }
            }
            _initialized_other_sims = GENERIC_EPS_SIM_SUCCESS;
        }
        
        /* TODO: Update data based on provided SVB and switch statesince last cycle */

        /* Prepare data size */
        out_data.resize(65, 0x00);

        /* Battery  - Voltage */
        out_data[0] = (_bus[0]._voltage >> 8) & 0x00FF;
        out_data[1] = _bus[0]._voltage & 0x00FF;
        /* Battery  - Temperature */
        out_data[2] = (_bus[0]._temperature >> 8) & 0x00FF; 
        out_data[3] = _bus[0]._temperature & 0x00FF;
        
        /* EPS      - 3.3 Voltage */
        out_data[4] = (_bus[1]._voltage >> 8) & 0x00FF; 
        out_data[5] = _bus[1]._voltage & 0x00FF; 
        /* EPS      - 5.0 Voltage */
        out_data[6] = (_bus[2]._voltage >> 8) & 0x00FF;
        out_data[7] = _bus[2]._voltage & 0x00FF;
        /* EPS      - 12.0 Voltage */
        out_data[8] = (_bus[3]._voltage >> 8) & 0x00FF; 
        out_data[9] = _bus[3]._voltage & 0x00FF; 
        /* EPS      - Temperature */
        out_data[10] = (_bus[3]._voltage >> 8) & 0x00FF;
        out_data[11] = _bus[3]._voltage & 0x00FF;

        /* Solar Array - Voltage */
        out_data[12] = (_bus[4]._voltage >> 8) & 0x00FF;
        out_data[13] = _bus[4]._voltage & 0x00FF;
        /* Solar Array - Temperature */
        out_data[14] = (_bus[4]._temperature >> 8) & 0x00FF;
        out_data[15] = _bus[4]._temperature & 0x00FF;

        std::uint16_t i = 0;
        std::uint16_t offset = 16;
        for(i = 0; i < 8; i++)
        {
            if ((_switch[i]._status & 0x00FF) == 0x00AA)
            {
                /* Switch[i], ON - Voltage */
                out_data[offset] = (_switch[i]._voltage >> 8) & 0x00FF;
                out_data[offset+1] = _switch[i]._voltage & 0x00FF;
                /* Switch[i], ON - Current */
                out_data[offset+2] = (_switch[i]._current >> 8) & 0x00FF;
                out_data[offset+3] = _switch[i]._current & 0x00FF;
            }
            else
            {
                /* Switch[i], OFF - Voltage */
                out_data[offset] = 0x00;
                out_data[offset+1] = 0x00;
                /* Switch[i], OFF - Current */
                out_data[offset+2] = 0x00;
                out_data[offset+3] = 0x00;
            }
            /* Switch[i] - Status */
            out_data[offset+4] = (_switch[i]._status >> 8) & 0x00FF;
            out_data[offset+5] = _switch[i]._status & 0x00FF;
            offset = offset + 6;
        }
        
        /* CRC */
        out_data[64] = generic_eps_crc8(out_data, 64);

        /*
        sim_logger->info("  _bus[0]._voltage = 0x%04x", _bus[0]._voltage);
        sim_logger->info("    (_bus[0]._voltage >> 8) & 0x00FF = 0x%02x", out_data[0]);
        sim_logger->info("    _bus[0]._voltage & 0x00FF = 0x%02x", out_data[1]);
        sim_logger->info("  _bus[0]._temperature = 0x%04x", _bus[0]._temperature);
        sim_logger->info("  _bus[1]._voltage = 0x%04x", _bus[1]._voltage);
        sim_logger->info("  _bus[2]._voltage = 0x%04x", _bus[2]._voltage);
        sim_logger->info("  _bus[3]._voltage = 0x%04x", _bus[3]._voltage);
        sim_logger->info("  _bus[4]._voltage = 0x%04x", _bus[4]._voltage);
        sim_logger->info("  _bus[4]._temperature = 0x%04x", _bus[4]._temperature);
        sim_logger->debug("create_generic_eps_data: %s", SimIHardwareModel::uint8_vector_to_hex_string(out_data).c_str());
        */
    }

    /* Protocol callback */
    std::uint8_t Generic_epsHardwareModel::determine_i2c_response_for_request(const std::vector<uint8_t>& in_data, std::vector<uint8_t>& out_data)
    {
        std::uint8_t valid = GENERIC_EPS_SIM_SUCCESS;
        std::uint8_t calc_crc8;
        
        /* Retrieve data and log in man readable format */
        sim_logger->debug("Generic_epsHardwareModel::determine_i2c_response_for_request:  REQUEST %s",
            SimIHardwareModel::uint8_vector_to_hex_string(in_data).c_str());

        /* Check simulator is enabled */
        if (_enabled != GENERIC_EPS_SIM_SUCCESS)
        {
            sim_logger->debug("Generic_epsHardwareModel::determine_i2c_response_for_request:  Generic_eps sim disabled!");
            valid = GENERIC_EPS_SIM_ERROR;
        }
        else
        {
            /* Check if message is incorrect size */
            if (in_data.size() != 3)
            {
                sim_logger->debug("Generic_epsHardwareModel::determine_i2c_response_for_request:  Invalid command size of %d received!", in_data.size());
                valid = GENERIC_EPS_SIM_ERROR;
            }
            else
            {
                /* Check CRC */
                calc_crc8 = generic_eps_crc8(in_data, 2);
                if (in_data[2] != calc_crc8)
                {
                    sim_logger->debug("Generic_epsHardwareModel::determine_i2c_response_for_request:  CRC8  of 0x%02x incorrect, expected 0x%02x!", in_data[2], calc_crc8);
                    valid = GENERIC_EPS_SIM_ERROR;
                }
                else
                {
                    if ((in_data[0] < 8) && (!((in_data[1] == 0x00) || (in_data[1] == 0xAA))))
                    {
                        sim_logger->debug("Generic_epsHardwareModel::determine_i2c_response_for_request:  Set switch %d state of 0x%02x invalid!", in_data[0], in_data[1]);
                        valid = GENERIC_EPS_SIM_ERROR;
                    }
                }
            }

            if (valid == GENERIC_EPS_SIM_SUCCESS)
            {   
                /* Process command */
                switch (in_data[0])
                {
                    case 0x00:
                    case 0x01:
                    case 0x02:
                    case 0x03:
                    case 0x04:
                    case 0x05:
                    case 0x06:
                    case 0x07:
                        /* Note intentional fall through to this point */
                        sim_logger->debug("Generic_epsHardwareModel::determine_i2c_response_for_request:  Set switch %d state to 0x%02x command received!", in_data[0], in_data[1]);
                        eps_switch_update(in_data[0], in_data[1]);
                        break;

                    case 0x70:
                        /* Telemetry Request */
                        sim_logger->debug("Generic_epsHardwareModel::determine_i2c_response_for_request:  Telemetry request command received!");
                        create_generic_eps_data(out_data);
                        break;

                    case 0xAA:
                        /* Reset */
                        sim_logger->debug("Generic_epsHardwareModel::determine_i2c_response_for_request:  Reset command received!");
                        /* TODO */
                        break;
                    
                    default:
                        /* Unused command code */
                        sim_logger->debug("Generic_epsHardwareModel::determine_i2c_response_for_request:  Unused command %d received!", in_data[0]);
                        valid = GENERIC_EPS_SIM_ERROR;
                        break;
                }
            }
        }
        return valid;
    }

    void Generic_epsHardwareModel::update_battery_values(void)
    {
    //sim_logger-> debug("Generic_epsHardwareModel::update_battery_values");
    boost::shared_ptr<Generic_epsDataPoint> data_point = boost::dynamic_pointer_cast<Generic_epsDataPoint>(_generic_eps_dp->get_data_point());
    double svb_X = (data_point->get_sun_vector_x() > 0) ? data_point->get_sun_vector_x() : 0.0;
    //double svb_minusX = (data_point->get_sun_vector_x() < 0) ? (-1)*data_point->get_sun_vector_x() : 0.0;
    double svb_Y = (data_point->get_sun_vector_y() > 0) ? data_point->get_sun_vector_y() : 0.0;
    //double svb_minusY = (data_point->get_sun_vector_x() < 0) ? (-1)*data_point->get_sun_vector_y() : 0.0;
    double svb_Z = (data_point->get_sun_vector_z() > 0) ? data_point->get_sun_vector_z() : 0.0;
    //double svb_minusZ = (data_point->get_sun_vector_x() < 0) ? (-1)*data_point->get_sun_vector_z() : 0.0;
    
    sim_logger-> debug("Generic_epsHardwareModel::update_battery_values: X = %.3f; Y = %.3f; Z = %.3f;",svb_X, svb_Y, svb_Z); 
    
    /* Assuming solar arrays on all -+ X, Y, and Z faces */
    // the cosine part is the most relevant part, affecting the power 
    // received, ...
    // Technically, when the angle to the sun is greater than 45 deg 
    // there begins to be significant light reflected away, an effect
    // which is not replicated here
    // ...
    // ...
    
    double p_out = 0;
    double testswitch = 0; //IRT CSS only for test
    
    for(int i = 1; i < 4; i++) 
    {
       p_out = p_out + (_bus[i]._voltage/1000.0)*(_bus[i]._current/1000.0);
    }
    for(int i = 0; i < 8; i++) 
    {
       int switchonoff = (_switch[i]._status != 0) ? 1 : 0;
       p_out = p_out + (_switch[i]._voltage/1000.0)*(_switch[i]._current/1000.0)*switchonoff;
       testswitch += switchonoff; //IRT CSS only for test
    }
    
    // IRT CSS : NOS3 computations for p_in are not clear ... using a different approach
    // SVB should be normalized 
    // p_in formula has to be refined
    // solar panels are only on X, Z faces 
    // TBD //TODO
    
    double magnitude_svb = sqrt(svb_X*svb_X+svb_Y*svb_Y+svb_Z*svb_Z); // svb Normalization 
    double svbn_X = svb_X/magnitude_svb;
    //double svbn_Y = svb_Y/magnitude_svb;
    double svbn_Z = svb_Z/magnitude_svb;
        
    double p_in = _power_per_panel*(std::max(svbn_X,0.0) + std::max(-svbn_X,0.0) + std::max(svbn_Z,0.0) + std::max(-svbn_Z,0.0)) ;  // expected between 0 and 20
    
    double delta_p = (_sim_microseconds_per_tick/1000000.0 * (p_in - p_out));
    _bus[0]._battery_watthrs = _bus[0]._battery_watthrs + (delta_p/3600); // the 3600 is for watt-sec to watt-hrs conversion
    
    // here is the code to increase or decrease the value of the battery
    // voltage. it is linear and +- 5% of the nominal voltage, which is 
    // a value I came across when doing some research
        
    double minF = 0.95;
    double deltaF = 0.1;
    
    if( testswitch > 3 ) {    // only for test when activating all switch //IRT CSS
    printf("**********  NB: %f Switch are ON ! /n",testswitch); 
    minF = 0.6+(1-testswitch/8.0)*0.35; // this is arbitrary and only for test on BATT_VOLTAGE
    deltaF = 0.2-(1-testswitch/8.0)*0.1; // this is arbitrary and only for test on BATT_VOLTAGE
    }
   
    double batt_min_voltage = minF*_nominal_batt_voltage;
    double batt_diff = deltaF*_nominal_batt_voltage;
    
    _bus[0]._voltage = (batt_min_voltage*1000.0 + batt_diff*1000.0*(_bus[0]._battery_watthrs / _max_battery));  // *1000
    
      
    // DEBUG Messages 
    //printf("Panel Sun vector X is %f /n", svb_X);
    //printf("Panel Sun vector Y is %f /n", svb_Y);
    //printf("Panel Sun vector Z is %f /n", svb_Z);
    printf("Power from the solar panel is %f /n", p_in);
    printf("Total power used is %f /n", p_out);
    printf("Battery Watt Hours are now %f /n", _bus[0]._battery_watthrs);
    printf("Battery Voltage is now %i /n", _bus[0]._voltage);
    
    }
           
    I2CSlaveConnection::I2CSlaveConnection(Generic_epsHardwareModel* hm,
        int bus_address, std::string connection_string, std::string bus_name)
        : NosEngine::I2C::I2CSlave(bus_address, connection_string, bus_name)
    {
        _hardware_model = hm;
    }

    size_t I2CSlaveConnection::i2c_read(uint8_t *rbuf, size_t rlen)
    {
        size_t num_read;
        if(_i2c_read_valid == GENERIC_EPS_SIM_SUCCESS)
        {
            for(num_read = 0; num_read < rlen; num_read++)
            {
                rbuf[num_read] = _i2c_out_data[num_read];
            }
            sim_logger->debug("i2c_read[%d]: %s", num_read, SimIHardwareModel::uint8_vector_to_hex_string(_i2c_out_data).c_str());
        }
        else
        {
            for(num_read = 0; num_read < rlen; num_read++)
            {
                rbuf[num_read] = 0x00;
            }
            sim_logger->debug("i2c_read[%d]: Invalid (0x00)", num_read);
        }

        return num_read;
    }

    size_t I2CSlaveConnection::i2c_write(const uint8_t *wbuf, size_t wlen)
    {
        std::vector<uint8_t> in_data(wbuf, wbuf + wlen);
        sim_logger->debug("i2c_write: %s",
            SimIHardwareModel::uint8_vector_to_hex_string(in_data).c_str()); // log data
        _i2c_read_valid = _hardware_model->determine_i2c_response_for_request(in_data, _i2c_out_data);
        return wlen;
    }
}
