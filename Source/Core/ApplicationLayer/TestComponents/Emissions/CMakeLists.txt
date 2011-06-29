#------------------------------------------------------------------------------------------
# Define a cache variable to hold the path to the libemissioncomponent.a library
#------------------------------------------------------------------------------------------
SET(CORE_ECU_TC_LIB_PATH ${CMAKE_CURRENT_BINARY_DIR}/Common CACHE PATH
	"Path to the core emissions component library" FORCE)

#------------------------------------------------------------------------------------------
# Define a cache variable to hold the path to the common component headers
#------------------------------------------------------------------------------------------
SET(ECU_INC_PATHS ${CMAKE_CURRENT_SOURCE_DIR}/Common ${CMAKE_CURRENT_SOURCE_DIR}/Include)
SET(CORE_ECU_TC_INC_PATH ${ECU_INC_PATHS} CACHE STRING
	"Path(s) to the core emissions component include files" FORCE)

#------------------------------------------------------------------------------------------
# Where to find the common headers
#------------------------------------------------------------------------------------------
INCLUDE_DIRECTORIES(${CORE_ECU_TC_INC_PATH})

#------------------------------------------------------------------------------------------
# Tell CMake what subdirectories to recurse into
#------------------------------------------------------------------------------------------
ADD_SUBDIRECTORY(Common)

#------------------------------------------------------------------------------------------
# All components link against the common library
#------------------------------------------------------------------------------------------
LINK_DIRECTORIES(${CORE_ECU_TC_LIB_PATH})
LINK_LIBRARIES(emissioncomponent)

