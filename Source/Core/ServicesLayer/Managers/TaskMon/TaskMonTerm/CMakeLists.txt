#------------------------------------------------------------------------------------------
# Create a list of all the cpp files in this directory needed to create the executable
#------------------------------------------------------------------------------------------
SET(TASKMONTERM_SOURCES
        TaskMonHeaderPanel.cpp
        TaskMonHelpWindow.cpp
        TaskMonPanel.cpp
        TaskMonStatusPanel.cpp
        TaskMonTaskPanel.cpp
        TaskMonTerm.cpp)

#------------------------------------------------------------------------------------------
# Define the basename of our executable (for us only...not required by CMake)
#------------------------------------------------------------------------------------------
SET(EXE_NAME TaskMonTerm)

#------------------------------------------------------------------------------------------
# Tell CMake what source file to build
#------------------------------------------------------------------------------------------
ADD_EXECUTABLE(${EXE_NAME} ${TASKMONTERM_SOURCES})

#------------------------------------------------------------------------------------------
# Tell CMake to link against the required curses libraries
#------------------------------------------------------------------------------------------
TARGET_LINK_LIBRARIES(${EXE_NAME} panel ncurses)

#------------------------------------------------------------------------------------------
# Add a QNX use message to the executable
#------------------------------------------------------------------------------------------
ADD_USE_MSSG(${EXE_NAME} ${CMAKE_CURRENT_SOURCE_DIR}/${EXE_NAME}.cpp)

#------------------------------------------------------------------------------------------
# Tell CMake where to install the executable
#------------------------------------------------------------------------------------------
INSTALL_TARGETS(${CORE_TOOLS_INSTALL_DIR} ${EXE_NAME})

#------------------------------------------------------------------------------------------
# Tell CMake where to install the scripts
#------------------------------------------------------------------------------------------
INSTALL_PROGRAMS(${CORE_TOOLS_INSTALL_DIR} TaskMon.new.sh)
