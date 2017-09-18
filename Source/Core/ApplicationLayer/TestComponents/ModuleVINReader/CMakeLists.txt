#------------------------------------------------------------------------------------------
# Define a cache variable to hold the path to the common component headers and .cpp
#------------------------------------------------------------------------------------------
SET(MODULE_SERVER_TC_PATHS ${CMAKE_CURRENT_SOURCE_DIR}/Include)
                
#------------------------------------------------------------------------------------------
# Add the Customer InputServer Include directory to the list of include directories
#------------------------------------------------------------------------------------------
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/Include)

#------------------------------------------------------------------------------------------
# Tell CMake what subdirectories to recurse into
#------------------------------------------------------------------------------------------
ADD_SUBDIRECTORY(VehicleModuleVinReader)