/* Copyright (C) 2015 - 2015 National Aeronautics and Space Administration. All Foreign Rights are Reserved to the U.S. Government.

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

#ifndef NOS3_SIMIDATAPROVIDER_HPP
#define NOS3_SIMIDATAPROVIDER_HPP

#include <sim_data_provider_maker.hpp>
#define REGISTER_DATA_PROVIDER(T,K) static Nos3::SimDataProviderMaker<T> maker(K) // T = type, K = key

#include <sim_config.hpp>
#include <sim_i_data_point.hpp>

namespace Nos3
{
    /// \brief Interface for a provider of simulation data.
    class SimIDataProvider
    {
    public:
        /// @name Constructors / destructors
        //@{
        /// \brief Constructor taking a configuration object.
        /// @param  sc  The configuration for the simulation
        SimIDataProvider(__attribute__((unused)) const boost::property_tree::ptree& config) {};
        /// \brief Destructor.
        virtual ~SimIDataProvider() {};
        //@}

        /// @name Non-mutating public worker methods
        //@{
        /** \brief Method to retrieve sim data for the current time.  This method must be overridden in a derived class.
         *
         * @returns                     A data point for the current time.
         */
        virtual boost::shared_ptr<SimIDataPoint> get_data_point() const = 0;
        //@}

    };
}

#endif
