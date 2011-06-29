#------------------------------------------------------------------------------------------
# Define a cache variable to hold the path to the liblosscompensationcomponent.a library
#------------------------------------------------------------------------------------------
SET(CORE_GAS_TC_LIB_PATH ${CMAKE_CURRENT_BINARY_DIR}/Common CACHE PATH
	"Path to the core gas analyzer component library" FORCE)

#------------------------------------------------------------------------------------------
# Define a cache variable to hold the path to the loss comp component headers
#------------------------------------------------------------------------------------------
SET(GAS_INC_PATHS ${CMAKE_CURRENT_SOURCE_DIR}/Common ${CMAKE_CURRENT_SOURCE_DIR}/Include)
SET(CORE_GAS_TC_INC_PATH ${GAS_INC_PATHS} CACHE STRING
	"Path(s) to the core gas analyzer component include files" FORCE)

#------------------------------------------------------------------------------------------
# Define the path to the component root for use in the TestComponent make files
#------------------------------------------------------------------------------------------
SET(CORE_GAS_COMP_ROOT ${CMAKE_CURRENT_SOURCE_DIR})

#------------------------------------------------------------------------------------------
# Where to find the common headers
#------------------------------------------------------------------------------------------
INCLUDE_DIRECTORIES(${CORE_GAS_TC_INC_PATH})

#------------------------------------------------------------------------------------------
# Build the library first
#------------------------------------------------------------------------------------------
ADD_SUBDIRECTORY(Common)

#------------------------------------------------------------------------------------------
# All components link against the common library
#------------------------------------------------------------------------------------------
LINK_DIRECTORIES(${CORE_GAS_TC_LIB_PATH})
LINK_LIBRARIES(gasanalyzercomponent)

#------------------------------------------------------------------------------------------
# Build the executable after the library
#------------------------------------------------------------------------------------------
ADD_SUBDIRECTORY(GasAnalyzer)

