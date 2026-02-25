#include <ItcLogger/Logger.hpp>

#include <sim_coordinate_transformations.hpp>
#include <truth_42_data_point.hpp>

namespace Nos3
{
    extern ItcLogger::Logger *sim_logger;

    /*************************************************************************
     * Constructors
     *************************************************************************/
    Truth42DataPoint::Truth42DataPoint(int16_t orbit, int16_t spacecraft, const boost::shared_ptr<Sim42DataPoint> dp) : 
        _dp(*dp), _orb(orbit), _sc(spacecraft), _not_parsed(true) 
    {
        sim_logger->trace("Truth42DataPoint::Truth42DataPoint:  Created instance using _orb=%d, _sc=%d, _dp=%s", 
            _orb, _sc, _dp.to_string().c_str());
    }
    
   /*************************************************************************
     * Mutators
     *************************************************************************/

    void Truth42DataPoint::do_parsing(void) const
    {
        std::ostringstream OrbMatchString;
        OrbMatchString << "Orb[" << _orb << "].";
        size_t OrbMSsize = OrbMatchString.str().size();
        std::ostringstream SCMatchString;
        SCMatchString << "SC[" << _sc << "].";
        size_t SCMSsize = SCMatchString.str().size();
        size_t position;

        _not_parsed = false;
        
        std::vector<std::string> lines = _dp.get_lines();

        try {
            // force the vectors to be initialized to the correct length in case there is no line data for them
            _pos.resize(3);
            _vel.resize(3);
            _svb.resize(3);
            _bvb.resize(3);
            _Hvb.resize(3);
            _wn.resize(3);
            _qn.resize(4);
            std::vector<double> posr(3), posn(3), velr(3), veln(3);
            for (unsigned int i = 0; i < lines.size(); i++) {
                if (lines[i].compare(0, 4, "TIME") == 0) { // e.g. TIME 2017-181-16:00:16.333600000
                    sim_logger->trace("Truth42DataPoint::do_parsing:  Found a string with the correct prefix = TIME.  String:  %s", lines[i].c_str());
                    sim_logger->trace("FOUND TIME STRING: %s",lines[i].c_str());
                    position = lines[i].find_first_of(" ");
                    _year = std::stoi(lines[i].substr(position+1, 4));
                    position = lines[i].find_first_of("-");
                    _doy =std::stoi(lines[i].substr(position+1, 3));
                    SimCoordinateTransformations::DOY2MD(_year, _doy, _month, _day);
                    position = lines[i].find_last_of("-"); 
                    _utc_hh = std::stoi(lines[i].substr(position+1, 2));
                    position = lines[i].find_first_of(":");
                    _utc_mm = std::stoi(lines[i].substr(position+1, 2));
                    position = lines[i].find_last_of(":");
                    _utc_ss = std::stod(lines[i].substr(position+1, std::string::npos));
                } else if (lines[i].compare(0, OrbMSsize, OrbMatchString.str()) == 0) {
                    if (lines[i].compare(OrbMSsize, 7, "PosN = ") == 0) {
                        _dp.parse_double_vector(lines[i].substr(OrbMSsize+7, std::string::npos), posn);
                    } else if (lines[i].compare(OrbMSsize, 7, "VelN = ") == 0) {
                        _dp.parse_double_vector(lines[i].substr(OrbMSsize+7, std::string::npos), veln);
                    }
                } else if (lines[i].compare(0, SCMSsize, SCMatchString.str()) == 0) {
                    if (lines[i].compare(SCMSsize, 7, "PosR = ") == 0) {
                        _dp.parse_double_vector(lines[i].substr(SCMSsize+7, std::string::npos), posr);
                    } else if (lines[i].compare(SCMSsize, 7, "VelR = ") == 0) {
                        _dp.parse_double_vector(lines[i].substr(SCMSsize+7, std::string::npos), velr);
                    } else if (lines[i].compare(SCMSsize, 6, "svb = ") == 0) {
                        _dp.parse_double_vector(lines[i].substr(SCMSsize+6, std::string::npos), _svb);
                    } else if (lines[i].compare(SCMSsize, 6, "bvb = ") == 0) {
                        _dp.parse_double_vector(lines[i].substr(SCMSsize+6, std::string::npos), _bvb);
                    } else if (lines[i].compare(SCMSsize, 6, "Hvb = ") == 0) {
                        _dp.parse_double_vector(lines[i].substr(SCMSsize+6, std::string::npos), _Hvb);
                    } else if (lines[i].compare(SCMSsize, 10, "B[0].wn = ") == 0) {
                        _dp.parse_double_vector(lines[i].substr(SCMSsize+10, std::string::npos), _wn);
                    } else if (lines[i].compare(SCMSsize, 10, "B[0].qn = ") == 0) {
                        _dp.parse_double_vector(lines[i].substr(SCMSsize+10, std::string::npos), _qn);
                    }
                }
            }
            for (int i = 0; i < 3; i++) {
                _pos[i] = posn[i] + posr[i];
                _vel[i] = veln[i] + velr[i];
            }
        } catch(const std::exception& e) {
            sim_logger->error("GNSS200DataPoint::do_parsing:  Parsing exception:  %s", e.what());
        }

        sim_logger->trace("GNSS200DataPoint::do_parsing:  Parsed data point:\n%s", to_string().c_str());
    }

    /*************************************************************************
     * Accessors
     *************************************************************************/

    std::string Truth42DataPoint::to_string(void) const
    {
        parse_data_point();
        
        std::stringstream ss;

        ss << std::fixed << std::setfill('0');
        ss << "Truth 42 Data Point:";
        ss << " UTC Year-DayOfYear-Time: " << std::setw(4) << _year << "-" << std::setw(3) << _doy << "(" << std::setw(2) << _month << "/" << std::setw(2) << _day << ")";
        ss << "T" << std::setw(2) << _utc_hh << ":" << std::setw(2) << _utc_mm << ":" << std::setw(9) << std::setprecision(6) << _utc_ss;
        ss << std::setprecision(0) << " Pos: " << _pos[0] << ", " << _pos[1] << ", " << _pos[2];
        ss << std::setprecision(0) << " Vel: " << _vel[0] << ", " << _vel[1] << ", " << _vel[2];
        ss << std::setprecision(3) << " svb: " << _svb[0] << ", " << _svb[1] << ", " << _svb[2];
        ss << std::setprecision(9) << " bvb: " << _bvb[0] << ", " << _bvb[1] << ", " << _bvb[2];
        ss << std::setprecision(3) << " Hvb: " << _Hvb[0] << ", " << _Hvb[1] << ", " << _Hvb[2];
        ss << std::setprecision(3) << " wn : " << _wn[0]  << ", " << _wn[1]  << ", " << _wn[2];
        ss << std::setprecision(3) << " qn : " << _qn[0]  << ", " << _qn[1]  << ", " << _qn[2]  << ", " << _qn[3];

        return ss.str();
    }
    
}
