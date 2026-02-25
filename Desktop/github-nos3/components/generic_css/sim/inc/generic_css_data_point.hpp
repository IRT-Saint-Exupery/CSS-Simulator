#ifndef NOS3_GENERIC_CSSDATAPOINT_HPP
#define NOS3_GENERIC_CSSDATAPOINT_HPP

#include <boost/shared_ptr.hpp>
#include <sim_42data_point.hpp>

namespace Nos3
{
    /* Standard for a data point used transfer data between a data provider and a hardware model */
    class Generic_cssDataPoint : public SimIDataPoint
    {
    public:
        /* Constructors */
        Generic_cssDataPoint(double count);
        Generic_cssDataPoint(double scaleFactor, int16_t spacecraft, const boost::shared_ptr<Sim42DataPoint> dp);

        /* Accessors */
        /* Provide the hardware model a way to get the specific data out of the data point */
        std::string to_string(void) const;
        std::vector<float> getValues(void) const {parse_data_point(); return _generic_css_data;}
    
    private:
        /* Disallow these */
        Generic_cssDataPoint(void) {};
        Generic_cssDataPoint(const Generic_cssDataPoint&) {};
        ~Generic_cssDataPoint(void) {};
        /// @name Private mutators
        //@{
        inline void parse_data_point(void) const {if (_not_parsed) do_parsing();}
        void do_parsing(void) const;
        //@}

        /* Specific data you need to get from the data provider to the hardware model */
        /* You only get to this data through the accessors above */
        mutable Sim42DataPoint _dp;
        int16_t _sc;
        double _scale_factor;
        mutable bool _not_parsed;
        mutable std::vector<float> _generic_css_data;
        static const int numChannels = 6;
    };
}

#endif
