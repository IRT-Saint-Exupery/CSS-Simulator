/* Copyright (C) 2016 - 2020 National Aeronautics and Space Administration. All Foreign Rights are Reserved to the U.S. Government.

   This software is provided "as is" without any warranty of any, kind either express, implied, or statutory, including, but not
   limited to, any warranty that the software will conform to, specifications any implied warranties of merchantability, fitness
   for a particular purpose, and freedom from infringement, and any warranty that the documentation will conform to the program, or
   any warranty that the software will be error free.

   In no event shall NASA be liable for any damages, including, but not limited to direct, indirect, special or consequential damages,
   arising out of, resulting from, or in any way connected with the software or its documentation.  Whether or not based upon warranty,
   contract, tort or otherwise, and whether or not loss was sustained from, or arose out of the results of, or use of, the software,
   documentation or services provided hereunder

   ITC Team
   NASA IV&V
   ivv-itc@lists.nasa.gov
*/

#include <generic_rw_hardware_model.hpp>
#include <generic_rw_sim_data_42socket_provider.hpp>

#include <ItcLogger/Logger.hpp>

#include <boost/property_tree/xml_parser.hpp>

namespace Nos3
{
    REGISTER_HARDWARE_MODEL(GenericRWHardwareModel,"GENERICREACTIONWHEELHARDWARE");

    extern ItcLogger::Logger *sim_logger;

    GenericRWHardwareModel::GenericRWHardwareModel(const boost::property_tree::ptree& config) : SimIHardwareModel(config), _keep_running(true)
    {
        sim_logger->trace("GenericRWHardwareModel::GenericRWHardwareModel:  Constructor executing");

        // Here's how to write out the config data passed
        //std::ostringstream oss;
        //write_xml(oss, config);
        //sim_logger->info("GenericRWHardwareModel::GenericRWHardwareModel:  "
        //    "configuration:\n%s", oss.str().c_str());

        // Here's how to get a time node to get time from
        std::string connection_string = config.get("common.nos-connection-string", "tcp://192.168.100.11:12001");

        std::string time_bus_name = "command";
        if (config.get_child_optional("hardware-model.connections")) 
        {
            BOOST_FOREACH(const boost::property_tree::ptree::value_type &v, config.get_child("hardware-model.connections")) 
            {
                if (v.second.get("type", "").compare("time") == 0) 
                {
                    time_bus_name = v.second.get("bus-name", "command");
                    break;
                }
            }
        }
        _time_bus.reset(new NosEngine::Client::Bus(_hub, connection_string, time_bus_name));

        // Here's how to get a UART node to communicate with
        std::string bus_name = "usart_2";
        int node_port = 2;
        if (config.get_child_optional("simulator.hardware-model.connections")) 
        {
            BOOST_FOREACH(const boost::property_tree::ptree::value_type &v, config.get_child("simulator.hardware-model.connections")) 
            {
                if (v.second.get("type", "").compare("usart") == 0) 
                {
                    bus_name = v.second.get("bus-name", bus_name);
                    node_port = v.second.get("node-port", node_port);
                    break;
                }
            }
        }
        _uart_connection.reset(new NosEngine::Uart::Uart(_hub, config.get("simulator.name", "generic-rw-sim"), connection_string,
            bus_name));
        _uart_connection->open(node_port);
        _uart_connection->set_read_callback(
            std::bind(&GenericRWHardwareModel::uart_read_callback, this, std::placeholders::_1, std::placeholders::_2));

        // Here's how to get a data provider
        std::string dp_name = config.get("simulator.hardware-model.data-provider.type", "GENERICRWSIMDATA42SOCKETPROVIDER");
        _sdp = SimDataProviderFactory::Instance().Create(dp_name, config);

        _wheel_number = config.get("simulator.hardware-model.data-provider.reactionwheel", 0);

        //_prev_data_sent_time = _absolute_start_time + 10.0;
        //_time_bus->add_time_tick_callback(std::bind(&GenericRWHardwareModel::send_periodic_data, this, std::placeholders::_1));

        sim_logger->trace("GenericRWHardwareModel::GenericRWHardwareModel:  Time node, UART node, data provider created; constructor exiting");
    }

    GenericRWHardwareModel::~GenericRWHardwareModel(void)
    {
        sim_logger->trace("GenericRWHardwareModel::GenericRWHardwareModel:  Destructor executing");
        delete _sdp; // Clean up the data provider we got
        _time_bus.reset(); // Must reset the time bus so the unique pointer does not try to delete the hub.  Do not destroy the time node, the bus will do it
        _uart_connection->close();
    }

