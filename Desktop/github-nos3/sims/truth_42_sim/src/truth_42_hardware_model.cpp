#include <truth_42_hardware_model.hpp>

namespace Nos3
{
    REGISTER_HARDWARE_MODEL(Truth42HardwareModel,"TRUTH42");

    extern ItcLogger::Logger *sim_logger;

    Truth42HardwareModel::Truth42HardwareModel(const boost::property_tree::ptree& config) : SimIHardwareModel(config)
    {
        std::string connection_string = config.get("common.nos-connection-string", "tcp://192.168.100.11:12001"); // Get the NOS engine connection string, needed for the busses
        sim_logger->info("SampleHardwareModel::SampleHardwareModel:  NOS Engine connection string: %s.", connection_string.c_str());

        /* vvv 1. Get a data provider */
        /* !!! If your sim does not *need* a data provider, delete this block. */
        std::string dp_name = config.get("simulator.hardware-model.data-provider.type", "TRUTH_42_PROVIDER");
        _truth_42_dp = SimDataProviderFactory::Instance().Create(dp_name, config);
        sim_logger->info("Truth42HardwareModel::Truth42HardwareModel:  Data provider %s created.", dp_name.c_str());
        /* ^^^ 1. Get a data provider */

        /* vvv 2. Get on the computer bus... in this case it is actually the COSMOS socket, since this is truth data and so it bypasses the flight software computer */
        boost::asio::io_service io_service;
        _socket = new boost::asio::ip::udp::socket(io_service);
        _remote = boost::asio::ip::udp::endpoint(boost::asio::ip::address::from_string(config.get("simulator.hardware-model.cosmos-ip", "192.168.100.5")), config.get("simulator.hardware-model.cosmos-port", 5111));
        _socket->open(boost::asio::ip::udp::v4());

        /* vvv 3. Streaming data */
        _initial_stream_time = config.get("simulator.hardware-model.initial-stream-time", 1.0); // Delta from start time to begin streaming
        _prev_time = _absolute_start_time + _initial_stream_time;
        _stream_period_ms = config.get<uint32_t>("simulator.hardware-model.stream-period-ms", 1000); // Time in milliseconds between streamed messages
        _use_nos_time = (config.get("simulator.hardware-model.nos-or-wall-time", "NOS").compare("WALL") != 0); // "NOS" to use NOS engine time ticks to drive streamed messages; "WALL" to use wall time to drive streamed messages

        std::string time_bus_name = "command"; // Initialize to default in case value not found in config file
        if (config.get_child_optional("simulator.hardware-model.connections")) 
        {
            BOOST_FOREACH(const boost::property_tree::ptree::value_type &v, config.get_child("simulator.hardware-model.connections")) // Loop through the connections for *this* hw model
            {
                if (v.second.get("type", "").compare("time") == 0) // v.second is the child tree (v.first is the name of the child)
                {
                    time_bus_name = v.second.get("bus-name", "command");
                    break; // Found it... don't need to go through any more items
                }
            }
        }
        _time_bus.reset(new NosEngine::Client::Bus(_hub, connection_string, time_bus_name));
        if (_use_nos_time) {
            _time_bus->add_time_tick_callback(std::bind(&Truth42HardwareModel::send_streaming_data, this, std::placeholders::_1));
            sim_logger->info("Truth42HardwareModel::Truth42HardwareModel:  Now on time bus %s, executing callback to stream data.", time_bus_name.c_str());
        } // else we are going to stream messages based on wall time in the run method
        /* ^^^ 3. Streaming data */
    }

    Truth42HardwareModel::~Truth42HardwareModel(void)
    {        
        // 1. Close the COSMOS connection
        _socket->close(); 
        delete _socket;

        // 2. Clean up the data provider we got
        delete _truth_42_dp;
        _truth_42_dp = nullptr;

        // 3. Don't need to clean up the time node, the bus will do it
    }

    void Truth42HardwareModel::run(void)
    {
        if (_use_nos_time) {
            SimIHardwareModel::run();
        } else {
            std::this_thread::sleep_for(std::chrono::microseconds((uint64_t)(_initial_stream_time * 1000000.0)));
            NosEngine::Common::SimTime time = 0;
            while(_keep_running)
            {
                std::this_thread::sleep_for(std::chrono::milliseconds(_stream_period_ms));
                send_streaming_data(time);
                time++;
            }
        }
    }

