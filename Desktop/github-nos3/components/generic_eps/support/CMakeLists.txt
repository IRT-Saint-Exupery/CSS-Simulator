cmake_minimum_required(VERSION 2.6.4)

# Specify the cross compiler executables
# Typically these would be installed in a home directory or somewhere
# in /opt.  However in this example the system compiler is used.
if(DEFINED ENV{TARGET_CROSS})
  SET(CMAKE_C_COMPILER            "$ENV{TARGET_CROSS}gcc")
  SET(CMAKE_CXX_COMPILER          "$ENV{TARGET_CROSS}g++")
else(NOT DEFINED ENV{TARGET_CROSS})
  SET(CMAKE_C_COMPILER            "/usr/bin/gcc")
  SET(CMAKE_CXX_COMPILER          "/usr/bin/g++")
endif()

project (generic_eps_checkout)

set(TGTNAME cpu2)
include(../../ComponentSettings.cmake)

include_directories("./")
include_directories("../fsw/platform_inc")
include_directories("../fsw/src")
include_directories("../../../fsw/apps/hwlib/fsw/public_inc")

add_executable(generic_eps_checkout 
               generic_eps_checkout.c 
               ../fsw/src/generic_eps_device.c
               ../../../fsw/apps/hwlib/fsw/linux/libi2c.c
              )
target_link_libraries(generic_eps_checkout)
