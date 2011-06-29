#------------------------------------------------------------------------------------------
# Where to find the common headers
#------------------------------------------------------------------------------------------
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/Include
		    ${BRAKE_MODULE_PATHS}
					${CORE_BRAKE_TC_INC_PATH})

#------------------------------------------------------------------------------------------
# All components link against the common libraries
#------------------------------------------------------------------------------------------
LINK_DIRECTORIES(${CORE_BRAKE_TC_LIB_PATH})
LINK_LIBRARIES(basebrake m)

#------------------------------------------------------------------------------------------
# Build the executables after the library
#------------------------------------------------------------------------------------------
ADD_SUBDIRECTORY(GM)