    void Truth42HardwareModel::send_streaming_data(NosEngine::Common::SimTime time)
    {
        const boost::shared_ptr<Truth42DataPoint> data_point =
            boost::dynamic_pointer_cast<Truth42DataPoint>(_truth_42_dp->get_data_point());

        double abs_time = _absolute_start_time + (double(time * _sim_microseconds_per_tick)) / 1000000.0;
        double next_time = _prev_time + _stream_period_ms/1000.0 - (_sim_microseconds_per_tick / 1000000.0) / 2; // within half a tick time period
        if (next_time < abs_time) { // Time to send more data
            std::vector<uint8_t> data = create_data(*data_point);
            sim_logger->debug("send_streaming_data:  Data point:  %s", data_point->to_string().c_str());
            sim_logger->debug("send_streaming_data:  Writing data:  %s\n", uint8_vector_to_hex_string(data).c_str());

            char s[197];
            for (unsigned int i=0; i < data.size(); i++) {
                s[i] = data[i];
            }
            s[196] = 0;
            _socket->send_to(boost::asio::buffer(s), _remote);
            _prev_time = abs_time;
        }
    }

    std::vector<uint8_t> Truth42HardwareModel::create_data(const Truth42DataPoint& data_point)
    {
        std::vector<uint8_t> out_data, append;
        std::vector<double> v;
        out_data.clear();
        append = int16_to_uint8_vector(data_point.get_year());
        out_data.insert(out_data.end(), append.begin(), append.end());
        append = int16_to_uint8_vector(data_point.get_doy());
        out_data.insert(out_data.end(), append.begin(), append.end());
        append = int16_to_uint8_vector(data_point.get_month());
        out_data.insert(out_data.end(), append.begin(), append.end());
        append = int16_to_uint8_vector(data_point.get_day());
        out_data.insert(out_data.end(), append.begin(), append.end());
        append = int16_to_uint8_vector(data_point.get_utc_hh());
        out_data.insert(out_data.end(), append.begin(), append.end());
        append = int16_to_uint8_vector(data_point.get_utc_mm());
        out_data.insert(out_data.end(), append.begin(), append.end());
        append = double_to_uint8_vector(data_point.get_utc_ss());
        out_data.insert(out_data.end(), append.begin(), append.end());

        v = data_point.get_pos();
        append = double_to_uint8_vector(v[0]);
        out_data.insert(out_data.end(), append.begin(), append.end());
        append = double_to_uint8_vector(v[1]);
        out_data.insert(out_data.end(), append.begin(), append.end());
        append = double_to_uint8_vector(v[2]);
        out_data.insert(out_data.end(), append.begin(), append.end());

        v = data_point.get_vel();
        append = double_to_uint8_vector(v[0]);
        out_data.insert(out_data.end(), append.begin(), append.end());
        append = double_to_uint8_vector(v[1]);
        out_data.insert(out_data.end(), append.begin(), append.end());
        append = double_to_uint8_vector(v[2]);
        out_data.insert(out_data.end(), append.begin(), append.end());

        v = data_point.get_svb();
        append = double_to_uint8_vector(v[0]);
        out_data.insert(out_data.end(), append.begin(), append.end());
        append = double_to_uint8_vector(v[1]);
        out_data.insert(out_data.end(), append.begin(), append.end());
        append = double_to_uint8_vector(v[2]);
        out_data.insert(out_data.end(), append.begin(), append.end());

        v = data_point.get_bvb();
        append = double_to_uint8_vector(v[0]);
        out_data.insert(out_data.end(), append.begin(), append.end());
        append = double_to_uint8_vector(v[1]);
        out_data.insert(out_data.end(), append.begin(), append.end());
        append = double_to_uint8_vector(v[2]);
        out_data.insert(out_data.end(), append.begin(), append.end());

        v = data_point.get_Hvb();
        append = double_to_uint8_vector(v[0]);
        out_data.insert(out_data.end(), append.begin(), append.end());
        append = double_to_uint8_vector(v[1]);
        out_data.insert(out_data.end(), append.begin(), append.end());
        append = double_to_uint8_vector(v[2]);
        out_data.insert(out_data.end(), append.begin(), append.end());

        v = data_point.get_wn();
        append = double_to_uint8_vector(v[0]);
        out_data.insert(out_data.end(), append.begin(), append.end());
        append = double_to_uint8_vector(v[1]);
        out_data.insert(out_data.end(), append.begin(), append.end());
        append = double_to_uint8_vector(v[2]);
        out_data.insert(out_data.end(), append.begin(), append.end());

        v = data_point.get_qn();
        append = double_to_uint8_vector(v[0]);
        out_data.insert(out_data.end(), append.begin(), append.end());
        append = double_to_uint8_vector(v[1]);
        out_data.insert(out_data.end(), append.begin(), append.end());
        append = double_to_uint8_vector(v[2]);
        out_data.insert(out_data.end(), append.begin(), append.end());
        append = double_to_uint8_vector(v[3]);
        out_data.insert(out_data.end(), append.begin(), append.end());

        return out_data;
    }
}
