#------------------------------------------------------------------------------------------
# Define the basename of our executable (for us only...not required by CMake)
#------------------------------------------------------------------------------------------
SET(EXE_NAME GMVehicleTest)

#------------------------------------------------------------------------------------------
# Add the ServicesLayer include directory to the list of include directories
#------------------------------------------------------------------------------------------
INCLUDE_DIRECTORIES(${CORE_VEHICLE_TEST_INC_PATH})

#------------------------------------------------------------------------------------------
# Make sure the linker can find libvehicletest.a once it is built.
#------------------------------------------------------------------------------------------
LINK_DIRECTORIES(${CORE_VEHICLE_TEST_LIB_PATH})
LINK_LIBRARIES(vehicletest)

#------------------------------------------------------------------------------------------
# This is a listing of all of the source files necessary to create
# a library for the current directory.
#------------------------------------------------------------------------------------------
STRING(TOLOWER ${EXE_NAME} TMP_LIB_NAME)
SET(LIB_NAME ${TMP_LIB_NAME}lib)
ADD_LIBRARY(${LIB_NAME} STATIC
        ${EXE_NAME}.cpp)

#------------------------------------------------------------------------------------------
# Tell CMake what source file to build
#------------------------------------------------------------------------------------------
CREATE_QXE(${EXE_NAME} ${EXE_NAME}Main.cpp)

TARGET_LINK_LIBRARIES(${EXE_NAME} ${LIB_NAME})

#------------------------------------------------------------------------------------------
# Tell CMake what source file to build
#------------------------------------------------------------------------------------------
#ADD_EXECUTABLE(${EXE_NAME}.${CMAKE_EXECUTABLE_SUFFIX} ${EXE_NAME}Main.cpp)

#TARGET_LINK_LIBRARIES(${EXE_NAME}.${CMAKE_EXECUTABLE_SUFFIX} ${LIB_NAME})

#------------------------------------------------------------------------------------------
# Make sure that the services layer lib is built first
#------------------------------------------------------------------------------------------
INCLUDE(${CCRT_ROOT}/LibServices.cmake)

#------------------------------------------------------------------------------------------
# Make sure the executable has the correct extension
#------------------------------------------------------------------------------------------
#INCLUDE(${CCRT_ROOT}/ExeQxe.cmake)

#------------------------------------------------------------------------------------------
# Add a QNX use message to the executable
#------------------------------------------------------------------------------------------
ADD_USE_MSSG(${EXE_NAME} ${CMAKE_CURRENT_SOURCE_DIR}/${EXE_NAME}Main.cpp)

#------------------------------------------------------------------------------------------
# Tell CMake where to install the executable
#------------------------------------------------------------------------------------------
INSTALL_TARGETS(${APPLICATION_LAYER_INSTALL_DIR} ${EXE_NAME})

#------------------------------------------------------------------------------------------
# Tell CMake what subdirectories to recurse into
#------------------------------------------------------------------------------------------
ADD_SUBDIRECTORY(Configuration)

