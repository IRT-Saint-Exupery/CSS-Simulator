/* Copyright (C) 2015 - 2016 National Aeronautics and Space Administration. All Foreign Rights are Reserved to the U.S. Government.

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

#ifndef NOS3_SIM42DATAPOINT_HPP
#define NOS3_SIM42DATAPOINT_HPP

#include <cstdint>
#include <string>
#include <vector>
#include <map>

#include <sim_i_data_point.hpp>

namespace Nos3
{

    /** \brief Class to contain an entry of 42 simulation data.
     *
     */
    class Sim42DataPoint : public SimIDataPoint
    {
    public:
        /// @name Constructors
        //@{
        /** \brief Default constructor
         *  Just has no lines of data.
         */
        Sim42DataPoint() {};
        /** \brief Constructor from a text message of string lines.
         *  Just sets the lines of data.
         */
        Sim42DataPoint(std::vector<std::string> &message);
        //@}

        /// @name Accessors
        //@{
        /// \brief Returns one long single string representation of the 42 simulation data point
        /// @return     A long single string representation of the 42 simulation data point
        std::string to_string(void) const;

        /// \brief Returns the lines stored in the 42 simulation data point
        /// @return     A vector of strings representing the 42 simulation data point
        std::vector<std::string> get_lines(void) const {return _lines;}

        /// \brief Returns the value for the key stored in the 42 simulation data point
        /// @param key  The key to find
        /// @return     The value corresponding to the input key
        std::string get_value_for_key(std::string key);
        //@}

        /// @name Static Methods
        //@{
        static void parse_double_vector(const std::string& text, std::vector<double>& dv);            
        //@}

    private:
        // Private data
        std::vector<std::string> _lines;
        std::map<std::string, std::string> _key_values;
    };

}

#endif

