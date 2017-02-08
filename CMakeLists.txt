# Copyright: (C) VVV17, Santa Margherita
# Copyright: (C) Team Orange
# Authors: Pedro Vicente <pvicente@isr.tecnico.ulisboa.pt>
# CopyPolicy: Released under the terms of the GNU GPL v2.0.

cmake_minimum_required(VERSION 2.6)

set(PROJECTNAME orange-team-superbuild)
project(${PROJECTNAME})

find_package(ICUBcontrib)
list(APPEND CMAKE_MODULE_PATH ${ICUBCONTRIB_MODULE_PATH})
include(ICUBcontribOptions)
include(ICUBcontribHelpers)

add_subdirectory(src)
add_subdirectory(app)

icubcontrib_add_uninstall_target()
