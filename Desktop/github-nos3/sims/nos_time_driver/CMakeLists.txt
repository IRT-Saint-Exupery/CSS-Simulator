project(nos_time_driver)

find_package(Boost REQUIRED QUIET COMPONENTS system program_options filesystem)
find_package(ITC_Common REQUIRED QUIET COMPONENTS itc_logger)
find_package(NOSENGINE REQUIRED QUIET COMPONENTS common transport client server)

include_directories(inc
                    ${sim_common_SOURCE_DIR}/inc
                    ${ITC_Common_INCLUDE_DIRS}
                    ${NOSENGINE_INCLUDE_DIRS}
					${Boost_INCLUDE_DIRS})

set(nos_time_driver_src
    src/time_driver.cpp
)

# For Code::Blocks and other IDEs
file(GLOB nos_time_driver_inc inc/*.hpp)

set(nos_time_driver_libs
    sim_common
    ${Boost_LIBRARIES}
    ${ITC_Common_LIBRARIES}
    ${NOSENGINE_LIBRARIES}
    ncurses
)

set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_RPATH}:$ORIGIN/../lib") # Pick up .so in install directory

add_library(nos_time_driver SHARED ${nos_time_driver_src} ${nos_time_driver_inc})
target_link_libraries(nos_time_driver ${nos_time_driver_libs})
install(TARGETS nos_time_driver LIBRARY DESTINATION lib ARCHIVE DESTINATION lib)
