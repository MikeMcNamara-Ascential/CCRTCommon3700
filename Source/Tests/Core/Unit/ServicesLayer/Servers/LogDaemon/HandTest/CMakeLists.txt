#------------------------------------------------------------------------------------------
# Tell CMake what subdirectories to recurse into
#------------------------------------------------------------------------------------------
#ADD_SUBDIRECTORY()

#------------------------------------------------------------------------------------------
# Tell CMake what source file to build
#------------------------------------------------------------------------------------------
ADD_EXECUTABLE(Benchmark.qxe Benchmark.cpp)
ADD_EXECUTABLE(BreakIt.qxe BreakIt.cpp)
ADD_EXECUTABLE(IncrementTestNumber.qxe IncrementTestNumber.cpp)

