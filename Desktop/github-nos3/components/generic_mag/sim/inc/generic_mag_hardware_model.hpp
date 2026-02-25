#ifndef NOS3_GENERIC_MAGHARDWAREMODEL_HPP
#define NOS3_GENERIC_MAGHARDWAREMODEL_HPP

/*
** Includes
*/
#include <map>

#include <boost/tuple/tuple.hpp>
#include <boost/property_tree/ptree.hpp>

#include <Client/Bus.hpp>
#include <Spi/Client/SpiSlave.hpp>

#include <sim_i_data_provider.hpp>
#include <generic_mag_data_point.hpp>
#include <sim_i_hardware_model.hpp>


/*
** Defines
*/
#define GENERIC_MAG_SIM_SUCCESS 0
#define GENERIC_MAG_SIM_ERROR   1


/*
** Namespace
*/
namespace Nos3
{
    /* Standard for a hardware model */
    class Generic_magHardwareModel : public SimIHardwareModel
    {
    public:
        /* Constructor and destructor */
        Generic_magHardwareModel(const boost::property_tree::ptree& config);
        ~Generic_magHardwareModel(void);
        void prepare_generic_mag_data_from_42(std::vector<uint8_t>& out_data); 

    private:
        /* Private helper methods */
        void command_callback(NosEngine::Common::Message msg); /* Handle backdoor commands and time tick to the simulator */

        /* Private data members */
        class SpiSlaveConnection*                           _spi_slave_connection;
        std::unique_ptr<NosEngine::Client::Bus>             _time_bus; /* Standard */

        SimIDataProvider*                                   _generic_mag_dp; /* Only needed if the sim has a data provider */

        /* Internal state data */
        std::uint8_t                                        _enabled;
        const float                                         _nano_conversion = 1000000000;
        const float                                         _mag_conv = 21474;
        const float                                         _mag_range = 100000;
    };

    class SpiSlaveConnection : public NosEngine::Spi::SpiSlave
    {
    public:
        SpiSlaveConnection(Generic_magHardwareModel* mag, int chip_select, std::string connection_string, std::string bus_name);
        size_t spi_read(uint8_t *rbuf, size_t rlen);
        size_t spi_write(const uint8_t *wbuf, size_t wlen);
    private:
        Generic_magHardwareModel*  _mag;
        std::vector<uint8_t>       _spi_out_data;
    };
}

#endif
