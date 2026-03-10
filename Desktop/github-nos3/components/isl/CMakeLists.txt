cmake_minimum_required(VERSION 2.6.4)
project(CFS_ISL_APP C)

include(../ComponentSettings.cmake)

include_directories(fsw/public_inc)
include_directories(fsw/mission_inc)
include_directories(fsw/platform_inc)
include_directories(fsw/src)
#IRT CSS IDS ---------
include_directories(../ids/fsw/src/RulesParser)
include_directories(../ids/fsw/src)
include_directories(../ids/fsw/mission_inc)
include_directories(../ids/fsw/platform_inc)
#----------------
include_directories(../novatel_oem615/fsw/mission_inc)
include_directories(../novatel_oem615/fsw/platform_inc)
include_directories(../novatel_oem615/fsw/src)

include_directories(${hwlib_MISSION_DIR}/fsw/public_inc)
include_directories(${io_lib_MISSION_DIR}/fsw/public_inc)

#IRT CSS IDS ---------
set(APP_SRC_FILES
    ../ids/fsw/src/ids_p0.c
    ../ids/fsw/src/ids_p1.c
    ../ids/fsw/src/ids_app.c
    ../ids/fsw/src/ids_device.c
)
#file(GLOB APP_SRC_FILES "../ids/fsw/src/ids_p[0-1].c|ids_app.c")
#file(GLOB APP_SRC_FILES "../ids/fsw/src/RulesParser/probe2_rules.c")
#aux_source_directory(../ids/fsw/src APP_SRC_FILES)
#----------------
aux_source_directory(fsw/src APP_SRC_FILES)

# Create the app module
add_cfe_app(isl ${APP_SRC_FILES})
