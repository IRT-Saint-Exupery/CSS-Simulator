cmake_minimum_required(VERSION 2.6.4)
project(CFS_GENERIC_THRUSTER_APP C)

include(../../ComponentSettings.cmake)

include_directories(public_inc)
include_directories(mission_inc)
include_directories(platform_inc)
include_directories(src)

include_directories(${hwlib_MISSION_DIR}/fsw/public_inc)

aux_source_directory(src APP_SRC_FILES)

# Create the app module
add_cfe_app(generic_thruster ${APP_SRC_FILES})
