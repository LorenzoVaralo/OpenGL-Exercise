@echo off
setlocal

set BUILD_DIR=build

:: Create the build directory if it doesn't exist
if not exist %BUILD_DIR% (
    mkdir %BUILD_DIR%
)

:: Run CMake in the build directory
cd %BUILD_DIR%
cmake ..
cmake --build .

endlocal
