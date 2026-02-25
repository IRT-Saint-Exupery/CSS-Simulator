cmake_minimum_required(VERSION 2.6.4)
project(CFS_GENERIC_ADCS_APP C)

include(../ComponentSettings.cmake)

include_directories(fsw/mission_inc)
include_directories(fsw/platform_inc)
include_directories(fsw/src)
include_directories(../generic_mag/fsw/platform_inc)
include_directories(../generic_mag/fsw/src)
include_directories(../generic_fss/fsw/platform_inc)
include_directories(../generic_fss/fsw/src)
include_directories(../generic_css/fsw/platform_inc)
include_directories(../generic_css/fsw/src)
include_directories(../generic_imu/fsw/platform_inc)
include_directories(../generic_imu/fsw/src)
include_directories(../generic_torquer/fsw/platform_inc)
include_directories(../generic_torquer/fsw/src)
include_directories(../generic_reaction_wheel/fsw/platform_inc)
include_directories(../generic_reaction_wheel/fsw/src)
# IRT CSS add GPS and star tracker
include_directories(../novatel_oem615/fsw/mission_inc)
include_directories(../novatel_oem615/fsw/platform_inc)
include_directories(../novatel_oem615/fsw/src)
include_directories(../generic_star_tracker/fsw/mission_inc)
include_directories(../generic_star_tracker/fsw/platform_inc)
include_directories(../generic_star_tracker/fsw/src)
include_directories(${hwlib_MISSION_DIR}/fsw/public_inc)

aux_source_directory(fsw/src APP_SRC_FILES)

# Create the app module
add_cfe_app(generic_adcs ${APP_SRC_FILES})

# Install GNC configuration files
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/fsw/cfg/Inp_DI.txt DESTINATION ${CMAKE_INSTALL_PREFIX}/${TGTSYS_${SYSVAR}}/${INSTALL_SUBDIR})
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/fsw/cfg/Inp_ADAC.txt DESTINATION ${CMAKE_INSTALL_PREFIX}/${TGTSYS_${SYSVAR}}/${INSTALL_SUBDIR})
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/fsw/cfg/Inp_DO.txt DESTINATION ${CMAKE_INSTALL_PREFIX}/${TGTSYS_${SYSVAR}}/${INSTALL_SUBDIR})
