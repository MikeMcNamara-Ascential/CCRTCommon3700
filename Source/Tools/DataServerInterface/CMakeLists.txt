#------------------------------------------------------------------------------------------
# Define the basename of our executable (for us only...not required by CMake)
#------------------------------------------------------------------------------------------
SET(EXE_NAME dsi)

#------------------------------------------------------------------------------------------
# Define the file where our QNX "use" message is located
#------------------------------------------------------------------------------------------
SET(USE_FILE_NAME ${CMAKE_CURRENT_SOURCE_DIR}/DataServerInterface.cpp)

#------------------------------------------------------------------------------------------
# Tell CMake what source file to build
#------------------------------------------------------------------------------------------
ADD_EXECUTABLE(dsi DataServerInterface.cpp)
TARGET_LINK_LIBRARIES(${EXE_NAME} ds)

#------------------------------------------------------------------------------------------
# Add a QNX use message to the executable
#------------------------------------------------------------------------------------------
#ADD_USE_MSSG(dsi ${CMAKE_CURRENT_SOURCE_DIR}/DataServerInterface.cpp)

#------------------------------------------------------------------------------------------
# Tell CMake where to install the executable
#------------------------------------------------------------------------------------------
INSTALL_TARGETS(${CORE_TOOLS_INSTALL_DIR} dsi)
