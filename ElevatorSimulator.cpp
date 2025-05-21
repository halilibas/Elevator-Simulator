// ECElevatorSimulatorObserver.cpp

#include "ECElevatorSimulatorObserver.h"
#include <algorithm>

ECElevatorSimulatorObserver::ECElevatorSimulatorObserver(ECGraphicViewImp& view, int windowWidth, int windowHeight)
    : view(view), windowWidth(windowWidth), windowHeight(windowHeight) {

    buildingFloors.resize(numFloors);
    floorHeight = windowHeight / numFloors;
    elevatorWidth = windowWidth / 8;
    elevatorHeight = floorHeight;
}

void ECElevatorSimulatorObserver::Update() {
    ECGVEventType evt = view.GetCurrEvent();

    if (evt == ECGV_EV_MOUSE_BUTTON_DOWN) {
        int x, y;
        view.GetCursorPosition(x, y);
        HandleMouseClick(x, y);
    } else if (evt >= ECGV_EV_KEY_DOWN_0 && evt <= ECGV_EV_KEY_DOWN_9) {
        int floor = evt - ECGV_EV_KEY_DOWN_0;
        AddDestinationFloor(floor);
    } else if (evt == ECGV_EV_KEY_DOWN_MINUS) {
        for (auto& passenger : passengersInElevator)
            passenger.destinationFloor = -2;
    }

    if (evt == ECGV_EV_TIMER) {
        MoveElevator();
        view.SetRedraw(true);
        DrawElevatorShaft();
        DrawFloors();
        DrawElevatorCabin();
        DrawWaitingPassengers();
        DrawFloorButtons();
    }
}

void ECElevatorSimulatorObserver::HandleMouseClick(int x, int y) {
    int floor = (windowHeight - y) / floorHeight;
    if (floor < 0 || floor >= numFloors) return;

    int shaftX = windowWidth / 2 - elevatorWidth / 2;
    int buttonWidth = windowWidth / 30;
    int buttonHeight = windowHeight / 40;

    int upButtonX1 = shaftX - buttonWidth - 20;
    int upButtonX2 = shaftX - 20;
    int upButtonY1 = windowHeight - (floor + 1) * floorHeight + 10;
    int upButtonY2 = upButtonY1 + buttonHeight;

    int downButtonY1 = upButtonY1 + 30;
    int downButtonY2 = downButtonY1 + buttonHeight;

    if (x > upButtonX1 && x < upButtonX2 && y > upButtonY1 && y < upButtonY2)
        RequestElevator(floor, true);
    else if (x > upButtonX1 && x < upButtonX2 && y > downButtonY1 && y < downButtonY2)
        RequestElevator(floor, false);
}

void ECElevatorSimulatorObserver::RequestElevator(int floor, bool goingUp) {
    Passenger newPassenger;

    if (goingUp) {
        buildingFloors[floor].waitingPassengersUp.push(newPassenger);
        buildingFloors[floor].upRequest = true;
    } else {
        buildingFloors[floor].waitingPassengersDown.push(newPassenger);
        buildingFloors[floor].downRequest = true;
    }

    targetFloors.insert(floor);
    UpdateElevatorDirection();

    if (!isMoving)
        isMoving = true;
}

void ECElevatorSimulatorObserver::AddDestinationFloor(int floor) {
    if (!passengersInElevator.empty()) {
        for (auto& passenger : passengersInElevator) {
            if (passenger.destinationFloor == -1) {
                passenger.destinationFloor = floor;
                targetFloors.insert(floor);
                UpdateElevatorDirection();
            }
        }
    }
}

void ECElevatorSimulatorObserver::UpdateElevatorDirection() {
    if (targetFloors.empty()) {
        isMoving = false;
        return;
    }

    int nextFloor = movingUp ? *targetFloors.begin() : *targetFloors.rbegin();
    movingUp = (nextFloor > currentFloor);
}

void ECElevatorSimulatorObserver::MoveElevator() {
    if (!isMoving || stopCounter > 0) {
        if (stopCounter > 0) --stopCounter;
        return;
    }

    if (targetFloors.empty()) {
        isMoving = false;
        return;
    }

    int targetFloor = movingUp ? *targetFloors.begin() : *targetFloors.rbegin();
    int targetPosition = targetFloor * floorHeight;

    if (movingUp) {
        elevatorPositionY += elevatorSpeed;
        if (elevatorPositionY >= targetPosition) {
            elevatorPositionY = targetPosition;
            currentFloor = targetFloor;
            StopAtFloor(currentFloor);
        }
    } else {
        elevatorPositionY -= elevatorSpeed;
        if (elevatorPositionY <= targetPosition) {
            elevatorPositionY = targetPosition;
            currentFloor = targetFloor;
            StopAtFloor(currentFloor);
        }
    }
}

void ECElevatorSimulatorObserver::StopAtFloor(int floor) {
    AlightPassengers(floor);
    BoardPassengers(floor);

    targetFloors.erase(floor);
    UpdateElevatorDirection();

    stopCounter = 120;  

    if (floor == 0)
        passengersInElevator.clear();
}

void ECElevatorSimulatorObserver::AlightPassengers(int floor) {
    passengersInElevator.erase(
        std::remove_if(passengersInElevator.begin(), passengersInElevator.end(),
            [floor](const Passenger& p) { return p.destinationFloor == floor || p.destinationFloor == -2; }),
        passengersInElevator.end()
    );
}

