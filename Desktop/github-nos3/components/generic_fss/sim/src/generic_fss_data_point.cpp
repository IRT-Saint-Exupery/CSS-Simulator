#include <ItcLogger/Logger.hpp>
#include <generic_fss_data_point.hpp>

namespace Nos3
{
    extern ItcLogger::Logger *sim_logger;

    Generic_fssDataPoint::Generic_fssDataPoint(int16_t spacecraft, const boost::shared_ptr<Sim42DataPoint> dp) : _dp(*dp), _sc(spacecraft), _not_parsed(true)
    {
        sim_logger->trace("Generic_fssDataPoint::Generic_fssDataPoint:  42 Constructor executed");

        /* Initialize data */
        _generic_fss_valid = false;
        _generic_fss_alpha = _generic_fss_beta = 0.0;
    }

    void Generic_fssDataPoint::do_parsing(void) const
    {
        try {
            /*
            ** Declare 42 telemetry string prefix
            ** 42 variables defined in `42/Include/42types.h`
            ** 42 data stream defined in `42/Source/IPC/SimWriteToSocket.c`
            */
            std::string valid_key;
            valid_key.append("SC[").append(std::to_string(_sc)).append("].AC.FSS[0].Valid"); // SC[N].AC.FSS[0].Valid
            std::string sunang_key;
            sunang_key.append("SC[").append(std::to_string(_sc)).append("].AC.FSS[0].SunAng"); // SC[N].AC.FSS[0].SunAng

            /* Parse 42 telemetry */
            std::string valid_value = _dp.get_value_for_key(valid_key);
            std::string sunang_values = _dp.get_value_for_key(sunang_key);

            _generic_fss_valid = (valid_value == "1");
            std::vector<double> data;
            parse_double_vector(sunang_values, data);
            _generic_fss_alpha = data[0];
            _generic_fss_beta = data[1];

            /* Debug print */
            sim_logger->trace("Generic_fssDataPoint::Generic_fssDataPoint:  Parsed valid = %s, sunang = %f %f", _generic_fss_valid?"True":"False", _generic_fss_alpha, _generic_fss_beta);

            _not_parsed = false;
        } 
        catch(const std::exception& e) 
        {
            /* Report error */
            sim_logger->error("Generic_fssDataPoint::Generic_fssDataPoint:  Parsing exception %s", e.what());
        }
    }

    /* Used for printing a representation of the data point */
    std::string Generic_fssDataPoint::to_string(void) const
    {
        sim_logger->trace("Generic_fssDataPoint::to_string:  Executed");
        
        std::stringstream ss;

        ss << std::fixed << std::setfill(' ');
        ss << "Generic_fss Data Point:   Valid: ";
        ss << std::setprecision(std::numeric_limits<double>::digits10); /* Full double precision */
        ss << " Generic_fss valid: "
           << (_generic_fss_valid?"True":"False")
           << "alpha: "
           << _generic_fss_alpha
           << "beta: "
           << _generic_fss_beta;

        return ss.str();
    }
} /* namespace Nos3 */
