#include <ItcLogger/Logger.hpp>
#include <isl_data_point.hpp>

namespace Nos3
{
    extern ItcLogger::Logger *sim_logger;

    IslDataPoint::IslDataPoint(double count)
    {
        sim_logger->trace("IslDataPoint::IslDataPoint:  Defined Constructor executed");

        /* Do calculations based on provided data */
        _isl_data_is_valid = true;
        _isl_data[0] = count * 0.001;
        _isl_data[1] = count * 0.002;
        _isl_data[2] = count * 0.003;
    }

    IslDataPoint::IslDataPoint(int16_t spacecraft, const boost::shared_ptr<Sim42DataPoint> dp) : _dp(*dp), _sc(spacecraft)
    {
        sim_logger->trace("IslDataPoint::IslDataPoint:  42 Constructor executed");

        /* Initialize data */
        _isl_data_is_valid = false;
        _isl_data[0] = _isl_data[1] = _isl_data[2] = 0.0;
    }
    
    void IslDataPoint::do_parsing(void) const
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

            _isl_data[0] = data[0];
            _isl_data[1] = data[1];
            _isl_data[2] = data[2];

            /* Mark data as valid */
            _isl_data_is_valid = true;

            _not_parsed = false;

            /* Debug print */
            sim_logger->trace("IslDataPoint::IslDataPoint:  Parsed svb = %f %f %f", _isl_data[0], _isl_data[1], _isl_data[2]);
        } catch (const std::exception &e) {
            sim_logger->error("IslDataPoint::IslDataPoint:  Error parsing svb.  Error=%s", e.what());
        }
    }

    /* Used for printing a representation of the data point */
    std::string IslDataPoint::to_string(void) const
    {
        sim_logger->trace("IslDataPoint::to_string:  Executed");
        
        std::stringstream ss;

        ss << std::fixed << std::setfill(' ');
        ss << "Isl Data Point:   Valid: ";
        ss << (_isl_data_is_valid ? "Valid" : "INVALID");
        ss << std::setprecision(std::numeric_limits<double>::digits10); /* Full double precision */
        ss << " Isl Data: "
           << _isl_data[0]
           << " "
           << _isl_data[1]
           << " "
           << _isl_data[2];

        return ss.str();
    }
} /* namespace Nos3 */
