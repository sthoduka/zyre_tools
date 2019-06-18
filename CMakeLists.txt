cmake_minimum_required(VERSION 2.8.6)

project(zyre_tools)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

include_directories(
    include
)
########################################################################
# LIBZMQ
########################################################################
find_package(libzmq REQUIRED)
IF (LIBZMQ_FOUND)
    include_directories(${LIBZMQ_INCLUDE_DIRS})
    list(APPEND LIBS ${LIBZMQ_LIBRARIES})
ELSE (LIBZMQ_FOUND)
    message( FATAL_ERROR "libzmq not found." )
ENDIF (LIBZMQ_FOUND)

########################################################################
# CZMQ
########################################################################
find_package(czmq REQUIRED)
IF (CZMQ_FOUND)
    include_directories(${CZMQ_INCLUDE_DIRS})
    list(APPEND LIBS ${CZMQ_LIBRARIES})
ELSE (CZMQ_FOUND)
    message( FATAL_ERROR "czmq not found." )
ENDIF (CZMQ_FOUND)

########################################################################
# Zyre
########################################################################
find_package(zyre REQUIRED)
IF (ZYRE_FOUND)
    include_directories(${ZYRE_INCLUDE_DIRS})
    list(APPEND LIBS ${ZYRE_LIBRARIES})
ELSE (ZYRE_FOUND)
    message( FATAL_ERROR "zyre not found." )
ENDIF (ZYRE_FOUND)


add_executable(zyrenode
    src/zyrenode.cpp
    src/helper_functions.cpp
)

target_link_libraries(zyrenode
    ${LIBS}
)

add_executable(zyregroup
    src/zyregroup.cpp
    src/helper_functions.cpp
)

target_link_libraries(zyregroup
    ${LIBS}
)

add_executable(zyrecli
    src/zyre_tools.cpp
    src/helper_functions.cpp
)

target_link_libraries(zyrecli
    ${LIBS}
    readline
)

install(TARGETS zyrenode
    DESTINATION bin
)
install(TARGETS zyregroup
    DESTINATION bin
)
install(TARGETS zyrecli
    DESTINATION bin
)
