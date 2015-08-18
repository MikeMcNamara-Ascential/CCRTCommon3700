#------------------------------------------------------------------------------
# Define the source files needed to build the common component library
#------------------------------------------------------------------------------
FILE(GLOB_RECURSE COMPONENT_SRC *.cpp)

#------------------------------------------------------------------------------
# Define the name of executable we will create
#------------------------------------------------------------------------------
SET(EXE_NAME SoundLevelTc.${CMAKE_EXECUTABLE_SUFFIX})

#------------------------------------------------------------------------------
# Define the path to the component root for use in the TestComponent make files
#------------------------------------------------------------------------------
SET(CUSTOMER_SOUND_COMP_ROOT ${CMAKE_CURRENT_SOURCE_DIR})

#------------------------------------------------------------------------------
# Where to find the common headers
#------------------------------------------------------------------------------
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}
                    ${CORE_COMMON_TC_INC_PATH})

#------------------------------------------------------------------------------
# Generate a library
#------------------------------------------------------------------------------
STRING(TOLOWER SoundLevelTc TMP_LIB_NAME)
SET(LIB_NAME ${TMP_LIB_NAME}lib)
ADD_LIBRARY(${LIB_NAME} STATIC ${COMPONENT_SRC})

#------------------------------------------------------------------------------
# Define a cache variable to hold the brake test components include path
#------------------------------------------------------------------------------
SET(CUSTOMER_SOUND_LEVEL_TC_INC ${CMAKE_CURRENT_SOURCE_DIR})
SET(CUSTOMER_SOUND_LEVEL_TC_INC_PATH ${CUSTOMER_SOUND_LEVEL_TC_INC} CACHE STRING 
        "Path to the customer sound level test component include files" FORCE)
        
#------------------------------------------------------------------------------
# Tell CMake what subdirectories to recurse into
#------------------------------------------------------------------------------
ADD_SUBDIRECTORY(Configuration)

#------------------------------------------------------------------------------
# Tell CMake what source file to build
#------------------------------------------------------------------------------
ADD_EXECUTABLE(${EXE_NAME} ${COMPONENT_SRC})
TARGET_LINK_LIBRARIES(${EXE_NAME} commoncomponent)

#------------------------------------------------------------------------------
# Make sure that the services layer lib is built first
#------------------------------------------------------------------------------
INCLUDE(${CCRT_ROOT}/LibServices.cmake)

#------------------------------------------------------------------------------
# Tell CMake where to install the executable
#------------------------------------------------------------------------------
INSTALL_TARGETS(${APPLICATION_LAYER_INSTALL_DIR} ${EXE_NAME})
