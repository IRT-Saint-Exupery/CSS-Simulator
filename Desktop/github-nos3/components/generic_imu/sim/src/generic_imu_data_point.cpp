#include <ItcLogger/Logger.hpp>
#include <generic_imu_data_point.hpp>

//#define IMU_SIM_DATAPOINT_DEBUG

namespace Nos3
{
    extern ItcLogger::Logger *sim_logger;

    Generic_imuDataPoint::Generic_imuDataPoint(double count): _not_parsed(false)
    {
        sim_logger->trace("Generic_imuDataPoint::Generic_imuDataPoint:  Defined Constructor executed");

        /* Do calculations based on provided data */
        _generic_imu_data_is_valid = true;
        _gyroRates[0] = count;
        _gyroRates[1] = count;
        _gyroRates[2] = count;
    }

    Generic_imuDataPoint::Generic_imuDataPoint(int16_t spacecraft, const boost::shared_ptr<Sim42DataPoint> dp) : _dp(*dp), _sc(spacecraft), _not_parsed(true)
    {
        sim_logger->trace("Generic_imuDataPoint::Generic_imuDataPoint:  42 Constructor executed");

        /* Initialize data */
        _generic_imu_data_is_valid = false;
    }

    void Generic_imuDataPoint::do_parsing(void) const
    {    
        try {
        /*
        ** Declare 42 telemetry string prefix
        ** 42 variables defined in `42/Include/42types.h`
        ** 42 data stream defined in `42/Source/IPC/SimWriteToSocket.c`
        */
            std::string keya0;
            keya0.append("SC[").append(std::to_string(_sc)).append("].AC.");
            std::string keya1(keya0), keya2(keya0), keyg0(keya0), keyg1(keya0), keyg2(keya0);
            keya0.append("Accel[0].Acc");
            keya1.append("Accel[1].Acc");
            keya2.append("Accel[2].Acc");
            keyg0.append("Gyro[0].Rate");
            keyg1.append("Gyro[1].Rate");
            keyg2.append("Gyro[2].Rate");
            
            /* Parse 42 telemetry */
            _accelRates[0] = std::stof(_dp.get_value_for_key(keya0));
            _accelRates[1] = std::stof(_dp.get_value_for_key(keya1));
            _accelRates[2] = std::stof(_dp.get_value_for_key(keya2));
            _gyroRates[0] = std::stof(_dp.get_value_for_key(keyg0));
            _gyroRates[1] = std::stof(_dp.get_value_for_key(keyg1));
            _gyroRates[2] = std::stof(_dp.get_value_for_key(keyg2));

            _generic_imu_data_is_valid = true;
            _not_parsed = false; 
        }
        catch(const std::exception& e) 
        {
            /* Report error */
            sim_logger->error("Generic_imuDataPoint::Generic_imuDataPoint:  Parsing exception %s", e.what());
        }
    }


   /*************************************************************************
    * Accessors
    *************************************************************************/
    /* Used for printing a representation of the data point */
    std::string Generic_imuDataPoint::to_string(void) const
    {
        sim_logger->trace("Generic_imuDataPoint::to_string:  Executed");
        
        std::stringstream ss;

        ss << std::fixed << std::setfill(' ');
        ss << "Generic_imu Data Point:   Valid: ";
        ss << (_generic_imu_data_is_valid ? "Valid" : "INVALID");
        ss << std::setprecision(std::numeric_limits<double>::digits10); /* Full double precision */
        ss << " Generic_imu Linear Acceleration Data: "
            << _accelRates[0]
            << ", "
            << _accelRates[1]
            << ", "
            << _accelRates[2]
            << "; "
            << " Generic_imu Angular Rotation Data: "
            << _gyroRates[0]
            << ", "
            << _gyroRates[1]
            << ", "
            << _gyroRates[2];

        return ss.str();
    }
} /* namespace Nos3 */
