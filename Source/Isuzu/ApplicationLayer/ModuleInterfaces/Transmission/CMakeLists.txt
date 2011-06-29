#------------------------------------------------------------------------------------------
# Define the source files needed to build the module interface library
#------------------------------------------------------------------------------------------
SET(TRANSMISSION_MODULE_PATHS
                ${CMAKE_CURRENT_SOURCE_DIR}/Aisin
                CACHE STRING "Path to the module interfaces files" FORCE)
                
#------------------------------------------------------------------------------------------
# Tell CMake what subdirectories to recurse into
#------------------------------------------------------------------------------------------
ADD_SUBDIRECTORY(Aisin)