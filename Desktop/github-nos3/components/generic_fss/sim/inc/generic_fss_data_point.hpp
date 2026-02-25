#ifndef NOS3_GENERIC_FSSDATAPOINT_HPP
#define NOS3_GENERIC_FSSDATAPOINT_HPP

#include <boost/shared_ptr.hpp>
#include <sim_42data_point.hpp>

namespace Nos3
{
    /* Standard for a data point used transfer data between a data provider and a hardware model */
    class Generic_fssDataPoint : public Sim42DataPoint
    {
    public:
        /* Constructors */
        Generic_fssDataPoint(int16_t spacecraft, const boost::shared_ptr<Sim42DataPoint> dp);

        /* Accessors */
        /* Provide the hardware model a way to get the specific data out of the data point */
        std::string to_string(void) const;
        bool        get_generic_fss_valid(void) const {parse_data_point(); return _generic_fss_valid;}
        double      get_generic_fss_alpha(void) const {parse_data_point(); return _generic_fss_alpha;}
        double      get_generic_fss_beta(void) const {parse_data_point(); return _generic_fss_beta;}
    
    private:
        /* Disallow these */
        Generic_fssDataPoint(void) {};
        Generic_fssDataPoint(const Generic_fssDataPoint& sdp) : Sim42DataPoint(sdp) {};
        ~Generic_fssDataPoint(void) {};

        /// @name Private mutators
        //@{
        inline void parse_data_point(void) const {if (_not_parsed) do_parsing();}
        void do_parsing(void) const;
        //@}

        /* Specific data you need to get from the data provider to the hardware model */
        /* You only get to this data through the accessors above */
        mutable Sim42DataPoint _dp;
        int16_t _sc;
        mutable bool _not_parsed;
        mutable bool   _generic_fss_valid;
        mutable double _generic_fss_alpha;
        mutable double _generic_fss_beta;
    };
}

#endif
