#------------------------------------------------------------------------------------------
# Define the source files needed to build the module interface library
#------------------------------------------------------------------------------------------
SET(GATEWAY_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR} 
    CACHE STRING "Path to the gateway module interface files" FORCE)

#------------------------------------------------------------------------------------------
# Tell CMake what subdirectories to recurse into
#------------------------------------------------------------------------------------------
ADD_SUBDIRECTORY(Configuration)
     
