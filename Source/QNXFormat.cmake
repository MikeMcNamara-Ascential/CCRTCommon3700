#------------------------------------------------------------------------------------------
# Create a target to convert the install scripts to QNX format
#------------------------------------------------------------------------------------------

# Only do the conversion if it has not already been done
IF(NOT SOURCES_REFORMATTED)

	SET(SOURCES_REFORMATTED TRUE CACHE BOOL "Flag indicating whether or not 'problematic' files have been converted to QNX format" FORCE)
	
	SET(REFORMAT_SOURCES 
		${CMAKE_CURRENT_SOURCE_DIR}/InstallDir.cmake 
		${CMAKE_CURRENT_SOURCE_DIR}/preInstall.sh.cmake 
		${CMAKE_CURRENT_SOURCE_DIR}/postInstall.sh.cmake 
		${CORE_SERVICES_LIB_DIR}/XmlProcessor/xmlparse/xmlparse.c 
		${CORE_SERVICES_LIB_DIR}/XmlProcessor/xmltok/xmltok.c 
		${CORE_SERVICES_LIB_DIR}/XmlProcessor/xmltok/xmltok_impl.c 
		${CORE_SERVICES_LAYER_INCLUDES}/xmlrole.h 
		${CORE_SERVICES_LAYER_INCLUDES}/xmltok.h)
	MESSAGE(STATUS "Formatting files for QNX...")
	EXEC_PROGRAM(chmod 
					ARGS a+w ${REFORMAT_SOURCES} 
					OUTPUT_VARIABLE CHMOD_RESULT)
	EXEC_PROGRAM(textto 
					ARGS -l ${REFORMAT_SOURCES} 
					OUTPUT_VARIABLE TEXTTO_RESULT)
ENDIF(NOT SOURCES_REFORMATTED)

