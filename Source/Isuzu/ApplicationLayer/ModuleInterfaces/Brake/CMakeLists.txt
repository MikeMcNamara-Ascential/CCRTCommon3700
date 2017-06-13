#------------------------------------------------------------------------------------------
# Define the source files needed to build the module interface library
#------------------------------------------------------------------------------------------
SET(BRAKE_MODULE_PATHS 
                ${CMAKE_CURRENT_SOURCE_DIR}/Advics
                ${CMAKE_CURRENT_SOURCE_DIR}/Bendix
                ${CMAKE_CURRENT_SOURCE_DIR}/Isuzu720ABS
                CACHE STRING "Path to the module interfaces files" FORCE)
                
#------------------------------------------------------------------------------------------
# Tell CMake what subdirectories to recurse into
#------------------------------------------------------------------------------------------
ADD_SUBDIRECTORY(Advics)
ADD_SUBDIRECTORY(Bendix)
ADD_SUBDIRECTORY(Isuzu720ABS)