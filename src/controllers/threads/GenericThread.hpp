#ifndef CONTROLLERS_THREADS_GENERICTHREAD_HPP_
#define CONTROLLERS_THREADS_GENERICTHREAD_HPP_

#include <string>
#include <thread>

#define THREAD_CONTROL_PIPE_OUT (1)
#define THREAD_CONTROL_PIPE_IN (0)

namespace Lego
{

class GenericThread
{
public:
    enum class Control
    {
        UNKNOWN,
        QUIT,
        TASK
    };

    GenericThread(const std::string& threadIdentifier, bool silent = false);
    virtual ~GenericThread();

    void start(void);
    void stop(void);

    bool threadHasFinished(void) const;
    std::string threadIdentifier(void) const;

protected:
    std::string m_threadIdentifier;
    bool m_silent;
    int m_controlPipe[2];
    bool m_threadFinished;
    std::thread* m_genericThread;
    int m_periodicTaskInterval;

    virtual void setup(void) = 0;
    virtual void task(fd_set& descriptors) = 0;
    virtual void periodicTask(void);
    virtual void shutdown(void);
    virtual void updateDescriptors(int& maxDescriptor, fd_set& descriptors, timeval& timeout);
    void setPeriodicTaskInterval(int seconds);

    int max(int a, int b) const;

private:
    virtual void run(void);
};

} /* namespace Lego */

#endif /* CONTROLLERS_THREADS_GENERICTHREAD_HPP_ */
