#pragma once

#include <string>
#include <unordered_map>
#include <iostream>
#include <fstream>
#include <sstream>
#include "Utilities.h"

template <typename Derived, typename T>
class ResourceManager
{
public:
    ResourceManager(const std::string& pathsFile)
    {
        LoadPaths(pathsFile);
    }

    virtual ~ResourceManager() noexcept
    {
        PurgeResources();
    }

    T* GetResource(const std::string& id)
    {
        auto res = Find(id);
        return (res ? res->first : nullptr);
    }

    std::string GetPath(const std::string& id)
    {
        const auto path = paths_.find(id);
        return (path != paths_.end() ? path->second : "");
    }

    bool RequireResource(const std::string& id)
    {
        auto res = Find(id);
        if (res)
        {
            ++res->second;
            return true;
        }
        const auto path = paths_.find(id);
        if (path == paths_.end()) { return false; }
        T* resource = Load(path->second);
        if (!resource) { return false; }
        resources_.emplace(id, std::make_pair(resource, 1));
        return true;
    }

    bool ReleaseResource(const std::string& id)
    {
        auto res = Find(id);
        if (!res) { return false; }
        --res->second;
        if (!res->second) { Unload(id); }
        return true;
    }

    void PurgeResources()
    {
        std::cout << "Purging all resources:" << std::endl;
        while (resources_.begin() != resources_.end())
        {
            std::cout << "Removing: "
                << resources_.begin()->first << std::endl;
            delete resources_.begin()->second.first;
            resources_.erase(resources_.begin());
        }
        std::cout << "Purging finished." << std::endl;
    }

protected:
    virtual T* Load(const std::string& path) // virtual ???
    {
        return static_cast<Derived*>(this)->Load(path);
    }

private:
    std::pair<T*, unsigned int>* Find(const std::string& id)
    {
        auto itr = resources_.find(id);
        return (itr != resources_.end() ? &itr->second : nullptr);
    }

    bool Unload(const std::string& id)
    {
        auto itr = resources_.find(id);
        if (itr == resources_.end()) { return false; }
        delete itr->second.first;
        resources_.erase(itr);
        return true;
    }

    void LoadPaths(const std::string& pathFile)
    {
        std::ifstream paths;
        paths.open(Utils::GetWorkingDirectory() + pathFile);
        if (paths.is_open())
        {
            std::string line;
            while (std::getline(paths, line))
            {
                std::stringstream keystream(line);
                std::string pathName;
                std::string path;
                keystream >> pathName;
                keystream >> path;
                paths_.emplace(pathName, path);
            }
            paths.close();
            return;
        }
        std::cerr << "! Failed loading the path file: " << pathFile << std::endl;
    }

    std::unordered_map<std::string,
                       std::pair<T*, unsigned int>> resources_;
    std::unordered_map<std::string, std::string> paths_;
};
