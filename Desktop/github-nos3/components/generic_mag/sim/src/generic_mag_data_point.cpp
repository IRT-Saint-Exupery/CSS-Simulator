#include <ItcLogger/Logger.hpp>
#include <generic_mag_data_point.hpp>

namespace Nos3
{
    extern ItcLogger::Logger *sim_logger;

    Generic_magDataPoint::Generic_magDataPoint(int16_t spacecraft, const boost::shared_ptr<Sim42DataPoint> dp) : _dp(*dp), _sc(spacecraft), _not_parsed(true)
    {
        sim_logger->trace("Generic_magDataPoint::Generic_magDataPoint:  42 Constructor executed");

        /* Initialize data */
        std::vector<float> axes(3, 0.0);
        _generic_mag_data = axes;
        _generic_mag_data[0] = _generic_mag_data[1] = _generic_mag_data[2] = 0.0;
    }

    void Generic_magDataPoint::do_parsing(void) const
    {
        try {
            /*
            ** Declare 42 telemetry string prefix
            ** 42 variables defined in `42/Include/42types.h`
            ** 42 data stream defined in `42/Source/IPC/SimWriteToSocket.c`
            */
           std::string key0; // SC[N].AC.MAG[M].Field
           key0.append("SC[").append(std::to_string(_sc)).append("].AC.MAG");
           std::string key1(key0), key2(key0);
           key0.append("[0].Field");
           key1.append("[1].Field");
           key2.append("[2].Field");
           
           /* Parse 42 telemetry */
           _generic_mag_data[0] = std::stof(_dp.get_value_for_key(key0));
           _generic_mag_data[1] = std::stof(_dp.get_value_for_key(key1));
           _generic_mag_data[2] = std::stof(_dp.get_value_for_key(key2));

           _not_parsed = false;
        } 
        catch(const std::exception& e) 
        {
            /* Force data to be set to known values */
            std::vector<float> axes(3, 0.0);
            _generic_mag_data = axes; 
            sim_logger->error("Generic_magDataPoint::Generic_magDataPoint:  Parsing exception %s", e.what());
        }
    }

    /* Used for printing a representation of the data point */
    std::string Generic_magDataPoint::to_string(void) const
    {
        sim_logger->trace("Generic_magDataPoint::to_string:  Executed");
        std::stringstream output;
        output << "Magnetometer values: ";
        for (unsigned int i = 0; i < _generic_mag_data.size(); i++) {
            output << _generic_mag_data[i];
            if (i < _generic_mag_data.size() - 1) {
                output << ", ";
            }
        }
        return output.str();
    }
} /* namespace Nos3 */
