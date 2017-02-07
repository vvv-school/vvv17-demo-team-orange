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
                    src
                    src/idl)

# buulding server
add_executable(moveGrounding
               src/idl/moveGrounding.cpp
        src/moveGroundingModule.cpp
               src/main.cpp)
target_link_libraries(moveGrounding ${YARP_LIBRARIES})
install(TARGETS moveGrounding DESTINATION bin)