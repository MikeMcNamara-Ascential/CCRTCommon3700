#------------------------------------------------------------------------------------------
# Define the source files needed to build the module interface library
#------------------------------------------------------------------------------------------
SET(BRAKE_MODULE_PATHS
                ${CMAKE_CURRENT_SOURCE_DIR}/Teves
                ${CMAKE_CURRENT_SOURCE_DIR}/TRW
                ${CMAKE_CURRENT_SOURCE_DIR}/Bosch9
                CACHE STRING "Path to the module interfaces files" FORCE)
                
#------------------------------------------------------------------------------------------
# Tell CMake what subdirectories to recurse into
#------------------------------------------------------------------------------------------
ADD_SUBDIRECTORY(Teves)
ADD_SUBDIRECTORY(TRW)
ADD_SUBDIRECTORY(Bosch9)

