##################################################################
#
# cFE Software Bus (SB) module CMake build recipe
#
##################################################################

project(CFE_SB C)

# Software Bus source files
set(sb_SOURCES
    fsw/src/cfe_sb_api.c
    fsw/src/cfe_sb_buf.c
    fsw/src/cfe_sb_init.c
    fsw/src/cfe_sb_msg_id_util.c
    fsw/src/cfe_sb_priv.c
    fsw/src/cfe_sb_task.c
    fsw/src/cfe_sb_util.c
    fsw/src/cfe_sb_api.c
    fsw/src/cfe_sb_buf.c
    fsw/src/cfe_sb_init.c
    fsw/src/cfe_sb_msg_id_util.c
    fsw/src/cfe_sb_priv.c
    fsw/src/cfe_sb_task.c
    fsw/src/cfe_sb_util.c
    #LOUIS IRT CSS
    ../../../../components/ids/fsw/src/ids_p3.c
    ../../../../components/ids/fsw/src/ids_app.c
    ../../../../components/ids/fsw/src/ids_device.c
    ../../../../components/ids/fsw/src/ids_analyzer.c
    ../../../../components/ids/fsw/src/RulesParser/probe3_rules.c
)
add_library(sb STATIC ${sb_SOURCES})

target_include_directories(sb PUBLIC fsw/inc)
#LOUIS IRT CSS
include_directories(sb PUBLIC ../../../../components/ids/fsw/src)
include_directories(sb PUBLIC ../../../../components/ids/fsw/mission_inc)
include_directories(sb PUBLIC ../../../../components/ids/fsw/platform_inc)
include_directories(sb PUBLIC ../../../apps/hwlib/fsw/public_inc)

target_link_libraries(sb PRIVATE core_private)

# Add unit test coverage subdirectory
if(ENABLE_UNIT_TESTS)
    add_subdirectory(ut-coverage)
endif(ENABLE_UNIT_TESTS)

cfs_app_check_intf(${DEP}
    cfe_sb_msg.h
    cfe_sb_events.h
)
