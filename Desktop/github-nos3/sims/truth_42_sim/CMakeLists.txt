project(truth_42_sim)

find_package(ITC_Common REQUIRED QUIET COMPONENTS itc_logger)
find_package(NOSENGINE REQUIRED QUIET COMPONENTS common client)

include_directories(inc
                    ${sim_common_SOURCE_DIR}/inc
                    ${ITC_Common_INCLUDE_DIRS}
                    ${NOSENGINE_INCLUDE_DIRS}
                    )

set(truth_42_sim_src
    src/truth_42_hardware_model.cpp
    src/truth_42_data_provider.cpp
    src/truth_42_data_point.cpp
)

# For Code::Blocks and other IDEs
file(GLOB truth_42_sim_inc inc/*.hpp)

set(truth_42_sim_libs
    sim_common
    ${ITC_Common_LIBRARIES}
    ${NOSENGINE_LIBRARIES}
)

set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_RPATH}:$ORIGIN/../lib") # Pick up .so in install directory

add_library(truth_42_sim SHARED ${truth_42_sim_src} ${truth_42_sim_inc})
target_link_libraries(truth_42_sim ${truth_42_sim_libs})
install(TARGETS truth_42_sim LIBRARY DESTINATION lib ARCHIVE DESTINATION lib)
