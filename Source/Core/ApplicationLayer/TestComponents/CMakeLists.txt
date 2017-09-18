#------------------------------------------------------------------------------------------
# Define a cache variable to hold the path to the libcommoncomponent.a library
#------------------------------------------------------------------------------------------
SET(CORE_COMMON_TC_LIB_PATH ${CMAKE_CURRENT_BINARY_DIR}/Common
	CACHE PATH "Path to the core common test component library"
	FORCE)

#------------------------------------------------------------------------------------------
# Define a cache variable to hold the path to the common component headers
#------------------------------------------------------------------------------------------
SET(CORE_COMMON_TC_PATHS ${CMAKE_CURRENT_SOURCE_DIR}/Common ${CMAKE_CURRENT_SOURCE_DIR}/Include)
SET(CORE_COMMON_TC_INC_PATH ${CORE_COMMON_TC_PATHS}
	CACHE STRING "Path(s) to the core common test component include files"
	FORCE)

#------------------------------------------------------------------------------------------
# All components need the protocol filter and module interface headers
#------------------------------------------------------------------------------------------
INCLUDE_DIRECTORIES(${CORE_MODULES_INC_PATH} ${CORE_PROTOCOLS_INC_PATH} ${CORE_COMMON_TC_INC_PATH})

#------------------------------------------------------------------------------------------
# Tell CMake what subdirectories to recurse into
#------------------------------------------------------------------------------------------
ADD_SUBDIRECTORY(Common)

#------------------------------------------------------------------------------------------
# All components link against the common library
#------------------------------------------------------------------------------------------
LINK_DIRECTORIES(${CORE_MODULES_LIB_PATH} ${CORE_PROTOCOLS_LIB_PATH} ${CORE_COMMON_TC_LIB_PATH})
LINK_LIBRARIES( moduleinterfaces protocolfilters commoncomponent)

#------------------------------------------------------------------------------------------
# Tell CMake what subdirectories to recurse into
#------------------------------------------------------------------------------------------
ADD_SUBDIRECTORY(Brake)
ADD_SUBDIRECTORY(Emissions)
ADD_SUBDIRECTORY(GasAnalyzer)
ADD_SUBDIRECTORY(LossCompensation)
ADD_SUBDIRECTORY(Machine)
ADD_SUBDIRECTORY(ModuleVINReader)
ADD_SUBDIRECTORY(SpeedControl)
ADD_SUBDIRECTORY(TransferCase)
ADD_SUBDIRECTORY(Transmission)