    void GenericRWHardwareModel::run(void)
    {
        int i = 0;
        boost::shared_ptr<SimIDataPoint> dp;
        while(_keep_running) 
        {
            sim_logger->trace("GenericRWHardwareModel::run:  Loop count %d, time %f", i++,
                _absolute_start_time + (double(_time_bus->get_time() * _sim_microseconds_per_tick)) / 1000000.0);
            sleep(5);
        }
    }

    void GenericRWHardwareModel::uart_read_callback(const uint8_t *buf, size_t len)
    {
        // Get the data out of the message bytes
        std::vector<uint8_t> in_data(buf, buf + len);
        std::string request = SimIHardwareModel::uint8_vector_to_ascii_string(in_data);
        sim_logger->debug("GenericRWHardwareModel::uart_read_callback:  REQUEST %s", request.c_str()); // log data in a man readable format

        // Figure out how to respond
        std::string response = handle_command(request);

        // Ship the message bytes off
        std::vector<uint8_t> out_data = SimIHardwareModel::ascii_string_to_uint8_vector(response);
        sim_logger->debug("GenericRWHardwareModel::uart_read_callback:  REPLY   %s\n", response.c_str()); // log data in a man readable format
        _uart_connection->write(&out_data[0], out_data.size());
    }

    std::string GenericRWHardwareModel::handle_command(std::string command)
    {
        std::string response = "Unknown_Command=" + command;
        boost::to_upper(command);
        if (command.substr(0,11).compare("SET_TORQUE=") == 0)
        {
            double torque = 0.0;
            try {
                torque = std::stod(command.substr(11));
            }
            catch (...) {
                sim_logger->error("Invalid torque command value");
            }
            std::stringstream ss;
            ss << "SC[0].AC.Whl[" << _wheel_number << "].Tcmd = ";
            ss << torque;

            dynamic_cast<GenericRWData42SocketProvider*>(_sdp)->send_command_to_socket(ss.str());
            response = "SET_TORQUE=" + ss.str();
        } else if (command.substr(0,16).compare("CURRENT_MOMENTUM") == 0)
        {
            const boost::shared_ptr<GenericRWDataPoint> data_point =
                boost::dynamic_pointer_cast<GenericRWDataPoint>(_sdp->get_data_point());
            response = "CURRENT_MOMENTUM=" + std::to_string(data_point->get_momentum());
        }

        return response;
    }

    void GenericRWHardwareModel::command_callback(NosEngine::Common::Message msg)
    {
        // Here's how to get the data out of the message
        NosEngine::Common::DataBufferOverlay dbf(const_cast<NosEngine::Utility::Buffer&>(msg.buffer));
        sim_logger->info("GenericRWHardwareModel::command_callback:  Received command: %s.", dbf.data);

        // Do something with the data
        std::string command = dbf.data;
        std::string response = "GenericRWHardwareModel::command_callback:  INVALID COMMAND! (Try STOP RWSIM)";
        boost::to_upper(command);
        if (command.substr(0,11).compare("STOP RWSIM") == 0) 
        {
            _keep_running = false;
            response = "GenericRWHardwareModel::command_callback:  STOPPING RWSIM";
        } else {
            response = handle_command(command);
        }

        // Here's how to send a reply
        sim_logger->info("GenericRWHardwareModel::command_callback:  Sending reply: %s.", response.c_str());
        _command_node->send_reply_message_async(msg, response.size(), response.c_str());
    }

    void GenericRWHardwareModel::send_periodic_data(NosEngine::Common::SimTime time)
    {
        const boost::shared_ptr<GenericRWDataPoint> data_point =
            boost::dynamic_pointer_cast<GenericRWDataPoint>(_sdp->get_data_point());

        std::vector<uint8_t> data;

        double abs_time = _absolute_start_time + (double(time * _sim_microseconds_per_tick)) / 1000000.0;
        double next_time = _prev_data_sent_time + _period - (_sim_microseconds_per_tick / 1000000.0) / 2; // within half a tick time period
        if (next_time < abs_time) { // Time to send more data
            _prev_data_sent_time = abs_time;
            create_rw_data(*data_point, data);
            _uart_connection->write(&data[0], data.size());
        }
    }

    void GenericRWHardwareModel::create_rw_data(const GenericRWDataPoint& data_point, std::vector<uint8_t>& out_data)
    {
        double momentum = data_point.get_momentum();
        std::stringstream ss;

        ss << "MOMENTUM=";
        ss << std::setprecision(6);
        ss << momentum;
        std::string message;
        message.append(ss.str());

        for (size_t i = 0; i < message.length(); i++) {
            out_data.push_back(message[i]);
        }

        sim_logger->trace("GenericRWHardwareModel::create_rw_data:  %s", message.c_str());
    }

}
