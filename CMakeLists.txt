cmake_minimum_required(VERSION 3.0.0)
project(mqtt VERSION 0.1.0)

# include(CTest)
# enable_testing()
# find_package(eclipse-paho-mqtt-c REQUIRED)
# find_package(PahoMqttCpp REQUIRED)

add_executable(mqtt main.cpp)
# target_include_directories(mqtt PUBLIC PahoMqttCpp)

add_library(pahomqttp3 STATIC IMPORTED)
set_target_properties(pahomqttp3 PROPERTIES IMPORTED_LOCATION  C:/vcpkg/installed/x64-windows/lib/paho-mqttpp3.lib)
set_target_properties(pahomqttp3 PROPERTIES INTERFACE_INCLUDE_DIRECTORIES C:/vcpkg/installed/x64-windows/include)


add_library(pahomqtt3as STATIC IMPORTED)
set_target_properties(pahomqtt3as PROPERTIES IMPORTED_LOCATION  C:/vcpkg/installed/x64-windows/lib/paho-mqtt3as.lib)
set_target_properties(pahomqtt3as PROPERTIES INTERFACE_INCLUDE_DIRECTORIES C:/vcpkg/installed/x64-windows/include)

target_link_libraries(mqtt PUBLIC pahomqttp3)
target_link_libraries(mqtt PUBLIC pahomqtt3as)
# include_directories("C:/vcpkg/installed/x64-windows/include")
set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
