#------------------------------------------------------------------------------------------
# Tell CMake to install all the config files in the Configuration/Servers directory
#------------------------------------------------------------------------------------------
INSTALL_FILES(${COMPONENT_CONFIG_DIR}/TransferCase FILES 
	      DCX.DCBR.GenericTcase.xml
	      DCX.DCBR.TcaseFaults.xml 
	      )
