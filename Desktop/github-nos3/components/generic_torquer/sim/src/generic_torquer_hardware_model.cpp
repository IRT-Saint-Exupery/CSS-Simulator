#include <generic_torquer_hardware_model.hpp>

namespace Nos3
{
    REGISTER_HARDWARE_MODEL(Generic_torquerHardwareModel,"GENERIC_TORQUER");

    extern ItcLogger::Logger *sim_logger;

    Generic_torquerHardwareModel::Generic_torquerHardwareModel(const boost::property_tree::ptree& config) : SimIHardwareModel(config), _enabled(0)
    {
        /* Get the NOS engine connection string */
        std::string connection_string = config.get("common.nos-connection-string", "tcp://192.168.100.11:12001"); 
        sim_logger->info("Generic_torquerHardwareModel::Generic_torquerHardwareModel:  NOS Engine connection string: %s.", connection_string.c_str());

        /* Get a data provider */
        std::string dp_name = config.get("simulator.hardware-model.data-provider.type", "GENERIC_TORQUER_42_PROVIDER");
        _generic_torquer_dp = SimDataProviderFactory::Instance().Create(dp_name, config);
        sim_logger->info("Generic_torquerHardwareModel::Generic_torquerHardwareModel:  Data provider %s created.", dp_name.c_str());

        /* Get on the command bus*/
        std::string time_bus_name = "command";
        if (config.get_child_optional("hardware-model.connections")) 
        {
            /* Loop through the connections for the hardware model */
            BOOST_FOREACH(const boost::property_tree::ptree::value_type &v, config.get_child("hardware-model.connections"))
            {
                /* v.first is the name of the child */
                /* v.second is the child tree */
                if (v.second.get("type", "").compare("time") == 0) // 
                {
                    time_bus_name = v.second.get("bus-name", "command");
                    /* Found it... don't need to go through any more items*/
                    break; 
                }
            }
        }
        _time_bus.reset(new NosEngine::Client::Bus(_hub, connection_string, time_bus_name));
        sim_logger->info("Generic_torquerHardwareModel::Generic_torquerHardwareModel:  Now on time bus named %s.", time_bus_name.c_str());

        /* Read MTB configuration */
        _num_mtbs = config.get("simulator.hardware-model.params.num-mtbs", 3);
        _max_trq.resize(_num_mtbs);
        std::stringstream max_trq;
        for (int i=0; i < _num_mtbs; i++) {
            /* get MTB max torques */
            max_trq << "simulator.hardware-model.params.max-trq-mtb-" << i;
            _max_trq[i] = config.get(max_trq.str(), 0.15);
            max_trq.str(std::string());
        }

        /* Initialize socket information */
        port_num = 14242;

        /* Create the socket */
        sockfd = socket(AF_INET, SOCK_DGRAM, 0);
        if (sockfd == -1) {
            sim_logger->fatal("Generic_torquerHardwareModel::Generic_torquerHardwareModel:  Error creating MTB socket");
            throw new std::runtime_error("Generic_torquerHardwareModel::Generic_torquerHardwareModel:  Error creating MTB socket");
        }

        /* Bind server sockets to localhost and port number
           Prepare the sockaddr_in structure
        */
        struct sockaddr_in sockaddr;
        sockaddr.sin_family = AF_INET;
        sockaddr.sin_addr.s_addr = INADDR_ANY; /* auto-fill with host ip */
        sockaddr.sin_port = htons(port_num);     

        /* Bind the socket */
        int ret = bind(sockfd, (struct sockaddr *)&sockaddr, sizeof(sockaddr));
        if (ret != 0) {
            sim_logger->fatal("Generic_torquerHardwareModel::Generic_torquerHardwareModel:  Error creating MTB socket");
            throw new std::runtime_error("Generic_torquerHardwareModel::Generic_torquerHardwareModel:  Error creating MTB socket");
        }  

        /* Set socket to non-blocking */
        socket_flags = fcntl(sockfd, F_GETFL, 0);
        fcntl(sockfd, F_SETFL, socket_flags | O_NONBLOCK);

        /* Construction complete */
        sim_logger->info("Generic_torquerHardwareModel::Generic_torquerHardwareModel:  Construction complete.");
    }


