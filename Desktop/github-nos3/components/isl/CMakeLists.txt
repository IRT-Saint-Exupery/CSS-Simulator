cmake_minimum_required(VERSION 2.6.4)
project(CFS_ISL_APP C)

include(../ComponentSettings.cmake)

include_directories(fsw/public_inc)
include_directories(fsw/mission_inc)
include_directories(fsw/platform_inc)
include_directories(fsw/src)
#LOUIS ---------
include_directories(../ids/fsw/src)
include_directories(../ids/fsw/mission_inc)
include_directories(../ids/fsw/platform_inc)
#----------------
include_directories(../novatel_oem615/fsw/mission_inc)
include_directories(../novatel_oem615/fsw/platform_inc)
include_directories(../novatel_oem615/fsw/src)

include_directories(${hwlib_MISSION_DIR}/fsw/public_inc)
include_directories(${io_lib_MISSION_DIR}/fsw/public_inc)

aux_source_directory(fsw/src APP_SRC_FILES)
#LOUIS ---------
aux_source_directory(../ids/fsw/src APP_SRC_FILES)
#----------------

# Create the app module
add_cfe_app(isl ${APP_SRC_FILES})
