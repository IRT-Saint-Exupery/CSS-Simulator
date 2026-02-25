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

#LOUIS --------
include_directories(../../../components/ids/fsw/src)
include_directories(../../../components/ids/fsw/mission_inc)
include_directories(../../../components/ids/fsw/platform_inc)
#--------------

include_directories(${MISSION_SOURCE_DIR}/../components/cryptolib/include)

include_directories(fsw/examples/${CI_TRANSPORT})
include_directories(${MISSION_SOURCE_DIR}/apps/to/fsw/examples/${TO_TRANSPORT})
aux_source_directory(fsw/src APP_SRC_FILES)
aux_source_directory(fsw/examples/${CI_TRANSPORT} APP_SRC_FILES)

#LOUIS --------
aux_source_directory(../../../components/ids/fsw/src APP_SRC_FILES)
#--------------

# Create the app module
add_cfe_app(ci ${APP_SRC_FILES})
