#------------------------------------------------------------------------------------------
# Where to find the common headers
#------------------------------------------------------------------------------------------
INCLUDE_DIRECTORIES(    ${BRAKE_MODULE_PATHS}
                                ${CORE_BRAKE_TC_INC_PATH}
                                ${COMMON_BRAKE_MODULE_PATHS}
                                ${BOSCH8_MODULE_INC_PATH})

#------------------------------------------------------------------------------------------
# All components link against the common libraries
#------------------------------------------------------------------------------------------
LINK_DIRECTORIES(${CORE_BRAKE_TC_LIB_PATH})
LINK_LIBRARIES(basebrake m) 

#------------------------------------------------------------------------------------------
# Tell CMake what subdirectories to recurse into
#------------------------------------------------------------------------------------------
ADD_SUBDIRECTORY(BaseBrake)
ADD_SUBDIRECTORY(RWAL)
ADD_SUBDIRECTORY(BIAbs)
