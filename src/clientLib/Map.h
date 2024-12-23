#pragma once
#include <SFML/Graphics.hpp>
#include <unordered_map>
#include <map>
#include <fstream>
#include <sstream>
#include "Utilities.h"
#include "SharedContext.h"
#include <cmath>

enum Sheet { Tile_Size = 32, Sheet_Width = 256, Sheet_Height = 256, Num_Layers = 4 };

using TileID = unsigned int;

struct TileInfo {
    TileInfo(SharedContext* _context, const std::string& _texture = "", 
        TileID _id = 0) : shared(_context), id(0), deadly(false) {
        TextureManager* textMgr = shared->textMgr;
        if (_texture == "") { id = _id; return; };
        if (!textMgr->RequireResource(_texture)) return;
        texture = _texture;
        id = _id;
        sprite.setTexture(*textMgr->GetResource(texture));
        sf::IntRect tileBoundaries(id % (Sheet::Sheet_Width / Sheet::Tile_Size), 
            id / (Sheet::Sheet_Height / Sheet::Tile_Size), Sheet::Tile_Size, Sheet::Tile_Size);
        sprite.setTextureRect(tileBoundaries);
    };
    ~TileInfo() {
        if (texture == "") return;
        shared->textMgr->ReleaseResource(texture);
    };

    sf::Sprite sprite;
    bool deadly;
    TileID id;
    sf::Vector2f friction;
    std::string name;
    SharedContext* shared;
    std::string texture;
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
    Map(SharedContext* _mgr);
    ~Map();

    TileInfo* GetDefaultTile();
    Tile* GetTile(unsigned int _x, unsigned int _y, unsigned int _layer);
    void LoadMap(const std::string& _path);
    unsigned int GetTileSize() const;
    int GetPlayerId() const;

    const sf::Vector2u& GetMapSize() const;
    const sf::Vector2f& GetPlayerStart() const;
    void Update(float _dt);
    void Draw(unsigned int _layer);
private:
    unsigned int ConvertCoords(unsigned int _x, unsigned int _y, unsigned int _layer);
    void LoadTiles(const std::string& _path);
    void PurgeMap();
    void PurgeTileSet();

    TileMap tileMap;
    TileSet tileSet;
    unsigned int tileCount;
    unsigned int tileSetCount;
    int playerId;

    TileInfo defaultTile;
    sf::Vector2u maxMapSize;
    sf::Vector2f playerStart;
    SharedContext* shared;
};