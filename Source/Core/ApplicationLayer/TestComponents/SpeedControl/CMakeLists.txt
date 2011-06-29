#------------------------------------------------------------------------------------------
# Define a cache variable to hold the path to the libspdcontrolcomponent.a library
#------------------------------------------------------------------------------------------
SET(CORE_SPEEDCTRL_TC_LIB_PATH ${CMAKE_CURRENT_BINARY_DIR}/Common CACHE PATH
	"Path to the core speed control component library" FORCE)

#------------------------------------------------------------------------------------------
# Define a cache variable to hold the path to the speed control component headers
#------------------------------------------------------------------------------------------
SET(SPEEDCTRL_INC_PATHS ${CMAKE_CURRENT_SOURCE_DIR}/Common ${CMAKE_CURRENT_SOURCE_DIR}/Include)
SET(CORE_SPEEDCTRL_TC_INC_PATH ${SPEEDCTRL_INC_PATHS} CACHE STRING
	"Path(s) to the core speed control component include files" FORCE)

#------------------------------------------------------------------------------------------
# Where to find the common headers
#------------------------------------------------------------------------------------------
INCLUDE_DIRECTORIES(${CORE_SPEEDCTRL_TC_INC_PATH})

#------------------------------------------------------------------------------------------
# Build the library first
#------------------------------------------------------------------------------------------
ADD_SUBDIRECTORY(Common)

#------------------------------------------------------------------------------------------
# All components link against the common library
#------------------------------------------------------------------------------------------
LINK_DIRECTORIES(${CORE_SPEEDCTRL_TC_LIB_PATH})
LINK_LIBRARIES(spdcontrolcomponent m)

ADD_SUBDIRECTORY(GenericSpeedControl)
