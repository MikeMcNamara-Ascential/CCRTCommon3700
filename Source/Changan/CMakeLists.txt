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
SET(CORE_DIRS_RM_LIST
	Configuration/Components/Emissions
	Configuration/Components/GasAnalyzer
	Configuration/Components/SpeedControl
	)
 
#------------------------------------------------------------------------------------------
# Delete unnecessary core files from the install dir
#------------------------------------------------------------------------------------------
SET(CORE_FILES_RM_LIST

	Configuration/Components/Brake/SampleAbsConfig.xml
	Configuration/Components/Brake/SampleBaseBrakeConfig.xml
	Configuration/Components/LossCompensation/SampleLossCompensationConfig.xml
	Configuration/Components/Machine/SampleDriverNumberList.xml
	Configuration/Components/Machine/SampleMachineComponentConfig.xml
	
	Configuration/Machine/ABMotorControllerConfig.xml
	Configuration/Machine/Baldor18hConfig.xml
	Configuration/Machine/Baldor18hParameters.xml
	Configuration/Machine/Baldor18hProtocol.xml
	Configuration/Machine/Baldor25mConfig.xml
	Configuration/Machine/Baldor25mParameters.xml
	Configuration/Machine/Baldor25mProtocol.xml
	Configuration/Machine/BaldorMotorControllerConfig.xml
	Configuration/Machine/LeftFrontBaldorDriveParameters-25M.xml
	Configuration/Machine/LeftRearBaldorDriveParameters-25M.xml
	Configuration/Machine/RightFrontBaldorDriveParameters-25M.xml
	Configuration/Machine/RightRearBaldorDriveParameters-25M.xml

	Configuration/Servers/ChinaSystemMonitorConfig.xml
	Configuration/Servers/DsaServerConfig.xml
	Configuration/Servers/HydraulicMotorControllerConfig.xml
	Configuration/Servers/IpIcmConfig.xml 
	Configuration/Servers/LauncherConfig.xml
	Configuration/Servers/PlantHostOutbound.xml
	Configuration/Servers/SampleInputServerConfig.xml
	Configuration/Servers/VehicleDataBrokerConfig.xml 
	Configuration/Servers/VehicleDataServerConfig.xml 
	Configuration/Servers/SystemScreenSaverConfig.xml 
	   
	ServicesLayer/Bin/AvtCanProxy.qxe
	ServicesLayer/Bin/AvtJ2190Proxy.qxe
	ServicesLayer/Bin/BaldorMotorController.qxe
	ServicesLayer/Bin/BepServer.qxe
	ServicesLayer/Bin/ChinaSystemMonitor.qxe
	ServicesLayer/Bin/CLV295ScannerMgr.qxe
	ServicesLayer/Bin/DaqInterface.qxe
	ServicesLayer/Bin/DsaServer.qxe
	ServicesLayer/Bin/GenericTextInterface.qxe
	ServicesLayer/Bin/HostInterface.qxe
	ServicesLayer/Bin/HydraulicMotorController.qxe
	ServicesLayer/Bin/MapleKeypadManager.qxe
	ServicesLayer/Bin/MultiComProxy.qxe
	ServicesLayer/Bin/PendantManager.qxe
	ServicesLayer/Bin/PlantHostOutbound.qxe
	ServicesLayer/Bin/SciCommProxy.qxe 
	ServicesLayer/Bin/SciIsoMuxMgr.qxe
	ServicesLayer/Bin/SystemScreenSaver.qxe
	ServicesLayer/Bin/TcpCommProxy.qxe
	ServicesLayer/Bin/VehicleDataServer.qxe

	ApplicationLayer/Bin/GasAnalyzerTC.qxe
	)

CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/UninstallCoreCfgFile.cmake 
                                ${CMAKE_CURRENT_BINARY_DIR}/UninstallCoreCfgFile
                                @ONLY)
ADD_CUSTOM_TARGET(RmCoreConfigFiles echo "Uninstalling unnecessary files")
SET_TARGET_PROPERTIES(RmCoreConfigFiles PROPERTIES PRE_INSTALL_SCRIPT 
                                ${CMAKE_CURRENT_BINARY_DIR}/UninstallCoreCfgFile)

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
