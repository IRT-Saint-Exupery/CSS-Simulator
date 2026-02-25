# Install script for directory: /home/nos3/Desktop/github-nos3/components/cryptolib/src

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "/home/nos3/Desktop/github-nos3/components/cryptolib/install")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Install shared libraries without execute permission?
if(NOT DEFINED CMAKE_INSTALL_SO_NO_EXE)
  set(CMAKE_INSTALL_SO_NO_EXE "1")
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "FALSE")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}/home/nos3/Desktop/github-nos3/components/cryptolib/install/lib/libcrypto.so" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}/home/nos3/Desktop/github-nos3/components/cryptolib/install/lib/libcrypto.so")
    file(RPATH_CHECK
         FILE "$ENV{DESTDIR}/home/nos3/Desktop/github-nos3/components/cryptolib/install/lib/libcrypto.so"
         RPATH "$ORIGIN/../lib")
  endif()
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "/home/nos3/Desktop/github-nos3/components/cryptolib/install/lib/libcrypto.so")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
file(INSTALL DESTINATION "/home/nos3/Desktop/github-nos3/components/cryptolib/install/lib" TYPE SHARED_LIBRARY FILES "/home/nos3/Desktop/github-nos3/components/cryptolib/build/src/libcrypto.so")
  if(EXISTS "$ENV{DESTDIR}/home/nos3/Desktop/github-nos3/components/cryptolib/install/lib/libcrypto.so" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}/home/nos3/Desktop/github-nos3/components/cryptolib/install/lib/libcrypto.so")
    file(RPATH_CHANGE
         FILE "$ENV{DESTDIR}/home/nos3/Desktop/github-nos3/components/cryptolib/install/lib/libcrypto.so"
         OLD_RPATH "::::::::::::::"
         NEW_RPATH "$ORIGIN/../lib")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/usr/bin/strip" "$ENV{DESTDIR}/home/nos3/Desktop/github-nos3/components/cryptolib/install/lib/libcrypto.so")
    endif()
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "/home/nos3/Desktop/github-nos3/components/cryptolib/install/include/crypto.h;/home/nos3/Desktop/github-nos3/components/cryptolib/install/include/crypto_config.h;/home/nos3/Desktop/github-nos3/components/cryptolib/install/include/crypto_config_structs.h;/home/nos3/Desktop/github-nos3/components/cryptolib/install/include/crypto_error.h;/home/nos3/Desktop/github-nos3/components/cryptolib/install/include/crypto_events.h;/home/nos3/Desktop/github-nos3/components/cryptolib/install/include/crypto_print.h;/home/nos3/Desktop/github-nos3/components/cryptolib/install/include/crypto_structs.h;/home/nos3/Desktop/github-nos3/components/cryptolib/install/include/cryptography_interface.h;/home/nos3/Desktop/github-nos3/components/cryptolib/install/include/key_interface.h;/home/nos3/Desktop/github-nos3/components/cryptolib/install/include/mc_interface.h;/home/nos3/Desktop/github-nos3/components/cryptolib/install/include/sa_interface.h")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
file(INSTALL DESTINATION "/home/nos3/Desktop/github-nos3/components/cryptolib/install/include" TYPE FILE FILES
    "/home/nos3/Desktop/github-nos3/components/cryptolib/src/../include/crypto.h"
    "/home/nos3/Desktop/github-nos3/components/cryptolib/src/../include/crypto_config.h"
    "/home/nos3/Desktop/github-nos3/components/cryptolib/src/../include/crypto_config_structs.h"
    "/home/nos3/Desktop/github-nos3/components/cryptolib/src/../include/crypto_error.h"
    "/home/nos3/Desktop/github-nos3/components/cryptolib/src/../include/crypto_events.h"
    "/home/nos3/Desktop/github-nos3/components/cryptolib/src/../include/crypto_print.h"
    "/home/nos3/Desktop/github-nos3/components/cryptolib/src/../include/crypto_structs.h"
    "/home/nos3/Desktop/github-nos3/components/cryptolib/src/../include/cryptography_interface.h"
    "/home/nos3/Desktop/github-nos3/components/cryptolib/src/../include/key_interface.h"
    "/home/nos3/Desktop/github-nos3/components/cryptolib/src/../include/mc_interface.h"
    "/home/nos3/Desktop/github-nos3/components/cryptolib/src/../include/sa_interface.h"
    )
endif()

