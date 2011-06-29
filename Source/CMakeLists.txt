#------------------------------------------------------------------------------------------
# Define the name of our project
#------------------------------------------------------------------------------------------
SET(PROJ_NAME "CCRT")
SET(CORE_REL 11041101 CACHE STRING "Core Release" FORCE)

SET(CORE_SOURCE_APP_DIR ${CMAKE_CURRECT_SOURCE_DIR}/Core/ApplicationLayer)

#------------------------------------------------------------------------------------------
# Define the name of our project
#------------------------------------------------------------------------------------------
PROJECT(${PROJ_NAME})

#------------------------------------------------------------------------------------------
# Set defaults for release variables (if not on command line)
#------------------------------------------------------------------------------------------
INCLUDE(Defaults.cmake OPTIONAL)

#------------------------------------------------------------------------------------------
# Determine which customer we are building for (if not on command line)
#------------------------------------------------------------------------------------------
INCLUDE(Customer.cmake OPTIONAL)

#------------------------------------------------------------------------------------------
# Build the version/release string so it can be #defined in our source
#------------------------------------------------------------------------------------------
IF(CUSTOMER MATCHES "Core")
        SET(CUSTOMER_RELEASE_VER R${CORE_REL} CACHE STRING "Customer CCRT Release Version" FORCE)
        MESSAGE(STATUS "Core Build being Generated. Release Ccrt${CUSTOMER_RELEASE_VER}")
ELSE(CUSTOMER MATCHES "Core")
        # make an all upper temp string of the customer variable for use in the release version ID
        STRING(TOUPPER ${CUSTOMER} TEMP_OUT)
    SET(CUSTOMER_RELEASE_VER R${CORE_REL}-${TEMP_OUT}-${CUST_REL} CACHE STRING "Customer CCRT Release Version" FORCE)
    MESSAGE(STATUS "Customer Build for ${CUSTOMER} being Generated. Release Ccrt${CUSTOMER_RELEASE_VER}")
ENDIF(CUSTOMER MATCHES "Core")

#------------------------------------------------------------------------------------------
# Add #defines for the software version
#------------------------------------------------------------------------------------------
ADD_DEFINITIONS(-DCORE_VERSION_STRING="${CORE_REL}")
ADD_DEFINITIONS(-DCUST_VERSION_STRING="${CUST_REL}")
ADD_DEFINITIONS(-DCUSTOMER_STRING="${CUSTOMER}")
ADD_DEFINITIONS(-DVERSION_STRING="${CUSTOMER_RELEASE_VER}")

#------------------------------------------------------------------------------------------
# Make a more meaningful "root path" variable
#------------------------------------------------------------------------------------------
SET(CCRT_ROOT ${CMAKE_CURRENT_SOURCE_DIR} CACHE STRING "Project Root Directory" FORCE)
SET(CCRT_ROOT_BIN ${CMAKE_CURRENT_BINARY_DIR} CACHE STRING "Project Root Binary Directory" FORCE)

#------------------------------------------------------------------------------------------
# Enable or disable installing the support libraries
#------------------------------------------------------------------------------------------
OPTION(INSTALL_LIBS "Install all libraries required for building executables for distribution" OFF)

#------------------------------------------------------------------------------------------
# Tell CMake where to put files when we do a make install (relative)
#------------------------------------------------------------------------------------------
SET(REL_INSTALL_PREFIX ${CUSTOMER}/Deploy CACHE INTERNAL
    "Install path prefix, relative to the project root directory."
    FORCE)
#------------------------------------------------------------------------------------------
# Tell CMake where to put files when we do a make install (absolute)
#------------------------------------------------------------------------------------------
SET(CMAKE_INSTALL_PREFIX ${CMAKE_CURRENT_BINARY_DIR}/${REL_INSTALL_PREFIX} CACHE INTERNAL
    "Install path prefix, prepended onto install directories."
    FORCE)

#------------------------------------------------------------------------------------------
# Make sure our cache entries are correct
#------------------------------------------------------------------------------------------
MESSAGE(STATUS "Project '${PROJ_NAME}' will be installed to ${CMAKE_INSTALL_PREFIX}")

#------------------------------------------------------------------------------------------
# Use qxe for executable extension
#------------------------------------------------------------------------------------------
SET(CMAKE_EXECUTABLE_SUFFIX "qxe" CACHE INTERNAL "Executable suffix." FORCE)

#------------------------------------------------------------------------------------------
# Tell CMake's install program to use 'cp -L'. This will force cp to "follow links" (i.e.
# copy what a link points to, not the link itself). We do this because CMake produces
# File.exe files by default on windows (not File.qxe). We create a link named File.qxe that
# points to File.exe. When we 'make install', cp -L will copy the real executable image to
# the install directory as File.qxe
#------------------------------------------------------------------------------------------
SET(CPPROG "$ENV{QNX_TARGET}/usr/bin/cp -L")

#------------------------------------------------------------------------------------------
# Global include path
#------------------------------------------------------------------------------------------
INCLUDE_DIRECTORIES( BEFORE "$ENV{QNX_TARGET}/usr/include")

