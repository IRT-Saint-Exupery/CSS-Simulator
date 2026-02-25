project(generic_css_sim)

find_package(ITC_Common REQUIRED QUIET COMPONENTS itc_logger)
find_package(NOSENGINE REQUIRED QUIET COMPONENTS common transport client i2c)

include_directories(inc
                    ${sim_common_SOURCE_DIR}/inc
                    ${ITC_Common_INCLUDE_DIRS}
                    ${NOSENGINE_INCLUDE_DIRS})

set(generic_css_sim_src
    src/generic_css_hardware_model.cpp
    src/generic_css_42_data_provider.cpp
    src/generic_css_data_provider.cpp
    src/generic_css_data_point.cpp
)

# For Code::Blocks and other IDEs
file(GLOB generic_css_sim_inc inc/*.hpp)

set(generic_css_sim_libs
    sim_common
    ${ITC_Common_LIBRARIES}
    ${NOSENGINE_LIBRARIES}
)

set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_RPATH}:$ORIGIN/../lib") # Pick up .so in install directory

add_library(generic_css_sim SHARED ${generic_css_sim_src} ${generic_css_sim_inc})
target_link_libraries(generic_css_sim ${generic_css_sim_libs})
install(TARGETS generic_css_sim LIBRARY DESTINATION lib ARCHIVE DESTINATION lib)
