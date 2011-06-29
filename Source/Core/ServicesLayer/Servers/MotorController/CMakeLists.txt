#------------------------------------------------------------------------------------------
# Define the Manager directory
#------------------------------------------------------------------------------------------
SET(CORE_MOTOR_CTRL_DIR "${CMAKE_CURRENT_SOURCE_DIR}")

#------------------------------------------------------------------------------------------
# Add the MotorController include directory to the list of include directories
#------------------------------------------------------------------------------------------
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/Include)

ADD_SUBDIRECTORY(Common)

#------------------------------------------------------------------------------------------
# Make sure the linker can find libmotorcontroller.a once it is built.
#------------------------------------------------------------------------------------------
LINK_DIRECTORIES(${CMAKE_CURRENT_BINARY_DIR}/Common)

#------------------------------------------------------------------------------------------
# All Motor Controller processes link against libmotorcontroller.a
#------------------------------------------------------------------------------------------
LINK_LIBRARIES(motorcontroller m)

#------------------------------------------------------------------------------------------
# Tell CMake where to install the utility scripts
#------------------------------------------------------------------------------------------
#INSTALL_PROGRAMS(${APPLICATION_LAYER_INSTALL_DIR}
#       run
#       rune
#       register
#       speed
#		torque
#       end)

#------------------------------------------------------------------------------------------
# Tell CMake what subdirectories to recurse into
#------------------------------------------------------------------------------------------
ADD_SUBDIRECTORY(Hydraulic)
ADD_SUBDIRECTORY(Electric)

