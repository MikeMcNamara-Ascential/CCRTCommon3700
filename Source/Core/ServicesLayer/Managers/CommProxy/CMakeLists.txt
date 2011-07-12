#------------------------------------------------------------------------------------------
# Define a cache variable to hold the path to the CommProxy include files
#------------------------------------------------------------------------------------------
SET(CORE_COMMPROXY_PATHS ${CMAKE_CURRENT_SOURCE_DIR}/Include ${CMAKE_CURRENT_SOURCE_DIR}/Common)
SET(CORE_COMMPROXY_INC_PATH ${CORE_COMMPROXY_PATHS} CACHE STRING
        "Path to the core comm proxy headers" FORCE)

#------------------------------------------------------------------------------------------
# Define a cache variable to hold the path to the libcommproxy.a library
#------------------------------------------------------------------------------------------
SET(CORE_COMMPROXY_LIB_PATH ${CMAKE_CURRENT_BINARY_DIR}/Common CACHE PATH
        "Path to the core comm proxy library")

#------------------------------------------------------------------------------------------
# Add the Include directory to the list of include directories
#------------------------------------------------------------------------------------------
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/Include)

#------------------------------------------------------------------------------------------
# Build the library first
#------------------------------------------------------------------------------------------
ADD_SUBDIRECTORY(Common)

#------------------------------------------------------------------------------------------
# All comm proxies link against the common library
#------------------------------------------------------------------------------------------
LINK_DIRECTORIES(${CMAKE_CURRENT_BINARY_DIR}/Common)
LINK_LIBRARIES(commproxy)

#------------------------------------------------------------------------------------------
# Tell CMake what subdirectories to recurse into
#------------------------------------------------------------------------------------------
ADD_SUBDIRECTORY(AVT)
ADD_SUBDIRECTORY(BBKWireless)
ADD_SUBDIRECTORY(Gryphon)
ADD_SUBDIRECTORY(MultiCom)
ADD_SUBDIRECTORY(Raw)
ADD_SUBDIRECTORY(TcpIp)

