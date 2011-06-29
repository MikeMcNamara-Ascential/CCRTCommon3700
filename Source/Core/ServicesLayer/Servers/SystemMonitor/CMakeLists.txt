#------------------------------------------------------------------------------------------
# Define a cache variable to hold the path to the libsysmon.a library
#------------------------------------------------------------------------------------------
SET(CORE_SYS_MON_LIB_DIR ${CMAKE_CURRENT_BINARY_DIR}/Library CACHE PATH
	"Path to the core sysmon library" FORCE)

#------------------------------------------------------------------------------------------
# Define a cache variable to hold the path to the SysMon headers
#------------------------------------------------------------------------------------------
SET(CORE_SYS_MON_INC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/Include CACHE PATH
	"Path to the core sysmon library" FORCE)

#------------------------------------------------------------------------------------------
# Add the SystemMonitor include directory to the list of include directories
#------------------------------------------------------------------------------------------
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/Include)

#------------------------------------------------------------------------------------------
# Make sure the linker can find libsystemmonitor.a once it is built.
#------------------------------------------------------------------------------------------
LINK_DIRECTORIES(${CMAKE_CURRENT_BINARY_DIR}/Library)

#------------------------------------------------------------------------------------------
# Define the basename of our executable (for us only...not required by CMake)
#------------------------------------------------------------------------------------------
SET(EXE_NAME ChinaSystemMonitor)

#------------------------------------------------------------------------------------------
# Tell CMake what subdirectories to recurse into
#------------------------------------------------------------------------------------------
ADD_SUBDIRECTORY(Library)

#------------------------------------------------------------------------------------------
# All SystemMonitor processes link against libsystemmonitor.a
#------------------------------------------------------------------------------------------
LINK_LIBRARIES(sysmon)

#------------------------------------------------------------------------------------------
# Tell CMake what source file to build
#------------------------------------------------------------------------------------------
CREATE_QXE(${EXE_NAME} ${EXE_NAME}Main.cpp)

#------------------------------------------------------------------------------------------
# Add a QNX use message to the executable
#------------------------------------------------------------------------------------------
ADD_USE_MSSG(${EXE_NAME} ${CMAKE_CURRENT_SOURCE_DIR}/${EXE_NAME}Main.cpp)

#------------------------------------------------------------------------------------------
# Tell CMake where to install the executable
#------------------------------------------------------------------------------------------
INSTALL_TARGETS(${SERVICES_LAYER_INSTALL_DIR} ${EXE_NAME})

ADD_SUBDIRECTORY(Configuration)

