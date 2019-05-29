#include "mediamanager.h"

#include "utils/mkpath.h"

#include <sys/types.h>

#include <fcntl.h>
#include <unistd.h>
#include <dirent.h>
#include <cstring>

#include <iostream>
#include <algorithm>

MediaManager::MediaManager(const char *mediaPath) : m_mediaPath(mediaPath)
{
    loadCache();
}


bool MediaManager::cacheAlbum(const Album &album)
{
    if(findByCDID(album.cdid))
        return false;

    m_cache.push_back(album);
    return true;
}

Album *MediaManager::findByAlbumName(const std::string &albumName)
{
    auto i = std::find_if(m_cache.begin(), m_cache.end(), [=](const Album &a) { return a.albumName == albumName; });
    return (i == m_cache.end()) ? nullptr : &(*i);
}

Album *MediaManager::findByCDID(const std::string &cdid)
{
    auto i = std::find_if(m_cache.begin(), m_cache.end(), [=](const Album &a) { return a.cdid == cdid; });
    return (i == m_cache.end()) ? nullptr : &(*i);
}

std::string MediaManager::getAlbumDir(const std::string &cdid)
{
    // cdid looks like a hex string.
    // This means each digit, of which there are 6, represents 16 values
    // Split the cdid into groups of 2, but don't ever loose a value
    if(cdid.empty())
        return "";

    std::string path;
    path = cdid.at(0);
    for(size_t i = 1; i < cdid.size(); i++)
    {
        path += cdid.at(i);
        if(i % 2)
            path += '/';
    }

    if(path.back() == '/')
        return path;

    return path + '/';
}

void MediaManager::loadCache(const std::string &path)
{
    DIR *dir;
    dirent *entry;

    loadCacheFile(path);

    if (!(dir = opendir(path.c_str())))
        return;

    while ((entry = readdir(dir)) != nullptr)
    {
        if (entry->d_type != DT_DIR)
            continue;

        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
            continue;

        loadCache(path + "/" + entry->d_name + "/");
    }

    closedir(dir);
}

void MediaManager::loadCache()
{
    // Recursive directory scan
    loadCache(m_mediaPath);
}

void MediaManager::loadCacheFile(const std::string &path)
{
    // Read the cache
    const int fd = open((path + "/cache").c_str(), O_RDONLY);
    if(fd < 0)
        return;

    Album a;

    struct IO
    {
        int year;
        int playtime;
        size_t titles;
    } io;

    const auto sr = read(fd, &io, sizeof(IO)); // get year and playtime

    if(sr != sizeof(IO))
    {
        std::cerr << __PRETTY_FUNCTION__ << " Invalid data! " << path << std::endl;
        close(fd);
        return;
    }

    a.year = io.year;
    a.playTime_s = io.playtime;

    a.titles.reserve(io.titles);

    auto getString = [=](bool &ok)
    {
        std::string str;
        do
        {
            char c;
            if(read(fd, &c, 1) != 1)
            {
                ok = false;
                break;
            }

            if(c == '\0')
                break;

            str += c;
        } while(true);
        return str;
    };

    bool ok = true;
    a.cdid = getString(ok);
    a.albumName = getString(ok);
    a.genre = getString(ok);

    if(!ok)
    {
        std::cerr << __PRETTY_FUNCTION__ << " Invalid data! " << path << std::endl;
        close(fd);
        return;
    }

    do
    {
        auto s = getString(ok);
        if(!ok)
            break;
        a.titles.push_back(s);

    } while(true);

    close(fd);

    if(a.titles.size() == size_t(io.titles))
        m_cache.push_back(a);
    else
        std::cerr << __PRETTY_FUNCTION__ << " Invalid data! " << path << std::endl;

}

void MediaManager::saveCache()
{
    for(const auto &i : m_cache)
    {
        const std::string &path = m_mediaPath + std::string("/") + getAlbumDir(i.cdid);

        if(!mkpath(path))
        {
            std::cerr << "Dir build failed " << path << " " << errno << std::endl;
            return;
        }

        // Read the cache
        const int fd = open((path + "/cache").c_str(), O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
        if(fd < 0)
            return;

        struct IO
        {
            int year;
            int playtime;
            size_t titles;
        } io;

        io.year = i.year;
        io.playtime = i.playTime_s;
        io.titles = i.titles.size();

        auto sr = write(fd, &io, sizeof(IO));
        if(sr != sizeof(IO))
            std::cerr << __PRETTY_FUNCTION__ << " Failed Write!" << std::endl;

        write(fd, i.cdid.c_str(),      i.cdid.size() + 1);
        write(fd, i.albumName.c_str(), i.albumName.size() + 1);
        write(fd, i.genre.c_str(),     i.genre.size() + 1);

        for(const auto &s : i.titles)
            write(fd, s.c_str(), s.size() + 1);

        close(fd);
    }
}
