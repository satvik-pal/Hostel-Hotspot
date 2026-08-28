#include "../include/MaintenanceManager.h"
#include<iostream>
#include <string>
#include <sstream>
#include <iomanip>
using namespace std;

MaintenanceManager::MaintenanceManager()
{
    workerCounter = 1;
}

void MaintenanceManager::addWorker(string n, string s, string l)
{
    stringstream workerIdStream;
    workerIdStream << "W" << setfill('0') << setw(3)<< workerCounter;
    string workerId = workerIdStream.str();

    MaintenanceWorker worker(workerId,n,s,l,true,0);
    workers.insert({workerId , worker});
    workerCounter++;

}

MaintenanceWorker* MaintenanceManager::findWorker(string workerId)
{
    auto worker = workers.find(workerId);

    if(worker != workers.end())
    {
        return &worker -> second;
    }
    else
    {
        return nullptr;
    }
}

void MaintenanceManager::removeWorker(string workerId)
{
    auto worker = workers.find(workerId);

    if(worker != workers.end())
    {
        workers.erase(worker);
        cout <<"Worker:" << workerId <<" removed" << endl;
    }
    else
    {
        cout <<"Worker:" << workerId <<" not found!" << endl;
    }
}