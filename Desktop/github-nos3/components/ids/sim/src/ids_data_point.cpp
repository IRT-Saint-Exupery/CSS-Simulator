#include <ItcLogger/Logger.hpp>
#include <ids_data_point.hpp>

namespace Nos3
{
    extern ItcLogger::Logger *sim_logger;

    IdsDataPoint::IdsDataPoint(double count)
    {
        sim_logger->trace("IdsDataPoint::IdsDataPoint:  Defined Constructor executed");

        /* Do calculations based on provided data */
        _ids_data_is_valid = true;
        _ids_data[0] = count * 0.001;
        _ids_data[1] = count * 0.002;
        _ids_data[2] = count * 0.003;
    }

    IdsDataPoint::IdsDataPoint(int16_t spacecraft, const boost::shared_ptr<Sim42DataPoint> dp) : _dp(*dp), _sc(spacecraft)
    {
        sim_logger->trace("IdsDataPoint::IdsDataPoint:  42 Constructor executed");

        /* Initialize data */
        _ids_data_is_valid = false;
        _ids_data[0] = _ids_data[1] = _ids_data[2] = 0.0;
    }
    
    void IdsDataPoint::do_parsing(void) const
    {
        try {
            /*
            ** Declare 42 telemetry string prefix
            ** 42 variables defined in `42/Include/42types.h`
            ** 42 data stream defined in `42/Source/IPC/SimWriteToSocket.c`
            */
            std::string key;
            key.append("SC[").append(std::to_string(_sc)).append("].svb"); // SC[N].svb

            /* Parse 42 telemetry */
            std::string values = _dp.get_value_for_key(key);

            std::vector<double> data;
            parse_double_vector(values, data);

            _ids_data[0] = data[0];
            _ids_data[1] = data[1];
            _ids_data[2] = data[2];

            /* Mark data as valid */
            _ids_data_is_valid = true;

            _not_parsed = false;

            /* Debug print */
            sim_logger->trace("IdsDataPoint::IdsDataPoint:  Parsed svb = %f %f %f", _ids_data[0], _ids_data[1], _ids_data[2]);
        } catch (const std::exception &e) {
            sim_logger->error("IdsDataPoint::IdsDataPoint:  Error parsing svb.  Error=%s", e.what());
        }
    }

    /* Used for printing a representation of the data point */
    std::string IdsDataPoint::to_string(void) const
    {
        sim_logger->trace("IdsDataPoint::to_string:  Executed");
        
        std::stringstream ss;

        ss << std::fixed << std::setfill(' ');
        ss << "Ids Data Point:   Valid: ";
        ss << (_ids_data_is_valid ? "Valid" : "INVALID");
        ss << std::setprecision(std::numeric_limits<double>::digits10); /* Full double precision */
        ss << " Ids Data: "
           << _ids_data[0]
           << " "
           << _ids_data[1]
           << " "
           << _ids_data[2];

        return ss.str();
    }
} /* namespace Nos3 */
