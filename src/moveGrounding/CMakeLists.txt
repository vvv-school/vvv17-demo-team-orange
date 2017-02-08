# requires minimum cmake version
cmake_minimum_required(VERSION 2.8.9)

# produce the cmake var PROJECT_NAME
project(moveGrounding)

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

# building module
include_directories(${YARP_INCLUDE_DIRS}
        ${PROJECT_SOURCE_DIR}/include
        ${movePlanner_BINARY_DIR}/include)

include(YarpIDL)
yarp_add_idl(IDL_GEN_FILES moveGrounding.thrift ${movePlanner_SOURCE_DIR}/movePlanner.thrift)

#message(status "XXXXXXXXXXXXXXXX ${movePlanner_BINARY_DIR}/src")

file(GLOB source src/*.cpp)
file(GLOB headers include/*.h
                  ${movePlanner_BINARY_DIR}/include/*.h)
set(idl ${PROJECT_NAME}.thrift)

source_group("Source Files" FILES ${source}) 
source_group("Header Files" FILES ${headers})
source_group("idl_files" FILES ${idl})

# buulding server
add_executable(${PROJECT_NAME}
        ${source}
        ${headers}
        ${idl}
        ${IDL_GEN_FILES})


target_link_libraries(moveGrounding ${YARP_LIBRARIES})
install(TARGETS moveGrounding DESTINATION bin)
yarp_install(FILES moveGrounding.xml DESTINATION ${ICUBCONTRIB_MODULES_INSTALL_DIR})


