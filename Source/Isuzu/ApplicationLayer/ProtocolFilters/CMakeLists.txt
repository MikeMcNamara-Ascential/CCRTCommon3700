#------------------------------------------------------------------------------------------
# Define a cache variable to hold the path to the libfordprotocolfilters.a library
#------------------------------------------------------------------------------------------
SET(CUSTOMER_PROTOCOLS_DIR ${CMAKE_CURRENT_BINARY_DIR} CACHE PATH
	"Path to the root of the customer protocol filter tree" FORCE)

#------------------------------------------------------------------------------------------
# Define a cache variable to hold the path to the Ford protocol interface headers
#------------------------------------------------------------------------------------------
SET(CUSTOMER_PROTOCOLS_INC_PATH ${CMAKE_CURRENT_SOURCE_DIR}/Include CACHE STRING 
	"Path to the Ford module interfaces include files" FORCE)

#------------------------------------------------------------------------------------------
# Where are our headers
#------------------------------------------------------------------------------------------
INCLUDE_DIRECTORIES(${CORE_PROTOCOLS_INC_PATH} ${CUSTOMER_PROTOCOLS_INC_PATH})

#------------------------------------------------------------------------------------------
# Where is the core protocol filter library located
#------------------------------------------------------------------------------------------
LINK_DIRECTORIES(${CORE_PROTOCOLS_LIB_PATH})

#------------------------------------------------------------------------------------------
# This is a listing of all of the source files necessary to create
# a library for the current directory.
#------------------------------------------------------------------------------------------
ADD_LIBRARY(fordprotocolfilters STATIC ${CMAKE_CURRENT_SOURCE_DIR}/CAN/FordCanProtocolFilter.cpp)

#------------------------------------------------------------------------------------------
# Link the common protocol filters library into this one
#------------------------------------------------------------------------------------------
TARGET_LINK_LIBRARIES(fordprotocolfilters protocolfilters)

ADD_SUBDIRECTORY(Isuzu)
