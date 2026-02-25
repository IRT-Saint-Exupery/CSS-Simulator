/* Copyright (C) 2016 - 2016 National Aeronautics and Space Administration. All Foreign Rights are Reserved to the U.S. Government.

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

#include <generic_rw_data_point.hpp>

#include <ItcLogger/Logger.hpp>

namespace Nos3
{
    extern ItcLogger::Logger *sim_logger;

    //GenericRWDataPoint::GenericRWDataPoint(void) : SimIDataPoint()
    //{
    //    sim_logger->trace("GenericRWDataPoint::GenericRWDataPoint:  Constructor executed");
    //}

    GenericRWDataPoint::GenericRWDataPoint(int16_t spacecraft, int16_t reactionwheel, const boost::shared_ptr<Sim42DataPoint> dp) : 
        _dp(*dp), _sc(spacecraft), _reactionwheel(reactionwheel) 
    {
        sim_logger->trace("GPSSimDataPoint::GPSSimDataPoint:  Created instance using _sc=%d, _reactionwheel=%d, _dp=%s", 
            _sc, _reactionwheel, _dp.to_string().c_str());
        /* Initialize data */
        _momentum = 0;
    }

    void GenericRWDataPoint::do_parsing(void) const  
    { 
        try {
            /*
            ** Declare 42 telemetry string prefix
            ** 42 variables defined in `42/Include/42types.h`
            ** 42 data stream defined in `42/Source/IPC/SimWriteToSocket.c`
            */
            // SC[N].AC.Whl[M].H
            std::string key;
            key.append("SC[").append(std::to_string(_sc)).append("].AC.Whl[").append(std::to_string(_reactionwheel)).append("].H");

            _momentum = std::stod(_dp.get_value_for_key(key));
        } catch (const std::exception& e) {
            sim_logger->error("GenericRWDataPoint::GenericRWDataPoint:  Parsing exception %s", e.what());
        }

    }

    GenericRWDataPoint::~GenericRWDataPoint(void)
    {
        sim_logger->trace("GenericRWDataPoint::~GenericRWDataPoint:  Destructor executed");
    }

    std::string GenericRWDataPoint::to_string(void) const
    {        
        std::stringstream ss;

        ss << std::fixed << std::setfill(' ');
        ss << "Generic Reaction Wheel Data Point: ";
        ss << std::setprecision(std::numeric_limits<double>::digits10); // Full double precision
        ss << " Momentum: " << _momentum;

        return ss.str();
    }
}
