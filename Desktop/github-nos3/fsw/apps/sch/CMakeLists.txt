cmake_minimum_required(VERSION 2.6.4)
project(CFS_SCH C)

include_directories(fsw/public_inc)
include_directories(fsw/mission_inc)
include_directories(fsw/platform_inc)
include_directories(fsw/src)
include_directories(${APPLICATION_PLATFORM_INC_LIST})

aux_source_directory(fsw/src APP_SRC_FILES)
aux_source_directory(fsw/tables APP_TABLE_FILES)

# Create the app module
add_cfe_app(sch ${APP_SRC_FILES})
add_cfe_tables(sch ${APP_TABLE_FILES})
