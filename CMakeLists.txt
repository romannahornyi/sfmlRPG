cmake_minimum_required(VERSION 3.28)
project(SFMLRPG LANGUAGES CXX)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
option(BUILD_SHARED_LIBS "Build shared libraries" OFF)

include(FetchContent)
FetchContent_Declare(SFML
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG 2.6.x
    GIT_SHALLOW ON
    EXCLUDE_FROM_ALL
    SYSTEM)
FetchContent_MakeAvailable(SFML)

ADD_LIBRARY(SharedDir SHARED 
    src/sharedDir/PacketTypes.h
    src/sharedDir/PacketTypes.cpp
    src/sharedDir/NetworkDefinitions.h
)
ADD_LIBRARY(ClientLib STATIC 
    src/clientLib/Client.h
    src/clientLib/Client.cpp
)
ADD_LIBRARY(ServerLib STATIC 
    src/serverLib/Server.h
    src/serverLib/Server.cpp
)
ADD_EXECUTABLE(client src/client/main.cpp)
ADD_EXECUTABLE(server src/server/main.cpp)
target_link_libraries(SharedDir sfml-graphics)
target_link_libraries(SharedDir sfml-audio)
target_link_libraries(SharedDir sfml-network)
target_link_libraries(ServerLib SharedDir)
target_link_libraries(ClientLib SharedDir)
target_link_libraries(client SharedDir)
target_link_libraries(server SharedDir)
target_link_libraries(client sfml-graphics)
target_link_libraries(client sfml-audio)
target_link_libraries(client sfml-network)
target_link_libraries(server sfml-graphics)
target_link_libraries(server sfml-audio)
target_link_libraries(server sfml-network)
target_link_libraries(server ServerLib)
target_link_libraries(client ClientLib)
target_include_directories(client PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src/sharedDir)
target_include_directories(server PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src/sharedDir)
target_include_directories(ServerLib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src/sharedDir)
target_include_directories(ClientLib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src/sharedDir)
target_include_directories(server PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src/serverLib)
target_include_directories(client PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src/clientLib)
target_compile_features(client PRIVATE cxx_std_17)
target_compile_features(server PRIVATE cxx_std_17)
if(WIN32)
    add_custom_command(
        TARGET client
        COMMENT "Copy OpenAL DLL"
        PRE_BUILD COMMAND ${CMAKE_COMMAND} -E copy ${SFML_SOURCE_DIR}/extlibs/bin/$<IF:$<EQUAL:${CMAKE_SIZEOF_VOID_P},8>,x64,x86>/openal32.dll $<TARGET_FILE_DIR:client>
        VERBATIM)
    add_custom_command(
        TARGET server
        COMMENT "Copy OpenAL DLL"
        PRE_BUILD COMMAND ${CMAKE_COMMAND} -E copy ${SFML_SOURCE_DIR}/extlibs/bin/$<IF:$<EQUAL:${CMAKE_SIZEOF_VOID_P},8>,x64,x86>/openal32.dll $<TARGET_FILE_DIR:server>
        VERBATIM)
endif()
