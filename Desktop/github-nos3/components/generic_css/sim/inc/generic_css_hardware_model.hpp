#ifndef NOS3_GENERIC_CSSHARDWAREMODEL_HPP
#define NOS3_GENERIC_CSSHARDWAREMODEL_HPP

/*
** Includes
*/
#include <map>

#include <boost/tuple/tuple.hpp>
#include <boost/property_tree/ptree.hpp>

#include <Client/Bus.hpp>
#include <I2C/Client/I2CSlave.hpp>

#include <sim_i_data_provider.hpp>
#include <generic_css_data_point.hpp>
#include <sim_i_hardware_model.hpp>


/*
** Defines
*/
#define GENERIC_CSS_SIM_SUCCESS 0
#define GENERIC_CSS_SIM_ERROR   1


/*
** Namespace
*/
namespace Nos3
{
    /* Standard for a hardware model */
    class Generic_cssHardwareModel : public SimIHardwareModel
    {
    public:
        /* Constructor and destructor */
        Generic_cssHardwareModel(const boost::property_tree::ptree& config);
        ~Generic_cssHardwareModel(void);
        void create_generic_css_data(std::vector<uint8_t>& out_data); 

    private:
        /* Private helper methods */
        void command_callback(NosEngine::Common::Message msg); /* Handle backdoor commands and time tick to the simulator */
        /* Private data members */
        class I2CSlaveConnection*                           _i2c_slave_connection;
        std::unique_ptr<NosEngine::Client::Bus>             _time_bus; /* Standard */

        SimIDataProvider*                                   _generic_css_dp; /* Only needed if the sim has a data provider */

        /* Internal state data */
        std::uint8_t                                        _enabled;
    };

    class I2CSlaveConnection : public NosEngine::I2C::I2CSlave
    {
    public:
        I2CSlaveConnection(Generic_cssHardwareModel* hm, int bus_address, std::string connection_string, std::string bus_name);
        size_t i2c_read(uint8_t *rbuf, size_t rlen);
        size_t i2c_write(const uint8_t *wbuf, size_t wlen);
    private:
        std::vector<uint8_t>  _i2c_out_data;
        Generic_cssHardwareModel* _hardware_model;
    };
}

#endif
