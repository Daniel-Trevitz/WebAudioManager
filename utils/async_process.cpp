#include "async_process.h"

#include <fcntl.h>
#include <unistd.h>
#include <cstring>
#include <csignal>

#include <iostream>

#include <signal.h>
#include <sys/wait.h>
#include <sys/types.h>

//#define DEBUG_EXEC

void async_process::close_pipes()
{
    if(pipe_stdout[0]) close(pipe_stdout[0]);
    if(pipe_stdout[1]) close(pipe_stdout[1]);
    if(pipe_stderr[0]) close(pipe_stderr[0]);
    if(pipe_stderr[1]) close(pipe_stderr[1]);

    pipe_stdout[0] = 0;
    pipe_stdout[1] = 0;
    pipe_stderr[0] = 0;
    pipe_stderr[1] = 0;
}

async_process::async_process(const std::string &proc_name, const std::string &arg1, const std::string &arg2,
                             const std::string &arg3, const std::string &arg4, const std::string &arg5) :
    m_process(proc_name)
{
    m_args.push_back(proc_name);
    do
    {
        if(arg1.empty()) break;
        m_args.push_back(arg1);
        if(arg2.empty()) break;
        m_args.push_back(arg2);
        if(arg3.empty()) break;
        m_args.push_back(arg3);
        if(arg4.empty()) break;
        m_args.push_back(arg4);
        if(arg5.empty()) break;
        m_args.push_back(arg5);

    } while(false);
}

async_process::async_process(const std::string &proc_name, const std::vector<std::string> &args) :
    m_process(proc_name),
    m_args(args)
{
    m_args.insert(m_args.begin(), proc_name);
}

async_process::~async_process()
{
    close_pipes();

    if(procID)
    {
        stop();
        kill();
    }
}

bool async_process::exec()
{
    close_pipes();

    // Create a pipes for between parrent and child
    if (pipe2(pipe_stdout, O_NONBLOCK) == -1)
    {
        std::cerr << __PRETTY_FUNCTION__ << "stdout pipe failed: "<< errno << std::endl;
        return false;
    }
    if (pipe2(pipe_stderr, O_NONBLOCK) == -1)
    {
        std::cerr << __PRETTY_FUNCTION__ << "stdout pipe failed: "<< errno << std::endl;
        return false;
    }

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
    if(ret < 0)
        std::cerr << "Failed to exec: " << m_process << " errno: " << errno << std::endl;
    exit(0); // child done
}

bool async_process::active() const
{
    if(procID == 0)
        return false;
    return 0 == ::kill(procID, 0);
}

void async_process::wait()
{
    if(procID != 0)
        waitpid(procID, nullptr, 0);
}

void async_process::stop()
{
    if(procID == 0)
        return;

    ::kill(procID, SIGSTOP);
    close_pipes();
}

void async_process::kill()
{
    if(procID == 0)
        return;

    ::kill(procID, SIGKILL);
    wait();
    close_pipes();
}

std::string async_process::readStdOut()
{
    char buffer[4096];
    memset(buffer, 0, sizeof(buffer));
    ssize_t count = read(pipe_stdout[0], buffer, sizeof(buffer));

    // TODO: Error handling... somehow
    // if ((count == -1) && (errno != EINTR))
    //    return "";
    if(count < 1)
        return "";

    return std::string(buffer);
}

std::string async_process::readStdErr()
{
    char buffer[4096];
    memset(buffer, 0, sizeof(buffer));
    ssize_t count = read(pipe_stderr[0], buffer, sizeof(buffer));

    // TODO: Error handling... somehow
    // if ((count == -1) && (errno != EINTR))
    //    return "";
    if(count < 1)
        return "";

    return std::string(buffer);
}
