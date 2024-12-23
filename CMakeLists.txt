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
    src/sharedDir/C_Base.h
    src/sharedDir/C_Client.h
    src/sharedDir/C_Collidable.h
    src/sharedDir/C_Controller.h
    src/sharedDir/C_Health.h
    src/sharedDir/C_Movable.h
    src/sharedDir/C_Name.h
    src/sharedDir/C_Position.h
    src/sharedDir/C_State.h
    src/sharedDir/C_TimedComponentBase.h
    src/sharedDir/Directions.h
    src/sharedDir/ECS_Types.h
    src/sharedDir/ResourceManager.h
    src/sharedDir/Utilities.h
    src/sharedDir/Message.h
    src/sharedDir/Observer.h
    src/sharedDir/S_Base.h
    src/sharedDir/Bitmask.h
    src/sharedDir/EntityManager.h
    src/sharedDir/EntityManager.cpp
    src/sharedDir/SystemManager.h
    src/sharedDir/SystemManager.cpp
    src/sharedDir/EventQueue.h
    src/sharedDir/Communicator.h
    src/sharedDir/EntityMessages.h
    src/sharedDir/EntityEvents.h
    src/sharedDir/MessageHandler.h
    src/sharedDir/S_Base.h
    src/sharedDir/S_Base.cpp
    src/sharedDir/S_Control.h
    src/sharedDir/S_Control.cpp
    src/sharedDir/S_State.h
    src/sharedDir/S_State.cpp
    src/sharedDir/EntitySnapshot.h
    src/sharedDir/EntitySnapshot.cpp
)
ADD_LIBRARY(ClientLib STATIC 
    src/clientLib/Client.h
    src/clientLib/Client.cpp
    src/clientLib/C_Drawable.h
    src/clientLib/TextureManager.h
    src/clientLib/Anim_Base.h
    src/clientLib/Anim_Base.cpp
    src/clientLib/Anim_Directional.h
    src/clientLib/Anim_Directional.cpp
    src/clientLib/C_SpriteSheet.h
    src/clientLib/SpriteSheet.h
    src/clientLib/SpriteSheet.cpp
    src/clientLib/SoundProps.h
    src/clientLib/AudioManager.h
    src/clientLib/SoundManager.h
    src/clientLib/SoundManager.cpp
    src/clientLib/C_SoundEmitter.h
    src/clientLib/C_SoundListener.h
    src/clientLib/C_UI_Element.h
    src/clientLib/StateManager.h
    src/clientLib/EventManager.h
    src/clientLib/EventManager.cpp
    src/clientLib/GUI_Event.h
    src/clientLib/Map.h
    src/clientLib/Map.cpp
    src/clientLib/ClientEntityManager.h
    src/clientLib/ClientEntityManager.cpp
    src/clientLib/SharedContext.h
    src/clientLib/Window.h
    src/clientLib/Window.cpp
    src/clientLib/BaseState.h
    src/clientLib/StateManager.cpp
)
ADD_LIBRARY(ServerLib STATIC 
    src/serverLib/Server.h
    src/serverLib/Server.cpp
    src/serverLib/C_Attacker.h
    src/serverLib/Map.h
    src/serverLib/Map.cpp
    src/serverLib/S_Movement.h
    src/serverLib/S_Movement.cpp
    src/serverLib/Map.h
    src/serverLib/Map.cpp
    src/serverLib/ServerEntityManager.h
    src/serverLib/ServerEntityManager.cpp
)
ADD_EXECUTABLE(client src/client/main.cpp)
ADD_EXECUTABLE(server src/server/main.cpp)
target_link_libraries(SharedDir sfml-graphics)
target_link_libraries(SharedDir sfml-audio)
target_link_libraries(SharedDir sfml-network)
target_link_libraries(SharedDir shlwapi)
target_link_libraries(ClientLib shlwapi)
target_link_libraries(ServerLib shlwapi)
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
