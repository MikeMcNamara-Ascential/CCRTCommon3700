#------------------------------------------------------------------------------------------
# Define a cache variable to hold the path to the liblosscompensationcomponent.a library
#------------------------------------------------------------------------------------------
SET(CORE_LC_TC_LIB_PATH ${CMAKE_CURRENT_BINARY_DIR}/Common CACHE PATH
	"Path to the core loss comp component library" FORCE)

#------------------------------------------------------------------------------------------
# Define a cache variable to hold the path to the loss comp component headers
#------------------------------------------------------------------------------------------
SET(LC_INC_PATHS ${CMAKE_CURRENT_SOURCE_DIR}/Common ${CMAKE_CURRENT_SOURCE_DIR}/Include)
SET(CORE_LC_TC_INC_PATH ${LC_INC_PATHS} CACHE STRING
	"Path(s) to the core loss comp component include files" FORCE)

#------------------------------------------------------------------------------------------
# Define the path to the component root for use in the TestComponent make files
#------------------------------------------------------------------------------------------
SET(CORE_LC_COMP_ROOT ${CMAKE_CURRENT_SOURCE_DIR})

#------------------------------------------------------------------------------------------
# Where to find the common headers
#------------------------------------------------------------------------------------------
INCLUDE_DIRECTORIES(${CORE_LC_TC_INC_PATH})

#------------------------------------------------------------------------------------------
# Build the library first
#------------------------------------------------------------------------------------------
ADD_SUBDIRECTORY(Common)
ADD_SUBDIRECTORY(PhotonLossCompensation)

#------------------------------------------------------------------------------------------
# All components link against the common library
#------------------------------------------------------------------------------------------
LINK_DIRECTORIES(${CORE_LC_TC_LIB_PATH})
LINK_LIBRARIES(losscompensationcomponent m)

#------------------------------------------------------------------------------------------
# Build the executable after the library
#------------------------------------------------------------------------------------------
ADD_SUBDIRECTORY(LossCompensation)

