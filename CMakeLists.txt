cmake_minimum_required(VERSION 3.0.0)
project(MiniTasker VERSION 0.1.0)


add_definitions(-DPLATFORM_X11_GL -DVERBOSE_BUILD -DVERBOSE_SHADER_BUILD)

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
target_link_libraries(MiniTasker EdgeUI stdc++ pthread m freetype GL X11 z i2c mosquitto curl)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)

