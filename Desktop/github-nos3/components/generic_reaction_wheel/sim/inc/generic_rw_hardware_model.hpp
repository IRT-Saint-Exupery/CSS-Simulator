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

#ifndef GENERICRWHARDWAREMODEL_HPP
#define GENERICRWHARDWAREMODEL_HPP

#include<generic_rw_data_point.hpp>

#include <sim_i_hardware_model.hpp>
#include <Client/Bus.hpp>
#include <Uart/Client/Uart.hpp>

#include <atomic>

namespace Nos3
{
    class GenericRWHardwareModel : public SimIHardwareModel
    {
    public:
        GenericRWHardwareModel(const boost::property_tree::ptree& config);
        ~GenericRWHardwareModel(void);
        void run(void);
        void uart_read_callback(const uint8_t *buf, size_t len);
        void command_callback(NosEngine::Common::Message msg);
    private:
        void send_periodic_data(NosEngine::Common::SimTime time);
        void create_rw_data(const GenericRWDataPoint& data_point, std::vector<uint8_t>& out_data);
        std::string handle_command(std::string command);


        std::atomic<bool>                       _keep_running;
        SimIDataProvider*                       _sdp;
        std::unique_ptr<NosEngine::Client::Bus> _time_bus;
        std::unique_ptr<NosEngine::Uart::Uart>  _uart_connection;
        double                                  _prev_data_sent_time = 0;
        double                                  _period = 0.1;
        int                                     _wheel_number;
    };
}

#endif
