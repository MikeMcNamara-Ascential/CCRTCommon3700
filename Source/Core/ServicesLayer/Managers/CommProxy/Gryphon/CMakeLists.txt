#------------------------------------------------------------------------------------------
# Define the root of the Gryphon source tree
#------------------------------------------------------------------------------------------
SET(CORE_GRYPH_ROOT ${CMAKE_CURRENT_SOURCE_DIR})

#------------------------------------------------------------------------------------------
# Define a cache variable to hold the path to the Gryphon headers
#------------------------------------------------------------------------------------------
SET(CORE_GRYPHON_INC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/Include CACHE PATH
	"Path to the core Gryphon library" FORCE)

#------------------------------------------------------------------------------------------
# Add an include path for the DG supplied headers
#------------------------------------------------------------------------------------------
INCLUDE_DIRECTORIES(${CORE_GRYPH_ROOT} ${CORE_GRYPH_ROOT}/hw ${CORE_GRYPH_ROOT}/Include)

#------------------------------------------------------------------------------------------
# Build the library first
#------------------------------------------------------------------------------------------
ADD_SUBDIRECTORY(Common)

#------------------------------------------------------------------------------------------
# All AVT proxies link against the common library
#------------------------------------------------------------------------------------------
LINK_DIRECTORIES(${CMAKE_CURRENT_BINARY_DIR}/Common)
LINK_LIBRARIES(GryphCommon)

#------------------------------------------------------------------------------------------
# Build the executables after the library
#------------------------------------------------------------------------------------------
ADD_SUBDIRECTORY(CAN)
#ADD_SUBDIRECTORY(J1850)
