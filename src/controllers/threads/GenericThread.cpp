#include <unistd.h>
#include <cassert>
#include <cstring>
#include <controllers/threads/GenericThread.hpp>
#include <utils/Log.hpp>

#define PERIODIC_TASK_INTERVAL_MIN_SECONDS (5)
#define PERIODIC_TASK_INTERVAL_DEFAULT_SECONDS (20)

namespace Lego
{

GenericThread::GenericThread(const std::string& threadIdentifier, bool silent):
    m_threadIdentifier(threadIdentifier),
    m_silent(silent),
    m_threadFinished(false),
    m_genericThread(nullptr),
    m_periodicTaskInterval(PERIODIC_TASK_INTERVAL_DEFAULT_SECONDS)
{
    bzero(m_controlPipe, sizeof(m_controlPipe));
    if (pipe(m_controlPipe) != 0)
    {
        throw std::runtime_error(threadIdentifier + ": could not set up internal control pipes");
    }
}

GenericThread::~GenericThread()
{
    close(m_controlPipe[THREAD_CONTROL_PIPE_OUT]);
    close(m_controlPipe[THREAD_CONTROL_PIPE_IN]);
}

void GenericThread::start(void)
{
    if (m_genericThread)
    {
        throw std::runtime_error("thread was already started");
    }

    setup();

    m_genericThread = new std::thread(&GenericThread::run, this);
}

void GenericThread::stop(void)
{
    if (!m_genericThread) {
        return;
    }

    Control threadControl = Control::QUIT;
    ssize_t bytesWritten = write(m_controlPipe[THREAD_CONTROL_PIPE_OUT], &threadControl, sizeof(threadControl));

    m_genericThread->join();
    delete m_genericThread;
    m_genericThread = nullptr;

    if (bytesWritten <= 0)
    {
        throw std::runtime_error("could send quit signal to the thread");
    }
}

bool GenericThread::threadHasFinished(void) const
{
    return m_threadFinished;
}

std::string GenericThread::threadIdentifier(void) const
{
    return m_threadIdentifier;
}

void GenericThread::periodicTask(void)
{
}

void GenericThread::shutdown(void)
{
}

void GenericThread::updateDescriptors(int& maxDescriptor, fd_set& descriptors, timeval& timeout)
{
}

void GenericThread::setPeriodicTaskInterval(int seconds)
{
    m_periodicTaskInterval = max(PERIODIC_TASK_INTERVAL_MIN_SECONDS, seconds);
}

int GenericThread::max(int a, int b) const
{
    return a > b ? a : b;
}

void GenericThread::run(void)
{
    timespec lastRunPeriodicTask;

    if (!m_silent) LOG_INFO("Started thread \"" << m_threadIdentifier << "\"");

    clock_gettime(CLOCK_MONOTONIC_COARSE, &lastRunPeriodicTask);

    do
    {
        int maxDescriptor;
        fd_set descriptors;
        timeval timeout;

        timeout.tv_sec = m_periodicTaskInterval / 2;
        timeout.tv_usec = 0;

        FD_ZERO(&descriptors);
        FD_SET(m_controlPipe[THREAD_CONTROL_PIPE_IN], &descriptors);
        maxDescriptor = m_controlPipe[THREAD_CONTROL_PIPE_IN];

        updateDescriptors(maxDescriptor, descriptors, timeout);

        if (select(maxDescriptor + 1, &descriptors, nullptr, nullptr, &timeout) > 0)
        {
            if (FD_ISSET(m_controlPipe[THREAD_CONTROL_PIPE_IN], &descriptors))
            {
                Control threadControl = Control::UNKNOWN;
                ssize_t readBytes = read(m_controlPipe[THREAD_CONTROL_PIPE_IN], &threadControl, sizeof(threadControl));
                if (readBytes < (ssize_t)sizeof(threadControl))
                {
                    LOG_WARNING("Thread \"" << m_threadIdentifier << "\" received invalid signal. Aborting thread.");
                    break;
                }
                else if (threadControl == Control::QUIT)
                {
                    if (!m_silent) LOG_DEBUG("Thread \"" << m_threadIdentifier << "\" received quit signal.");
                    break;
                }
                else if (threadControl == Control::TASK)
                {
                    try
                    {
                        task(descriptors);
                    }
                    catch(const std::exception& e)
                    {
                        LOG_ERROR("Task of thread \"" << m_threadIdentifier << "\" failed (" << e.what() << ")!");
                        break;
                    }
                }
            }
            else
            {
                try
                {
                    task(descriptors);
                }
                catch(const std::exception& e)
                {
                    LOG_ERROR("Task of thread \"" << m_threadIdentifier << "\" failed (" << e.what() << ")!");
                    break;
                }
            }
        }

        timespec currentTime;
        clock_gettime(CLOCK_MONOTONIC_COARSE, &currentTime);
        if (currentTime.tv_sec >= (lastRunPeriodicTask.tv_sec + m_periodicTaskInterval))
        {
            try
            {
                periodicTask();
            }
            catch(const std::exception& e)
            {
                LOG_ERROR("Periodic task of thread \"" << m_threadIdentifier << "\" failed (" << e.what() << ")!");
                break;
            }

            lastRunPeriodicTask = currentTime;
        }
    }
    while (!m_threadFinished);
    m_threadFinished = true;

    shutdown();

    if (!m_silent) LOG_INFO("Stopped thread \"" << m_threadIdentifier << "\"");
}

} /* namespace Lego */
