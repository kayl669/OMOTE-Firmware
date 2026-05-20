#include <thread>
#include <queue>
#include <mutex>
#include <string>
#include <cstdio>
#include <cstring>

static std::queue<std::string> lineQueue;
static std::mutex              queueMutex;

static void stdinThread() {
    char buf[128];
    while (fgets(buf, sizeof(buf), stdin) != nullptr) {
        std::lock_guard<std::mutex> lock(queueMutex);
        lineQueue.push(std::string(buf));
    }
}

void serialIOInit() {
    std::thread(stdinThread).detach();
    printf("Waiting for commands. Type 'help'.\n");
}

bool serialIOReadLine(char* buf, size_t maxLen) {
    std::lock_guard<std::mutex> lock(queueMutex);
    if (lineQueue.empty()) return false;

    std::string line = lineQueue.front();
    lineQueue.pop();
    strncpy(buf, line.c_str(), maxLen - 1);
    buf[maxLen - 1] = '\0';
    return true;
}
