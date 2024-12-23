#pragma once
#include <SFML/Graphics.hpp>
#include <unordered_map>
#include <map>
#include <fstream>
#include <sstream>
#include "Utilities.h"
#include "ServerEntityManager.h"

enum Sheet { Tile_Size = 32, Sheet_Width = 256, Sheet_Height = 256, Num_Layers = 4 };

using TileID = unsigned int;

struct TileInfo {
    TileInfo(TileID _id = 0) {
        deadly = false;
        id = _id;
    };
    ~TileInfo() {};

    bool deadly;
    TileID id;
    sf::Vector2f friction;
    std::string name;
};

struct Tile {
    TileInfo* properties;
    bool warp;
    bool solid;
};

using TileMap = std::unordered_map<TileID, Tile*>;
using TileSet = std::unordered_map<TileID, TileInfo*>;

class Map {
public:
    Map(ServerEntityManager* _mgr);
    ~Map();

    TileInfo* GetDefaultTile();
    Tile* GetTile(unsigned int _x, unsigned int _y, unsigned int _layer);
    void LoadMap(const std::string& _path);
    unsigned int GetTileSize();

    const sf::Vector2u& GetMapSize();
    const sf::Vector2f& GetPlayerStart();
    void Update(float _dt);
private:
    unsigned int ConvertCoords(unsigned int _x, unsigned int _y, unsigned int _layer);
    void LoadTiles(const std::string& _path);
    void PurgeMap();
    void PurgeTileSet();

    TileMap tileMap;
    TileSet tileSet;
    unsigned int tileCount;
    unsigned int tileSetCount;

    TileInfo defaultTile;
    sf::Vector2u maxMapSize;
    sf::Vector2f playerStart;
    ServerEntityManager* entityMgr;
};