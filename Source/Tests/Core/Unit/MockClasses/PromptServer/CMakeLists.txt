#------------------------------------------------------------------------------------------
# Tell CMake where to find the necessary .h files
#------------------------------------------------------------------------------------------
INCLUDE_DIRECTORIES(${CORE_SERVICES_LAYER_DIR}/Servers/PromptServer)

#------------------------------------------------------------------------------------------
# Tell CMake what is needed for the library
#------------------------------------------------------------------------------------------
ADD_LIBRARY(mockpromptserverlib STATIC MockPromptServer.cpp)

