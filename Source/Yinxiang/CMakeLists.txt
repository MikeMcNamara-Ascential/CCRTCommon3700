SET(CORE_SOURCE_APP_DIR ${CMAKE_CURRENT_SOURCE_DIR}/ApplicationLayer)
SET(CUST_REL "151125R1" CACHE STRING "GM release version" FORCE)

#------------------------------------------------------------------------------------------
# Tell CMake what subdirectories to recurse into
#------------------------------------------------------------------------------------------
ADD_SUBDIRECTORY(ServicesLayer)
ADD_SUBDIRECTORY(ApplicationLayer)
