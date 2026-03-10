cmake_minimum_required(VERSION 2.6.4)
project(CI C)

include_directories(fsw/mission_inc)
include_directories(fsw/platform_inc)
include_directories(fsw/src)
include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${MISSION_SOURCE_DIR}/apps/inc)
include_directories(${MISSION_SOURCE_DIR}/apps/to/fsw/platform_inc)
include_directories(${MISSION_SOURCE_DIR}/apps/to/fsw/mission_inc)
include_directories(${MISSION_SOURCE_DIR}/apps/to/fsw/src)
include_directories(${MISSION_SOURCE_DIR}/apps/io_lib/fsw/public_inc/)
include_directories(${APPLICATION_PLATFORM_INC_LIST})

#IRT CSS IDS --------
include_directories(../../../components/ids/fsw/src)
include_directories(../../../components/ids/fsw/src/RulesParser)
include_directories(../../../components/ids/fsw/mission_inc)
include_directories(../../../components/ids/fsw/platform_inc)
#--------------

include_directories(${MISSION_SOURCE_DIR}/../components/cryptolib/include)

include_directories(fsw/examples/${CI_TRANSPORT})
include_directories(${MISSION_SOURCE_DIR}/apps/to/fsw/examples/${TO_TRANSPORT})

#IRT CSS IDS --------
set(APP_SRC_FILES
    ../../../components/ids/fsw/src/ids_p2.c
    ../../../components/ids/fsw/src/ids_app.c
    ../../../components/ids/fsw/src/ids_device.c
    ../../../components/ids/fsw/src/ids_analyzer.c
    ../../../components/ids/fsw/src/RulesParser/probe2_rules.c
)

aux_source_directory(fsw/src APP_SRC_FILES)
aux_source_directory(fsw/examples/${CI_TRANSPORT} APP_SRC_FILES)

# Create the app module
add_cfe_app(ci ${APP_SRC_FILES})
