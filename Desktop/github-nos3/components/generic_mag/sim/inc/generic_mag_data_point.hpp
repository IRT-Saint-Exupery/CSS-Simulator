#ifndef NOS3_GENERIC_MAGDATAPOINT_HPP
#define NOS3_GENERIC_MAGDATAPOINT_HPP

#include <boost/shared_ptr.hpp>
#include <sim_42data_point.hpp>

namespace Nos3
{
    /* Standard for a data point used transfer data between a data provider and a hardware model */
    class Generic_magDataPoint : public SimIDataPoint
    {
    public:
        /* Constructors */
        Generic_magDataPoint(double count);
        Generic_magDataPoint(int16_t spacecraft, const boost::shared_ptr<Sim42DataPoint> dp);

        /* Accessors */
        /* Provide the hardware model a way to get the specific data out of the data point */
        std::string to_string(void) const;
        std::vector<float> getValues(void) const {parse_data_point(); return _generic_mag_data;}
    
    private:
        /* Disallow these */
        Generic_magDataPoint(void) {};
        Generic_magDataPoint(const Generic_magDataPoint&) {};
        ~Generic_magDataPoint(void) {};

        // Private mutators
        inline void parse_data_point(void) const {if (_not_parsed) do_parsing();}
        void do_parsing(void) const;

        // Private data
        mutable Sim42DataPoint _dp;
        int16_t _sc;
        // mutable below so parsing can be on demand:
        mutable bool _not_parsed;

        /* Specific data you need to get from the data provider to the hardware model */
        /* You only get to this data through the accessors above */
        mutable std::vector<float> _generic_mag_data;
        static const int numAxes = 3;
    };
}

#endif
