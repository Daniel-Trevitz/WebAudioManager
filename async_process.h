#ifndef ASYNC_PROCESS_H
#define ASYNC_PROCESS_H

#include <vector>
#include <string>

#include <semaphore.h>

class async_process
{
    int pipe_stdout[2];
    int pipe_stderr[2];
    sem_t lock;
    pid_t procID;
    std::string m_process;
    std::vector<std::string> m_args;

public:
    async_process(const std::string &proc_name, const std::vector<std::string> &args);
    ~async_process();
    bool exec();
    void wait();
    std::string readStdOut();
    std::string readStdErr();
};

#endif // ASYNC_PROCESS_H
