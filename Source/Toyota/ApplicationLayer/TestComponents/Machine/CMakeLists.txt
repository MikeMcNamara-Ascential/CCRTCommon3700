#------------------------------------------------------------------------------------------
# Define the source files needed to build the common component library
#------------------------------------------------------------------------------------------
FILE(GLOB_RECURSE COMPONENT_SRC *.cpp)

#------------------------------------------------------------------------------------------
# Define the name of executable we will create
#------------------------------------------------------------------------------------------
SET(EXE_NAME ToyotaMachineTc)

#------------------------------------------------------------------------------------------
# Where to find the common headers
#------------------------------------------------------------------------------------------
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}
		    ${CORE_MACHINE_TC_INC_PATH})

#------------------------------------------------------------------------------------------
# All components link against the common library
#------------------------------------------------------------------------------------------
LINK_DIRECTORIES(${CORE_MACHINE_TC_LIB_PATH})
LINK_LIBRARIES(machinecomponent)

#------------------------------------------------------------------------------------------
# Tell CMake what subdirectories to recurse into
#------------------------------------------------------------------------------------------
ADD_SUBDIRECTORY(Configuration)

#------------------------------------------------------------------------------------------
# Tell CMake what source file to build
#------------------------------------------------------------------------------------------
#ADD_EXECUTABLE(${EXE_NAME}.${CMAKE_EXECUTABLE_SUFFIX} ${COMPONENT_SRC})
CREATE_QXE(${EXE_NAME} ${EXE_NAME}Main.cpp ToyotaMachineTC.cpp)
TARGET_LINK_LIBRARIES(${EXE_NAME} m -lds -lm)

#------------------------------------------------------------------------------------------
# Make sure that the services layer lib is built first
#------------------------------------------------------------------------------------------
INCLUDE(${CCRT_ROOT}/LibServices.cmake)

#------------------------------------------------------------------------------------------
# Make sure the executable has the correct extension
#------------------------------------------------------------------------------------------
INCLUDE(${CCRT_ROOT}/ExeQxe.cmake)

#------------------------------------------------------------------------------------------
# Tell CMake where to install the executable
#------------------------------------------------------------------------------------------
INSTALL_TARGETS(${APPLICATION_LAYER_INSTALL_DIR} ${EXE_NAME}.qxe)
