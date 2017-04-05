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
	
	Configuration/Components/LossCompensation/SampleLossCompensationConfig.xml
	Configuration/Components/Machine/SampleDriverNumberList.xml
	Configuration/Components/Machine/SampleMachineComponentConfig.xml
	Configuration/Components/Brake/BaseBrake.xml
	Configuration/Components/Brake/Sample*.xml
	Configuration/Components/Emissions/Sample*
	Configuration/Components/GasAnalyzer/*
	Configuration/Components/SpeedControl/*
	Configuration/Components/TransferCase/*
	Configuration/Machine/*Baldor*
	Configuration/Machine/Hydraulic*
	Configuration/Machine/AB700MotorControllerConfig.xml
	Configuration/Machine/ABMotorControllerConfig.xml
	
	Configuration/Servers/LauncherConfig.xml
	Configuration/Servers/PlantHostOutbound.xml
	Configuration/Servers/SampleInputServerConfig.xml
	Configuration/Servers/SerialPlcConfig.xml
	Configuration/Servers/TestResultServerConfig.xml
	Configuration/Servers/VehicleDataBrokerConfig.xml 
	Configuration/Servers/SystemScreenSaverConfig.xml  
	Configuration/Servers/ChinaSystemMonitorConfig.xml
	Configuration/Servers/DsaServerConfig.xml
	Configuration/Servers/InterCcrtCommServerConfig.xml
	Configuration/Servers/NdbHostInterfaceMap.xml
	Configuration/Servers/VehicleBuildTable.xml
	Configuration/Servers/VehicleDataServerConfig.xml
	Configuration/Servers/WinCcrtInterfaceConfig.xml
	Configuration/Servers/IpIcmConfig.xml
	Configuration/Servers/NDBConfig.xml
	
	ServicesLayer/Bin/PlantHostInbound.qxe
	ServicesLayer/Bin/PlantHostOutbound.qxe
	ServicesLayer/Bin/AvtCanProxy.qxe
	ServicesLayer/Bin/AvtJ2190Proxy.qxe
	ServicesLayer/Bin/CLV295ScannerMgr.qxe
	ServicesLayer/Bin/DaqInterface.qxe
	ServicesLayer/Bin/IsoCommProxy.qxe
	ServicesLayer/Bin/MapleKeypadManager.qxe
	ServicesLayer/Bin/PendentManager.qxe
	ServicesLayer/Bin/SymbolScannerMgr.qxe 
	ServicesLayer/Bin/BepServer.qxe
	ServicesLayer/Bin/HostInterface.qxe
	ServicesLayer/Bin/InputServer.qxe
	ServicesLayer/Bin/MultiComProxy.qxe
	ServicesLayer/Bin/PendantManager.qxe
	ServicesLayer/Bin/SciCommProxy.qxe 
	ServicesLayer/Bin/SciIsoMuxMgr.qxe
	ServicesLayer/Bin/SystemScreenSaver.qxe               
	ServicesLayer/Bin/SymbolScannerMgr.qxe
	ServicesLayer/Bin/BaldorMotorController.qxe
	ServicesLayer/Bin/BBKWirelessCommProxy.qxe
	ServicesLayer/Bin/ChinaSystemMonitor.qxe
	ServicesLayer/Bin/DsaServer.qxe
	ServicesLayer/Bin/HydraulicMotorController.qxe
	ServicesLayer/Bin/InterCcrtCommServer.qxe
	ServicesLayer/Bin/NdbInterface.qxe
	ServicesLayer/Bin/VehicleDataServer.qxe
	ServicesLayer/Bin/WinCcrtInterface.qxe
	ServicesLayer/Bin/WinPendantInterface.qxe
	            
	ApplicationLayer/Bin/MachineTC.qxe
	ApplicationLayer/Bin/GenericTcaseTest.qxe
	ApplicationLayer/Bin/GenericSpeedControl.qxe
	ApplicationLayer/Bin/GasAnalyzerTC.qxe
	ApplicationLayer/Bin/GenericBaseBrakeTest.qxe
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
