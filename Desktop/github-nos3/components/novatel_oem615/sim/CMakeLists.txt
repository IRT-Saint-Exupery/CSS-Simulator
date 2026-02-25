project(gps_sim)

find_package(Boost REQUIRED QUIET COMPONENTS system program_options filesystem)
find_package(ITC_Common REQUIRED QUIET COMPONENTS itc_logger)
find_package(NOSENGINE REQUIRED QUIET COMPONENTS common transport client uart)

include_directories(SYSTEM ${Boost_INCLUDE_DIRS})
include_directories(inc
                    ${sim_common_SOURCE_DIR}/inc
                    ${ITC_Common_INCLUDE_DIRS}
                    ${NOSENGINE_INCLUDE_DIRS}
)

set(gps_sim_src
    src/gps_sim_hardware_model_OEM615.cpp
    src/gps_sim_data_file_provider.cpp
    src/gps_sim_data_42socket_provider.cpp
    src/gps_sim_data_point.cpp
)

# For Code::Blocks and other IDEs
file(GLOB gps_sim_inc inc/*.hpp)

set(gps_sim_libs
    sim_common
    ${Boost_LIBRARIES}
    ${ITC_Common_LIBRARIES}
    ${NOSENGINE_LIBRARIES}
)

set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_RPATH}:$ORIGIN/../lib") # Pick up .so in install directory

add_library(gps_sim SHARED ${gps_sim_src} ${gps_sim_inc})

set_target_properties(gps_sim PROPERTIES COMPILE_FLAGS "" LINK_FLAGS "")
target_link_libraries(gps_sim ${gps_sim_libs})
install(TARGETS gps_sim LIBRARY DESTINATION lib ARCHIVE DESTINATION lib)
