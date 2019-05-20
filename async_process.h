#ifndef ASYNC_PROCESS_H
#define ASYNC_PROCESS_H

#include <vector>
#include <string>

#include <semaphore.h>

class async_process
{
    int pipe_stdout[2] = { 0, 0 };
    int pipe_stderr[2] = { 0, 0 };
    pid_t procID = 0;
    std::string m_process;
    std::vector<std::string> m_args;
    void close_pipes();

public:
    async_process(const std::string &proc_name, const std::string &arg1 = "", const std::string &arg2 = "", const std::string &arg3 = "");
    async_process(const std::string &proc_name, const std::vector<std::string> &args);
    ~async_process();
    bool exec();
    bool active();
    void wait();
    void stop();
    void kill();
    std::string readStdOut();
    std::string readStdErr();
};

#endif // ASYNC_PROCESS_H
