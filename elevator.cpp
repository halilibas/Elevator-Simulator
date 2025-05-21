//
//  ECElevatorSim.cpp
//  

#include "elevator.h"
#include <vector>

using namespace std;

// *************************************************************************************************
// ECElevatorSim

// Constructor: Initializes simulation parameters
ECElevatorSim::ECElevatorSim(int numFloors, std::vector<ECElevatorSimRequest> &listRequests) : listRequests(listRequests)
{
    this->numFloors = numFloors; // Set number of floors
    currFloor = 1; // Start at floor 1
    currDir = EC_ELEVATOR_STOPPED; // Elevator is stopped initially
    currState = new ECElevatorStopped(); // Initial state is stopped
    time = 0; // Start time at 0
}

// Destructor: Frees memory for current state
ECElevatorSim::~ECElevatorSim()
{
    delete currState;
}

// Simulate for a given time period
void ECElevatorSim::Simulate(int lenSim)
{
    for(int time = 0; time < lenSim; time++)
    {
        currState->NextDecision(*this); // Make next decision at each time step
    }
}

// State query/set methods
int ECElevatorSim::GetNumFloors() const { return numFloors; }
int ECElevatorSim::GetCurrFloor() const { return currFloor; }
void ECElevatorSim::SetCurrFloor(int f) { currFloor = f; }
EC_ELEVATOR_DIR ECElevatorSim::GetCurrDir() const { return currDir; }
void ECElevatorSim::SetCurrDir(EC_ELEVATOR_DIR dir) { currDir = dir; }
int ECElevatorSim::GetTime() const { return time; }
void ECElevatorSim::SetTime(int t) { time = t; }
ECElevatorState* ECElevatorSim::GetCurrState() const { return currState; }
vector<ECElevatorSimRequest>& ECElevatorSim::GetListRequests() const { return listRequests; }

void ECElevatorSim::ChangeState(ECElevatorState* newState)
{
    delete currState;
    currState = newState;
}

// *************************************************************************************************
// ECElevatorState

// Get active requests (before current time and not serviced)
vector<ECElevatorSimRequest*>& ECElevatorState::GetActiveRequests(ECElevatorSim& elevatorSim, vector<ECElevatorSimRequest*>& activeRequests) const
{
    activeRequests.clear();
    for(int i = 0; i < elevatorSim.GetListRequests().size(); i++)
    {
        if(elevatorSim.GetListRequests()[i].GetTime() <= elevatorSim.GetTime() && !elevatorSim.GetListRequests()[i].IsServiced())
        {
            activeRequests.push_back(&elevatorSim.GetListRequests()[i]);
        }
    }
    return activeRequests;
}

// Handle requests at current floor
bool ECElevatorState::DoCurrentFloorRequests(ECElevatorSim& elevatorSim, vector<ECElevatorSimRequest*>& requests) const
{
    bool requestFound = false;
    for(int i = 0; i < requests.size(); i++)
    {
        if(requests[i]->GetFloorSrc() == elevatorSim.GetCurrFloor() && !requests[i]->IsFloorRequestDone())
        {
            requestFound = true;
            requests[i]->SetFloorRequestDone(true); // Mark as picked up
        }
        else if(requests[i]->GetFloorDest() == elevatorSim.GetCurrFloor() && requests[i]->IsFloorRequestDone())
        {
            requestFound = true;
            requests[i]->SetArriveTime(elevatorSim.GetTime()); // Set arrival time
            requests[i]->SetServiced(true); // Mark as serviced
            requests.erase(requests.begin() + i); // Remove request
            i--; // Adjust index after removal
        }
    }
    return requestFound;
}

// *************************************************************************************************
// ECElevatorStopped

// Get closest request floor
int ECElevatorStopped::GetClosestRequestFloor(ECElevatorSim& elevatorSim, vector<ECElevatorSimRequest*>& activeRequests) const
{
    int minDist = elevatorSim.GetCurrFloor() + elevatorSim.GetNumFloors();
    int closestFloor = 0;
    for(auto& request : activeRequests)
    {
        int currDist = abs(request->GetFloorSrc() - elevatorSim.GetCurrFloor());
        int currFloor = request->GetFloorSrc();
        if(currDist < minDist)
        {
            minDist = currDist;
            closestFloor = currFloor;
        }
    }
    return closestFloor;
}

