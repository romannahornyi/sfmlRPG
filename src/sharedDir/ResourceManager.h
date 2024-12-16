#pragma once
#include <string> 
#include <unordered_map>
#include <iostream>
#include <sstream>
#include <fstream>
#include "Utilities.h"

template<typename Derived, typename T>
class ResourceManager {
public:
    ResourceManager(const std::string& _pathsFile) {
        LoadPaths(_pathsFile);
    };
    virtual ~ResourceManager() { PurgeResources(); };
    
    T* GetResource(const std::string& _id) {
        auto res = Find(_id);
        return (res != nullptr ? res->first : nullptr);
    };

    std::string GetPath(const std::string& _id) {
        auto itr = paths.find(_id);
        return (itr != paths.end() ? itr->second : "");
    };

    bool RequireResource(const std::string& _id) {
        auto res = Find(_id);
        if (res) {
            ++res->second;
            return true;
        }
        auto path = paths.find(_id);
        if (path == paths.end()) return false;
        T* resource = Load(path->second);
        if (!resource) return false;
        resources.emplace(_id, std::make_pair(resource, 1));
        return true;
    };

    bool ReleaseResource(const std::string& _id) {
        auto itr = Find(_id);
        if (!itr) return false;
        --itr->second;
        if (!itr->second) Unload(_id);
        return true;
    };

    void PurgeResources() {
        std::cout << "Purging all resources" << std::endl;
        while (resources.begin() != resources.end()) {
            std::cout << "Removing resources: " << resources.begin()->first << std::endl;
            delete resources.begin()->second.first;
            resources.erase(resources.begin());
        }
        std::cout << "Purging finished." << std::endl;
    };
protected:
    T* Load(const std::string& _path) {
        return static_cast<Derived*>(this)->Load(_path);
    };
private:
    std::pair<T*, unsigned int>* Find(const std::string& _id) {
        auto itr = resources.find(_id);
        return (itr != resources.end() ? &itr->second : nullptr);
    };

    bool Unload(const std::string& _id) {
        auto itr = resources.find(itr);
        if (itr == resources.end()) return false;
        delete itr->second.first;
        resources.erase(itr);
        return true;
    };

    void LoadPaths(const std::string& _pathsFile) {
        std::ifstream file;
        file.open(Utils::GetWorkingDirectory() + _pathsFile);
        if (file.is_open()) {
            std::string line;
            while (std::getline(file, line)) {
                std::stringstream keystream(line);
                std::string pathId;
                std::string path;
                keystream >> pathId;
                keystream >> path;
                paths.emplace(pathId, path);
            }
            paths.close();
            return;
        }
        std::cout << "Failed loading the paths file: " << _pathsFile << std::endl;
    };
    
    std::unordered_map<std::string, std::pair<T*, unsigned int>> resources;
    std::unordered_map<std::string, std::string> paths;
};