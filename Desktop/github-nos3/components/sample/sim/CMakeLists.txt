project(sample_sim)

find_package(ITC_Common REQUIRED QUIET COMPONENTS itc_logger)
find_package(NOSENGINE REQUIRED QUIET COMPONENTS common transport client uart)

include_directories(inc
                    ${sim_common_SOURCE_DIR}/inc
                    ${ITC_Common_INCLUDE_DIRS}
                    ${NOSENGINE_INCLUDE_DIRS})

set(sample_sim_src
    src/sample_hardware_model.cpp
    src/sample_42_data_provider.cpp
    src/sample_data_provider.cpp
    src/sample_data_point.cpp
)

# For Code::Blocks and other IDEs
file(GLOB sample_sim_inc inc/*.hpp)

set(sample_sim_libs
    sim_common
    ${ITC_Common_LIBRARIES}
    ${NOSENGINE_LIBRARIES}
)

set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_RPATH}:$ORIGIN/../lib") # Pick up .so in install directory

add_library(sample_sim SHARED ${sample_sim_src} ${sample_sim_inc})
target_link_libraries(sample_sim ${sample_sim_libs})
install(TARGETS sample_sim LIBRARY DESTINATION lib ARCHIVE DESTINATION lib)
