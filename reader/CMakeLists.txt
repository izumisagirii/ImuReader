# reader backend for Linux/Windows
# Find and import CH34x and bmi160 files
# 06/24 Yongheng CUI

aux_source_directory(. sources)

set(IMU_DIR ${PROJECT_SOURCE_DIR}/3rdparty/BMI160_SensorAPI)
set(WCH_DIR ${PROJECT_SOURCE_DIR}/3rdparty/WCH_Driver)

if(WIN32)
  set(WCH_INCLUDE_DIR ${WCH_DIR}/windows)

  if(CMAKE_SIZEOF_VOID_P EQUAL 8)
    if(CMAKE_CL_64)
      # AMD64
      find_library(CH341_LIBRARY NAMES CH341DLLA64 PATHS ${WCH_INCLUDE_DIR}/amd64 NO_DEFAULT_PATH)
    else()
      # ARM64
      find_library(CH341_LIBRARY NAMES CH341DLLA64 PATHS ${WCH_INCLUDE_DIR}/arm64 NO_DEFAULT_PATH)
    endif()
  elseif(CMAKE_SIZEOF_VOID_P EQUAL 4)
    # i386
    find_library(CH341_LIBRARY NAMES CH341DLL PATHS ${WCH_INCLUDE_DIR}/i386 NO_DEFAULT_PATH)
  endif()
elseif(UNIX AND NOT APPLE)
  # Other ARCHs, not tested
  if(CMAKE_SYSTEM_PROCESSOR MATCHES "aarch64")
    set(WCH_INCLUDE_DIR ${WCH_DIR}/linux/aarch64/dynamic)
  elseif(CMAKE_SYSTEM_PROCESSOR MATCHES "mips")
    set(WCH_INCLUDE_DIR ${WCH_DIR}/linux/mips32/dynamic)
  elseif(CMAKE_SYSTEM_PROCESSOR MATCHES "mips64")
    set(WCH_INCLUDE_DIR ${WCH_DIR}/linux/mips64/dynamic)
  else() # x64 default
    set(WCH_INCLUDE_DIR ${WCH_DIR}/linux/x64/dynamic)
  endif()

  find_library(CH341_LIBRARY NAMES ch347 PATHS ${WCH_INCLUDE_DIR} NO_DEFAULT_PATH)
endif()

set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)

add_library(reader STATIC ${sources} ${IMU_DIR}/bmi160.c)
target_include_directories(reader PUBLIC ${IMU_DIR} ${WCH_INCLUDE_DIR})

if(CH341_LIBRARY)
  target_link_libraries(reader ${CH341_LIBRARY})
else()
  message(FATAL_ERROR "Cannot find ch34x library")
endif()