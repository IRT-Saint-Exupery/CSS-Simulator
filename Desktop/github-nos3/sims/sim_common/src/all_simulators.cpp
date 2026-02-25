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

#include <iostream>
#include <vector>
#include <thread>
#include <ItcLogger/Logger.hpp>
#include <sim_config.hpp>

namespace Nos3
{
    ItcLogger::Logger *sim_logger;
}

int
main(int argc, char *argv[])
{
    // Determine the configuration and run all simulators
    Nos3::SimConfig sc(argc, argv);
    std::vector<std::thread *> threads;
    std::vector<std::string> names = sc.get_simulator_names();
    if (names.size() > 0) {
        for(std::vector<std::string>::size_type i = 1; i < names.size(); i++) {
            Nos3::sim_logger->info("main:  Spawning thread for simulator \"%s\"", names[i].c_str());
            threads.push_back(new std::thread(std::bind(&Nos3::SimConfig::run_simulator, sc, names[i]), NULL)); // Spawn thread to run simulator
        }

        sc.run_simulator(names[0]); // run the first simulator name in the main thread
    }
}
