#------------------------------------------------------------------------------------------
# Define the source files needed to build the module interface library
#------------------------------------------------------------------------------------------
SET(BRAKE_MODULE_PATHS
                ${CMAKE_CURRENT_SOURCE_DIR}/Include
                ${CMAKE_CURRENT_SOURCE_DIR}/Bosch8
                CACHE STRING "Path to the module interfaces files" FORCE)

#------------------------------------------------------------------------------------------
# Tell CMake what subdirectories to recurse into
#------------------------------------------------------------------------------------------
ADD_SUBDIRECTORY(Bosch8)

