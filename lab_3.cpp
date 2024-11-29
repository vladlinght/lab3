#include <iostream>
#include <vector>
#include <random>
#include <queue>
#include <algorithm>
#include <iomanip>

struct Process {
    int id;
    int arrivalTime;  // Час прибуття
    int burstTime;    // Час виконання
    int remainingTime; // Залишковий час виконання
    int priority;     // Пріоритет процесу
    int startTime;    // Час початку виконання
    int finishTime;   // Час завершення виконання
    int waitingTime;  // Час очікування
    int turnAroundTime; // Час обробки (waitingTime + burstTime)
    
    Process(int _id, int _arrivalTime, int _burstTime, int _priority)
        : id(_id), arrivalTime(_arrivalTime), burstTime(_burstTime),
          remainingTime(_burstTime), priority(_priority), startTime(-1), finishTime(-1),
          waitingTime(0), turnAroundTime(0) {}
};

// Випадкове генерування параметрів процесів
std::vector<Process> generateProcesses(int n) {
    std::vector<Process> processes;
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> arrivalDist(0, 10);
    std::uniform_int_distribution<> burstDist(1, 10);
    std::uniform_int_distribution<> priorityDist(1, 5);
    
    for (int i = 0; i < n; ++i) {
        int arrivalTime = arrivalDist(gen);
        int burstTime = burstDist(gen);
        int priority = priorityDist(gen);
        processes.push_back(Process(i + 1, arrivalTime, burstTime, priority));
    }
    
    return processes;
}

// Алгоритм Shortest Job First (SJF)
void sjf(std::vector<Process>& processes) {
    int time = 0;
    std::sort(processes.begin(), processes.end(), [](const Process& a, const Process& b) {
        return a.arrivalTime < b.arrivalTime;
    });

    std::vector<Process*> readyQueue;
    std::vector<Process*> processesReady = processes;
    
    while (!processesReady.empty() || !readyQueue.empty()) {
        for (auto it = processesReady.begin(); it != processesReady.end(); ) {
            if (it->arrivalTime <= time) {
                readyQueue.push_back(&(*it));
                it = processesReady.erase(it);
            } else {
                ++it;
            }
        }

        if (!readyQueue.empty()) {
            // Сортуємо чергу за часом виконання
            std::sort(readyQueue.begin(), readyQueue.end(), [](const Process* a, const Process* b) {
                return a->burstTime < b->burstTime;
            });
            Process* current = readyQueue.front();
            readyQueue.erase(readyQueue.begin());
            
            if (current->startTime == -1) current->startTime = time;
            
            time += current->burstTime;
            current->finishTime = time;
            current->turnAroundTime = current->finishTime - current->arrivalTime;
            current->waitingTime = current->turnAroundTime - current->burstTime;
        } else {
            ++time; // Час рухається вперед, якщо черга пуста
        }
    }

    // Виведення результатів
    for (const auto& process : processes) {
        std::cout << "Process ID: " << process.id << " Arrival: " << process.arrivalTime
                  << " Burst: " << process.burstTime << " Start: " << process.startTime
                  << " Finish: " << process.finishTime << " Waiting: " << process.waitingTime
                  << " Turnaround: " << process.turnAroundTime << std::endl;
    }
}

// Механізм старіння для пріоритетного планування
void prioritySchedulingWithAging(std::vector<Process>& processes, int agingInterval) {
    int time = 0;
    std::sort(processes.begin(), processes.end(), [](const Process& a, const Process& b) {
        return a.arrivalTime < b.arrivalTime;
    });
    
    std::vector<Process*> readyQueue;
    std::vector<Process*> processesReady = processes;
    
    while (!processesReady.empty() || !readyQueue.empty()) {
        for (auto it = processesReady.begin(); it != processesReady.end(); ) {
            if (it->arrivalTime <= time) {
                readyQueue.push_back(&(*it));
                it = processesReady.erase(it);
            } else {
                ++it;
            }
        }

        if (!readyQueue.empty()) {
            // Старіння: збільшення пріоритету для процесів, які довго чекають
            for (auto& p : readyQueue) {
                if (time - p->arrivalTime >= agingInterval) {
                    p->priority = std::max(1, p->priority - 1); // Підвищення пріоритету
                }
            }

            // Сортуємо за пріоритетом
            std::sort(readyQueue.begin(), readyQueue.end(), [](const Process* a, const Process* b) {
                return a->priority < b->priority || (a->priority == b->priority && a->arrivalTime < b->arrivalTime);
            });

            Process* current = readyQueue.front();
            readyQueue.erase(readyQueue.begin());
            
            if (current->startTime == -1) current->startTime = time;
            
            time += current->burstTime;
            current->finishTime = time;
            current->turnAroundTime = current->finishTime - current->arrivalTime;
            current->waitingTime = current->turnAroundTime - current->burstTime;
        } else {
            ++time; // Час рухається вперед, якщо черга пуста
        }
    }

    // Виведення результатів
    for (const auto& process : processes) {
        std::cout << "Process ID: " << process.id << " Arrival: " << process.arrivalTime
                  << " Burst: " << process.burstTime << " Priority: " << process.priority
                  << " Start: " << process.startTime << " Finish: " << process.finishTime
                  << " Waiting: " << process.waitingTime << " Turnaround: " << process.turnAroundTime << std::endl;
    }
}

// Функція для обчислення середнього часу очікування та обробки
void calculateAverageTimes(const std::vector<Process>& processes) {
    double totalWaitingTime = 0, totalTurnaroundTime = 0;
    for (const auto& process : processes) {
        totalWaitingTime += process.waitingTime;
        totalTurnaroundTime += process.turnAroundTime;
    }
    
    std::cout << "Average Waiting Time: " << totalWaitingTime / processes.size() << std::endl;
    std::cout << "Average Turnaround Time: " << totalTurnaroundTime / processes.size() << std::endl;
}

int main() {
    int n = 5; // Кількість процесів
    std::vector<Process> processes = generateProcesses(n);

    std::cout << "Shortest Job First Scheduling:\n";
    sjf(processes);
    calculateAverageTimes(processes);

    std::cout << "\nPriority Scheduling with Aging:\n";
    prioritySchedulingWithAging(processes, 4); // Старіння після 4 одиниць часу
    calculateAverageTimes(processes);

    return 0;
}
