#include <iostream>
#include <vector>
#include <algorithm>
#include <random>
#include <chrono>

using namespace std;

class Job
{
public:
    int jobId;
    int processingTime;

    Job(int id, int time) : jobId(id), processingTime(time) {}
};

class Schedule
{
public:
    vector<vector<int>> machineJobs;
    int fitness;

    Schedule(int numMachines) : machineJobs(numMachines), fitness(0) {}

    void addJob(int jobIndex, int machineIndex)
    {
        machineJobs[machineIndex].push_back(jobIndex);
    }

    int calculateFitness(const vector<Job> &jobs)
    {
        vector<int> machineTimes(machineJobs.size(), 0);
        for (size_t i = 0; i < machineJobs.size(); ++i)
        {
            for (int jobIndex : machineJobs[i])
            {
                machineTimes[i] += jobs[jobIndex].processingTime;
            }
        }
        fitness = *max_element(machineTimes.begin(), machineTimes.end());
        return fitness;
    }
};

class JobSchedule
{
private:
    vector<Job> jobs;
    int numMachines;
    int populationSize;
    int generations;
    double mutationRate;
    vector<Schedule> population;
    random_device rd;
    mt19937 gen;

public:
    JobSchedule(const vector<Job> &j, int m, int popSize, int gen, double mutRate)
        : jobs(j), numMachines(m), populationSize(popSize), generations(gen), mutationRate(mutRate), gen(rd()) {}

    Schedule evolve()
    {
        initializePopulation();
        for (int i = 0; i < generations; ++i)
        {
            evaluatePopulation();
            vector<Schedule> newPopulation;
            while (newPopulation.size() < populationSize)
            {
                Schedule parent1 = selectParent();
                Schedule parent2 = selectParent();
                Schedule child = crossover(parent1, parent2);
                mutate(child);
                newPopulation.push_back(child);
            }
            population = newPopulation;
        }
        evaluatePopulation();
        return *min_element(population.begin(), population.end(),
                            [](const Schedule &a, const Schedule &b)
                            { return a.fitness < b.fitness; });
    }

private:
    void initializePopulation()
    {
        for (int i = 0; i < populationSize; ++i)
        {
            Schedule schedule(numMachines);
            for (size_t j = 0; j < jobs.size(); ++j)
            {
                int randomMachine = uniform_int_distribution<>(0, numMachines - 1)(gen);
                schedule.addJob(j, randomMachine);
            }
            population.push_back(schedule);
        }
    }

    void evaluatePopulation()
    {
        for (auto &schedule : population)
        {
            schedule.calculateFitness(jobs);
        }
    }

    Schedule selectParent()
    {
        uniform_int_distribution<> dis(0, populationSize - 1);
        const Schedule &a = population[dis(gen)];
        const Schedule &b = population[dis(gen)];
        return (a.fitness < b.fitness) ? a : b;
    }

    Schedule crossover(const Schedule &parent1, const Schedule &parent2)
    {
        Schedule child(numMachines);
        for (size_t i = 0; i < jobs.size(); ++i)
        {
            if (uniform_real_distribution<>(0, 1)(gen) < 0.5)
            {
                int machineIndex = findMachineForJob(parent1, i);
                child.addJob(i, machineIndex);
            }
            else
            {
                int machineIndex = findMachineForJob(parent2, i);
                child.addJob(i, machineIndex);
            }
        }
        return child;
    }

    int findMachineForJob(const Schedule &schedule, int jobIndex)
    {
        for (size_t i = 0; i < schedule.machineJobs.size(); ++i)
        {
            if (find(schedule.machineJobs[i].begin(), schedule.machineJobs[i].end(), jobIndex) != schedule.machineJobs[i].end())
            {
                return i;
            }
        }
        return 0;
    }

    void mutate(Schedule &schedule)
    {
        for (size_t i = 0; i < jobs.size(); ++i)
        {
            if (uniform_real_distribution<>(0, 1)(gen) < mutationRate)
            {
                int oldMachine = findMachineForJob(schedule, i);
                int newMachine;
                do
                {
                    newMachine = uniform_int_distribution<>(0, numMachines - 1)(gen);
                } while (newMachine == oldMachine);

                auto &oldMachineJobs = schedule.machineJobs[oldMachine];
                oldMachineJobs.erase(remove(oldMachineJobs.begin(), oldMachineJobs.end(), i), oldMachineJobs.end());
                schedule.machineJobs[newMachine].push_back(i);
            }
        }
    }
};

void runTestCase(const string &testName, const vector<Job> &jobs, int numMachines)
{
    cout << testName << "\n";
    JobSchedule scheduler(jobs, numMachines, 100, 1000, 0.01);
    auto start = chrono::high_resolution_clock::now();
    Schedule bestSchedule = scheduler.evolve();
    auto end = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<chrono::microseconds>(end - start).count();

    for (size_t i = 0; i < bestSchedule.machineJobs.size(); ++i)
    {
        cout << "Machine " << i << ": ";
        for (int jobIndex : bestSchedule.machineJobs[i])
        {
            cout << jobs[jobIndex].jobId << " ";
        }
        cout << "\n";
    }
    cout << "Makespan: " << bestSchedule.fitness << "\n";
    cout << "Runtime: " << duration << " microseconds\n\n";
}

int main()
{
    runTestCase("Test Case 1: Basic Test", {{1, 2}, {2, 3}, {3, 5}, {4, 7}, {5, 1}}, 2);
    runTestCase("Test Case 2: All Jobs of Equal Length", {{1, 5}, {2, 5}, {3, 5}, {4, 5}}, 2);
    runTestCase("Test Case 3: More Machines than Jobs", {{1, 6}, {2, 2}, {3, 8}}, 4);
    runTestCase("Test Case 4: Single Job", {{1, 10}}, 3);
    runTestCase("Test Case 5: Complex Test", {{1, 2}, {2, 1}, {3, 2}, {4, 7}, {5, 3}, {6, 6}}, 3);
    return 0;
}