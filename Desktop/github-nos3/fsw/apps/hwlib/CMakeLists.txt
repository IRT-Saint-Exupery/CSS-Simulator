cmake_minimum_required(VERSION 2.6.4)
project(CFS_HWLIB C)

# flight/nos psp names (based on CFE_SYSTEM_PSPNAME set by the toolchain from targets.cmake)
set(NOS_PSPNAME "nos-linux")

include(../../../components/ComponentSettings.cmake)

# hwlib include directories
include_directories(fsw/mission_inc
                    fsw/platform_inc
                    fsw/public_inc)

# include directories for flight configuration
IF(CFE_SYSTEM_PSPNAME STREQUAL "pc-linux")
    message(STATUS "hwlib: linux configuration")
    aux_source_directory(fsw/linux APP_SRC_FILES)
ELSE()
    message(STATUS "hwlib: NON-FLIGHT configuration")
ENDIF()

# create the hwlib app module
aux_source_directory(fsw/src APP_SRC_FILES)
add_cfe_app(hwlib ${APP_SRC_FILES})

# stub out hardware for all non-flight configurations
IF(CFE_SYSTEM_PSPNAME STREQUAL NOS_PSPNAME)
    add_subdirectory(sim)
    target_link_libraries(hwlib noslink)
ELSEIF(CFE_SYSTEM_PSPNAME STREQUAL "pc-linux")
	target_link_libraries(hwlib socketcan)
ENDIF()
