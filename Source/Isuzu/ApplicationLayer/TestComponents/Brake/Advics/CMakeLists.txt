#------------------------------------------------------------------------------------------
# Define the root of the Brake Test Component source tree
#------------------------------------------------------------------------------------------
#SET(ADVICS_BRAKE_COMP_ROOT ${CUSTOMER_BRAKE_COMP_ROOT}/Advics)

#------------------------------------------------------------------------------------------
# Define the name of executable we will create
#------------------------------------------------------------------------------------------
SET(EXE_NAME AdvicsABSTC)

#------------------------------------------------------------------------------------------
# Define the default file where our QNX use message is located
#------------------------------------------------------------------------------------------
SET(USE_FILE_NAME ${CMAKE_CURRENT_SOURCE_DIR}/${EXE_NAME}Main.cpp)

#------------------------------------------------------------------------------------------
# Where is the core protocol filter library located
#------------------------------------------------------------------------------------------
LINK_DIRECTORIES(${BRAKE_MODULE_PATHS})
LINK_LIBRARIES(moduleinterfaces protocolfilters)

#------------------------------------------------------------------------------------------
# Tell CMake what subdirectories to recurse into
#------------------------------------------------------------------------------------------
ADD_SUBDIRECTORY(Configuration)

#------------------------------------------------------------------------------------------
# Tell CMake what source file to build
#------------------------------------------------------------------------------------------
ADD_EXECUTABLE(${EXE_NAME}.${CMAKE_EXECUTABLE_SUFFIX} ${EXE_NAME}Main.cpp ${EXE_NAME}.cpp)

#------------------------------------------------------------------------------------------
# Make sure that the services layer lib is built first
#------------------------------------------------------------------------------------------
INCLUDE(${CCRT_ROOT}/LibServices.cmake)

#------------------------------------------------------------------------------------------
# Make sure the executable has the correct extension
#------------------------------------------------------------------------------------------
INCLUDE(${CCRT_ROOT}/ExeQxe.cmake)

#------------------------------------------------------------------------------------------
# Tell CMake where to install the executables
#------------------------------------------------------------------------------------------
INSTALL_TARGETS(${APPLICATION_LAYER_INSTALL_DIR} ${EXE_NAME}.qxe)

