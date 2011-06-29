#------------------------------------------------------------------------------------------
# Define the source files needed to build the common component library
#------------------------------------------------------------------------------------------
FILE(GLOB_RECURSE GAS_COMPONENTS *.cpp)

#------------------------------------------------------------------------------------------
# Generate a Test components library
#------------------------------------------------------------------------------------------
ADD_LIBRARY(gasanalyzercomponent STATIC ${GAS_COMPONENTS})
