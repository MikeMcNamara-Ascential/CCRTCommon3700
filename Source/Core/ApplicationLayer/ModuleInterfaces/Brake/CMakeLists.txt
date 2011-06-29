#------------------------------------------------------------------------------------------
# Define a cache variable to hold the path to the China common component headers and .cpp
#------------------------------------------------------------------------------------------
SET(COMMON_BRAKE_MODULE_PATHS
		${CMAKE_CURRENT_SOURCE_DIR}/Include
		${CMAKE_CURRENT_SOURCE_DIR}/Bosch8
		CACHE PATH "Path to the common brake module interface files" FORCE)


#------------------------------------------------------------------------------------------
# Tell CMake what subdirectories to recurse into
#------------------------------------------------------------------------------------------
ADD_SUBDIRECTORY(Bosch8)