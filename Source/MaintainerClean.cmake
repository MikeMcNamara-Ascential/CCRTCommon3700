#------------------------------------------------------------------------------------------
# Create a runtime distribution
#------------------------------------------------------------------------------------------
ADD_CUSTOM_TARGET(maintainer-clean
	@echo cleaning for source distribution
	DEPENDS clean)

ADD_CUSTOM_COMMAND(TARGET maintainer-clean
                    COMMAND find
					ARGS . -type f \\\(
						-name CMakeCCompiler.cmake -o
						-name CMakeCXXCompiler.cmake -o
						-name CMakeCache.txt -o
						-name CMakeCache.txt.cbuild -o
						-name CMakeOutput.log -o
						-name CMakeSystem.cmake -o
						-name InstallDir -o
						-name Makefile -o
						-name cmake.check_cache -o
						-name cmake.check_depends -o
						-name cmake.depends -o
						-name cmake_install.cmake -o
						-name install_manifest.txt -o
						-name package.qpg -o
						-name postInstall.sh -o
						-name preInstall.sh
						\\\) -remove! &&
						rm -Rf CMakeTmp
						)

