#include "directory_server.h"
#include <dirent.h>

#include <iostream>
#include <vector>

#include "file_response.h"

bool directory_server::add_dir(httpserver::webserver &ws, const std::string &directory, bool recursive,
                               const std::string &path, const std::string &basedir)
{
    DIR *d = opendir(directory.c_str());
    if (!d)
    {
        std::cerr << "Could not read directory! " << directory << std::endl;
        return false;
    }

    dirent *dir;
    std::vector<std::string> files, dirs;
    while ((dir = readdir(d)))
    {
        std::string node(dir->d_name);
        if(node == "." || node == "..")
            ;
        else if(dir->d_type == DT_REG)
            files.push_back(node);
        else if(dir->d_type == DT_DIR)
            dirs.push_back(node);
    }
    closedir(d);

    bool okay = true;

    if(recursive)
    {
        for(const auto &d : dirs)
            okay &= add_dir(ws, d, true, path + "/" + d, basedir);
    }

    for(const auto &f : files)
        okay &= add_file(ws, path + "/" + f, basedir);

    return okay;
}

bool directory_server::add_file(httpserver::webserver &ws, const std::string &filename, const std::string &basedir)
{
    std::cout << "Adding resource: " << filename << std::endl;

    // TODO: Is this a leak? Does register_resource take ownership? Does it matter if it doesn't?
    return ws.register_resource(filename, new file_response(basedir + filename));
}
