#------------------------------------------------------------------------------------------
# CMake template used to delete unnecessary files from the install directory
#------------------------------------------------------------------------------------------
FOREACH(file @CORE_FILES_RM_LIST@)
	MESSAGE(STATUS "Uninstalling ${file}")
	EXEC_PROGRAM(rm @CMAKE_INSTALL_PREFIX@ ARGS -f ${file} OUTPUT_VARIABLE RM_SUPERFLUOUS_FILES)
ENDFOREACH(file)
