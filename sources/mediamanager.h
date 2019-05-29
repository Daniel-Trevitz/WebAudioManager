#ifndef MEDIAMANAGER_H
#define MEDIAMANAGER_H

#include <string>
#include <vector>
#include <list>

struct Album
{
    int year;
    int playTime_s;
    std::string cdid;
    std::string albumName;
    std::string genre;
    std::vector<std::string> titles;
};

class MediaManager
{
public:
    MediaManager(const char *mediaPath);

    void saveCache();
    void loadCache();
    bool cacheAlbum(const Album &album);

    Album *findByAlbumName(const std::string &albumName);
    Album *findByCDID(const std::string &cdid);

    static std::string getAlbumDir(const std::string &cdid);

private:
    void loadCacheFile(const std::string &path);
    void loadCache(const std::string &path);
    const char *m_mediaPath;
    std::list<Album> m_cache;
};

#endif // MEDIAMANAGER_H
