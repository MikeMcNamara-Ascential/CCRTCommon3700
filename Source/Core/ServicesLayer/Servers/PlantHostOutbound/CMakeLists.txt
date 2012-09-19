#------------------------------------------------------------------------------
# Define the basename of our executable (for us only...not required by CMake)
#------------------------------------------------------------------------------
SET(EXE_NAME PlantHostOutbound)

#------------------------------------------------------------------------------
# Define a cache variable to hold the path to the libPlantHostOut.a library
#------------------------------------------------------------------------------
SET(CORE_PLANT_HOST_OUT_LIB_DIR ${CMAKE_CURRENT_BINARY_DIR}/Library CACHE PATH
        "Path to the core plant host outbound library" FORCE)

#------------------------------------------------------------------------------
# Define a cache variable to hold the path to the libhostinterfacelib.a library
#------------------------------------------------------------------------------
SET(CORE_HOST_INTERFACE_LIB_DIR ${CMAKE_CURRENT_BINARY_DIR}/HostInterface 
    CACHE PATH "Path to the core host interface library" FORCE)

#------------------------------------------------------------------------------
# Define a cache variable to hold the path to the plant host outbound headers
#------------------------------------------------------------------------------
SET(CORE_PLANT_HOST_OUT_INC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/Include CACHE PATH
        "Path to the core plant host outbound headers" FORCE)

#------------------------------------------------------------------------------
# Add the PlantHostOutbound include directory to the list of include 
# directories
#------------------------------------------------------------------------------
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/Include)

#------------------------------------------------------------------------------
# Tell CMake what subdirectories to recurse into
#------------------------------------------------------------------------------
ADD_SUBDIRECTORY(Library)
ADD_SUBDIRECTORY(HostInterface)
ADD_SUBDIRECTORY(GenericText)
ADD_SUBDIRECTORY(DAQ)
ADD_SUBDIRECTORY(NDB)
ADD_SUBDIRECTORY(ResultFileCopy)

#------------------------------------------------------------------------------
# Make sure the linker can find libplanthostoutbound.a once it is built.
#------------------------------------------------------------------------------
LINK_DIRECTORIES(${CMAKE_CURRENT_BINARY_DIR}/Library)

#------------------------------------------------------------------------------
# All PlantHostOutbound processes link against libplanthostoutbound.a
#------------------------------------------------------------------------------
LINK_LIBRARIES(PlantHostOut)

#------------------------------------------------------------------------------
# Tell CMake what source file to build
#------------------------------------------------------------------------------
CREATE_QXE(${EXE_NAME}
        ${EXE_NAME}Main.cpp)

#------------------------------------------------------------------------------
# Add a QNX use message to the executable
#------------------------------------------------------------------------------
ADD_USE_MSSG(${EXE_NAME} ${CMAKE_CURRENT_SOURCE_DIR}/${EXE_NAME}Main.cpp)

#------------------------------------------------------------------------------
# Tell CMake where to install the executable
#------------------------------------------------------------------------------
INSTALL_TARGETS(${SERVICES_LAYER_INSTALL_DIR} ${EXE_NAME})

