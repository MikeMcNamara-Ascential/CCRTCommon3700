#------------------------------------------------------------------------------------------
# Define the basename of our executable (for us only...not required by CMake)
#------------------------------------------------------------------------------------------
SET(EXE_NAME PlantHostInbound)

#------------------------------------------------------------------------------------------
# Define a cache variable to hold the path to the libPlantHostIn.a library
#------------------------------------------------------------------------------------------
SET(CORE_PLANT_HOST_IN_LIB_DIR ${CMAKE_CURRENT_BINARY_DIR}/Library CACHE PATH
        "Path to the core plant host inbound library" FORCE)

#------------------------------------------------------------------------------------------
# Define a cache variable to hold the path to the plant host inbound headers
#------------------------------------------------------------------------------------------
SET(CORE_PLANT_HOST_IN_INC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/Include CACHE PATH
        "Path to the core plant host inbound headers" FORCE)

#------------------------------------------------------------------------------------------
# Add the PlantHostInbound include directory to the list of include directories
#------------------------------------------------------------------------------------------
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/Include)

#------------------------------------------------------------------------------------------
# Tell CMake what subdirectories to recurse into
#------------------------------------------------------------------------------------------
ADD_SUBDIRECTORY(Library)

#------------------------------------------------------------------------------------------
# Make sure the linker can find libplanthostinbound.a once it is built.
#------------------------------------------------------------------------------------------
LINK_DIRECTORIES(${CMAKE_CURRENT_BINARY_DIR}/Library)

#------------------------------------------------------------------------------------------
# All PlantHostInbound processes link against libplanthostinbound.a
#------------------------------------------------------------------------------------------
LINK_LIBRARIES(PlantHostIn)

#------------------------------------------------------------------------------------------
# Tell CMake what source file to build
#------------------------------------------------------------------------------------------
CREATE_QXE(${EXE_NAME}
        ${EXE_NAME}Main.cpp)

#------------------------------------------------------------------------------------------
# Add a QNX use message to the executable
#------------------------------------------------------------------------------------------
ADD_USE_MSSG(${EXE_NAME} ${CMAKE_CURRENT_SOURCE_DIR}/${EXE_NAME}Main.cpp)

#------------------------------------------------------------------------------------------
# Tell CMake where to install the executable
#------------------------------------------------------------------------------------------
INSTALL_TARGETS(${SERVICES_LAYER_INSTALL_DIR} ${EXE_NAME})
