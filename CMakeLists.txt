cmake_minimum_required(VERSION 3.0.0)
project(MiniTasker VERSION 0.1.0)

find_package(PkgConfig REQUIRED)
pkg_check_modules(GTK4 REQUIRED gtk4)


include_directories(
    /usr/include/freetype2
    /usr/include/libdrm
    ./EdgeUI/include
    ./source
    ${GTK4_INCLUDE_DIRS}
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
#target_link_libraries(MiniTasker EdgeUI.GTK stdc++ pthread m freetype GLESv2 EGL z i2c mosquitto curl ${GTK4_LINK_LIBRARIES})
target_link_libraries(MiniTasker EdgeUI.DRM stdc++ pthread m freetype GLESv2 EGL z i2c mosquitto curl gbm drm)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)

