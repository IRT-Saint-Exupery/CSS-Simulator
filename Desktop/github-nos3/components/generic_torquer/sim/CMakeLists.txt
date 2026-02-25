project(generic_torquer_sim)

find_package(ITC_Common REQUIRED QUIET COMPONENTS itc_logger)
find_package(NOSENGINE REQUIRED QUIET COMPONENTS common transport client)

include_directories(inc
                    ${sim_common_SOURCE_DIR}/inc
                    ${ITC_Common_INCLUDE_DIRS}
                    ${NOSENGINE_INCLUDE_DIRS})

set(generic_torquer_sim_src
    src/generic_torquer_hardware_model.cpp
    src/generic_torquer_42_data_provider.cpp
)

# For Code::Blocks and other IDEs
file(GLOB generic_torquer_sim_inc inc/*.hpp)

set(generic_torquer_sim_libs
    sim_common
    ${ITC_Common_LIBRARIES}
    ${NOSENGINE_LIBRARIES}
)

set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_RPATH}:$ORIGIN/../lib") # Pick up .so in install directory

add_library(generic_torquer_sim SHARED ${generic_torquer_sim_src} ${generic_torquer_sim_inc})
target_link_libraries(generic_torquer_sim ${generic_torquer_sim_libs})
install(TARGETS generic_torquer_sim LIBRARY DESTINATION lib ARCHIVE DESTINATION lib)
