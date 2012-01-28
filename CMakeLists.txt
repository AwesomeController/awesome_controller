#=============================================================================#
# Author: QueezyTheGreat                                                      #
# Date:   26.04.2011                                                          #
#                                                                             #
# Description: Arduino CMake example                                          #
#                                                                             #
#=============================================================================#
set(CMAKE_MODULE_PATH    ${CMAKE_SOURCE_DIR}/arduino-cmake/cmake/modules)  # CMake module search path
set(CMAKE_TOOLCHAIN_FILE ${CMAKE_SOURCE_DIR}/arduino-cmake/cmake/toolchains/Arduino.cmake) # Arduino Toolchain

set(ARDUINO_SDK /home/wes/Downloads/arduino-1.0)
set(ARDUINO_SDK_PATH /home/wes/Downloads/arduino-1.0)
set(ARDUINO_SDK_VERSION 1.0)
set(VERSION_VAR 1.0)

cmake_minimum_required(VERSION 2.8)
#====================================================================#
#  Setup Project                                                     #
#====================================================================#
project(ArduinoExample C CXX)
find_package(Arduino)

add_subdirectory(awesome_controller)  # Add example directory to build

