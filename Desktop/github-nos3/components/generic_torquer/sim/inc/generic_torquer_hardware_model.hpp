#ifndef NOS3_GENERIC_TORQUERHARDWAREMODEL_HPP
#define NOS3_GENERIC_TORQUERHARDWAREMODEL_HPP

/*
** Includes
*/
#include <map>

#include <boost/tuple/tuple.hpp>
#include <boost/property_tree/ptree.hpp>

#include <Client/Bus.hpp>

#include <sim_i_data_provider.hpp>
#include <sim_i_hardware_model.hpp>
#include <generic_torquer_42_data_provider.hpp>

#include <sys/socket.h>
#include <arpa/inet.h>	
#include <fcntl.h>


/*
** Defines
*/
#define GENERIC_TORQUER_SIM_SUCCESS 0
#define GENERIC_TORQUER_SIM_ERROR   1


/*
** Namespace
*/
namespace Nos3
{
    /* Standard for a hardware model */
    class Generic_torquerHardwareModel : public SimIHardwareModel
    {
    public:
        /* Constructor and destructor */
        Generic_torquerHardwareModel(const boost::property_tree::ptree& config);
        ~Generic_torquerHardwareModel(void);

    private:
        /* Private helper methods */
        void run(void);
        void command_callback(NosEngine::Common::Message msg); /* Handle backdoor commands and time tick to the simulator */

        /* Private data members */
        std::unique_ptr<NosEngine::Client::Bus>             _time_bus; /* Standard */

        SimIDataProvider*                                   _generic_torquer_dp; /* Only needed if the sim has a data provider */

        /* Internal state data */
        std::uint8_t                                        _enabled;
        int _num_mtbs;
        std::vector<double> _max_trq;
        int sockfd;
        int port_num;
        char* ip_address;
        sa_family_t address_family;
        int socket_flags;
    };
}

#endif
