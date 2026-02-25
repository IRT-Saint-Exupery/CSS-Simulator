#ifndef NOS3_GENERIC_RADIOHARDWAREMODEL_HPP
#define NOS3_GENERIC_RADIOHARDWAREMODEL_HPP

/*
** Includes
*/
#include <map>

#include <arpa/inet.h>	
#include <boost/tuple/tuple.hpp>
#include <boost/property_tree/ptree.hpp>
#include <netinet/in.h>
#include <thread>
#include <string.h>
#include <sys/socket.h>

#include <Client/Bus.hpp>


#include <sim_i_data_provider.hpp>
#include <generic_radio_data_point.hpp>
#include <sim_i_hardware_model.hpp>


/*
** Namespace
*/
namespace Nos3
{
    /* Standard for a hardware model */
    class Generic_radioHardwareModel : public SimIHardwareModel
    {
    public:
        /* Constructor and destructor */
        Generic_radioHardwareModel(const boost::property_tree::ptree& config);
        ~Generic_radioHardwareModel(void);
        void run(void);

    private:
        void create_generic_radio_hk(std::uint8_t out_data[16]); 
        void process_radio_command(const uint8_t *buf, size_t len); /* Handle data the hardware receives */
        void command_callback(NosEngine::Common::Message msg); /* Handle backdoor commands and time tick to the simulator */

        const int GENERIC_RADIO_SIM_SUCCESS = 0;
        const int GENERIC_RADIO_SIM_ERROR = 0;

        typedef struct 
        {
            int sockfd;
            std::string ip;
            int port;
        } udp_info_t;

        int32_t udp_init(udp_info_t* sock);
        void forward_loop(udp_info_t* rcv_sock, udp_info_t* fwd_sock);

        udp_info_t                                          _fsw_ci;
        udp_info_t                                          _fsw_to;
        udp_info_t                                          _fsw_radio;
        udp_info_t                                          _radio_cmd;
        udp_info_t                                          _gsw_cmd;
        udp_info_t                                          _gsw_tlm;
        udp_info_t                                          _prox_rcv;
        udp_info_t                                          _prox_fsw;
        udp_info_t                                          _prox_fwd;
        udp_info_t                                          _prox_dest;

        std::unique_ptr<NosEngine::Client::Bus>             _time_bus; /* Standard */
        SimIDataProvider*                                   _generic_radio_dp; /* Only needed if the sim has a data provider */

        /* Internal state data */
        std::uint8_t                                        _enabled;
        std::uint32_t                                       _count;
        std::uint32_t                                       _config;
        std::uint32_t                                       _prox_signal;
    };
}

#endif
