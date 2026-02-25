/* Copyright (C) 2009 - 2021 National Aeronautics and Space Administration. All Foreign Rights are Reserved to the U.S. Government.

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

/* Standard Includes */
#include <thread>
#include <string>
#include <curses.h>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/foreach.hpp>

#include <ItcLogger/Logger.hpp>

#include <Client/Bus.hpp>

#include <sim_hardware_model_factory.hpp>
#include <sim_i_hardware_model.hpp>
#include <sim_config.hpp>
#include <sim_coordinate_transformations.hpp>
#include <time_driver.hpp>

namespace Nos3
{
    REGISTER_HARDWARE_MODEL(TimeDriver,"TimeDriver");

    ItcLogger::Logger *sim_logger;

    /*************************************************************************
     * Constructors / destructors
     *************************************************************************/

    TimeDriver::TimeDriver(const boost::property_tree::ptree& config) : SimIHardwareModel(config),
        _active(config.get("simulator.active", true)),
        _time_counter(0)
    {
        std::string default_time_uri = config.get("common.nos-connection-string", "tcp://192.168.100.11:12001");
        std::string default_time_bus_name = "command";
        struct TimeBusInfo tbi;

        if (_active) 
        {
            sim_logger->debug("TimeDriver::TimeDriver: Creating time sender\n");

            // Use config data if it exists
            if (config.get_child_optional("simulator.hardware-model.connections")) 
            {
                BOOST_FOREACH(const boost::property_tree::ptree::value_type &v, config.get_child("simulator.hardware-model.connections")) 
                {
                    std::ostringstream oss;
                    write_xml(oss, v.second);
                    sim_logger->trace("TimeDriver::TimeDriver - simulator.hardware-model.connections.connection subtree:\n%s", oss.str().c_str());

                    // v.first is the name of the child.
                    // v.second is the child tree.
                    if (v.second.get("type", "").compare("time") == 0) {
                        tbi.time_bus_name = v.second.get("bus-name", default_time_bus_name);
                        tbi.time_uri = v.second.get("nos-connection-string-override", default_time_uri);
                        bool found = false;
                        // Prefer slow search of vector in constructor so that the run method has fast indexing/iteration
                        for (unsigned int i = 0; i < _time_bus_info.size(); i++) {
                            if ((tbi.time_bus_name.compare(_time_bus_info[i].time_bus_name) == 0) &&
                                (tbi.time_uri.compare(_time_bus_info[i].time_uri) == 0)              ) {
                                    found = true;
                                    break;
                            }
                        }
                        if (!found) {
                            tbi.time_bus.reset(new NosEngine::Client::Bus(_hub, tbi.time_uri, tbi.time_bus_name));
                            tbi.time_bus->enable_set_time();
                            _time_bus_info.push_back(std::move(tbi));
                        }
                    }
                }
            }

            sim_logger->debug("TimeDriver::TimeDriver: Time sender created!\n");
        }
    }

    /*************************************************************************
     * Mutating public worker methods
     *************************************************************************/

    void TimeDriver::run(void)
    {
        if (_active) 
        {
            initscr();
            erase();
            keypad(stdscr, TRUE);
            nodelay(stdscr, TRUE);
            noecho();
            std::size_t N = _time_bus_info.size();
            int key = getch();
            bool pause = false;
            int32_t year, month, day, hour, minute;
            double second;
            while (1) 
            {
                std::this_thread::sleep_for(std::chrono::microseconds(_real_microseconds_per_tick));
    		 int64_t ticks_per_second = 1000000/_real_microseconds_per_tick;
                if ((_time_counter % ticks_per_second) == 0) { // only report about every second
                    wmove(stdscr, 0, 0);
                    double abs_time = _absolute_start_time + (double(_time_counter * _sim_microseconds_per_tick)) / 1000000.0;
                    SimCoordinateTransformations::AbsTime2YMDHMS(abs_time, year, month, day, hour, minute, second);
                    double speed_up = ((double)_sim_microseconds_per_tick) / ((double)_real_microseconds_per_tick);
                    printw("TimeDriver::send_tick_to_nos_engine:\n");
                    printw("  tick = %d, absolute time = %f = %4.4d/%2.2d/%2.2dT%2.2d:%2.2d:%05.2f\n", _time_counter, abs_time, year, month, day, hour, minute, second);
                    printw("  real microseconds per tick = %d, ", _real_microseconds_per_tick);
                    printw("attempted speed-up = %5.2f\n\n", speed_up);
                    printw("Press: 'p' to pause/unpause,\n       'f' to decrease delay by 2x,\n       'd' to increase delay by 2x\n");
                    refresh();
                }

                switch(key) {
                case 'p':                
                case 'P':
                    pause = !pause;
                    //printw("case p \n"); //IRT CSS
                    break;
                case 'f':
                case 'F':
                    _real_microseconds_per_tick /= 2;
                    //printw("case f \n");
                    break;
                case 'd':  
                case 'D':                  
                    if ( _real_microseconds_per_tick < 80000 ) 
                    	{
                    	_real_microseconds_per_tick *= 2;
                    	//printw("case d \n");
                    	}
                    break;
                }

                if (!pause) {
                    for (unsigned int i = 0; i < N; i++) {

                        if(!_time_bus_info[i].time_bus->is_connected())
                        {
                            sim_logger->info("time bus disconnected... reconnecting");
                            _time_bus_info[i].time_bus.reset(new NosEngine::Client::Bus(_hub, _time_bus_info[i].time_uri, _time_bus_info[i].time_bus_name));
                            _time_bus_info[i].time_bus->enable_set_time();
                        }
                        _time_bus_info[i].time_bus->set_time(_time_counter);
                    }

                    _time_counter++;
                }

                key = getch();
                
                
            }
            endwin();
        } 
        else 
        {
            sim_logger->info("TimeDriver::run:  Time driver is not active");
        }
    }

}