    Generic_torquerHardwareModel::~Generic_torquerHardwareModel(void)
    {        
        /* Clean up the data provider */
        delete _generic_torquer_dp;
        _generic_torquer_dp = nullptr;

        /* Close socket */
        close(sockfd);
        sim_logger->debug("Generic_torquerHardwareModel::~Generic_torquerHardwareModel:  Closed MTB socket");

        /* The bus will clean up the time node */
    }
    
     void Generic_torquerHardwareModel::run(void)
    {
        char socket_recv_buf[512];            
        int retVal;
        int trq_num;
        double trq_percent_high_dir;

        while(_keep_running)
        {            
            socket_recv_buf[0] = '\0';
            retVal = -1;

            int c;
            struct sockaddr_in remote_sockaddr;            

            /* Read trq command string from the socket */
            c = sizeof(struct sockaddr_in);
            remote_sockaddr.sin_family = AF_INET;
            remote_sockaddr.sin_port = htons(port_num);
            remote_sockaddr.sin_addr.s_addr = INADDR_ANY;
            retVal = recvfrom(sockfd, socket_recv_buf, sizeof(socket_recv_buf), 0, (struct sockaddr *)&remote_sockaddr, (socklen_t*)&c);

            /* Forward trq command string to 42 */
            if (retVal != -1) 
            {
                if (sscanf(socket_recv_buf, "%d %lf", &trq_num, &trq_percent_high_dir) == 2) 
                {
                    if ((trq_num >= 0) && 
                        (trq_num < _num_mtbs) && 
                        (-100.0 <= trq_percent_high_dir) && 
                        (trq_percent_high_dir <= 100.0)) 
                    {
                        dynamic_cast<Generic_torquer42DataProvider*>(_generic_torquer_dp)->cmd_torque(trq_num, trq_percent_high_dir*_max_trq[trq_num]/100.0);
                    } 
                    else 
                    {
                        sim_logger->debug("Generic_torquerHardwareModel::run:  Expected: 0 <= trq_num < %d, got %d.  Expected -100.0 <= trq_percentage_high_dir <= 100.0, got %f.", _num_mtbs, trq_num, trq_percent_high_dir);
                    }
                } 
                else 
                {
                    sim_logger->error("Generic_torquerHardwareModel::run:  Expected: \"%%d %%lf\", received: %s", socket_recv_buf);
                }
            }     
            std::this_thread::sleep_for(std::chrono::microseconds(_real_microseconds_per_tick));
        }
        
        // Close socket
        close(sockfd);
        sim_logger->debug("Generic_torquerHardwareModel::run:  Closed MTB socket");
    }

    /* Automagically set up by the base class to be called */
    void Generic_torquerHardwareModel::command_callback(NosEngine::Common::Message msg)
    {
        /* Get the data out of the message */
        NosEngine::Common::DataBufferOverlay dbf(const_cast<NosEngine::Utility::Buffer&>(msg.buffer));
        sim_logger->info("Generic_torquerHardwareModel::command_callback:  Received command: %s.", dbf.data);

        /* Do something with the data */
        std::string command = dbf.data;
        std::string response = "Generic_torquerHardwareModel::command_callback:  INVALID COMMAND! (Try HELP)";
        boost::to_upper(command);
        if (command.compare(0,4,"HELP") == 0) 
        {
            response = "Generic_torquerHardwareModel::command_callback: Valid commands are HELP, ENABLE, DISABLE, STATUS=X, or STOP";
        }
        else if (command.compare(0,6,"ENABLE") == 0) 
        {
            _enabled = GENERIC_TORQUER_SIM_SUCCESS;
            response = "Generic_torquerHardwareModel::command_callback:  Enabled";
        }
        else if (command.compare(0,7,"DISABLE") == 0) 
        {
            _enabled = GENERIC_TORQUER_SIM_ERROR;
            response = "Generic_torquerHardwareModel::command_callback:  Disabled";
        }
        else if (command.compare(0,4,"STOP") == 0) 
        {
            _keep_running = false;
            response = "Generic_torquerHardwareModel::command_callback:  Stopping";
        }
        /* TODO: Add anything additional commands here */

        /* Send a reply */
        sim_logger->info("Generic_torquerHardwareModel::command_callback:  Sending reply: %s.", response.c_str());
        _command_node->send_reply_message_async(msg, response.size(), response.c_str());
    }
}