#------------------------------------------------------------------------------------------
# Set some CMake environment variables
#------------------------------------------------------------------------------------------
SET(GLOBAL_INCLUDES "-I$ENV{QNX_TARGET}/usr/include/cpp -I$ENV{QNX_TARGET}/usr/include")

#------------------------------------------------------------------------------------------
# Define a macros we can use to add use messages. 'targetName' is the name of the file to
#       add the use message to. 'useFileName' is the name of the source file containing the use
#       message
#------------------------------------------------------------------------------------------
MACRO(ADD_USE_MSSG targetName useFileName)
        GET_TARGET_PROPERTY(QXE_PATH ${targetName} LOCATION)
        ADD_CUSTOM_COMMAND(TARGET ${targetName}
                        POST_BUILD
                        COMMAND $ENV{QNX_HOST}/usr/bin/usemsg
                        ARGS -c ${QXE_PATH} ${useFileName}
                        COMMENT Add a usage message to an executable)
ENDMACRO(ADD_USE_MSSG)

#------------------------------------------------------------------------------------------
# Define a macros we can use to properly create a qxe for QNX. We need to add
#       the ".qxe" extension to the traget name.
#------------------------------------------------------------------------------------------
MACRO(CREATE_QXE targetName fileList)

        ADD_EXECUTABLE(${targetName} ${fileList} ${ARGN})
        # Get the extension of the given file
        GET_FILENAME_COMPONENT(targetSuffix ${targetName} EXT)
        # If no extension exists for the target
        IF(NOT targetSuffix)
                # Add the .qxe extension to the traget
                SET_TARGET_PROPERTIES(${targetName} PROPERTIES SUFFIX ".qxe")
                #MESSAGE(STATUS "Adding qxe suffix to ${targetName}")
        ELSE(NOT targetSuffix)
                MESSAGE(STATUS "Target ${targetName} has existing suffix of ${targetSuffix}")
        ENDIF(NOT targetSuffix)
        
ENDMACRO(CREATE_QXE)

#------------------------------------------------------------------------------------------
# Setup the install paths for executables
#------------------------------------------------------------------------------------------
SET(SERVICES_LAYER_INSTALL_DIR          "/ServicesLayer/Bin")
SET(APPLICATION_LAYER_INSTALL_DIR       "/ApplicationLayer/Bin")

#------------------------------------------------------------------------------------------
# Set up the tools
#------------------------------------------------------------------------------------------
SET(CORE_TOOLS_INSTALL_DIR                      "/Tools")

#------------------------------------------------------------------------------------------
# Set up the installation paths for config files
#------------------------------------------------------------------------------------------
SET(COMPONENT_CONFIG_DIR                        "/Configuration/Components")
SET(VEHICLETEST_CONFIG_DIR                      "/Configuration/VehicleTest")
SET(MACHINE_CONFIG_DIR                          "/Configuration/Machine")
SET(CLIENT_CONFIG_DIR                           "/Configuration/Client")
SET(SERVERS_CONFIG_DIR                          "/Configuration/Servers")

#------------------------------------------------------------------------------------------
# Where are the CPPUnit headers and libraries found
#------------------------------------------------------------------------------------------
SET(CPPUNIT_INCLUDES                "/usr/local/include")

#------------------------------------------------------------------------------------------
# Where to install our libraries (relative to the deploy directory)
#------------------------------------------------------------------------------------------
SET(LIB_DEPLOY_DIR                                      "/Libs")

#------------------------------------------------------------------------------------------
# Add the ServicesLayer include directory to the list of include directories
#------------------------------------------------------------------------------------------
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/Core/ServicesLayer/Include)

#------------------------------------------------------------------------------------------
# Make sure the linker can find the ServicesLayerlibrary once it is built.
#------------------------------------------------------------------------------------------
LINK_DIRECTORIES(${CMAKE_CURRENT_BINARY_DIR}/Core/ServicesLayer/Library)

#------------------------------------------------------------------------------------------
# Everybody links against the ServicesLayer library and socket library
#------------------------------------------------------------------------------------------
LINK_LIBRARIES(serviceslayer socket ds c)

#------------------------------------------------------------------------------------------
# Path to the root of the install directory
#------------------------------------------------------------------------------------------
SET(ROOT_DEPLOY_DIR /)

#------------------------------------------------------------------------------------------
# We will ALWAYS build the core
#------------------------------------------------------------------------------------------
ADD_SUBDIRECTORY(Core)

#------------------------------------------------------------------------------------------
# Determine whether core or customer build, and setup the release version
#------------------------------------------------------------------------------------------
MESSAGE(STATUS "Customer is ----->${CUSTOMER}<-----")

IF(CUSTOMER MATCHES "Core")
        SET(CUSTOMER_RELEASE_VER R${CORE_REL} CACHE STRING "Customer CCRT Release Version" FORCE)
        MESSAGE(STATUS "Core Build being Generated. Release Ccrt${CUSTOMER_RELEASE_VER}")
