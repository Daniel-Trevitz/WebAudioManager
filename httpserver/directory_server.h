#ifndef DIRECTORY_SERVER_H
#define DIRECTORY_SERVER_H

#include <string>

#include <httpserver.hpp>

//TODO: this is really sloppy. make it better.
//TODO: This only allows for text/html responses. If you need more well, tough. fix it.

class directory_server
{
public:
    // Add a directory to the server's allowed responses.
    // if recursive is true then all sub directories will be recursivly added as well
    static bool add_dir(httpserver::webserver &ws, const std::string &directory, bool recursive = true, const std::string &path = "", const std::string &basedir = ".");

    // Add a file to the server's allowed response.
    static bool add_file(httpserver::webserver &ws, const std::string &filename, const std::string &basedir = "");
};

#endif // DIRECTORY_SERVER_H
