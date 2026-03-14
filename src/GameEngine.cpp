#include "epch.h"
#include "GameEngine.h"
#include "World.h"

namespace Skeleton {

GameEngine::GameEngine()
    : m_WorkGuard(std::make_unique<asio::io_context::work>(m_WorkContext))
{
    std::cout << "GameEngine::GameEngine() - Constructor completed" << std::endl;
}

GameEngine::~GameEngine() {
    if (m_Running) {
        stop();
    }
}

void GameEngine::pushTask(std::unique_ptr<Task> task) {
    {
        std::lock_guard<std::mutex> lock(m_TaskQueueMutex);
        m_TaskQueue.push_back(std::move(task));
    }
    m_TaskQueueCV.notify_one();
}

void GameEngine::submitLogic(std::function<void()> runnable) {
    class LambdaTask : public Task {
        std::function<void()> m_Func;
    public:
        LambdaTask(std::function<void()> func) : m_Func(std::move(func)) {}
        void execute(GameEngine& engine) override {
            try {
                m_Func();
            } catch (const std::exception& e) {
                LOG_ERROR("Exception in logic task: {}", e.what());
            }
        }
    };
    
    pushTask(std::make_unique<LambdaTask>(std::move(runnable)));
}

void GameEngine::submitWork(std::function<void()> runnable) {
    asio::post(m_WorkContext, [runnable = std::move(runnable)]() {
        try {
            runnable();
        } catch (const std::exception& e) {
            LOG_ERROR("Exception in work task: {}", e.what());
        }
    });
}

void GameEngine::start() {
    if (m_Running.exchange(true)) {
        throw std::runtime_error("GameEngine is already running");
    }

    LOG_INFO("GameEngine::start() - Starting work thread");
    m_WorkThread = std::thread([this]() {
        LOG_INFO("Work thread started");
        m_WorkContext.run();
        LOG_INFO("Work thread finished");
    });

    LOG_INFO("GameEngine::start() - Starting logic thread");
    m_LogicThread = std::thread([this]() {
        LOG_INFO("Logic thread started");
        run();
        LOG_INFO("Logic thread finished");
    });

    LOG_INFO("GameEngine::start() - Starting pulse thread");
    m_PulseThread = std::thread([this]() {
        LOG_INFO("Pulse thread started");
        runPulse();
        LOG_INFO("Pulse thread finished");
    });

    LOG_INFO("GameEngine::start() - Starting fast pulse thread (face direction updates)");
    m_FastPulseThread = std::thread([this]() {
        LOG_INFO("Fast pulse thread started (50ms cycle for face direction)");
        runFastPulse();
        LOG_INFO("Fast pulse thread finished");
    });
    LOG_INFO("GameEngine::start() - All threads started");
}

void GameEngine::stop() {
    if (!m_Running.exchange(false)) {
        throw std::runtime_error("GameEngine is already stopped");
    }

    m_TaskQueueCV.notify_all();

    if (m_FastPulseThread.joinable()) {
        m_FastPulseThread.join();
    }

    if (m_PulseThread.joinable()) {
        m_PulseThread.join();
    }

    if (m_LogicThread.joinable()) {
        m_LogicThread.join();
    }

    m_WorkGuard.reset();
    m_WorkContext.stop();

    if (m_WorkThread.joinable()) {
        m_WorkThread.join();
    }
}

void GameEngine::run() {
    while (m_Running) {
        std::unique_ptr<Task> task;
        
        {
            std::unique_lock<std::mutex> lock(m_TaskQueueMutex);
            m_TaskQueueCV.wait(lock, [this] {
                return !m_TaskQueue.empty() || !m_Running;
            });

            if (!m_Running && m_TaskQueue.empty()) {
                break;
            }

            if (!m_TaskQueue.empty()) {
                task = std::move(m_TaskQueue.front());
                m_TaskQueue.pop_front();
            }
        }

        if (task) {
            try {
                task->execute(*this);
            } catch (const std::exception& e) {
                LOG_ERROR("Exception executing task: {}", e.what());
            }
        }
    }
}

void GameEngine::runPulse() {
    using namespace std::chrono;
    const int CYCLE_TIME_MS = 600;

    std::this_thread::sleep_for(milliseconds(1000));

    while (m_Running) {
        auto start = steady_clock::now();

        try {
            incrementTick();
            World::Instance().Process();
        } catch (const std::exception& e) {
            LOG_ERROR("Exception in pulse: {}", e.what());
        }

        auto elapsed = duration_cast<milliseconds>(steady_clock::now() - start).count();
        if (elapsed < CYCLE_TIME_MS) {
            std::this_thread::sleep_for(milliseconds(CYCLE_TIME_MS - elapsed));
        } else {
            LOG_WARN("Pulse overran: {}ms", elapsed);
        }
    }
}

void GameEngine::runFastPulse() {
    using namespace std::chrono;
    const int FAST_CYCLE_TIME_MS = 50;  // 20 updates per second

    std::this_thread::sleep_for(milliseconds(1000));

    while (m_Running) {
        auto start = steady_clock::now();

        try {
            World::Instance().ProcessFastUpdates();
        } catch (const std::exception& e) {
            LOG_ERROR("Exception in fast pulse: {}", e.what());
        }

        auto elapsed = duration_cast<milliseconds>(steady_clock::now() - start).count();
        if (elapsed < FAST_CYCLE_TIME_MS) {
            std::this_thread::sleep_for(milliseconds(FAST_CYCLE_TIME_MS - elapsed));
        }
    }
}

}
