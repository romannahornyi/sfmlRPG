#include "Map.h"

Map::Map(SharedContext* _context) : shared(_context), defaultTile(shared), playerId(-1), maxMapSize(32, 32) {
    shared->gameMap = nullptr;
    LoadTiles("tiles.cfg");
};

Map::~Map() {
    PurgeMap();
    PurgeTileSet();
    shared->gameMap = nullptr;
};

Tile* Map::GetTile(unsigned int _x, unsigned int _y, unsigned int _layer) {
    if (_x < 0 || _x >= maxMapSize.x || _y < 0 || _y >= maxMapSize.y || _layer < 0 || _layer >= Sheet::Num_Layers) {
        return nullptr;
    }
    auto itr = tileMap.find(ConvertCoords(_x, _y, _layer));
    if (itr == tileMap.end()) return nullptr;
    return itr->second;
};

TileInfo* Map::GetDefaultTile() { return &defaultTile; };
const sf::Vector2u& Map::GetMapSize() const { return maxMapSize; };
const sf::Vector2f& Map::GetPlayerStart() const { return playerStart; };
unsigned int Map::GetTileSize() const { return Sheet::Tile_Size; };
int Map::GetPlayerId() const { return playerId; };

void Map::LoadMap(const std::string& _path) {
    std::ifstream file;
    file.open(Utils::GetWorkingDirectory() + _path);
    if (!file.is_open()) {
        std::cout << "[Map]Failed to open map file: " << _path << std::endl;
        return;
    }
    std::string line;
    while (std::getline(file, line)) {
        if (line[0] == '|') continue;
        std::stringstream keystream(line);
        std::string type;
        keystream >> type;
        if (type == "TILE") {
            int tileId = 0;
            keystream >> tileId;
            if (tileId < 0) {
                std::cout << "[Map] TileID is invalid." << std::endl;
                continue;
            }
            auto itr = tileSet.find(tileId);
            if (itr != tileSet.end()) {
                std::cout << "[Map] TileID(" << tileId << ") was found in: " << _path << std::endl;
                continue; 
            }
            sf::Vector2i tileCoords;
            unsigned int tileLayer;
            unsigned int tileSolidity;
            keystream >> tileCoords.x >> tileCoords.y >> tileLayer >> tileSolidity;
            if (tileCoords.x >= maxMapSize.x || tileCoords.y >= maxMapSize.y || tileLayer >= Sheet::Num_Layers) {
                std::cout << "[Map] Invalid tileCoords or sheet layer." << std::endl;
                continue;
            }
            Tile* tile = new Tile();
            tile->properties = itr->second;
            tile->solid = (bool)tileSolidity;
            if (!tileMap.emplace(ConvertCoords(tileCoords.x, tileCoords.y, tileLayer), tile).second) {
                std::cout << "Duplicate tile found: " << tileCoords.x << " " << tileCoords.y << std::endl;
                delete tile;
                tile = nullptr;
                continue;
            }
            std::string warp;
            keystream >> warp;
            tile->warp = false;
            if (warp == "WARP") tile->warp = true;
        } else if (type == "SIZE") {
            keystream >> maxMapSize.x >> maxMapSize.y;
        } else if (type == "DEFAULT_FRICTION") {
            keystream >> defaultTile.friction.x >> defaultTile.friction.y;
        } else if (type == "ENTITY") {
            std::string name;
            keystream >> name;
            int entityId = shared->entityMgr->AddEntity(name);
            if (entityId < 0) continue;
            C_Base* pos = nullptr;
            if (pos = shared->entityMgr->GetComponent<C_Position>(entityId, Component::Position)) keystream >> *pos;
        } else {
            std::cout << "[Map] Unknown type /" << type << "/" << std::endl;
        }
    }
    file.close();
    std::cout << "------------------Map Loaded------------------" << std::endl;
};

void Map::LoadTiles(const std::string& _path) {
    std::ifstream file;
    file.open(Utils::GetWorkingDirectory() + _path);
    if (!file.is_open()) {
        std::cout << "[Map]Failed to open tiles file: " << _path << std::endl;
        return;
    }
    std::string line;
    while (std::getline(file, line)) {
        if (line[0] == '|') continue;
        std::stringstream keystream(line);
        int tileId;
        keystream >> tileId;
        if (tileId < 0) continue;
        TileInfo* info = new TileInfo(shared, "TileSheet", tileId);
        keystream >> info->name >> info->friction.x >> info->friction.y >> info->deadly;
        if (!tileSet.emplace(tileId, info).second) {
            std::cout << "[Map] Duplicate tiles: " << tileId << std::endl;
            delete info;
        }
    }
    file.close();
};

void Map::Update(float _dt) {};

unsigned int Map::ConvertCoords(unsigned int _x, unsigned int _y, unsigned int _layer) {
    return ((_layer * maxMapSize.y + _y) * maxMapSize.x + _x);
};

void Map::PurgeMap() {
    while (tileMap.begin() != tileMap.end()) {
        delete tileMap.begin()->second;
        tileMap.erase(tileMap.begin());
    }
    tileCount = 0;
    shared->entityMgr->Purge();
};

void Map::PurgeTileSet() {
    while (tileSet.begin() != tileSet.end()) {
        delete tileSet.begin()->second;
        tileSet.erase(tileSet.begin());
    }
    tileSetCount = 0;
};

void Map::Draw(unsigned int _layer) {
    if (_layer >= Sheet::Num_Layers) return;
    sf::RenderWindow* wind = shared->window->GetRenderWindow();
    sf::FloatRect viewSpace = shared->window->GetViewSpace();
    sf::Vector2i tileBegin(std::floor(viewSpace.left / Sheet::Tile_Size), std::floor(viewSpace.top / Sheet::Tile_Size));
    sf::Vector2i tileEnd(
        std::ceil((viewSpace.left + viewSpace.width) / Sheet::Tile_Size),
        std::ceil((viewSpace.top + viewSpace.height) / Sheet::Tile_Size)
    );
    unsigned int count = 0;
    for (int x = tileBegin.x; x <= tileEnd.x; ++x) {
        for (int y = tileBegin.y; y <= tileEnd.y; ++y) {
            Tile* tile = GetTile(x, y, _layer);
            if (!tile) continue;
            sf::Sprite& sprite = tile->properties->sprite;
            sprite.setPosition(x * Sheet::Tile_Size, y * Sheet::Tile_Size);
            wind->draw(sprite);
            ++count;
        }
    }
};