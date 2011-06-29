#------------------------------------------------------------------------------------------
# Define a cache variable to hold the path to the Brake component headers and .cpp
#------------------------------------------------------------------------------------------
SET(CUSTOMER_BRAKE_TC_PATHS
                ${CMAKE_CURRENT_SOURCE_DIR}/BaseBrake
                ${CMAKE_CURRENT_SOURCE_DIR}/Teves)
                
#------------------------------------------------------------------------------------------
# Where to find the common headers
#------------------------------------------------------------------------------------------
INCLUDE_DIRECTORIES( 
                ${CUSTOMER_BRAKE_TC_PATHS}
                ${BRAKE_MODULE_PATHS}
                ${CORE_BRAKE_TC_INC_PATH})

#------------------------------------------------------------------------------------------
# All components link against the common library
#------------------------------------------------------------------------------------------
LINK_DIRECTORIES(${CORE_BRAKE_TC_LIB_PATH})
LINK_LIBRARIES(basebrake)

#------------------------------------------------------------------------------------------
# Tell CMake what subdirectories to recurse into
#------------------------------------------------------------------------------------------
ADD_SUBDIRECTORY(BaseBrake)
ADD_SUBDIRECTORY(Teves)
ADD_SUBDIRECTORY(TRW)
