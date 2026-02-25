#include <ItcLogger/Logger.hpp>
#include <generic_radio_data_point.hpp>

namespace Nos3
{
    extern ItcLogger::Logger *sim_logger;

    Generic_radioDataPoint::Generic_radioDataPoint(double count)
    {
        sim_logger->trace("Generic_radioDataPoint::Generic_radioDataPoint:  Defined Constructor executed");

        /* Do calculations based on provided data */
        _generic_radio_data_is_valid = true;
        _generic_radio_data[0] = count * 0.001;
        _generic_radio_data[1] = count * 0.002;
        _generic_radio_data[2] = count * 0.003;
    }

    Generic_radioDataPoint::Generic_radioDataPoint(int16_t spacecraft, const boost::shared_ptr<Sim42DataPoint> dp)
    {
        sim_logger->trace("Generic_radioDataPoint::Generic_radioDataPoint:  42 Constructor executed");

        /* Initialize data */
        _generic_radio_data_is_valid = false;
        _generic_radio_data[0] = 0.0;
        _generic_radio_data[1] = 0.0;
        _generic_radio_data[2] = 0.0;

        /*
        ** Declare 42 telemetry string prefix
        ** 42 variables defined in `42/Include/42types.h`
        ** 42 data stream defined in `42/Source/IPC/SimWriteToSocket.c`
        */
        std::ostringstream MatchString;
        MatchString << "SC[" << spacecraft << "].svb = "; /* TODO: Change me to match the data from 42 you are interested in */
        size_t MSsize = MatchString.str().size();

        /* Parse 42 telemetry */
        std::vector<std::string> lines = dp->get_lines();
        try 
        {
            for (unsigned int i = 0; i < lines.size(); i++) 
            {
                /* Compare prefix */
                if (lines[i].compare(0, MSsize, MatchString.str()) == 0) 
                {
                    size_t found = lines[i].find_first_of("=");
                    /* Parse line */
                    std::istringstream iss(lines[i].substr(found+1, lines[i].size()-found-1));
                    /* Custom work to extract the data from the 42 string and save it off in the member data of this data point */
                    std::string s;
                    iss >> s;
                    _generic_radio_data[0] = std::stod(s);
                    iss >> s;
                    _generic_radio_data[1] = std::stod(s);
                    iss >> s;
                    _generic_radio_data[2] = std::stod(s);
                    /* Mark data as valid */
                    _generic_radio_data_is_valid = true;
                    /* Debug print */
                    sim_logger->trace("Generic_radioDataPoint::Generic_radioDataPoint:  Parsed svb = %f %f %f", _generic_radio_data[0], _generic_radio_data[1], _generic_radio_data[2]);
                }
            }
        } 
        catch(const std::exception& e) 
        {
            /* Report error */
            sim_logger->error("Generic_radioDataPoint::Generic_radioDataPoint:  Parsing exception %s", e.what());
        }
    }

    /* Used for printing a representation of the data point */
    std::string Generic_radioDataPoint::to_string(void) const
    {
        sim_logger->trace("Generic_radioDataPoint::to_string:  Executed");
        
        std::stringstream ss;

        ss << std::fixed << std::setfill(' ');
        ss << "Generic_radio Data Point:   Valid: ";
        ss << (_generic_radio_data_is_valid ? "Valid" : "INVALID");
        ss << std::setprecision(std::numeric_limits<double>::digits10); /* Full double precision */
        ss << " Generic_radio Data: "
           << _generic_radio_data[0]
           << " "
           << _generic_radio_data[1]
           << " "
           << _generic_radio_data[2];

        return ss.str();
    }
} /* namespace Nos3 */
