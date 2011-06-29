SET(CORE_SOURCE_APP_DIR ${CMAKE_CURRENT_SOURCE_DIR}/ApplicationLayer)

#------------------------------------------------------------------------------------------
# Tell CMake what subdirectories to recurse into
#------------------------------------------------------------------------------------------
ADD_SUBDIRECTORY(ServicesLayer)
ADD_SUBDIRECTORY(ApplicationLayer)
ADD_SUBDIRECTORY(ClientConfigurations)

