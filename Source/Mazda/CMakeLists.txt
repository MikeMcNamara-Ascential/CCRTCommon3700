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
	Configuration/Components/SpeedControl
	Configuration/Components/TransferCase
	)
 
#------------------------------------------------------------------------------------------
# Delete unnecessary core files from the install dir
#------------------------------------------------------------------------------------------
SET(CORE_FILES_RM_LIST
	Configuration/Client/PLCDisplayConfig.xml

	Configuration/Components/Brake/SampleAbsConfig.xml
	Configuration/Components/Brake/SampleBaseBrakeConfig.xml
	Configuration/Components/Emissions/SampleEmissionsConfig.xml
	Configuration/Components/GasAnalyzer/GasAnalyzerTC.xml
	Configuration/Components/GasAnalyzer/MexaAnalyzer.xml
	Configuration/Components/LossCompensation/SampleLossCompensationConfig.xml
	Configuration/Components/Machine/SampleDriverNumberList.xml
	Configuration/Components/Machine/SampleMachineComponentConfig.xml
	Configuration/Components/SpeedControl/SampleSpeedControlConfig.xml
	Configuration/Components/TransferCase/SampleTransferCaseConfig.xml
	Configuration/Components/SampleFaultList.xml
	Configuration/Machine/*Baldor*.xml
	Configuration/Machine/ABMotorControllerConfig.xml

	Configuration/Servers/ChinaSystemMonitorConfig.xml
	Configuration/Servers/DsaServerConfig.xml
	Configuration/Servers/HydraulicMotorControllerConfig.xml
	Configuration/Servers/IpIcmConfig.xml
	Configuration/Servers/LauncherConfig.xml
	Configuration/Servers/PlantHostOutbound.xml
	Configuration/Servers/SampleInputServerConfig.xml
	Configuration/Servers/SystemScreenSaverConfig.xml  
	Configuration/Servers/VehicleDataBrokerConfig.xml
	Configuration/Servers/VehicleDataServerConfig.xml

	ServicesLayer/Bin/AvtCanProxy.qxe
	ServicesLayer/Bin/AvtJ2190Proxy.qxe
	ServicesLayer/Bin/BaldorMotorController.qxe
	ServicesLayer/Bin/BepServer.qxe
	ServicesLayer/Bin/ChinaSystemMonitor.qxe
	ServicesLayer/Bin/CLV295ScannerMgr.qxe
	ServicesLayer/Bin/DaqInterface.qxe
	ServicesLayer/Bin/DsaServer.qxe
	ServicesLayer/Bin/GenericTextInterface.qxe
	ServicesLayer/Bin/GryphonCANCommProxy.qxe
	ServicesLayer/Bin/HostInterface.qxe
	ServicesLayer/Bin/HydraulicMotorController.qxe
	ServicesLayer/Bin/IsoCommProxy.qxe
	ServicesLayer/Bin/MachineDataBroker.qxe
	ServicesLayer/Bin/MapleKeypadManager.qxe
	ServicesLayer/Bin/MultiComProxy.qxe
	ServicesLayer/Bin/PendantManager.qxe
	ServicesLayer/Bin/PlantHostOutbound.qxe
	ServicesLayer/Bin/SciCommProxy.qxe 
	ServicesLayer/Bin/SciIsoMuxMgr.qxe
	ServicesLayer/Bin/SystemScreenSaver.qxe               
	ServicesLayer/Bin/VehicleDataServer.qxe            

	ApplicationLayer/Bin/GasAnalyzerTC.qxe
	ApplicationLayer/Bin/GenericBaseBrakeTest.qxe
	ApplicationLayer/Bin/GenericSpeedControl.qxe
	ApplicationLayer/Bin/GenericTcaseTest.qxe
	ApplicationLayer/Bin/LossCompensationTC.qxe
	ApplicationLayer/Bin/MachineTC.qxe
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
