#------------------------------------------------------------------------------------------
# Define a cache variable to hold the path to the libmachinecomponent.a library
#------------------------------------------------------------------------------------------
SET(CORE_MACHINE_TC_LIB_PATH ${CMAKE_CURRENT_BINARY_DIR}/Common CACHE PATH
	"Path to the core machine component library" FORCE)

#------------------------------------------------------------------------------------------
# Define a cache variable to hold the path to the machine component headers
#------------------------------------------------------------------------------------------
SET(MACHINE_INC_PATHS ${CMAKE_CURRENT_SOURCE_DIR}/Common ${CMAKE_CURRENT_SOURCE_DIR}/Include)
SET(CORE_MACHINE_TC_INC_PATH ${MACHINE_INC_PATHS} CACHE STRING
	"Path(s) to the core machine component include files" FORCE)

#------------------------------------------------------------------------------------------
# Where to find the common headers
#------------------------------------------------------------------------------------------
INCLUDE_DIRECTORIES(${CORE_MACHINE_TC_INC_PATH})

#------------------------------------------------------------------------------------------
# Build the library first
#------------------------------------------------------------------------------------------
ADD_SUBDIRECTORY(Common)

#------------------------------------------------------------------------------------------
# All components link against the common library
#------------------------------------------------------------------------------------------
LINK_DIRECTORIES(${CORE_MACHINE_TC_LIB_PATH})
LINK_LIBRARIES(machinecomponent m)

#------------------------------------------------------------------------------------------
# Build the executable after the library
#------------------------------------------------------------------------------------------
ADD_SUBDIRECTORY(Machine)

