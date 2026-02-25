######################################################################
#
# CMAKE build recipe for pc-linux PSP component
#
######################################################################

# This contains the fully platform-specific code to
# run CFE on this target.

# Build the pc-linux implementation as a library
add_library(psp-${CFE_PSP_TARGETNAME}-impl OBJECT
    src/cfe_psp_exception.c
    ../pc-linux/src/cfe_psp_memory.c
    #../pc-linux/src/cfe_psp_memtab.c
    ../pc-linux/src/cfe_psp_ssr.c
    src/cfe_psp_start.c
    ../pc-linux/src/cfe_psp_support.c
    src/cfe_psp_timer.c
    ../pc-linux/src/cfe_psp_watchdog.c
)

# The _GNU_SOURCE directive is required to call non-posix APIs
# that are specific to the Linux/glibc environment.
# Code outside the pc-linux PSP should _not_ depend on this.
target_compile_definitions(psp-${CFE_SYSTEM_PSPNAME}-impl PRIVATE
    _GNU_SOURCE
    $<TARGET_PROPERTY:psp_module_api,INTERFACE_COMPILE_DEFINITIONS>
)

target_include_directories(psp-${CFE_PSP_TARGETNAME}-impl PRIVATE
    inc
    ${NOSENGINE_INCLUDE_DIRS}
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../apps/hwlib/sim/inc
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../osal/src/os/nos/inc
    $<TARGET_PROPERTY:psp_module_api,INTERFACE_INCLUDE_DIRECTORIES>
)
