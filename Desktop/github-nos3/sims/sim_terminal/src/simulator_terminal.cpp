/* Copyright (C) 2015 - 2021 National Aeronautics and Space Administration. All Foreign Rights are Reserved to the U.S. Government.

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

#include <simulator_terminal.hpp>

#include <iostream>
#include <thread>
#include <memory>
#include <stdexcept>

#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#include <readline/readline.h>
#include <readline/history.h>

#include <ItcLogger/Logger.hpp>
#include <Client/Bus.hpp>
#include <Client/DataNode.hpp>
#include <Utility/Buffer.hpp>
#include <Utility/BufferOverlay.hpp>
#include <I2C/Client/I2CMaster.hpp>
#include <Can/Client/CanMaster.hpp>
#include <Spi/Client/SpiMaster.hpp>
#include <Uart/Client/Uart.hpp>

#include <sim_hardware_model_factory.hpp>
#include <sim_i_hardware_model.hpp>
#include <sim_config.hpp>

#include <boost/algorithm/string/find.hpp>

#include <bus_connections.hpp>

namespace Nos3
{
    REGISTER_HARDWARE_MODEL(SimTerminal,"SimTerminal");

    ItcLogger::Logger *sim_logger;


    // Constructors
    SimTerminal::SimTerminal(const boost::property_tree::ptree& config) : SimIHardwareModel(config),
        _bus_name(config.get("simulator.hardware-model.bus.name", "command")),
        _other_node_name(config.get("simulator.hardware-model.other-node-name", "time")),
        _current_in_mode((config.get("simulator.hardware-model.input-mode", "").compare("HEX") == 0) ? HEX : ASCII),
        _current_out_mode((config.get("simulator.hardware-model.output-mode", "").compare("HEX") == 0) ? HEX : ASCII),
        _prompt(LONG),
        _terminal_type((config.get("simulator.hardware-model.terminal.type", "STDIO").compare("STDIO") == 0) ? STDIO : UDP),
        _udp_port(config.get("simulator.hardware-model.terminal.udp-port", 5555)),
        _suppress_output(config.get("simulator.hardware-model.terminal.suppress-output", false))
    {
        std::string bus_type = config.get("simulator.hardware-model.bus.type", "command");
        if (!set_bus_type(bus_type)) {
            sim_logger->error("Invalid bus type setting %s.  Setting bus type to COMMAND.", bus_type.c_str());
        }
        _nos_connection_string = config.get("common.nos-connection-string", "tcp://192.168.100.11:12001");
        _command_node_name = config.get("simulator.hardware-model.terminal-node-name", "terminal");

        _connection_strings["default"] = _nos_connection_string;

        BOOST_FOREACH(const boost::property_tree::ptree::value_type &v, config.get_child("simulator.hardware-model.other-nos-connections")) 
        {
            std::string name = v.second.get("name", "");
            std::string connection_string = v.second.get("connection-string", "");
            if ((name.compare("") != 0) && (name.compare("default") != 0)) {
                _connection_strings[name] = connection_string;
            }
        }

        _active_connection_name = "default";

        reset_bus_connection();

        if (config.get_child_optional("simulator.hardware-model.startup-commands")) 
        {
            BOOST_FOREACH(const boost::property_tree::ptree::value_type &v, config.get_child("simulator.hardware-model.startup-commands")) 
            {
                if (v.first.compare("command") == 0) {
                    process_command(v.second.data());
                }
            }
        }
    }

    /// @name Mutating public worker methods
    //@{
    /// \brief Runs the server, creating the NOS Engine bus and the transports for the simulator and simulator client to connect to.
    void SimTerminal::run(void)
    {
        try
        {
            // when handle_* returns... it is time to quit
            switch(_terminal_type)
            {
            case STDIO:
                handle_input();
                break;
            case UDP:
                handle_udp();
                break;
            default:
                Nos3::sim_logger->error("SimTerminal::run: Invalid terminal type %s (valid types are STDIO, UDP)", _terminal_type_string[_terminal_type].c_str());            
            }
        }
        catch(...)
        {
            Nos3::sim_logger->error("SimTerminal::run:  Exception caught!");
        }
    }
    //@}

    void SimTerminal::write_message_to_cout(const char* buf, size_t len){
        for (unsigned int i = 0; i < len; i++) {
            if (_current_out_mode == HEX) {
                std::cout << " 0x" << convert_hexhexchar_to_asciihexchars(buf[i]);
            } else {
                std::cout << buf[i];
            }
        }

        std::cout << std::endl;
    }

    std::stringstream SimTerminal::write_message_to_stream(const char* buf, size_t len){
        std::stringstream ss;
        for (unsigned int i = 0; i < len; i++) {
            if (_current_out_mode == HEX) {
                ss << " 0x" << convert_hexhexchar_to_asciihexchars(buf[i]);
            } else {
                ss << buf[i];
            }
        }
        return ss;
    }

    void SimTerminal::write_message_to_cout(const NosEngine::Common::Message& msg)
    {
        NosEngine::Common::DataBufferOverlay dbf(const_cast<NosEngine::Utility::Buffer&>(msg.buffer));
        write_message_to_cout(dbf.data, dbf.len);
    }

    void SimTerminal::handle_udp(void)
    {
        int sockfd;
        char buffer[_MAXLINE];
        struct sockaddr_in servaddr, cliaddr;
        if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
            std::cout << "SimTerminal::handle_udp - Failed to create a socket";
        } else {
            memset(&servaddr, 0, sizeof(servaddr));
            servaddr.sin_family = AF_INET;
            servaddr.sin_addr.s_addr = INADDR_ANY;
            servaddr.sin_port = htons(_udp_port);
            if (bind(sockfd, (const struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) {
                std::cout << "SimTerminal::handle_udp - Failed to bind to socket";
            } else {
                socklen_t len;
                int n;
                len = sizeof(cliaddr);
                std::string input(buffer);
                boost::to_upper(input);
                boost::trim(input);
                while(input.compare("QUIT") != 0) {
                    n = recvfrom(sockfd, (char *)buffer, _MAXLINE, 0, (struct sockaddr *)&cliaddr, &len);
                    buffer[n] = '\0';
                    input = buffer;
                    std::string result = process_command(input);
                    if (!_suppress_output) {
                        if (result.size() > 0) sendto(sockfd, result.c_str(), result.size(), 0, (const struct sockaddr *)&cliaddr, len);
                    }
                    std::string prompt = string_prompt();
                    if (prompt.size() > 0) sendto(sockfd, prompt.c_str(), prompt.size(), 0, (const struct sockaddr *)&cliaddr, len);

                    boost::to_upper(input);
                    boost::trim(input);
                }
            }
        }
    }

    void SimTerminal::handle_input(void)
    {
        std::string input;
        std::cout << "This is the simulator terminal program.  Type 'HELP' for help." << std::endl << std::endl;
        while(getline(string_prompt(), input)) // keep looping and getting the next command line
        {
            std::string result = process_command(input);
            if (result.compare("QUIT") == 0) {
                break;
            } else {
                if (!_suppress_output) std::cout << result;
            }
        }

        std::cout << "SimTerminal is quitting!" << std::endl;
    }

    std::string SimTerminal::string_prompt(void)
    {
        std::stringstream ss;
        if (!_suppress_output) {
            if (_prompt == LONG) {
                ss  <<         _command_node_name 
                    << "-"  << _active_connection_name
                    << "<"  << _other_node_name            << ">" 
                    << ":(" << _bus_type_string[_bus_type] << ")"    << _bus_name
                    << ":[" << mode_as_string()            << "] $ ";
            } else if (_prompt == SHORT) {
                ss  <<         _command_node_name
                    << "-"  << _active_connection_name
                    << "->" << _other_node_name
                    << "@(" << _bus_type_string[_bus_type] << ")"    << _bus_name
                    << "["  << mode_as_string()            << "] $ ";
            } // else, no prompt, let ss blank
        } // else, suppress output, let ss blank
        return ss.str();
    }

    bool SimTerminal::getline(const std::string& prompt, std::string& input)
    {
        bool retval = true;
        input.clear();
        char *line_read = readline(prompt.c_str());
        if (line_read) {
            if (*line_read) add_history(line_read); // don't add blank lines
            input.append(line_read);
            free(line_read);
            retval = true;
        } else {
            retval = false;
        }
        return retval;
    }

    std::string SimTerminal::process_command(std::string input){
        std::string input_trimmed = input;
        boost::trim(input_trimmed);
        std::stringstream tokenizer(input_trimmed);
        std::string token;
        std::vector<std::string> input_tokens, input_tokens_upper;
        while (tokenizer.good()) {
            tokenizer >> token;
            input_tokens.push_back(token);
            boost::to_upper(token);
            input_tokens_upper.push_back(token);
        }
        std::stringstream ss;

        if ((input_tokens_upper.size() == 1) && (input_tokens_upper[0].compare("HELP") == 0))
        {
            ss << "This is help for the simulator terminal program." << std::endl;
            ss << "  The prompt shows the <simulator terminal node name@simulator bus name> and <simulator node being commanded> " << std::endl;
            ss << "  Commands (case only matters for non-enumerated arguments; whitespace only matters for separating tokens):" << std::endl;
            ss << "    HELP - Displays this help" << std::endl;
            ss << "    QUIT - Exits the program" << std::endl;
            ss << "    SET SIMNODE <sim node> - Sets the simulator node being commanded to '<sim node>'" << std::endl;
            ss << "    SET SIMBUS <sim bus> - Sets the simulator bus for the simulator node being commanded to '<sim bus>'" << std::endl;
            ss << "    SET SIMBUSTYPE <bus type> - Sets the simulator bus type for the simulator node being commanded to '<bus type>'" << std::endl; 
            ss << "        (BASE, I2C, CAN, SPI, UART, COMMAND are valid)" << std::endl;
            ss << "    SET TERMNODE <term node> - Sets the name of this terminal's node to '<term node>'" << std::endl;
            ss << "    SET <ASCII|HEX> <IN|OUT> - Sets the terminal mode to ASCII mode or HEX mode; optionally IN or OUT only" << std::endl;
            ss << "    SET PROMPT <LONG|SHORT|NONE> - Sets the prompt to long format, short format, or none" << std::endl;
            ss << "    SUPPRESS OUTPUT <ON|OFF> - Suppresses output or not" << std::endl;
            ss << "    LIST NOS CONNECTIONS - Lists all of the known NOS Engine connection strings along with a name for selecting them" << std::endl;
            ss << "    SET NOS CONNECTION <name> - Sets the NOS Engine connection to the one associated with <name> (initially \"default\")" << std::endl;
            ss << "    ADD NOS CONNECTION <name> <uri> - Adds NOS Engine URI connection string <uri> to the list of known connection strings and associates it with <name>" << std::endl;
            ss << "    WRITE <data> - Writes <data> to the current node. Interprets <data> as ascii or hex depending on input setting." << std::endl;
            ss << "    READ <length> - Reads the given number of bytes from the current node. Only works on SPI and I2C buses." << std::endl;
            ss << "    TRANSACT <read length> <data> - Performs a transaction. Sends the given data, and expects a return value of the given length." << std::endl;
            ss << "             Interprets everything after the first space after <read length> as data to be written." << std::endl;
        } 
        else if ((input_tokens_upper.size() == 3) && (input_tokens_upper[0].compare("SET") == 0) && (input_tokens_upper[1].compare("SIMNODE") == 0))
        {
            _other_node_name = input_tokens[2];
            _bus_connection->set_target(_other_node_name);
        } 
        else if ((input_tokens_upper.size() == 3) && (input_tokens_upper[0].compare("SET") == 0) && (input_tokens_upper[1].compare("SIMBUS") == 0))
        {
            std::string new_command_bus_name = input_tokens[2];
            if(new_command_bus_name.compare(_bus_name) != 0){
                _bus_name = new_command_bus_name;
                reset_bus_connection();
            }else{
                ss << "Already on bus: " << _bus_name << "." << std::endl;
            }
        } 
        else if ((input_tokens_upper.size() == 3) && (input_tokens_upper[0].compare("SET") == 0) && (input_tokens_upper[1].compare("SIMBUSTYPE") == 0))
        {
            std::string new_command_bus_type = input_tokens_upper[2];
            if (set_bus_type(new_command_bus_type)) {
                reset_bus_connection();
            } else {
                 ss << "Invalid bus type setting: " << new_command_bus_type << ".  Not changing bus type." << std::endl;
            }
        } 
        else if ((input_tokens_upper.size() == 3) && (input_tokens_upper[0].compare("SET") == 0) && (input_tokens_upper[1].compare("TERMNODE") == 0))
        {
            _command_node_name = input_tokens[2];
            reset_bus_connection();
        } 
        else if ((input_tokens_upper.size() >=2) && (input_tokens_upper[0].compare("SET") == 0) && (input_tokens_upper[1].compare("ASCII") == 0))
        {
            std::string in_out = "";
            if (input_tokens_upper.size() >= 3) in_out = input_tokens_upper[2];
            if ((in_out.compare("") == 0) || (in_out.compare("IN") == 0)) _current_in_mode = ASCII;
            if ((in_out.compare("") == 0) || (in_out.compare("OUT") == 0)) _current_out_mode = ASCII;
        } 
        else if ((input_tokens_upper.size() >=2) && (input_tokens_upper[0].compare("SET") == 0) && (input_tokens_upper[1].compare("HEX") == 0))
        {
            std::string in_out = "";
            if (input_tokens_upper.size() >= 3) in_out = input_tokens_upper[2];
            if ((in_out.compare("") == 0) || (in_out.compare("IN") == 0)) _current_in_mode = HEX;
            if ((in_out.compare("") == 0) || (in_out.compare("OUT") == 0)) _current_out_mode = HEX;
        } 
        else if ((input_tokens_upper.size() == 3) && (input_tokens_upper[0].compare("SET") == 0) && (input_tokens_upper[1].compare("PROMPT") == 0))
        {
            std::string prompt_type = input_tokens_upper[2];
            if (prompt_type.compare("LONG") == 0) _prompt = LONG;
            else if (prompt_type.compare("SHORT") == 0) _prompt = SHORT;
            else if (prompt_type.compare("NONE") == 0) _prompt = NONE;
            else ss << "Invalid prompt length specified: " << prompt_type << "." << std::endl;
        }
        else if ((input_tokens_upper.size() == 3) && (input_tokens_upper[0].compare("SUPPRESS") == 0) && (input_tokens_upper[1].compare("OUTPUT") == 0))
        {
            std::string on_off = input_tokens_upper[2];
            if (on_off.compare("ON") == 0) _suppress_output = true;
            else if (on_off.compare("OFF") == 0) _suppress_output = false;
            else ss << "Invalid suppress output flag specified (valid values are ON, OFF): " << on_off << "." << std::endl;
        }
        else if ((input_tokens_upper.size() == 3) && (input_tokens_upper[0].compare("LIST") == 0) && (input_tokens_upper[1].compare("NOS") == 0) && (input_tokens_upper[2].compare("CONNECTIONS") == 0))
        {
            for (std::map<std::string, std::string>::const_iterator it = _connection_strings.begin(); it != _connection_strings.end(); it++)
                ss << "    name=" << it->first << ", connection string=" << it->second << std::endl;
        }
        else if ((input_tokens_upper.size() == 4) && (input_tokens_upper[0].compare("SET") == 0) && (input_tokens_upper[1].compare("NOS") == 0) && (input_tokens_upper[2].compare("CONNECTION") == 0))
        {
            std::string name = input_tokens[3];
            std::string old_connection_string = _nos_connection_string;
            try {
                std::string connection_string = _connection_strings.at(name);
                if (connection_string.compare(_nos_connection_string) != 0) {
                    _nos_connection_string = connection_string;
                    reset_bus_connection();
                    _active_connection_name = name; // set after reset in case it throws
                } else {
                    ss << "Connection string is the same as the current one; doing nothing." << std::endl;
                }
            } catch (std::exception&) {
                _nos_connection_string = old_connection_string;
                ss << "Invalid connection: \"" << name << "\"." << std::endl;
            }
        }
        else if ((input_tokens_upper.size() == 5) && (input_tokens_upper[0].compare("ADD") == 0) && (input_tokens_upper[1].compare("NOS") == 0) && (input_tokens_upper[2].compare("CONNECTION") == 0))
        {
            _connection_strings[input_tokens[3]] = input_tokens[4];
        }
        else if ((input_tokens_upper.size() == 1) && (input_tokens_upper[0].compare("QUIT") == 0))
        {
            ss << "QUIT";
        }
        else if ((input_tokens_upper.size() >= 2) && (input_tokens_upper[0].compare("WRITE") == 0))
        {
            if(_bus_connection.get() == nullptr){
                ss << "Connection has not been instantiated. Connect to a bus with SET SIMBUS." << std::endl;
            } else {
                std::string buf = input.substr(6, input.length() - 6);
                boost::trim(buf);
                if(_current_in_mode == HEX){
                    buf = convert_asciihex_to_hexhex(buf);
                }
                int wlen = buf.length();

                try{
                    _bus_connection->write(buf.c_str(), wlen);
                }catch (std::runtime_error &e){
                    ss << e.what() << std::endl;
                }
            }
        }
        else if ((input_tokens_upper.size() == 2) && (input_tokens_upper[0].compare("READ") == 0))
        {
            if(_bus_connection.get() == nullptr){
                ss << "Connection has not been instantiated. Connect to a bus with SET SIMBUS." << std::endl;
            } else {
                char buf[255];
                int len;
                std::string len_string = input_tokens[1];
                try{
                    len = stoi(len_string);
                }catch (std::invalid_argument &e){
                    len = 0;
                }

                try {
                    _bus_connection->read(buf, len);
                    ss = write_message_to_stream(buf, len);
                }catch (std::runtime_error &e){
                    ss << e.what() << std::endl;
                }
            }            
        }
        else if ((input_tokens_upper.size() >= 3) && (input_tokens_upper[0].compare("TRANSACT") == 0))
        {
            if(_bus_connection.get() == nullptr){
                ss << "Connection has not been instantiated. Connect to a bus with SET SIMBUS." << std::endl;
            } else {
                char rbuf[255];
                int rlen;
                int dataStart;

                boost::iterator_range<std::string::iterator> r = boost::find_nth(input, " ", 1);
                dataStart = std::distance(input.begin(), r.begin()) + 1;

                std::string wbuf = input.substr(dataStart, input.length() - dataStart);
                boost::trim(wbuf);
                if(_current_in_mode == HEX){
                    wbuf = convert_asciihex_to_hexhex(wbuf);
                }
                int wlen = wbuf.length();
                try {
                    rlen = stoi(input_tokens[1]);
                    //ss << "rlen: " << rlen << ", Data: " << input.substr(dataStart, input.length() - dataStart) << std::endl;
                    _bus_connection->transact(wbuf.c_str(), wlen, rbuf, rlen);
                    ss = write_message_to_stream(rbuf, rlen);
                }catch (std::invalid_argument &e){
                    ss << "\"" << input_tokens[1] << "\" is not a valid number." << std::endl;
                }catch (std::runtime_error &e){
                    ss << e.what() << std::endl;
                }
            }
        }
        else if (input.length() > 0)
        {
            ss << "Unrecognized command \"" << input << "\". Type \"HELP\" for help." << std::endl;
        }

        std::string retval = ss.str();
        return retval;
    }

    void SimTerminal::reset_bus_connection(){
         // Figure out where _old_connection lives and what data structure it belongs to
         if (_bus_type == I2C){
            int master_address;
            try{
                master_address = stoi(_command_node_name);
            }catch(std::invalid_argument &e){
                master_address = 127;
                _command_node_name = "127";
                std::cout << "\"" << _command_node_name << "\" is not a valid I2C address for the terminal. Defaulting to 127." << std::endl;
            }
            // if old connection exists, free it
            if (_bus_connection.get() != nullptr) {
                BusConnection* old = _bus_connection.release();
                delete old;
                // Nos3::sim_logger->debug("reset_bus_connection: deleting old bus connection");
            }
            
            I2CConnection* i2c = new I2CConnection(master_address, _nos_connection_string, _bus_name);
            _bus_connection.reset(i2c);
        } else if (_bus_type == CAN){
            int master_identifier;
            try{
                master_identifier = stoi(_command_node_name);
            }catch(std::invalid_argument &e){
                master_identifier = 127;
                _command_node_name = "127";
                std::cout << "\"" << _command_node_name << "\" is not a valid CAN identifier for the terminal. Defaulting to 127." << std::endl;
            }
            // if old connection exists, free it
            if (_bus_connection.get() != nullptr) {
                BusConnection* old = _bus_connection.release();
                delete old;
                // Nos3::sim_logger->debug("reset_bus_connection: deleting old bus connection");
            }
            
            CANConnection* can = new CANConnection(master_identifier, _nos_connection_string, _bus_name);
            _bus_connection.reset(can);
        } else if(_bus_type == SPI){
            // if old connection exists, free it
            if (_bus_connection.get() != nullptr) {
                BusConnection* old = _bus_connection.release();
                delete old;
                // Nos3::sim_logger->debug("reset_bus_connection: deleting old bus connection");
            }
           _bus_connection.reset(new SPIConnection(_nos_connection_string, _bus_name));
        }else if(_bus_type == UART){
            // if old connection exists, free it
            if (_bus_connection.get() != nullptr) {
                BusConnection* old = _bus_connection.release();
                delete old;
                // Nos3::sim_logger->debug("reset_bus_connection: deleting old bus connection");
            }
            _bus_connection.reset(new UartConnection(this, _command_node_name, _nos_connection_string, _bus_name));
        }else{ // not differentiating between BASE and COMMAND types... yet
            // if old connection exists, free it
            if (_bus_connection.get() != nullptr) {
                BusConnection* old = _bus_connection.release();
                delete old;
                // Nos3::sim_logger->debug("reset_bus_connection: deleting old bus connection");
            }
            _bus_connection.reset(new BaseConnection(this, _command_node_name, _nos_connection_string, _bus_name));
        }
        _bus_connection->set_target(_other_node_name);
    }

    std::string SimTerminal::mode_as_string(void)
    {
        std::string mode;
        if (_prompt == LONG) {
            if (_current_in_mode == ASCII) mode.append("IN=ASCII:");
            if (_current_in_mode == HEX) mode.append("IN=HEX:");
            if (_current_out_mode == ASCII) mode.append("OUT=ASCII");
            if (_current_out_mode == HEX) mode.append("OUT=HEX");
        } else if (_prompt == SHORT) {
            if (_current_in_mode == ASCII) mode.append("I=A:");
            if (_current_in_mode == HEX) mode.append("I=H:");
            if (_current_out_mode == ASCII) mode.append("O=A");
            if (_current_out_mode == HEX) mode.append("O=H");
        } // else, no prompt, let mode blank
        return mode;
    }

    std::string SimTerminal::convert_hexhexchar_to_asciihexchars(uint8_t in)
    {
        std::string out;
        uint8_t inupper = (in & 0xF0) >> 4;
        uint8_t inlower = in & 0x0F;
        out.push_back(convert_hexhexnibble_to_asciihexchar(inupper));
        out.push_back(convert_hexhexnibble_to_asciihexchar(inlower));
        return out;
    }

    char SimTerminal::convert_hexhexnibble_to_asciihexchar(uint8_t in)
    {
        char out = '.';
        if (in <= 0x9) out = in - 0x0 + '0';
        if ((0xA <= in) && (in <= 0xF)) out = in - 0xA + 'A';
        return out;
    }

    std::string SimTerminal::convert_asciihex_to_hexhex(std::string in)
    {
        std::string out;
        in.push_back('0'); // in case there are an odd number of characters, tack a 0 on the end
        for (size_t i = 0; i < in.size() - 1; i += 2) {
            out.push_back(convert_asciihexcharpair_to_hexhexchar(in[i], in[i+1]));
        }
        return out;
    }

    uint8_t SimTerminal::convert_asciihexcharpair_to_hexhexchar(char in1, char in2)
    {
        uint8_t outupper = convert_asciihexchar_to_hexhexchar(in1);
        uint8_t outlower = convert_asciihexchar_to_hexhexchar(in2);
        uint8_t out = ((outupper << 4) + outlower);
        return out;
    }

    uint8_t SimTerminal::convert_asciihexchar_to_hexhexchar(char in)
    {
        uint8_t out = 0;
        if (('0' <= in) && (in <= '9')) out = in - '0';
        if (('A' <= in) && (in <= 'F')) out = in - 'A' + 10;
        if (('a' <= in) && (in <= 'f')) out = in - 'a' + 10;
        return out;
    }

    bool SimTerminal::set_bus_type(std::string type)
    {
        bool succeeded = true;
        boost::to_upper(type);
        boost::trim(type);
        if (type.compare("BASE") == 0) {
            _bus_type = BASE;
        } else if (type.compare("I2C") == 0) {
            _bus_type = I2C;
        } else if (type.compare("CAN") == 0) {
            _bus_type = CAN;
        } else if (type.compare("SPI") == 0) {
            _bus_type = SPI;
        } else if (type.compare("UART") == 0) {
            _bus_type = UART;
        } else if (type.compare("COMMAND") == 0) {
            _bus_type = COMMAND;
        } else {
            succeeded = false;
        }
        return succeeded;
    }
}
