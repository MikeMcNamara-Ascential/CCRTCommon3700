#******************************************************************************************
#------------------------------------------------------------------------------------------
# Define the name of executable we will create
#------------------------------------------------------------------------------------------
SET(EXE_NAME Bosch9TC)

#------------------------------------------------------------------------------------------
# Define the default file where our QNX use message is located
#------------------------------------------------------------------------------------------
SET(USE_FILE_NAME ${CMAKE_CURRENT_SOURCE_DIR}/${EXE_NAME}Main.cpp)

#------------------------------------------------------------------------------------------
# Tell the compiler where to find the module interface files
#------------------------------------------------------------------------------------------
INCLUDE_DIRECTORIES(${COMMON_BRAKE_MODULE_PATHS}
                    ${BRAKE_MODULE_PATHS}
                    ${CORE_BRAKE_TC_INC_PATH}
                    ${BOSCH8_TC_PATH}
                    ${BOSCH8_MODULE_INC_PATH}
                    ${BOSCH9_MODULE_INC_PATH}
                    ${CMAKE_CURRENT_SOURCE_DIR})

#------------------------------------------------------------------------------------------
# Tell CMake what source file to build
#------------------------------------------------------------------------------------------
CREATE_QXE(${EXE_NAME} ${EXE_NAME}Main.cpp)

#------------------------------------------------------------------------------------------
# Tell CMake where to install the executable
#------------------------------------------------------------------------------------------
INSTALL_TARGETS(${APPLICATION_LAYER_INSTALL_DIR} ${EXE_NAME})

#------------------------------------------------------------------------------------------
# Tell CMake what subdirectories to recurse into
#------------------------------------------------------------------------------------------
ADD_SUBDIRECTORY(Configuration)