void ECElevatorSimulatorObserver::BoardPassengers(int floor) {
    auto& floorInfo = buildingFloors[floor];
    auto& queue = movingUp ? floorInfo.waitingPassengersUp : floorInfo.waitingPassengersDown;

    while (!queue.empty()) {
        passengersInElevator.push_back(queue.front());
        queue.pop();
    }

    if (movingUp)
        floorInfo.upRequest = false;
    else
        floorInfo.downRequest = false;
}

void ECElevatorSimulatorObserver::DrawElevatorShaft() {
    int shaftX = windowWidth / 2 - elevatorWidth / 2;
    view.DrawFilledRectangle(shaftX, 0, shaftX + elevatorWidth, windowHeight, ECGV_NONE);
    view.DrawRectangle(shaftX, 0, shaftX + elevatorWidth, windowHeight, 3, ECGV_BLACK);
}

void ECElevatorSimulatorObserver::DrawFloors() {
    int shaftX = windowWidth / 2 - elevatorWidth / 2;

    for (int i = 0; i <= numFloors; ++i) {
        int y = windowHeight - i * floorHeight;
        view.DrawLine(shaftX, y, shaftX + elevatorWidth, y, 2, ECGV_BLACK);

        if (i < numFloors) {
            std::string floorText = std::to_string(i);
            view.DrawText(shaftX + elevatorWidth + 20, y - floorHeight / 2 - 10, floorText.c_str(), ECGV_BLACK);
        }
    }
}

void ECElevatorSimulatorObserver::DrawWaitingPassengers() {
    int shaftX = windowWidth / 2 - elevatorWidth / 2;
    int offsetDistance = 25;

    for (int i = 1; i < numFloors; ++i) {
        int y = windowHeight - i * floorHeight - floorHeight / 2;

        auto drawPassengers = [&](std::queue<Passenger>& queue, int direction) {
            int offset = 0;
            std::queue<Passenger> tempQueue = queue;
            while (!tempQueue.empty()) {
                int passengerX = shaftX + direction * (elevatorWidth / 2 + 50 + offset);
                DrawPassenger(passengerX, y, ECGV_BLUE);
                tempQueue.pop();
                offset += offsetDistance;
            }
        };

        drawPassengers(buildingFloors[i].waitingPassengersUp, -1);
        drawPassengers(buildingFloors[i].waitingPassengersDown, 1);
    }
}

void ECElevatorSimulatorObserver::DrawFloorButtons() {
    int shaftX = windowWidth / 2 - elevatorWidth / 2;
    int buttonWidth = windowWidth / 30;
    int buttonHeight = windowHeight / 40;

    for (int i = 0; i < numFloors; ++i) {
        int y = windowHeight - i * floorHeight;
        int x1 = shaftX - buttonWidth - 20;
        int x2 = shaftX - 20;

        auto drawButton = [&](int yPos, ECGVColor color, bool up) {
            view.DrawFilledRectangle(x1, yPos, x2, yPos + buttonHeight, color);
            view.DrawRectangle(x1, yPos, x2, yPos + buttonHeight, 1, ECGV_BLACK);

            int centerX = x1 + buttonWidth / 2;
            int centerY = yPos + buttonHeight / 2;
            if (up) {
                view.DrawFilledTriangle(
                    centerX, centerY - (buttonHeight / 4),
                    centerX - (buttonWidth / 4), centerY + (buttonHeight / 4),
                    centerX + (buttonWidth / 4), centerY + (buttonHeight / 4),
                    ECGV_WHITE
                );
            } else {
                view.DrawFilledTriangle(
                    centerX, centerY + (buttonHeight / 4),
                    centerX - (buttonWidth / 4), centerY - (buttonHeight / 4),
                    centerX + (buttonWidth / 4), centerY - (buttonHeight / 4),
                    ECGV_WHITE
                );
            }
        };

        int upY = y - floorHeight + 10;
        ECGVColor upColor = buildingFloors[i].upRequest ? ECGV_GREEN : ECGV_BLACK;
        drawButton(upY, upColor, true);

        int downY = upY + 30;
        ECGVColor downColor = buildingFloors[i].downRequest ? ECGV_GREEN : ECGV_BLACK;
        drawButton(downY, downColor, false);
    }
}

void ECElevatorSimulatorObserver::DrawElevatorCabin() {
    int shaftX = windowWidth / 2 - elevatorWidth / 2;
    int cabinY = windowHeight - elevatorPositionY - floorHeight;

    view.DrawFilledRectangle(shaftX, cabinY, shaftX + elevatorWidth, cabinY + floorHeight, ECGV_BLUE);
    view.DrawRectangle(shaftX, cabinY, shaftX + elevatorWidth, cabinY + floorHeight, 2, ECGV_BLACK);

    if (currentFloor != 0) {
        int offsetX = 10;
        int offsetY = cabinY + floorHeight - 20;

        for (size_t i = 0; i < passengersInElevator.size(); ++i) {
            DrawPassenger(shaftX + offsetX, offsetY, ECGV_RED);
            offsetX += 20;
            if (offsetX > elevatorWidth - 20) {
                offsetX = 10;
                offsetY -= 30;
            }
        }
    }
}

void ECElevatorSimulatorObserver::DrawPassenger(int x, int y, ECGVColor color) {
    view.DrawFilledCircle(x, y - 10, 7, color);
    view.DrawFilledRectangle(x - 4, y - 5, x + 4, y + 15, color);
}
