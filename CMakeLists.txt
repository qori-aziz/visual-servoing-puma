cmake_minimum_required(VERSION 3.0.0)
project(mqtt VERSION 0.1.0)
set(CMAKE_BUILD_TYPE Release)

# include(CTest)
# enable_testing()
list(APPEND CMAKE_PREFIX_PATH "C:/mqtt/paho-c")
list(APPEND CMAKE_PREFIX_PATH "C:/mqtt/paho-cpp")
find_package(eclipse-paho-mqtt-c REQUIRED PATH C:/mqtt/paho-c)
# set(CMAKE_PREFIX_PATH C:/mqtt)
# set(PahoMqttCpp_DIR "C:/mqtt/paho-cpp")
SET(PAHOMQTTCPP_ROOT C:/mqtt/paho-cpp)
find_package(PahoMqttCpp REQUIRED)

add_executable(mqtt main.cpp)
# target_include_directories(mqtt PUBLIC PahoMqttCpp)

# ## For C library
# add_library(pahomqtt3a STATIC IMPORTED)
# set_target_properties(pahomqtt3a PROPERTIES INTERFACE_INCLUDE_DIRECTORIES C:/mqtt/paho-c/include)
# set_target_properties(pahomqtt3a PROPERTIES IMPORTED_LOCATION  C:/mqtt/paho-c/lib/paho-mqtt3a-static.lib)

# ## For CPP Library
# add_library(pahomqttp3 STATIC IMPORTED)
# set_target_properties(pahomqttp3 PROPERTIES INTERFACE_INCLUDE_DIRECTORIES C:/mqtt/paho-cpp/include)
# set_target_properties(pahomqttp3 PROPERTIES IMPORTED_LOCATION  C:/mqtt/paho-cpp/lib/paho-mqttpp3-static.lib)

target_link_libraries(mqtt PUBLIC eclipse-paho-mqtt-c::paho-mqtt3a PahoMqttCpp::paho-mqttpp3-static)
# target_link_libraries(mqtt PUBLIC pahomqttp3)
# include_directories("C:/vcpkg/installed/x64-windows/include")
set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
