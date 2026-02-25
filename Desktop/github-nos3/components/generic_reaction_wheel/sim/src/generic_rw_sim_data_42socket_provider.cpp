/* Copyright (C) 2016 - 2016 National Aeronautics and Space Administration. All Foreign Rights are Reserved to the U.S. Government.

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

#include <generic_rw_sim_data_42socket_provider.hpp>
#include <generic_rw_data_point.hpp>

#include <ItcLogger/Logger.hpp>

#include <boost/property_tree/xml_parser.hpp>

namespace Nos3
{
    REGISTER_DATA_PROVIDER(GenericRWData42SocketProvider,"GENERICRWSIMDATA42SOCKETPROVIDER");

    extern ItcLogger::Logger *sim_logger;

    GenericRWData42SocketProvider::GenericRWData42SocketProvider(const boost::property_tree::ptree& config) : SimData42SocketProvider(config)
    {
        sim_logger->trace("GenericRWData42SocketProvider::GenericRWData42SocketProvider:  Constructor executed");

        std::ostringstream oss;
        write_xml(oss, config);
        sim_logger->info("GenericRWData42SocketProvider::GenericRWData42SocketProvider:  "
            "configuration:\n%s", oss.str().c_str());

        connect_reader_thread_as_42_socket_client(config.get("simulator.hardware-model.data-provider.hostname", "192.168.100.42"),
            config.get("simulator.hardware-model.data-provider.port", 4242));
        _sc = config.get("simulator.hardware-model.data-provider.spacecraft", 0);
        _reactionwheel = config.get("simulator.hardware-model.data-provider.reactionwheel", 0);

        sim_logger->trace("GenericRWData42SocketProvider::GenericRWData42SocketProvider:  Constructor exiting");
    }

    GenericRWData42SocketProvider::~GenericRWData42SocketProvider(void)
    {
        sim_logger->trace("GenericRWData42SocketProvider::~GenericRWData42SocketProvider:  Destructor executed");
    }

    boost::shared_ptr<SimIDataPoint> GenericRWData42SocketProvider::get_data_point(void) const
    {
        const boost::shared_ptr<Sim42DataPoint> dp42 =
            boost::dynamic_pointer_cast<Sim42DataPoint>(SimData42SocketProvider::get_data_point());

        SimIDataPoint *dp = new GenericRWDataPoint(_sc, _reactionwheel, dp42);

        if (sim_logger->is_level_enabled(ItcLogger::LOGGER_TRACE)) {
            sim_logger->trace("GenericRWDataPoint::get_data_point: %s", dp->to_string().c_str()); // log data in a man readable format
        }
        return boost::shared_ptr<SimIDataPoint>(dp);
    }
}

