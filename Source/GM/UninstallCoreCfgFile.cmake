#------------------------------------------------------------------------------------------
# CMake template used to delete unnecessary files from the install directory
#------------------------------------------------------------------------------------------
FOREACH(file @CORE_FILES_RM_LIST@)
	MESSAGE(STATUS "Uninstalling ${file}")
	EXEC_PROGRAM(rm @CMAKE_INSTALL_PREFIX@ ARGS -f ${file} OUTPUT_VARIABLE RM_SUPERFLUOUS_FILES)
ENDFOREACH(file)

FOREACH(dir @CORE_DIRS_RM_LIST@)
	MESSAGE(STATUS "Removing ${dir}")
	EXEC_PROGRAM(rm @CMAKE_INSTALL_PREFIX@ ARGS -Rf ${dir} OUTPUT_VARIABLE RM_SUPERFLOUS_FILES)
ENDFOREACH(dir) 
