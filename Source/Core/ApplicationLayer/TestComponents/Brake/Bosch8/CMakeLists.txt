#------------------------------------------------------------------------------------------
# Define the name of executable we will create
#------------------------------------------------------------------------------------------
SET(EXE_NAME Bosch8TC)

#------------------------------------------------------------------------------------------
# Define the default file where our QNX use message is located
#------------------------------------------------------------------------------------------
SET(USE_FILE_NAME ${CMAKE_CURRENT_SOURCE_DIR}/${EXE_NAME}Main.cpp)


#------------------------------------------------------------------------------------------
# Define a cache variable to hold the path to the Bosch8 component headers
#------------------------------------------------------------------------------------------
SET(BOSCH8_TC_PATH ${CMAKE_CURRENT_SOURCE_DIR})

#------------------------------------------------------------------------------------------
# Tell the compiler where to find the module interface files
#------------------------------------------------------------------------------------------
INCLUDE_DIRECTORIES(${CORE_MODULES_DIR/Brake/Bosch8}
		    ${CORE_MODULES_DIR/Brake/Include}
                    ${CMAKE_CURRENT_SOURCE_DIR})

#------------------------------------------------------------------------------------------
# Tell CMake what source file to build
#------------------------------------------------------------------------------------------
CREATE_QXE(${EXE_NAME} ${EXE_NAME}Main.cpp)

#------------------------------------------------------------------------------------------
# Tell CMake where to install the executable
#------------------------------------------------------------------------------------------
INSTALL_TARGETS(${APPLICATION_LAYER_INSTALL_DIR} ${EXE_NAME})


