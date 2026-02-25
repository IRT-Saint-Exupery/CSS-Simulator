project(ids_sim)

find_package(ITC_Common REQUIRED QUIET COMPONENTS itc_logger)
find_package(NOSENGINE REQUIRED QUIET COMPONENTS common transport client uart)

include_directories(inc
                    ${sim_common_SOURCE_DIR}/inc
                    ${ITC_Common_INCLUDE_DIRS}
                    ${NOSENGINE_INCLUDE_DIRS})

set(ids_sim_src
    src/ids_hardware_model.cpp
    src/ids_42_data_provider.cpp
    src/ids_data_provider.cpp
    src/ids_data_point.cpp
)

# For Code::Blocks and other IDEs
file(GLOB ids_sim_inc inc/*.hpp)

set(ids_sim_libs
    sim_common
    ${ITC_Common_LIBRARIES}
    ${NOSENGINE_LIBRARIES}
)

set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_RPATH}:$ORIGIN/../lib") # Pick up .so in install directory

add_library(ids_sim SHARED ${ids_sim_src} ${ids_sim_inc})
target_link_libraries(ids_sim ${ids_sim_libs})
install(TARGETS ids_sim LIBRARY DESTINATION lib ARCHIVE DESTINATION lib)
