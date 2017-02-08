# Copyright: (C) VVV17, Santa Margherita
#C opyright: (C) Team Orange
# Authors: Pedro Vicente <pvicente@isr.ist.utl.pt>
# CopyPolicy: Released under the terms of the GNU GPL v2.0.
# requires minimum cmake version
cmake_minimum_required(VERSION 2.8.9)

# produce the cmake var PROJECT_NAME
project(movePlanner)

# mandatory use of these packages
find_package(YARP REQUIRED)
find_package(ICUBcontrib REQUIRED)

# extend the current search path used by cmake to load helpers
list(APPEND CMAKE_MODULE_PATH ${YARP_MODULE_PATH})
list(APPEND CMAKE_MODULE_PATH ${ICUBCONTRIB_MODULE_PATH})

# helpers defining certain macros (e.g. "yarp_install")
include(YarpInstallationHelpers)
include(ICUBcontribHelpers)
include(ICUBcontribOptions)

# specify to install in $ICUBcontrib_DIR/bin
icubcontrib_set_default_prefix()

# import math symbols from standard cmath
#add_definitions(-D_MY_DEFINITION)

# generate thrift idl file
yarp_add_idl(IDL_GEN_FILES idl/${PROJECT_NAME}.thrift)

# building module
include_directories(${YARP_INCLUDE_DIRS}
                    ssrc
                    ${CMAKE_CURRENT_BINARY_DIR}/include/idl/)

add_executable(${PROJECT_NAME}
               src/main.cpp
               src/movePlanner.cpp
               src/movePlanner.h
               ${idl}
               ${IDL_GEN_FILES})

target_link_libraries(${PROJECT_NAME} ${YARP_LIBRARIES})
install(TARGETS ${PROJECT_NAME} DESTINATION bin)


# generate ad-hoc project to perform "make uninstall"
#icubcontrib_add_uninstall_target()

#file(GLOB scripts ${CMAKE_SOURCE_DIR}/app/scripts/*.xml)
#yarp_install(FILES ${scripts} DESTINATION ${ICUBCONTRIB_APPLICATIONS_INSTALL_DIR})

#file(GLOB scripts ${CMAKE_SOURCE_DIR}/app/scripts/*.xml)
yarp_install(FILES movePlanner.xml DESTINATION ${ICUBCONTRIB_MODULES_INSTALL_DIR})

