cmake_minimum_required(VERSION 3.12.0)
project(imuReader VERSION 0.1.0 LANGUAGES C CXX)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin")

if(WIN32)
  add_definitions(-DWINDOWS)
elseif(UNIX AND NOT APPLE)
  add_definitions(-DLINUX)
endif()

include(FetchContent)

FetchContent_Declare(
  bmi160_sensor_api
  GIT_REPOSITORY "https://github.com/boschsensortec/BMI160_SensorAPI"
  GIT_TAG master
  SOURCE_DIR "${CMAKE_SOURCE_DIR}/3rdparty/BMI160_SensorAPI"
)

FetchContent_GetProperties(bmi160_sensor_api)

if(NOT bmi160_sensor_api_POPULATED)
  FetchContent_MakeAvailable(bmi160_sensor_api)
endif()

include_directories(./)

subdirs(
  reader
  rawSample
)
