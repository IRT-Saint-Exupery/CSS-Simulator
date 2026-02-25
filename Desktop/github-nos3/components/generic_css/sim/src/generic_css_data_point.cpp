/* Copyright (C) 2016 - 2021 National Aeronautics and Space Administration. All Foreign Rights are Reserved to the U.S. Government.

   This software is provided "as is" without any warranty of any, kind either express, implied, or statutory, including, but not
   limited to, any warranty that the software will conform to, specifications any implied warranties of merchantability, fitness
   for a particular purpose, and freedom from infringement, and any warranty that the documentation will conform to the program, or
   any warranty that the software will be error free.

   In no event shall NASA be liable for any damages, including, but not limited to direct, indirect, special or consequential damages,
   arising out of, resulting from, or in any way connected with the software or its documentation.  Whether or not based upon warranty,
   contract, tort or otherwise, and whether or not loss was sustained from, or arose out of the results of, or use of, the software,
   documentation or services provided hereunder

   ITC Team
   NASA IV&V
   ivv-itc@lists.nasa.gov
*/

#include <generic_css_data_point.hpp>
#include <ItcLogger/Logger.hpp>

namespace Nos3
{
    extern ItcLogger::Logger *sim_logger;

    Generic_cssDataPoint::Generic_cssDataPoint(double count) : _not_parsed(false)
    {
        sim_logger->trace("Generic_cssDataPoint::Generic_cssDataPoint:  Defined Constructor executed");

        /* Do calculations based on provided data */
        std::vector<bool>   valid(6, true);
        std::vector<float> illum(6, 0.0);
        _generic_css_data = illum;
        _generic_css_data[0] = count * 0.001;
        _generic_css_data[1] = count * 0.001;
        _generic_css_data[2] = count * 0.001;
        _generic_css_data[3] = count * 0.001;
        _generic_css_data[4] = count * 0.001;
        _generic_css_data[5] = count * 0.001;
    }

    Generic_cssDataPoint::Generic_cssDataPoint(double scale_factor, int16_t spacecraft, const boost::shared_ptr<Sim42DataPoint> dp) : _dp(*dp), _sc(spacecraft), _scale_factor(scale_factor), _not_parsed(true)
    {
        sim_logger->trace("Generic_cssDataPoint::Generic_cssDataPoint:  42 Constructor executed");
    }

    void Generic_cssDataPoint::do_parsing(void) const
    {
        /* Initialize data */
        std::vector<bool>   valid(6, false);
        std::vector<float> illum(6, 0.0);
        _generic_css_data = illum;

        try{
            /*
            ** Declare 42 telemetry string prefix
            ** 42 variables defined in `42/Include/42types.h`
            ** 42 data stream defined in `42/Source/IPC/SimWriteToSocket.c`
            */
            std::string key0v;
            key0v.append("SC[").append(std::to_string(_sc)).append("].AC.CSS"); // SC[N].AC.CSS
            std::string key0i(key0v), key1v(key0v), key1i(key0v), key2v(key0v), key2i(key0v), key3v(key0v), key3i(key0v), key4v(key0v), key4i(key0v), key5v(key0v), key5i(key0v);
            key0v.append("[0].Valid");
            key0i.append("[0].Illum");
            key1v.append("[1].Valid");
            key1i.append("[1].Illum");
            key2v.append("[2].Valid");
            key2i.append("[2].Illum");
            key3v.append("[3].Valid");
            key3i.append("[3].Illum");
            key4v.append("[4].Valid");
            key4i.append("[4].Illum");
            key5v.append("[5].Valid");
            key5i.append("[5].Illum");

            /* Parse 42 telemetry */
            _generic_css_data[0] = std::stof(_dp.get_value_for_key(key0i)) / _scale_factor;
            if (_dp.get_value_for_key(key0v) == "0") _generic_css_data[0] = 0.0;
            _generic_css_data[1] = std::stof(_dp.get_value_for_key(key1i)) / _scale_factor;
            if (_dp.get_value_for_key(key1v) == "0") _generic_css_data[1] = 0.0;
            _generic_css_data[2] = std::stof(_dp.get_value_for_key(key2i)) / _scale_factor;
            if (_dp.get_value_for_key(key2v) == "0") _generic_css_data[2] = 0.0;
            _generic_css_data[3] = std::stof(_dp.get_value_for_key(key3i)) / _scale_factor;
            if (_dp.get_value_for_key(key3v) == "0") _generic_css_data[3] = 0.0;
            _generic_css_data[4] = std::stof(_dp.get_value_for_key(key4i)) / _scale_factor;
            if (_dp.get_value_for_key(key4v) == "0") _generic_css_data[4] = 0.0;
            _generic_css_data[5] = std::stof(_dp.get_value_for_key(key5i)) / _scale_factor;
            if (_dp.get_value_for_key(key5v) == "0") _generic_css_data[5] = 0.0;

            _not_parsed = false;
        } 
        catch(const std::exception& e) 
        {
            /* Force data to be set to known values */
            std::vector<float> illum(6, 0.0);
            _generic_css_data = illum; 
            sim_logger->error("Generic_cssDataPoint::Generic_cssDataPoint:  Parsing exception %s", e.what());
        }
    }

    /* Used for printing a representation of the data point */
    std::string Generic_cssDataPoint::to_string(void) const
    {
        sim_logger->trace("Generic_cssDataPoint::to_string:  Executed");
        std::stringstream output;
        output << "Channel values: ";
        for (unsigned int i = 0; i < _generic_css_data.size(); i++) {
            output << _generic_css_data[i];
            if (i < _generic_css_data.size() - 1) {
                output << ", ";
            }
        }
        return output.str();

    }
} /* namespace Nos3 */
