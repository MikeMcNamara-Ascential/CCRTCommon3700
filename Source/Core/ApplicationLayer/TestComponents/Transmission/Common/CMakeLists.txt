#------------------------------------------------------------------------------------------
# Define the source files needed to build the common component library
#------------------------------------------------------------------------------------------
FILE(GLOB_RECURSE TRANS_COMPONENTS *.cpp)

#------------------------------------------------------------------------------------------
# Generate a Test components library
#------------------------------------------------------------------------------------------
ADD_LIBRARY(transcomponent STATIC ${TRANS_COMPONENTS})
