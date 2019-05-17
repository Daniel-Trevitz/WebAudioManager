#include "async_process.h"

#include <fcntl.h>
#include <unistd.h>
#include <cstring>

#include <iostream>

#include <sys/wait.h>

//#define DEBUG_EXEC

async_process::async_process(const std::string &procname, const std::vector<std::string> &args) :
    m_process(procname),
    m_args(args)
{
    m_args.insert(m_args.begin(), procname);
//    sem_init(&lock, 0,0);
}

async_process::~async_process()
{
//    sem_destroy(&lock);
}

bool async_process::exec()
{
    // Create a pipes for between parrent and child
    if (pipe2(pipe_stdout, O_NONBLOCK) == -1)
        return false;
    if (pipe2(pipe_stderr, O_NONBLOCK) == -1)
        return false;

    procID = fork();
    if(procID != 0)
        return true;

    // Child Process
    while ((dup2(pipe_stdout[1], STDOUT_FILENO) == -1) && (errno == EINTR)) {}
    while ((dup2(pipe_stderr[1], STDERR_FILENO) == -1) && (errno == EINTR)) {}

    const size_t argc = m_args.size();
    std::vector<const char *> argv;
    for(size_t i = 0; i < argc; i++)
        argv.push_back(m_args.at(i).c_str());
    argv.push_back(nullptr);

#ifdef DEBUG_EXEC
    std::cout << "Executing: " << m_process << " ";
    for(size_t i = 0; i < argc; i++)
        std::cout << argv[i] << " ";
    std::cout << std::endl;
#endif

    int ret = execv(m_process.c_str(), (char * const *)argv.data());
    exit(0); // child done
}

void async_process::wait()
{
    waitpid(procID, nullptr, 0);
}

std::string async_process::readStdOut()
{
    char buffer[4096];

    ssize_t count = read(pipe_stdout[0], buffer, sizeof(buffer));

    // TODO: Error handling... somehow
    // if ((count == -1) && (errno != EINTR))
    //    return "";
    if(count == -1)
        return "";

    return std::string(buffer);
}

std::string async_process::readStdErr()
{
    char buffer[4096];

    ssize_t count = read(pipe_stderr[0], buffer, sizeof(buffer));

    // TODO: Error handling... somehow
    // if ((count == -1) && (errno != EINTR))
    //    return "";
    if(count == -1)
        return "";

    return std::string(buffer);
}
