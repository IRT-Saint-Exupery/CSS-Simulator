/* Copyright (C) 2015 - 2015 National Aeronautics and Space Administration. All Foreign Rights are Reserved to the U.S. Government.

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

#ifndef NOS3_SIMULATOR_TERMINAL_HPP
#define NOS3_SIMULATOR_TERMINAL_HPP

#include <iostream>
#include <thread>
#include <memory>
#include <stdexcept>

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

#include <bus_connections.hpp>

namespace Nos3
{
    class SimTerminal : public SimIHardwareModel
    {
    public:
        // Constructors
        SimTerminal(const boost::property_tree::ptree& config);

        // Mutators
        void run(void);

        // Accessors
        void write_message_to_cout(const char* buf, size_t len);
        void write_message_to_cout(const NosEngine::Common::Message& msg);

    private:
        // private types
        enum SimTerminalMode {HEX, ASCII};
        enum BusType {BASE, I2C, CAN, SPI, UART, COMMAND};
        const std::string _bus_type_string[6] = {"BASE", "I2C", "CAN", "SPI", "UART", "COMMAND"};
        enum PromptType {LONG, SHORT, NONE};
        enum TerminalType {STDIO, UDP};
        const std::string _terminal_type_string[2] = {"STDIO", "UDP"};

        // private helper methods
        void handle_input(void);
        void handle_udp(void);
        std::string string_prompt(void);
        bool getline(const std::string& prompt, std::string& input);
        std::string process_command(std::string input);
        void reset_bus_connection();
        
        // private helper helpers
        std::stringstream write_message_to_stream(const char* buf, size_t len);
        std::string mode_as_string(void);
        std::string convert_hexhexchar_to_asciihexchars(uint8_t in);
        char convert_hexhexnibble_to_asciihexchar(uint8_t in);
        std::string convert_asciihex_to_hexhex(std::string in);
        uint8_t convert_asciihexcharpair_to_hexhexchar(char in1, char in2);
        uint8_t convert_asciihexchar_to_hexhexchar(char in);
        bool set_bus_type(std::string type);

        // private data
        static const int _MAXLINE = 1024;
        std::map<std::string, std::string> _connection_strings;
        std::string _nos_connection_string;
        std::string _active_connection_name;
        std::string _bus_name;
        BusType _bus_type;
        std::string _other_node_name;
        std::unique_ptr<class BusConnection> _bus_connection;
        enum SimTerminalMode _current_in_mode;
        enum SimTerminalMode _current_out_mode;
        enum PromptType _prompt;
        enum TerminalType _terminal_type;
        int _udp_port;
        bool _suppress_output;
    };
}

#endif
