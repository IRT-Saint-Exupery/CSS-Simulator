/* Copyright (C) 2015 - 2016 National Aeronautics and Space Administration. All Foreign Rights are Reserved to the U.S. Government.

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

#ifndef NOS3_SIMDATA42SOCKETPROVIDER_HPP
#define NOS3_SIMDATA42SOCKETPROVIDER_HPP

#include <thread>
#include <mutex>

#include <boost/shared_ptr.hpp>

#include <sim_i_data_provider.hpp>
#include <sim_42data_point.hpp>

namespace Nos3
{
    /** \brief Class for a provider of simulation data that provides data from a 42 socket connection.
     *
     *  This class can concretely retrieve data from a 42 socket... but is still virtual because it is up
     *  to a derived class to determine what 42 data it should be a provider of... the get_data_point()
     *  method is still pure virtual.  Now it does need its derived class to perform
     *  connect_reader_thread_as_42_socket_client(), otherwise the 42 data point will never be
     *  set with any valid data... this allows the derived class to specify the endpoint
     *  information, but places all the shared code for reading 42 data in this class.
     */
    class SimData42SocketProvider : public SimIDataProvider
    {
    public:
        /// @name Constructors / destructors
        //@{
        /// \brief Constructor taking a configuration object.
        /// @param  sc  The configuration for the simulation
        SimData42SocketProvider(const boost::property_tree::ptree& config);
        ~SimData42SocketProvider(void);
        //@}

        /// @name Non-mutating public worker methods
        //@{
        /** \brief Method to retrieve simulation data.
         *
         * @returns                     A data point of simulation data.
         */
        virtual boost::shared_ptr<SimIDataPoint> get_data_point(void) const
        {
            boost::shared_ptr<Sim42DataPoint> dp;
            {
                std::lock_guard<std::mutex> lock(_data_point_mutex);
                dp = boost::shared_ptr<Sim42DataPoint>(new Sim42DataPoint(_data_point));
                // Lock is released when scope ends
            }
            return dp;
        }

        /** \brief Method to send a simulation command to 42.
         *
         * @param       message    Text command message to send.
         */
        void send_command_to_socket(const std::string& message);
        //@}

    protected:
        /// @name Mutating protected worker methods
        //@{
        /** \brief Method to connect to a 42 socket and start reading data
         *
         * @param       server_host        The host name or IP address of the 42 server.
         * @param       server_port        The port number of the 42 server.
         */
        void connect_reader_thread_as_42_socket_client(std::string server_host, uint16_t server_port);

    private:
        // Private helper methods
        void connect_command_socket_as_42_socket_client(void);
        bool connect_as_42_socket_client(std::string a_42_host, uint16_t a_42_port, int &socket_fd);
        void telemetry_socket_reader(void);
        void read_telemetry_socket_data(std::vector<std::string>& message);
        static char rgetc(int fd);
        static char *rgets(char *s, int n, int fd);

        // Private data
        // ... connection data
        std::string _server_host;
        std::string _server_telemetry_port;
        uint16_t _server_command_port;
        int _max_connection_attempts;
        int _retry_wait_seconds;
        int _telemetry_socket_fd;
        int _command_socket_fd;
        double _absolute_start_time;

        // ... telemetry reader thread / thread state data
        std::thread *_telemetry_socket_client_thread;
        bool _not_terminating; // Used to signal the thread when we are terminating so the telemetry reader thread quits reading the socket

        // ... command state data
        bool _command_port_connected;

        // ... a data point of data read from the socket
        Sim42DataPoint _data_point;
        mutable std::mutex _data_point_mutex;  // protects _data_point

    };
}

#endif