// Decide next action in stopped state
void ECElevatorStopped::NextDecision(ECElevatorSim& elevatorSim)
{
    vector<ECElevatorSimRequest*> activeRequests;
    GetActiveRequests(elevatorSim, activeRequests);
    if(activeRequests.size() > 0)
    {
        DoCurrentFloorRequests(elevatorSim, activeRequests);
        if(GetClosestRequestFloor(elevatorSim, activeRequests) > elevatorSim.GetCurrFloor())
        {
            elevatorSim.ChangeState(new ECElevatorUp());
            elevatorSim.SetCurrFloor(elevatorSim.GetCurrFloor() + 1);
        }
        else
        {
            elevatorSim.ChangeState(new ECElevatorDown());
            elevatorSim.SetCurrFloor(elevatorSim.GetCurrFloor() - 1);
        }
        elevatorSim.SetCurrDir(elevatorSim.GetCurrState()->GetDirection());
    }
    elevatorSim.SetTime(elevatorSim.GetTime() + 1);
}

// Get direction for stopped state
EC_ELEVATOR_DIR ECElevatorStopped::GetDirection() const { return EC_ELEVATOR_STOPPED; }

// *************************************************************************************************
// ECElevatorUp

// Check if request is on current path
bool ECElevatorUp::RequestOnPath(ECElevatorSim& elevatorSim, vector<ECElevatorSimRequest*>& activeRequests) const
{
    for(auto& request : activeRequests)
    {
        if((request->GetFloorSrc() > elevatorSim.GetCurrFloor()) && !(request->IsFloorRequestDone()))
        {
            return true;
        }
        else if((request->GetFloorDest() > elevatorSim.GetCurrFloor()) && request->IsFloorRequestDone())
        {
            return true;
        }
    }
    return false;
}

// Decide next action when moving up
void ECElevatorUp::NextDecision(ECElevatorSim& elevatorSim)
{
    vector<ECElevatorSimRequest*> activeRequests;
    GetActiveRequests(elevatorSim, activeRequests);
    if(activeRequests.size() == 0)
    {
        elevatorSim.ChangeState(new ECElevatorStopped());
        elevatorSim.SetTime(elevatorSim.GetTime() + 1);
        return;
    }
    if(elevatorSim.GetCurrDir() == EC_ELEVATOR_STOPPED || !DoCurrentFloorRequests(elevatorSim, activeRequests))
    {
        if(RequestOnPath(elevatorSim, activeRequests))
        {
            elevatorSim.SetCurrFloor(elevatorSim.GetCurrFloor() + 1);
        }
        else
        {
            elevatorSim.ChangeState(new ECElevatorDown());
            elevatorSim.SetCurrFloor(elevatorSim.GetCurrFloor() - 1);
        }
        elevatorSim.SetCurrDir(elevatorSim.GetCurrState()->GetDirection());
    }
    else
    {
        elevatorSim.SetCurrDir(EC_ELEVATOR_STOPPED);
    }
    elevatorSim.SetTime(elevatorSim.GetTime() + 1);
}

// Get direction for up state
EC_ELEVATOR_DIR ECElevatorUp::GetDirection() const { return EC_ELEVATOR_UP; }

// *************************************************************************************************
// ECElevatorDown

// Check if request is on current path
bool ECElevatorDown::RequestOnPath(ECElevatorSim& elevatorSim, vector<ECElevatorSimRequest*>& activeRequests) const
{
    for(auto& request : activeRequests)
    {
        if((request->GetFloorSrc() < elevatorSim.GetCurrFloor()) && !(request->IsFloorRequestDone()))
        {
            return true;
        }
        else if((request->GetFloorDest() < elevatorSim.GetCurrFloor()) && request->IsFloorRequestDone())
        {
            return true;
        }
    }
    return false;
}

// Decide next action when moving down
void ECElevatorDown::NextDecision(ECElevatorSim& elevatorSim)
{
    vector<ECElevatorSimRequest*> activeRequests;
    GetActiveRequests(elevatorSim, activeRequests);
    if(activeRequests.size() == 0)
    {
        elevatorSim.ChangeState(new ECElevatorStopped());
        elevatorSim.SetTime(elevatorSim.GetTime() + 1);
        return;
    }
    if(elevatorSim.GetCurrDir() == EC_ELEVATOR_STOPPED || !DoCurrentFloorRequests(elevatorSim, activeRequests))
    {
        if(RequestOnPath(elevatorSim, activeRequests))
        {
            elevatorSim.SetCurrFloor(elevatorSim.GetCurrFloor() - 1);
        }
        else
        {
            elevatorSim.ChangeState(new ECElevatorUp());
            elevatorSim.SetCurrFloor(elevatorSim.GetCurrFloor() + 1);
        }
        elevatorSim.SetCurrDir(elevatorSim.GetCurrState()->GetDirection());
    }
    else
    {
        elevatorSim.SetCurrDir(EC_ELEVATOR_STOPPED);
    }
    elevatorSim.SetTime(elevatorSim.GetTime() + 1);
}

// Get direction for down state
EC_ELEVATOR_DIR ECElevatorDown::GetDirection() const { return EC_ELEVATOR_DOWN; }
