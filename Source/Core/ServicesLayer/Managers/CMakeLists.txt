#------------------------------------------------------------------------------------------
# Define a cache variable to hold the path to the resource manager include files
#------------------------------------------------------------------------------------------
SET(CORE_RESMANAGER_PATHS ${CMAKE_CURRENT_SOURCE_DIR}/Include ${CMAKE_CURRENT_SOURCE_DIR}/Common)
SET(CORE_RESMANAGER_INC_PATH ${CORE_RESMANAGER_PATHS} CACHE STRING
        "Path to the core resource manager headers" FORCE)

#------------------------------------------------------------------------------------------
# Define a cache variable to hold the path to the libresmanager.a library
#------------------------------------------------------------------------------------------
SET(CORE_RESMANAGER_LIB_PATH ${CMAKE_CURRENT_BINARY_DIR}/Common CACHE PATH
        "Path to the core resource manager library")

#------------------------------------------------------------------------------------------
# Add the ResManager include directory to the list of include directories
#------------------------------------------------------------------------------------------
INCLUDE_DIRECTORIES(${CORE_RESMANAGER_INC_PATH})

#------------------------------------------------------------------------------------------
# Build the library first
#------------------------------------------------------------------------------------------
ADD_SUBDIRECTORY(Common)

#------------------------------------------------------------------------------------------
# Make sure the linker can find libresmanager.a once it is built.
#------------------------------------------------------------------------------------------
LINK_DIRECTORIES(${CORE_RESMANAGER_LIB_PATH})

#------------------------------------------------------------------------------------------
# All ResManagers links against libresmanager.a
#------------------------------------------------------------------------------------------
LINK_LIBRARIES(resmanager)

#------------------------------------------------------------------------------------------
# Tell CMake what subdirectories to recurse into
#------------------------------------------------------------------------------------------
ADD_SUBDIRECTORY(CommProxy)
ADD_SUBDIRECTORY(ICM)
ADD_SUBDIRECTORY(PlcMgr)
ADD_SUBDIRECTORY(SciIsoMuxMgr)
ADD_SUBDIRECTORY(SerialServer)
ADD_SUBDIRECTORY(TaskMon)
