#------------------------------------------------------------------------------------------
# Define a cache variable to hold the path to the libbasebrake.a library
#------------------------------------------------------------------------------------------
SET(CORE_BRAKE_TC_LIB_PATH ${CMAKE_CURRENT_BINARY_DIR}/Common CACHE PATH
	"Path to the core base brake component library" FORCE)

#------------------------------------------------------------------------------------------
# Define a cache variable to hold the path to the common component headers
#------------------------------------------------------------------------------------------
SET(BRAKE_INC_PATHS ${CMAKE_CURRENT_SOURCE_DIR}/Common ${CMAKE_CURRENT_SOURCE_DIR}/Include)
SET(CORE_BRAKE_TC_INC_PATH ${BRAKE_INC_PATHS} CACHE STRING
	"Path(s) to the core base brake component include files" FORCE)

#------------------------------------------------------------------------------------------
# Where to find the common headers
#------------------------------------------------------------------------------------------
INCLUDE_DIRECTORIES(${COMMON_BRAKE_MODULE_PATHS}
				${CORE_BRAKE_TC_INC_PATH})

#------------------------------------------------------------------------------------------
# Build the library first
#------------------------------------------------------------------------------------------
ADD_SUBDIRECTORY(Common)

#------------------------------------------------------------------------------------------
# All components link against the common library
#------------------------------------------------------------------------------------------
LINK_DIRECTORIES(${CORE_BRAKE_TC_LIB_PATH})
LINK_LIBRARIES(basebrake m)

#------------------------------------------------------------------------------------------
# Build executables last
#------------------------------------------------------------------------------------------
ADD_SUBDIRECTORY(BaseBrake)
ADD_SUBDIRECTORY(Bosch8)

