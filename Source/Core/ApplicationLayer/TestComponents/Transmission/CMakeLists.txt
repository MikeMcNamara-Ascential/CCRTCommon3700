#------------------------------------------------------------------------------------------
# Define a cache variable to hold the path to the libtranscomponent.a library
#------------------------------------------------------------------------------------------
SET(CORE_TRANS_TC_LIB_PATH ${CMAKE_CURRENT_BINARY_DIR}/Common CACHE PATH
	"Path to the core transmission component library" FORCE)

#------------------------------------------------------------------------------------------
# Define a cache variable to hold the path to the transmission component headers
#------------------------------------------------------------------------------------------
SET(TRANS_INC_PATHS ${CMAKE_CURRENT_SOURCE_DIR}/Common ${CMAKE_CURRENT_SOURCE_DIR}/Include)
SET(CORE_TRANS_TC_INC_PATH ${TRANS_INC_PATHS} CACHE STRING
	"Path(s) to the core transmission component include files" FORCE)

#------------------------------------------------------------------------------------------
# Where to find the common headers
#------------------------------------------------------------------------------------------
INCLUDE_DIRECTORIES(${CORE_TRANS_TC_INC_PATH})

#------------------------------------------------------------------------------------------
# Build the library first
#------------------------------------------------------------------------------------------
ADD_SUBDIRECTORY(Common)

#------------------------------------------------------------------------------------------
# All components link against the common library
#------------------------------------------------------------------------------------------
LINK_DIRECTORIES(${CORE_TRANS_TC_LIB_PATH})
LINK_LIBRARIES(transcomponent)
