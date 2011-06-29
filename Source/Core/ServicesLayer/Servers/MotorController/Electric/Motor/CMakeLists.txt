#------------------------------------------------------------------------------------------
# Tell CMake what subdirectories to recurse into
#------------------------------------------------------------------------------------------
ADD_SUBDIRECTORY(Configuration)

#------------------------------------------------------------------------------------------
# Add  the motor objects to the motor controller library
#------------------------------------------------------------------------------------------
FILE(GLOB_RECURSE MOTOR_SOURCES *.cpp)

#------------------------------------------------------------------------------------------
# This is a listing of all of the source files necessary to create
# a library for the current directory.
#------------------------------------------------------------------------------------------
ADD_LIBRARY(motor STATIC ${MOTOR_SOURCES})
