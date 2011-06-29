#------------------------------------------------------------------------------------------
# Define the source files needed to build the common component library
#------------------------------------------------------------------------------------------
FILE(GLOB_RECURSE MACHINE_COMPONENTS *.cpp)

#------------------------------------------------------------------------------------------
# Generate a Test components library
#------------------------------------------------------------------------------------------
ADD_LIBRARY(machinecomponent STATIC ${MACHINE_COMPONENTS})
