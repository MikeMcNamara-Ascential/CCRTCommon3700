#------------------------------------------------------------------------------------------
# Define a cache variable to hold the path to the Brake component headers and .cpp
#------------------------------------------------------------------------------------------
SET(CHANGAN_BRAKE_TC_PATHS
                ${CMAKE_CURRENT_SOURCE_DIR}/Common
                ${CMAKE_CURRENT_SOURCE_DIR}/BaseBrake
                ${CMAKE_CURRENT_SOURCE_DIR}/Teves
                ${CMAKE_CURRENT_SOURCE_DIR}/TRW
                )

#------------------------------------------------------------------------------------------
# Where to find the common headers
#------------------------------------------------------------------------------------------
INCLUDE_DIRECTORIES( 
                ${CHANGAN_BRAKE_TC_PATHS}
                ${BRAKE_MODULE_PATHS}
                ${COMMON_BRAKE_MODULE_PATHS}
                ${CORE_BRAKE_TC_INC_PATH})

#------------------------------------------------------------------------------------------
# All components link against the common library
#------------------------------------------------------------------------------------------
LINK_DIRECTORIES(${CORE_BRAKE_TC_LIB_PATH})
LINK_LIBRARIES(basebrake m)

#------------------------------------------------------------------------------------------
# Tell CMake what subdirectories to recurse into
#------------------------------------------------------------------------------------------
ADD_SUBDIRECTORY(BaseBrake)
#ADD_SUBDIRECTORY(Common)
ADD_SUBDIRECTORY(Teves)
ADD_SUBDIRECTORY(TRW)
ADD_SUBDIRECTORY(Bosch)
