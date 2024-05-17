cmake_minimum_required(VERSION 3.5)

project(Transmission LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

#file(GLOB PROTOBUF_LIBRARY "${CMAKE_HOME_DIRECTORY}/protobuf-2.5.0/lib/*.a")
#set(PROTOBUF_INCLUDE_DIR "${CMAKE_HOME_DIRECTORY}/protobuf-2.5.0/include/")
#find_package(protobuf REQUIRED)

#message(===================================)
#foreach(FILE ${Protobuf_INCLUDE_DIRS})
#    message("file: ${FILE}")
#endforeach()
#message(===================================)

find_package(Qt5 COMPONENTS Widgets REQUIRED)
find_package(Qt5 COMPONENTS Network REQUIRED)

file(GLOB PROTO_FILES ${CMAKE_HOME_DIRECTORY}/proto/*.proto)

set(ProtocolFolder ${CMAKE_HOME_DIRECTORY}/proto_cxx)

if(EXISTS ${ProtocolFolder})
    file(REMOVE_RECURSE ${ProtocolFolder})
endif()

file(MAKE_DIRECTORY ${ProtocolFolder})

set(ProtocExecute ${CMAKE_HOME_DIRECTORY}/protobuf-2.5.0/bin/protoc.exe)
set(ProtoPath     ${CMAKE_HOME_DIRECTORY}/proto)
set(ProtoCppOut   ${CMAKE_HOME_DIRECTORY}/proto_cxx)

foreach(FILE ${PROTO_FILES})
    execute_process(COMMAND ${ProtocExecute}
                            --proto_path=${ProtoPath}
                            --cpp_out=${ProtoCppOut}
                            ${FILE})
endforeach()

file(GLOB SRC_FILES ${CMAKE_HOME_DIRECTORY}/*.cpp
                    ${CMAKE_HOME_DIRECTORY}/*.h
                    ${CMAKE_HOME_DIRECTORY}/*.ui
                    ${CMAKE_HOME_DIRECTORY}/common/*.h
                    ${CMAKE_HOME_DIRECTORY}/common/*.cpp
                    ${CMAKE_HOME_DIRECTORY}/example/*.h
                    ${CMAKE_HOME_DIRECTORY}/example/*.cpp
                    ${CMAKE_HOME_DIRECTORY}/proto/*.proto
                    ${CMAKE_HOME_DIRECTORY}/proto_cxx/*.pb.h
                    ${CMAKE_HOME_DIRECTORY}/proto_cxx/*.pb.cc)

add_executable(Transmission ${SRC_FILES})

SET(PROTO_INCLUDE "${CMAKE_HOME_DIRECTORY}/protobuf-2.5.0/include/")

set(HDR_FILES ${CMAKE_HOME_DIRECTORY}
              ${Protobuf_INCLUDE_DIRS}
              ${PROTO_INCLUDE}
              ${CMAKE_HOME_DIRECTORY}/common
              ${CMAKE_HOME_DIRECTORY}/proto_cxx
              ${CMAKE_HOME_DIRECTORY}/transceiver)

target_include_directories(Transmission PUBLIC ${HDR_FILES})

add_library(protobuf2 STATIC IMPORTED)
set(PATH_STATIC_LIB_PROTOBUF)
set(PATH_STATIC_LIB_PROTOBUF ${PATH_STATIC_LIB_PROTOBUF} ${CMAKE_HOME_DIRECTORY}/protobuf-2.5.0/lib/libprotobuf.a)
set_target_properties(protobuf2 PROPERTIES IMPORTED_LOCATION ${PATH_STATIC_LIB_PROTOBUF})

target_link_libraries(Transmission PRIVATE
                      Qt5::Widgets
                      Qt5::Network
                      #${Protobuf_LIBRARIES}
                      protobuf2
                      )

