#include <iostream>
#include <thread>
#include <chrono>
#include <atomic>
#include <mutex>
#include <ftxui/dom/elements.hpp>
#include <ftxui/screen/screen.hpp>
#include <ftxui/component/component.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include "CpuMonitor.hpp"
#include "MemoryMonitor.hpp"
#include "ProcessMonitor.hpp"
#include <iomanip>
#include <sstream>

// Helper to format bytes to human readable string
std::string formatBytes(size_t bytes) {
    const char* suffixes[] = {"B", "KB", "MB", "GB", "TB"};
    int i = 0;
    double dblBytes = static_cast<double>(bytes);
    if (bytes > 1024) {
        for (i = 0; (bytes / 1024) > 0 && i < 4; i++, bytes /= 1024)
            dblBytes = bytes / 1024.0;
    }
    std::stringstream ss;
    ss << std::fixed << std::setprecision(1) << dblBytes << " " << suffixes[i];
    return ss.str();
}

int main() {
    using namespace ftxui;
    
    // State shared between updater thread and renderer
    struct AppState {
        std::atomic<int> cpuUsage{0};
        std::atomic<int> memPercent{0};
        std::atomic<size_t> memUsed{0};
        std::atomic<size_t> memTotal{0};
        
        std::mutex processMutex;
        std::vector<ProcessInfo> processes;
    } state;

    // Monitors (only accessed by updater thread)
    CpuMonitor cpuMonitor;
    MemoryMonitor memoryMonitor;
    ProcessMonitor processMonitor;

    // Initial fetch
    memoryMonitor.update();
    state.memTotal = memoryMonitor.getTotalMemory();

    auto screen = ScreenInteractive::Fullscreen();

    // Component for the process list
    auto renderer = Renderer([&] {
        // Render only reads the state
        int cpu = state.cpuUsage;
        int memPercent = state.memPercent;
        std::string memUsed = formatBytes(state.memUsed);
        std::string memTotal = formatBytes(state.memTotal);
        
        // Create process table
        std::vector<std::vector<std::string>> tableData;
        tableData.push_back({"PID", "Name", "CPU %", "Memory"}); // Header

        {
            std::lock_guard<std::mutex> lock(state.processMutex);
            int count = 0;
            for (const auto& p : state.processes) {
                if (count++ > 20) break; // Show top 20 for now to fit screen
                
                std::stringstream cpuStream;
                cpuStream << std::fixed << std::setprecision(1) << p.cpuUsage;
                
                tableData.push_back({
                    std::to_string(p.pid),
                    p.name,
                    cpuStream.str(),
                    formatBytes(p.memoryUsage)
                });
            }
        }

        // Build table element
        std::vector<Element> rows;
        for (size_t i = 0; i < tableData.size(); ++i) {
            auto& row = tableData[i];
            auto rowElement = hbox({
                text(row[0]) | size(WIDTH, EQUAL, 8),
                text(row[1]) | flex,
                text(row[2]) | size(WIDTH, EQUAL, 8),
                text(row[3]) | size(WIDTH, EQUAL, 12),
            });
            
            if (i == 0) rowElement = rowElement | bold | inverted; // Header style
            rows.push_back(rowElement);
        }

        return vbox({
            text("UIMonitor - System Resource Monitor") | bold | center,
            separator(),
            hbox({
                text("CPU Usage: ") | size(WIDTH, EQUAL, 12),
                gauge(cpu / 100.0f) | flex,
                text(" " + std::to_string(cpu) + "%") | size(WIDTH, EQUAL, 6),
            }) | border,
            hbox({
                text("RAM Usage: ") | size(WIDTH, EQUAL, 12),
                gauge(memPercent / 100.0f) | flex,
                text(" " + memUsed + "/" + memTotal) | size(WIDTH, EQUAL, 20),
            }) | border,
            separator(),
            text("Top Processes") | bold,
            vbox(std::move(rows)) | border,
            text("Press Ctrl+C to exit") | center | color(Color::GrayDark),
        });
    });

    // Update loop to refresh data
    std::thread updater([&] {
        while (true) {
            // Perform heavy system calls here
            cpuMonitor.update();
            memoryMonitor.update();
            processMonitor.update();

            // Update shared state
            state.cpuUsage = (int)cpuMonitor.getGlobalUsage();
            state.memPercent = (int)memoryMonitor.getMemoryUsagePercentage();
            state.memUsed = memoryMonitor.getUsedMemory();
            
            {
                std::lock_guard<std::mutex> lock(state.processMutex);
                state.processes = processMonitor.getProcesses();
            }
            
            // Trigger a redraw
            screen.PostEvent(Event::Custom);
            
            using namespace std::chrono_literals;
            std::this_thread::sleep_for(1s); // Update processes every 1s
        }
    });
    updater.detach();

    screen.Loop(renderer);

    return 0;
}
