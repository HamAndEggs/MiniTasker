cmake_minimum_required(VERSION 3.18.0)
project(MiniTasker VERSION 0.1.0)

#find_package(PkgConfig REQUIRED)
#pkg_check_modules(GTK4 REQUIRED gtk4)
#${GTK4_INCLUDE_DIRS}


include_directories(
    /usr/include/freetype2
    /usr/include/libdrm
    ./EdgeUI/include
    ./source
)

add_subdirectory(./EdgeUI)

add_executable(MiniTasker
    ./source/DisplayAirQuality.cpp
    ./source/DisplayBitcoinPrice.cpp
    ./source/DisplayClock.cpp
    ./source/DisplaySystemStatus.cpp
    ./source/DisplayWeather.cpp
    ./source/i2c_device.cpp
    ./source/main.cpp
    ./source/MQTTData.cpp
    ./source/sgp30.cpp
    ./source/TinyTools.cpp
    ./source/TinyWeather.cpp
)

set_property(TARGET MiniTasker PROPERTY CXX_STANDARD 17)

if( TARGET_PLATFORM STREQUAL "GTK4" )
    target_link_libraries(MiniTasker EdgeUI.GTK4 stdc++ pthread m freetype GLESv2 EGL z i2c mosquitto curl ${GTK4_LINK_LIBRARIES})
elseif( TARGET_PLATFORM STREQUAL "DRM" )
    target_link_libraries(MiniTasker EdgeUI.DRM stdc++ pthread m freetype GLESv2 EGL z i2c mosquitto curl gbm drm)
elseif( TARGET_PLATFORM STREQUAL "X11" )
    target_link_libraries(MiniTasker EdgeUI.X11 stdc++ pthread m freetype GLESv2 EGL z i2c mosquitto curl X11 GL)
else()
    message(FATAL_ERROR "TARGET_PLATFORM not defined")
endif()

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)

