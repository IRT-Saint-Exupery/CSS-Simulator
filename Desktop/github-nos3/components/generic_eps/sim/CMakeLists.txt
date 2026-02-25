project(generic_eps_sim)

find_package(ITC_Common REQUIRED QUIET COMPONENTS itc_logger)
find_package(NOSENGINE REQUIRED QUIET COMPONENTS common transport client i2c)

include_directories(inc
                    ${sim_common_SOURCE_DIR}/inc
                    ${ITC_Common_INCLUDE_DIRS}
                    ${NOSENGINE_INCLUDE_DIRS})

set(generic_eps_sim_src
    src/generic_eps_hardware_model.cpp
    src/generic_eps_42_data_provider.cpp
    src/generic_eps_data_provider.cpp
    src/generic_eps_data_point.cpp
)

# For Code::Blocks and other IDEs
file(GLOB generic_eps_sim_inc inc/*.hpp)

set(generic_eps_sim_libs
    sim_common
    ${ITC_Common_LIBRARIES}
    ${NOSENGINE_LIBRARIES}
)

set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_RPATH}:$ORIGIN/../lib") # Pick up .so in install directory

add_library(generic_eps_sim SHARED ${generic_eps_sim_src} ${generic_eps_sim_inc})
target_link_libraries(generic_eps_sim ${generic_eps_sim_libs})
install(TARGETS generic_eps_sim LIBRARY DESTINATION lib ARCHIVE DESTINATION lib)
