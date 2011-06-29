#------------------------------------------------------------------------------------------
# Define the source files needed to build the common component library
#------------------------------------------------------------------------------------------
FILE(GLOB_RECURSE LC_COMPONENTS *.cpp)

#------------------------------------------------------------------------------------------
# Generate a Test components library
#------------------------------------------------------------------------------------------
ADD_LIBRARY(losscompensationcomponent STATIC ${LC_COMPONENTS})
