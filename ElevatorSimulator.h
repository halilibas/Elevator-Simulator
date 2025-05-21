// ECElevatorSimulatorObserver.h

#ifndef ECELEVATORSIMULATOROBSERVER_H
#define ECELEVATORSIMULATOROBSERVER_H

#include "ECObserver.h"
#include "ECGraphicViewImp.h"
#include <vector>
#include <queue>
#include <set>

class Passenger {
public:
    explicit Passenger(int destFloor = -1) : destinationFloor(destFloor) {}
    int destinationFloor;
};

struct Floor {
    std::queue<Passenger> waitingPassengersUp;
    std::queue<Passenger> waitingPassengersDown;
    bool upRequest = false;
    bool downRequest = false;
};

class ECElevatorSimulatorObserver : public ECObserver {
public:
    ECElevatorSimulatorObserver(ECGraphicViewImp& view, int windowWidth, int windowHeight);
    void Update() override;

private:
    void MoveElevator();
    void StopAtFloor(int floor);
    void BoardPassengers(int floor);
    void AlightPassengers(int floor);

    void DrawElevatorShaft();
    void DrawElevatorCabin();
    void DrawFloors();
    void DrawWaitingPassengers();
    void DrawFloorButtons();

    void HandleMouseClick(int x, int y);
    void RequestElevator(int floor, bool goingUp);
    void AddDestinationFloor(int floor);
    void UpdateElevatorDirection();
    void DrawPassenger(int x, int y, ECGVColor color);

    ECGraphicViewImp& view;

    int elevatorPositionY = 0;
    int currentFloor = 0;
    int elevatorSpeed = 3;
    bool isMoving = false;
    bool movingUp = true;

    std::set<int> targetFloors;             // Floors the elevator needs to visit
    std::vector<Floor> buildingFloors;      // Information for each floor
    std::vector<Passenger> passengersInElevator;

    static constexpr int numFloors = 6;     // Number of floors
    int floorHeight;
    int elevatorWidth;
    int elevatorHeight;

    int windowWidth;
    int windowHeight;

    int stopCounter = 0;                    // Timer for stopping at floors
};

#endif // ECELEVATORSIMULATOROBSERVER_H
