cmake_minimum_required(VERSION 3.5)

project(Transmission LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt5 COMPONENTS Widgets REQUIRED)
find_package(Qt5 COMPONENTS Network REQUIRED)

if(WIN32)
    set(PATH libs_win)
elseif(UNIX)
    set(PATH libs_unix)
endif()

set(PRTOTO_LIB ${CMAKE_HOME_DIRECTORY}/../${PATH}/protobuf-2.6.1)

if(NOT EXISTS ${PRTOTO_LIB})
    message(FATAL_ERROR "Not found ${PRTOTO_LIB} !!!")
endif()

file(GLOB PROTO_FILES ${CMAKE_HOME_DIRECTORY}/proto/*.proto)

set(ProtocolFolder ${CMAKE_HOME_DIRECTORY}/proto_cxx)

if(EXISTS ${ProtocolFolder})
    file(REMOVE_RECURSE ${ProtocolFolder})
endif()

file(MAKE_DIRECTORY ${ProtocolFolder})

set(ProtocExecute ${PRTOTO_LIB}/bin/protoc)
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
                    ${CMAKE_HOME_DIRECTORY}/proto_cxx/*.pb.cc
                    ${CMAKE_HOME_DIRECTORY}/video/*.h
                    ${CMAKE_HOME_DIRECTORY}/video/*.cpp)

add_executable(Transmission ${SRC_FILES})

SET(PROTO_INCLUDE ${PRTOTO_LIB}//include/)

set(HDR_FILES ${CMAKE_HOME_DIRECTORY}
              ${Protobuf_INCLUDE_DIRS}
              ${PROTO_INCLUDE}
              ${CMAKE_HOME_DIRECTORY}/common
              ${CMAKE_HOME_DIRECTORY}/proto_cxx
              ${CMAKE_HOME_DIRECTORY}/transceiver
              ${CMAKE_HOME_DIRECTORY}/video)

target_include_directories(Transmission PUBLIC ${HDR_FILES})

add_library(protobuf2 STATIC IMPORTED)
set(PATH_STATIC_LIB_PROTOBUF)
set(PATH_STATIC_LIB_PROTOBUF ${PATH_STATIC_LIB_PROTOBUF} "${PRTOTO_LIB}/lib/libprotobuf.a")
set_target_properties(protobuf2 PROPERTIES IMPORTED_LOCATION ${PATH_STATIC_LIB_PROTOBUF})

target_link_libraries(Transmission PRIVATE
                      Qt5::Widgets
                      Qt5::Network
                      protobuf2
                      )

