#pragma once
#include "epch.h"
#include "Task.h"

namespace Skeleton {

class GameEngine {
public:
    GameEngine();
    ~GameEngine();

    void pushTask(std::unique_ptr<Task> task);

    void submitLogic(std::function<void()> runnable);
    void submitWork(std::function<void()> runnable);

    void start();
    void stop();

    bool isRunning() const { return m_Running; }
    
    uint64_t getTickCount() const { return m_TickCount; }
    void incrementTick() { m_TickCount++; }

private:
    void run();
    void runPulse();
    void runFastPulse();  // High-frequency updates (face direction)
    void processLogicQueue();

private:
    std::atomic<bool> m_Running{false};
    std::atomic<uint64_t> m_TickCount{0};

    std::deque<std::unique_ptr<Task>> m_TaskQueue;
    std::mutex m_TaskQueueMutex;
    std::condition_variable m_TaskQueueCV;

    std::thread m_LogicThread;
    std::thread m_PulseThread;
    std::thread m_FastPulseThread;  // High-frequency face direction updates

    asio::io_context m_WorkContext;
    std::unique_ptr<asio::io_context::work> m_WorkGuard;
    std::thread m_WorkThread;
};

}
