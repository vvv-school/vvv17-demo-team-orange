# Copyright: (C) VVV17, Santa Margherita
#C opyright: (C) Team Orange
# Authors: Pedro Vicente <pvicente@isr.ist.utl.pt>
# CopyPolicy: Released under the terms of the GNU GPL v2.0.

cmake_minimum_required(VERSION 2.6)

set(PROJECTNAME orange-team)
project(${PROJECTNAME})

find_package(YARP)
find_package(ICUBcontrib)
# find_package(OpenCV) IF we need Opencv 
list(APPEND CMAKE_MODULE_PATH ${YARP_MODULE_PATH}
                              ${ICUBCONTRIB_MODULE_PATH})

include(YarpInstallationHelpers)
include(ICUBcontribHelpers)
include(ICUBcontribOptions)
icubcontrib_set_default_prefix()

# modules that do not need OpenCV
add_subdirectory(master)
add_subdirectory(pickAndPlace)
#add_subdirectory(movePlanner)
add_subdirectory(moveGrounding)
