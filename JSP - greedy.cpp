#include <iostream>
#include <vector>
#include <algorithm>
#include <chrono>

using namespace std;

class Job {
public:
    int jobId;
    int processingTime;

    Job(int id, int processingTime) : jobId(id), processingTime(processingTime) {}
};

class Machine {
public:
    int machineId;
    int totalLoad;
    vector<int> assignedJobs;

    Machine(int id) : machineId(id), totalLoad(0) {}

    void addJob(const Job& job) 
    {
        totalLoad += job.processingTime;
        assignedJobs.push_back(job.jobId);
    }

    bool operator<(const Machine& other) const 
    {
        return totalLoad < other.totalLoad;
    }
};

class JobSchedule {
private:
    vector<Machine> machines;
    vector<Job> jobs;

public:
    JobSchedule(int numMachines, const vector<Job>& jobs) : jobs(jobs) 
    {
        for (int i = 0; i < numMachines; ++i) 
        {
            machines.emplace_back(i);
        }
    }

    void scheduleJobs()
    {
        for (const auto& job : jobs) 
        {
            auto leastLoadedMachine = min_element(machines.begin(), machines.end());
            leastLoadedMachine->addJob(job);
        }
    }

    void printSchedule() const
    {
        for (const auto& machine : machines) 
        {
            cout << "Machine " << machine.machineId << ": " << machine.totalLoad << " units (Jobs:";
            for (int jobId : machine.assignedJobs)
            {
                cout << " " << jobId;
            }
            cout << ")\n";
        }
    }
};

template<typename Func>
long long measureTime(Func func) {
    auto start = chrono::high_resolution_clock::now();
    func();
    auto end = chrono::high_resolution_clock::now();
    return chrono::duration_cast<std::chrono::microseconds>(end - start).count();
}

void runTestCase(const string& testName, const vector<Job>& jobs, int numMachines) {
    cout << testName << "\n";
    JobSchedule scheduler(numMachines, jobs);
    long long runtime = measureTime([&]() {
        scheduler.scheduleJobs();
        });
    scheduler.printSchedule();
    cout << "Runtime: " << runtime << " microseconds\n\n";
}

int main() {
    runTestCase("Test Case 1: Basic Test", { {1, 2}, {2, 3}, {3, 5}, {4, 7}, {5, 1} }, 2);
    runTestCase("Test Case 2: All Jobs of Equal Length", { {1, 5}, {2, 5}, {3, 5}, {4, 5} }, 2);
    runTestCase("Test Case 3: More Machines than Jobs", { {1, 6}, {2, 2}, {3, 8} }, 4);
    runTestCase("Test Case 4: Single Job", { {1, 10} }, 3);
    runTestCase("Test Case 5: Complex Test", { {1, 2}, {2, 1}, {3, 2}, {4, 7}, {5, 3}, {6, 6} }, 3);
    return 0;
}