ELSE(CUSTOMER MATCHES "Core")
    IF(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/${CUSTOMER})
        # make an all upper temp string of the customer variable for use in the release version ID
        STRING(TOUPPER ${CUSTOMER} TEMP_OUT)
            SET(CUSTOMER_RELEASE_VER R${CORE_REL}-${TEMP_OUT}-${CUST_REL} CACHE STRING "Customer CCRT Release Version" FORCE)
            MESSAGE(STATUS "Customer directory for ${CUSTOMER} exists")
                ADD_SUBDIRECTORY(${CUSTOMER})
    ELSE(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/${CUSTOMER})
        MESSAGE(FATAL_ERROR "Unable to build for ${CUSTOMER}, ${CMAKE_CURRENT_SOURCE_DIR}/${CUSTOMER} does not exist")
        SET(CUSTOMER_RELEASE_VER "BuildError" CACHE STRING "Customer CCRT Release Version" FORCE)
    ENDIF(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/${CUSTOMER})
ENDIF(CUSTOMER MATCHES "Core")

#------------------------------------------------------------------------------------------
# Determine if Tests and/or Tools are expected and Dirs exist
#------------------------------------------------------------------------------------------
IF(TOOLS)
    IF(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/Tools)
        MESSAGE(STATUS "Building Tools...")
                ADD_SUBDIRECTORY(Tools)
    ELSE(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/Tools)
        MESSAGE(STATUS "Unable to build Tools, ${CMAKE_CURRENT_SOURCE_DIR}/Tools does not exist")
    ENDIF(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/Tools)
ELSE(TOOLS)
    MESSAGE(STATUS "NOT building Tools")
ENDIF(TOOLS)

IF(TESTS)
    IF(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/Tests)
        MESSAGE(STATUS "Building Tests...")
                ADD_SUBDIRECTORY(Tests)
    ELSE(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/Tests)
        MESSAGE(STATUS "Unable to build Tests, ${CMAKE_CURRENT_SOURCE_DIR}/Tests does not exist")
    ENDIF(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/Tests)
ELSE(TESTS)
    MESSAGE(STATUS "NOT building Tests")
ENDIF(TESTS)

#------------------------------------------------------------------------------------------
# Create a 'maintainer-clean' target to really clean up the directory
#------------------------------------------------------------------------------------------
INCLUDE(MaintainerClean.cmake)

#------------------------------------------------------------------------------------------
# Create a 'runtime' target to make all, install and then create a CCRT dist package
#------------------------------------------------------------------------------------------
INCLUDE(RuntimeDist.cmake)

#------------------------------------------------------------------------------------------
# Create a target to make a backup of the current source tree
#------------------------------------------------------------------------------------------
ADD_CUSTOM_TARGET( backup @echo Creating backup for ${CUSTOMER_RELEASE_VER})
ADD_CUSTOM_COMMAND(TARGET backup
                    COMMAND tar
                                        ARGS -zcvf ${CMAKE_CURRENT_SOURCE_DIR}/../CcrtSrc.${CUSTOMER_RELEASE_VER}.tgz
                                        -C ${CMAKE_CURRENT_SOURCE_DIR}/..
                                        Source)

#------------------------------------------------------------------------------------------
# Create a target to convert the install scripts to QNX format
#       (MUST come BEFORE the CONFIGURE_FILE commands below)
#------------------------------------------------------------------------------------------
INCLUDE(QNXFormat.cmake OPTIONAL)

#------------------------------------------------------------------------------------------
# Create the default installation files
#------------------------------------------------------------------------------------------
CONFIGURE_FILE("${CMAKE_CURRENT_SOURCE_DIR}/InstallDir.cmake" "${CMAKE_CURRENT_BINARY_DIR}/InstallDir")
CONFIGURE_FILE("${CMAKE_CURRENT_SOURCE_DIR}/preInstall.sh.cmake" "${CMAKE_CURRENT_BINARY_DIR}/preInstall.sh")
CONFIGURE_FILE("${CMAKE_CURRENT_SOURCE_DIR}/postInstall.sh.cmake" "${CMAKE_CURRENT_BINARY_DIR}/postInstall.sh")

#------------------------------------------------------------------------------------------
# Make sure the InstallDir is in QNX format or else the updateCCRT script will not process
#       the path inside it properly (cross compiler change)
#------------------------------------------------------------------------------------------
IF(CYGWIN)
        EXEC_PROGRAM(chmod ARGS a-x ${CMAKE_CURRENT_BINARY_DIR}/InstallDir OUTPUT_VARIABLE IGNORE_ME)
        EXEC_PROGRAM(chmod ARGS a+w ${CMAKE_CURRENT_BINARY_DIR}/InstallDir OUTPUT_VARIABLE IGNORE_ME)
        EXEC_PROGRAM(textto ARGS -l ${CMAKE_CURRENT_BINARY_DIR}/InstallDir OUTPUT_VARIABLE IGNORE_ME)
ENDIF(CYGWIN)

#------------------------------------------------------------------------------------------
# The InstallDir file and installation scripts need to be installed in the root deploy dir
#------------------------------------------------------------------------------------------
INSTALL_FILES(/ FILES InstallDir)
INSTALL_PROGRAMS(/ preInstall.sh postInstall.sh)
