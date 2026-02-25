project(isl_sim)

find_package(ITC_Common REQUIRED QUIET COMPONENTS itc_logger)
find_package(NOSENGINE REQUIRED QUIET COMPONENTS common transport client uart)

include_directories(inc
                    ${sim_common_SOURCE_DIR}/inc
                    ${ITC_Common_INCLUDE_DIRS}
                    ${NOSENGINE_INCLUDE_DIRS})

set(isl_sim_src
    src/isl_hardware_model.cpp
    src/isl_42_data_provider.cpp
    src/isl_data_provider.cpp
    src/isl_data_point.cpp
)

# For Code::Blocks and other IDEs
file(GLOB isl_sim_inc inc/*.hpp)

set(isl_sim_libs
    sim_common
    ${ITC_Common_LIBRARIES}
    ${NOSENGINE_LIBRARIES}
)

set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_RPATH}:$ORIGIN/../lib") # Pick up .so in install directory

add_library(isl_sim SHARED ${isl_sim_src} ${isl_sim_inc})
target_link_libraries(isl_sim ${isl_sim_libs})
install(TARGETS isl_sim LIBRARY DESTINATION lib ARCHIVE DESTINATION lib)
