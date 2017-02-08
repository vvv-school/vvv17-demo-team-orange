# Copyright: (C) VVV17, Santa Margherita
# Copyright: (C) Team Orange
# Authors: Pedro Vicente <pvicente@isr.tecnico.ulisboa.pt>
# CopyPolicy: Released under the terms of the GNU GPL v2.0.

cmake_minimum_required(VERSION 2.6)

set(PROJECTNAME master)
project(${PROJECTNAME})

find_package(YARP)
find_package(ICUBcontrib)

list(APPEND CMAKE_MODULE_PATH ${YARP_MODULE_PATH})
list(APPEND CMAKE_MODULE_PATH ${ICUBCONTRIB_MODULE_PATH})

include(YarpIDL)
set(IDL_GENERATED_CODE_DIR "${CMAKE_CURRENT_SOURCE_DIR}")
yarp_idl_to_dir(${PROJECTNAME}.thrift ${IDL_GENERATED_CODE_DIR})

include(YarpInstallationHelpers)
include(ICUBcontribHelpers)
include(ICUBcontribOptions)
icubcontrib_set_default_prefix()

file(GLOB source src/*.cpp)
file(GLOB header include/*.h)

source_group("Source Files" FILES ${source})
source_group("Header Files" FILES ${header})

include_directories(${PROJECT_SOURCE_DIR}/include
                    ${YARP_INCLUDE_DIRS}
                    ${IDL_GENERATED_CODE_DIR}/include
)

add_executable(${PROJECTNAME} ${source} ${header})

target_link_libraries(${PROJECTNAME} ${YARP_LIBRARIES}) 

install(TARGETS ${PROJECTNAME} DESTINATION bin)

yarp_install(FILES master.xml DESTINATION ${ICUBCONTRIB_MODULES_INSTALL_DIR})




