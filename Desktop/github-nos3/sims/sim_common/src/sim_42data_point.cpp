/* Copyright (C) 2015 - 2017 National Aeronautics and Space Administration. All Foreign Rights are Reserved to the U.S. Government.

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

#include <iomanip>
#include <limits>

#include <boost/algorithm/string.hpp>
#include <boost/tokenizer.hpp>
#include <boost/lexical_cast.hpp>

#include <ItcLogger/Logger.hpp>

#include <sim_42data_point.hpp>

namespace Nos3
{

    extern ItcLogger::Logger *sim_logger;

    /*************************************************************************
     * Constructors
     *************************************************************************/

    Sim42DataPoint::Sim42DataPoint(std::vector<std::string> &message) : _lines(message)
    {
        for (std::vector<std::string>::const_iterator iter = _lines.begin(); iter != _lines.end(); iter++) {
            size_t equals = iter->find("=");
            if (equals != std::string::npos) {
                std::string key(iter->substr(0, equals));
                boost::trim(key);
                std::string value(iter->substr(equals+1));
                boost::trim(value);
                _key_values.insert({key, value});
            } else {
                std::string key(*iter);
                boost::trim(key);
                _key_values.insert({key, ""});
            }
        }
        sim_logger->trace("Sim42DataPoint::Sim42DataPoint:  Constructed data point with:  %s", to_string().c_str());
        sim_logger->trace("Sim42DataPoint::Sim42DataPoint - key/values:");
        for (std::map<std::string, std::string>::const_iterator iter = _key_values.begin(); iter != _key_values.end(); iter++) {
            sim_logger->trace("  %s, %s", iter->first.c_str(), iter->second.c_str());
        }
    }

    /*************************************************************************
     * Accessors
     *************************************************************************/

    std::string Sim42DataPoint::to_string(void) const
    {
        std::stringstream ss;

        ss << std::fixed << std::setfill(' ');
        ss << "42 Data Point: ";
        for (std::vector<std::string>::const_iterator it = _lines.begin(); it != _lines.end(); ++it) {
            ss << *it;
        }
        return ss.str();
    }

    std::string Sim42DataPoint::get_value_for_key(std::string key) {
        return _key_values[key];
    }

    /*************************************************************************
     * Static methods
     *************************************************************************/

    void Sim42DataPoint::parse_double_vector(const std::string& text, std::vector<double>& dv) 
    {
        dv.clear();
        std::istringstream iss(text);
        for (std::string s; iss >> s; )
            dv.push_back(std::stod(s));
    }
    

}
