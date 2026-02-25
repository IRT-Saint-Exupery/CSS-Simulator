#ifndef NOS3_GENERIC_FSSHARDWAREMODEL_HPP
#define NOS3_GENERIC_FSSHARDWAREMODEL_HPP

/*
** Includes
*/
#include <vector>

#include <boost/property_tree/ptree.hpp>

#include <Client/Bus.hpp>
#include <Spi/Client/SpiSlave.hpp>

#include <sim_i_data_provider.hpp>
#include <sim_i_hardware_model.hpp>


/*
** Defines
*/
#define GENERIC_FSS_SIM_SUCCESS 0
#define GENERIC_FSS_SIM_ERROR   1


/*
** Namespace
*/
namespace Nos3
{
    class SpiSlaveConnection;

    /* Standard for a hardware model */
    class Generic_fssHardwareModel : public SimIHardwareModel
    {
    public:
        /* Constructor and destructor */
        Generic_fssHardwareModel(const boost::property_tree::ptree& config);
        ~Generic_fssHardwareModel(void);
        std::vector<uint8_t> determine_spi_response_for_request(const std::vector<uint8_t>& in_data); /* Handle data the hardware receives from its protocol bus */

    private:
        /* Private helper methods */
        void create_generic_fss_data(); 
        void command_callback(NosEngine::Common::Message msg); /* Handle backdoor commands and time tick to the simulator */
        void double_to_4bytes_little_endian(double in, uint8_t out[4]);
        uint8_t compute_checksum(std::vector<uint8_t>& in, int starting_byte, int number_of_bytes);
        /* Private data members */
        std::unique_ptr<NosEngine::Client::Bus>             _time_bus; /* Standard */
        SpiSlaveConnection*                                 _spi;

        SimIDataProvider*                                   _generic_fss_dp; /* Only needed if the sim has a data provider */

        /* Internal state data */
        std::uint8_t                                        _enabled;
        std::vector<uint8_t>                                _queued_data;
    };

    class SpiSlaveConnection : public NosEngine::Spi::SpiSlave
    {
    public:
        SpiSlaveConnection(Generic_fssHardwareModel* fss, int chip_select, std::string connection_string, std::string bus_name);
        size_t spi_read(uint8_t *rbuf, size_t rlen);
        size_t spi_write(const uint8_t *wbuf, size_t wlen);
    private:
        Generic_fssHardwareModel*  _fss;
        std::vector<uint8_t>       _spi_out_data;
    };
}

#endif
