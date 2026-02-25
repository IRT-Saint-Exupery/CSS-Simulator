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

#ifndef NOS3_BARDATAPOINT_HPP
#define NOS3_BARDATAPOINT_HPP

#include <boost/shared_ptr.hpp>

#include <sim_42data_point.hpp>
#include <sim_i_data_point.hpp>

namespace Nos3
{
    class GenericRWDataPoint : public SimIDataPoint
    {
    public:
        //GPSSimDataPoint(void) : _not_parsed(false) /* nothing to parse */ {};
        GenericRWDataPoint(int16_t spacecraft, int16_t wheel, const boost::shared_ptr<Sim42DataPoint> dp);
        ~GenericRWDataPoint(void);
        std::string to_string(void) const;

        double get_momentum(void) const {parse_data_point(); return _momentum;}
    private:
        // Private mutators
        inline void parse_data_point(void) const {if (_not_parsed) do_parsing();}
        void do_parsing(void) const;

        // Private data
        mutable Sim42DataPoint _dp;
        int16_t _sc;
        int16_t _reactionwheel;
        // mutable below so parsing can be on demand:
        mutable bool _not_parsed;
        mutable double _momentum; // Momentum - Nms
    };
}

#endif
