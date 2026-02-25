/* Copyright (C) 2016 - 2015 National Aeronautics and Space Administration. All Foreign Rights are Reserved to the U.S. Government.

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

#ifndef NOS3_SIMIHARDWAREMODEL_HPP
#define NOS3_SIMIHARDWAREMODEL_HPP

#include <atomic>
#include <cstdint>
#include <vector>
#include <iomanip>

#include <boost/property_tree/ptree.hpp>
#include <boost/foreach.hpp>

#include <ItcLogger/Logger.hpp>
#include <Utility/BufferOverlay.hpp>
#include <Common/DataBufferOverlay.hpp>
#include <Common/Message.hpp>
#include <Client/Bus.hpp>
#include <Client/DataNode.hpp>

#include <sim_hardware_model_maker.hpp>
#define REGISTER_HARDWARE_MODEL(T,K) static Nos3::SimHardwareModelMaker<T> maker(K) // T = type, K = key
#include <sim_i_data_provider.hpp>
#include <sim_config.hpp>

namespace Nos3
{
    extern ItcLogger::Logger *sim_logger;

    /** \brief Interface for a hardware model.
     *
     */
    class SimIHardwareModel
    {
    public:
        /// @name Constructors / destructors
        //@{
        /// \brief Constructor taking a configuration object.
        /// @param  provider_key The name of the data provider
        /// @param  sc  The configuration for the simulation
        SimIHardwareModel(const boost::property_tree::ptree& config) :
            _keep_running(true),
            _absolute_start_time(config.get("common.absolute-start-time", 552110400.0)),
            _sim_microseconds_per_tick(config.get("common.sim-microseconds-per-tick", 1000000)),
            _real_microseconds_per_tick(config.get("common.real-microseconds-per-tick", 1000000)),
            _command_bus(nullptr),
            _command_node(nullptr)
        {
            if (config.get_child_optional("simulator.hardware-model.connections")) 
            {
                BOOST_FOREACH(const boost::property_tree::ptree::value_type &v, config.get_child("simulator.hardware-model.connections")) 
                {
                    // v.first is the name of the child.
                    // v.second is the child tree.
                    if (v.second.get("type", "").compare("command") == 0) 
                    {
                        // Set up the command node for this hardware model
                        _command_bus_name = v.second.get("bus-name", "command");
                        _command_node_name = v.second.get("node-name", "SimIHardwareModel");
                        _command_bus.reset(new NosEngine::Client::Bus(_hub, config.get("common.nos-connection-string", "tcp://127.0.0.1:12001"),
                            _command_bus_name));
                        _command_node = _command_bus->get_or_create_data_node(_command_node_name);
                        _command_node->set_message_received_callback(std::bind(&SimIHardwareModel::command_callback, this, std::placeholders::_1));
                        sim_logger->debug("SimIHardwareModel::SimIHardwareModel:  Command node %s now active on command bus %s.",
                            _command_node_name.c_str(), _command_bus_name.c_str());
                        break;
                    }
                }
            }
        }

        /// \brief Destructor.
        virtual ~SimIHardwareModel()
        {
            _command_bus.reset();
        }
        //@}

        /// @name Mutating public worker methods
        //@{

        /** \brief Method to run the hardware model simulation.
         */
        virtual void run(void)
        {
            // Spin so the callbacks remain valid
            while(_keep_running)
            {
                std::this_thread::sleep_for(std::chrono::microseconds(_real_microseconds_per_tick));
            }
        }

        /** \brief Method to determine what to do with a command to the simulator received on the command bus.  The default is to do nothing.
         *
         * @param       msg         The NOS Engine message sent with the command.
         */
        virtual void command_callback(NosEngine::Common::Message msg)
        {
            // default is no command handling... override me!!
            NosEngine::Common::DataBufferOverlay dbf(const_cast<NosEngine::Utility::Buffer&>(msg.buffer));
            sim_logger->debug("SimIHardwareModel::command_callback:  Received command: %s.  Doing nothing and returning UNIMPLEMENTED!", dbf.data);
            _command_node->send_reply_message_async(msg, 14, "UNIMPLEMENTED!");
        }
        //@}

        /// @name Non-mutating public worker methods
        //@{
        /** \brief Method to convert a vector of uint8_t to an ASCII hex string.
         *
         * @param       v   The buffer (vector) of bytes to be converted.
         * @return          The string with the converted bytes.
         */
        static std::string  uint8_vector_to_hex_string(const std::vector<uint8_t> & v)
        {
            std::stringstream ss;
            ss << std::hex << std::setfill('0');
            std::vector<uint8_t>::const_iterator it;

            for (it = v.begin(); it != v.end(); it++) 
            {
                ss << " 0x" << std::setw(2) << static_cast<unsigned>(*it);
            }

            return ss.str();
        };
        //@}

        //@{
        /** \brief Method to convert a vector of uint8_t to a string where each uint8_t is interpreted using its ASCII value.
         *
         * @param       v   The buffer (vector) of bytes to be converted.
         * @return          The string with the converted bytes.
         */
        static std::string  uint8_vector_to_ascii_string(const std::vector<uint8_t> & v)
        {
            std::stringstream ss;
            std::vector<uint8_t>::const_iterator it;

            for (it = v.begin(); it != v.end(); it++) 
            {
                ss << static_cast<char>(*it);
            }

            return ss.str();
        };
        //@}
        //@{
        /** \brief Method to convert an ASCII string to a vector of uint8_t.
         *
         * @param  in_data  The string of characters to convert.
         * @return          The buffer (vector) of converted bytes.
         */
        static std::vector<uint8_t> ascii_string_to_uint8_vector(const std::string& in_data)
        {
            std::vector<uint8_t> out_data;
            for (size_t i = 0; i < in_data.length(); i++) {
                out_data.push_back(in_data[i]);
            }
            return out_data;
        }
        //@}
        //@{
        /** \brief Method to convert a double to a vector of uint8_t.
         *
         * @param  in_data  The double to convert.
         * @return          The buffer (vector) of converted bytes.
         */
        static std::vector<uint8_t> double_to_uint8_vector(const double& in_data)
        {
            int64_t ival;
            static_assert(sizeof(double) == sizeof(int64_t), 
                "On this platform, double is not 64 bits.  This will cause issues with sending telemetry to COSMOS."); // not portable, but no surprises on the COSMOS end either and our assumed platform has 64 bit doubles
            std::memcpy(&ival, &in_data, sizeof(in_data));

            std::vector<uint8_t> out_data;
            for (size_t i = 0; i < sizeof(int64_t)/sizeof(uint8_t); i++) {
                out_data.push_back((uint8_t)( (ival >> (7-i)*8) & 0x000000FF) );
            }
            
            return out_data;
        }
        //@}
        //@{
        /** \brief Method to convert an int16_t to a vector of uint8_t.
         *
         * @param  in_data  The int16_t to convert.
         * @return          The buffer (vector) of converted bytes.
         */
        static std::vector<uint8_t> int16_to_uint8_vector(const int16_t& in_data)
        {
            std::vector<uint8_t> out_data;
            out_data.push_back((uint8_t)( (in_data >> 8) & 0x00FF) );
            out_data.push_back((uint8_t)( (in_data     ) & 0x00FF) );
            return out_data;
        }
        //@}
		
    protected:
        // Protected data
        std::atomic<bool>                            _keep_running;
        const double                                 _absolute_start_time;
        const int64_t                                _sim_microseconds_per_tick;
        int64_t                                      _real_microseconds_per_tick;
        NosEngine::Transport::TransportHub           _hub;
        std::string                                  _command_bus_name;
        std::string                                  _command_node_name;
        std::unique_ptr<NosEngine::Client::Bus>      _command_bus;
        NosEngine::Client::DataNode*                 _command_node;
    };
}

#endif
