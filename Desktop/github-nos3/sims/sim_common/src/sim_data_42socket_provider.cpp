/* Copyright (C) 2015 - 2017 National Aeronautics and Space Administration. All Foreign Rights are Reserved to the U.S. Government.

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

#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
//#include <fcntl.h>

#include <ItcLogger/Logger.hpp>

#include <sim_data_42socket_provider.hpp>

namespace Nos3
{
    //REGISTER_DATA_PROVIDER(SimData42SocketProvider,"42SOCKET");

    extern ItcLogger::Logger *sim_logger;

    /*************************************************************************
     * Constructors / Destructors
     *************************************************************************/

    SimData42SocketProvider::SimData42SocketProvider(const boost::property_tree::ptree& config)
        : SimIDataProvider(config),
          _server_host(config.get("simulator.hardware-model.data-provider.hostname", "192.168.100.11")),
          _server_command_port(config.get("simulator.hardware-model.data-provider.command-port", 0)), // default is no command port needed (0)... e.g. for sensor only hardware like IMUs, Star Trackers, etc.
          _max_connection_attempts(config.get("simulator.hardware-model.data-provider.max-connection-attempts", 5)),
          _retry_wait_seconds(config.get("simulator.hardware-model.data-provider.retry-wait-seconds", 5)),
          _absolute_start_time(config.get("common.absolute-start-time", 552110400.0)), _telemetry_socket_client_thread(NULL), _not_terminating(true), _command_port_connected(false)
    {
        SimData42SocketProvider::connect_command_socket_as_42_socket_client();
    }

    SimData42SocketProvider::~SimData42SocketProvider(void)
    {
        _not_terminating = false;
        if (_telemetry_socket_client_thread != NULL) {
            _telemetry_socket_client_thread->join();
            delete _telemetry_socket_client_thread;
        }
        close(_telemetry_socket_fd); // close the socket
        close(_command_socket_fd); // close the socket
    }

    /*************************************************************************
     * Non-mutating public worker methods
     *************************************************************************/

     void SimData42SocketProvider::send_command_to_socket(const std::string& message)
     {
        if (_command_port_connected) {
            ssize_t bytes_sent = send(_command_socket_fd, message.c_str(), message.length(), 0);
            if (bytes_sent == (ssize_t)message.length()) {
                sim_logger->debug("SimData42SocketProvider::send_command_to_socket:  Successfully sent command to host %s, port %u.  Command %s", _server_host.c_str(), _server_command_port, message.c_str());
            } else {
                sim_logger->error("SimData42SocketProvider::send_command_to_socket:  Unsuccessful sending command to host %s, port %u.  Bytes to send %u, bytes sent/return value %u.  Command %s",
                    _server_host.c_str(), _server_command_port, message.length(), bytes_sent, message.c_str());
            }
        } else {
            sim_logger->error("SimData42SocketProvider::send_command_to_socket:  Not connected.  Not sending command to host %s, port %u.  Command %s", _server_host.c_str(), _server_command_port, message.c_str());
        }
     }

    /*************************************************************************
     * Protected mutating worker methods
     *************************************************************************/

    void SimData42SocketProvider::connect_reader_thread_as_42_socket_client(std::string server_host, uint16_t server_telemetry_port)
    {
        bool connected = connect_as_42_socket_client(server_host, server_telemetry_port, _telemetry_socket_fd);
        if (connected) {
            _telemetry_socket_client_thread = new std::thread(std::bind(&SimData42SocketProvider::telemetry_socket_reader, this), NULL); // Spawn thread to listen to/read from socket
            sim_logger->debug("SimData42SocketProvider::connect_reader_thread_as_42_socket_client:  Successfully connected TELEMETRY host %s, port %u to 42 and started listener thread!",
                server_host.c_str(), server_telemetry_port);
        } else {
            sim_logger->error("SimData42SocketProvider::connect_reader_thread_as_42_socket_client:  Unable to connect TELEMETRY host %s, port %u to 42, no thread will be started to listen :-(",
                server_host.c_str(), server_telemetry_port);
        }
        return;
    }

    /*************************************************************************
     * Private helper methods
     *************************************************************************/

    void SimData42SocketProvider::connect_command_socket_as_42_socket_client(void)
    {
        if (_server_command_port > 0) {
            _command_port_connected = connect_as_42_socket_client(_server_host, _server_command_port, _command_socket_fd);
            if (_command_port_connected) sim_logger->debug("SimData42SocketProvider::SimData42SocketProvider:  Successfully connected COMMAND host %s, port %u to 42!", _server_host.c_str(), _server_command_port);
            else sim_logger->error("SimData42SocketProvider::SimData42SocketProvider:  Unable to connect COMMAND host %s, port %u to 42 :-(", _server_host.c_str(), _server_command_port);
        } else sim_logger->debug("SimData42SocketProvider::SimData42SocketProvider:  No COMMAND port (%u) to 42 requested, none connected.", _server_command_port);
    }

    bool SimData42SocketProvider::connect_as_42_socket_client(std::string a_42_host, uint16_t a_42_port, int &socket_fd)
    {
        // http://stackoverflow.com/questions/8257714/how-to-convert-an-int-to-string-in-c/8257728#8257728
        int length = (int)((ceil(log10(a_42_port))+1)*sizeof(char));
        char a_42_port_string[length];
        snprintf(a_42_port_string, length, "%ud", a_42_port);

        // http://beej.us/guide/bgnet/output/html/singlepage/bgnet.html
        struct addrinfo hints, *servinfo, *p;
        int rv;

        memset(&hints, 0, sizeof hints);
        hints.ai_family = AF_UNSPEC;
        hints.ai_socktype = SOCK_STREAM;

        for (int i = 0; i < _max_connection_attempts + 1; i++)
        {
            if ((rv = getaddrinfo(a_42_host.c_str(), a_42_port_string, &hints, &servinfo)) != 0)
            {
                sim_logger->error("SimData42SocketProvider::connect_as_42_socket_client:  Error getting address for host %s, port %u: %s", a_42_host.c_str(), a_42_port, gai_strerror(rv));
                return false;
            }

            // loop through all the results and connect to the first we can
            for(p = servinfo; p != NULL; p = p->ai_next)
            {
                if ((socket_fd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1)
                {
                    sim_logger->warning("SimData42SocketProvider::connect_as_42_socket_client:  Continuing, but could not create socket for host %s, port %u: %s", a_42_host.c_str(), a_42_port, strerror(errno));
                    continue;
                }
                if (connect(socket_fd, p->ai_addr, p->ai_addrlen) == -1)
                {
                    close(socket_fd);
                    sim_logger->warning("SimData42SocketProvider::connect_as_42_socket_client:  Continuing, but could not connect socket for host %s, port %u: %s", a_42_host.c_str(), a_42_port, strerror(errno));
                    continue;
                } else break; // got a good connection
            }

            if (p != NULL) // got a good connection
            {
                break;
            } else
            {
                if (i == _max_connection_attempts)
                {
                    sim_logger->error("SimData42SocketProvider::connect_as_42_socket_client:  Maximum number of connection attempts readched.   Host %s, port %u failed to connect!", a_42_host.c_str(), a_42_port);
                    return false;
                }
                else
                {
                    sim_logger->warning("SimData42SocketProvider::connect_as_42_socket_client:  Warning... failed to connect host %s, port %d... retrying in %u seconds.", a_42_host.c_str(), a_42_port,
                                        _retry_wait_seconds);
                    sleep(_retry_wait_seconds);
                }
            }
        }
        return true; // connection succeeded... may have taken a while, but it succeeded
    }

    void SimData42SocketProvider::telemetry_socket_reader(void)
    {
        std::vector<std::string> message;

        while (_not_terminating)
        {
            read_telemetry_socket_data(message);

            {
                std::lock_guard<std::mutex> lock(_data_point_mutex);
                Sim42DataPoint dp(message);
                _data_point = dp;
                // Lock is released when scope ends
            }

            //sim_logger->debug("SimData42SocketProvider::socket_reader:  Data Point=%s\n", _data_point.to_formatted_string().c_str());
        }
    }

    void SimData42SocketProvider::read_telemetry_socket_data(std::vector<std::string>& message)
    {
        long Done = 0;
        char line[512] = "Blank";
        char *LineIsValid;

        message.clear();
        while(!Done)
        {
            LineIsValid = rgets(line, 511, _telemetry_socket_fd);
            sim_logger->trace("SimData42SocketProvider::read_socket_data:  Line=%s", line);
            if (LineIsValid == NULL)
            {
                Done = 1;
            } else {
                message.push_back(line);
                if (!strncmp(line,"[EOF]",5)) Done = 1;
            }
        }
    }

    char SimData42SocketProvider::rgetc(int fd)
    {
        char buf;
        if (read(fd, &buf, 1) != 1)
            return EOF;
        return buf;
    }

    char* SimData42SocketProvider::rgets(char *s, int n, int fd) /* K&R, 2nd, p. 165 */
    {
        register int c;
        register char *cs;

        cs = s;
        while (--n > 0 && (c = rgetc(fd)) != EOF) // ssize_t read(int fd, void *buf, size_t count);
            if ((*cs++ = c) == '\n')
                break;
        *cs = '\0';
        if (cs != s) *(cs-1) = '\0'; // Hack off the \n
        return (c == EOF && cs == s) ? NULL : s;
    }

}
