#------------------------------------------------------------------------------------------
# Define the source files needed to build the common component library
#------------------------------------------------------------------------------------------
FILE(GLOB_RECURSE COMMON_COMPONENT_SRC *.cpp)

#------------------------------------------------------------------------------------------
# Tell CMake what subdirectories to recurse into
#------------------------------------------------------------------------------------------
ADD_SUBDIRECTORY(Configuration)

#------------------------------------------------------------------------------------------
# Generate a Test components library
#------------------------------------------------------------------------------------------
ADD_LIBRARY(commoncomponent STATIC ${COMMON_COMPONENT_SRC})

#------------------------------------------------------------------------------------------
# Tell CMake where to install the library
#------------------------------------------------------------------------------------------
IF(INSTALL_LIBS)
        INSTALL_TARGETS(${LIB_DEPLOY_DIR}/ApplicationLayer/TestComponents/Common commoncomponent)
ENDIF(INSTALL_LIBS)

