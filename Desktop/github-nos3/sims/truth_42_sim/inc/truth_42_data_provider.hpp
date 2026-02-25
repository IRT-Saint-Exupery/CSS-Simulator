#ifndef NOS3_TRUTH42DATAPROVIDER_HPP
#define NOS3_TRUTH42DATAPROVIDER_HPP

#include <boost/property_tree/ptree.hpp>

#include <sim_data_42socket_provider.hpp>

namespace Nos3
{
    // vvv This is pretty standard for a 42 data provider (if one is needed for your sim)
    class Truth42DataProvider : public SimData42SocketProvider
    {
    public:
        // Constructors / destructor
        Truth42DataProvider(const boost::property_tree::ptree& config);

        // Accessors
        boost::shared_ptr<SimIDataPoint> get_data_point(void) const;

    private:
        // Disallow these
        ~Truth42DataProvider(void) {};
        Truth42DataProvider& operator=(const Truth42DataProvider&) {return *this;};
        int16_t _orb; // Which orbit number to parse out of 42 data
        int16_t _sc;  // Which spacecraft number to parse out of 42 data
    };
}

#endif
