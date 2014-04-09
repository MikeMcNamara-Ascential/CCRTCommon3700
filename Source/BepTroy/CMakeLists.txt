#------------------------------------------------------------------------------------------
# Define the root of the customer specfic source tree
#------------------------------------------------------------------------------------------
SET(CUSTOMER_ROOT ${CMAKE_CURRENT_SOURCE_DIR})

#------------------------------------------------------------------------------------------
# Add customer specific include directory
#------------------------------------------------------------------------------------------
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/ServicesLayer/Include)

#------------------------------------------------------------------------------------------
# Tell CMake what subdirectories to recurse into
#------------------------------------------------------------------------------------------
ADD_SUBDIRECTORY(ApplicationLayer)
ADD_SUBDIRECTORY(ServicesLayer)

#------------------------------------------------------------------------------------------
# Delete unnecessary core directories from the install dir
#------------------------------------------------------------------------------------------
#
# SET(CORE_DIRS_RM_LIST
#	Configuration/Components/Emissions
#	)
 
#------------------------------------------------------------------------------------------
# Delete unnecessary core files from the install dir
#------------------------------------------------------------------------------------------
SET(CORE_FILES_RM_LIST
	Configuration/Client/PLCDisplayConfig.xml
	Configuration/Components/LossCompensation/SampleLossCompensationConfig.xml
	Configuration/Components/Machine/SampleDriverNumberList.xml
	Configuration/Components/Machine/SampleMachineComponentConfig.xml
	Configuration/Servers/LauncherConfig.xml
	Configuration/Servers/PlantHostOutbound.xml
	Configuration/Servers/SampleInputServerConfig.xml
	Configuration/Servers/SerialPlcConfig.xml
	Configuration/Servers/TestResultServerConfig.xml
	Configuration/Servers/VehicleDataBrokerConfig.xml 
	Configuration/Servers/SystemScreenSaverConfig.xml    
	ServicesLayer/Bin/PlantHostInbound.qxe
	ServicesLayer/Bin/PlantHostOutbound.qxe
	ServicesLayer/Bin/AvtCanProxy.qxe
	ServicesLayer/Bin/AvtJ2190Proxy.qxe
	ServicesLayer/Bin/CLV295ScannerMgr.qxe
	ServicesLayer/Bin/DaqInterface.qxe
	ServicesLayer/Bin/IsoCommProxy.qxe
	ServicesLayer/Bin/MapleKeypadManager.qxe
	ServicesLayer/Bin/PendentManager.qxe
	ServicesLayer/Bin/BepServer.qxe
	ServicesLayer/Bin/HostInterface.qxe
	ServicesLayer/Bin/MultiComProxy.qxe
	ServicesLayer/Bin/PendantManager.qxe
	ServicesLayer/Bin/SciCommProxy.qxe 
	ServicesLayer/Bin/SciIsoMuxMgr.qxe
	ServicesLayer/Bin/SystemScreenSaver.qxe               
	)

#------------------------------------------------------------------------------------------
# Create a rule to install the system update files
#------------------------------------------------------------------------------------------
IF(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/SysUpdates.tgz)
	INSTALL_FILES(/ FILES SysUpdates.tgz)
ENDIF(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/SysUpdates.tgz)

#------------------------------------------------------------------------------------------
# Create a rule to install the NSIS installer script
#------------------------------------------------------------------------------------------
INCLUDE(NSISInstaller.cmake OPTIONAL)
