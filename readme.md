# Elevator Simulator Front-End Project

## Overview

This project involves creating the front-end for an elevator simulator. The back-end functionality is already implemented, and now it's time to design an interactive user interface (UI) that allows the user to control and monitor the elevator's movement and interactions with passengers. The goal is to create an engaging, smooth, and visually appealing UI for this elevator simulation.

## Features

### Required Features
- **Elevator Position**: Display the current position of the elevator cabin (i.e., the floor the elevator is on).
- **Elevator Movement**: Animate the elevator's movement, ensuring it moves smoothly between floors.
- **Passenger Count**: Show if there are passengers in the elevator and display the number of passengers.
- **Passenger Requests**: Indicate whether passengers are waiting for the elevator on each floor, and show the direction they want to go (up or down).

### Interactive Controls
- **Up Key (UP)**: Move the elevator upwards. The elevator will continue moving up unless it reaches the top floor or other user actions are performed.
- **Down Key (DOWN)**: Move the elevator downwards. The elevator will continue moving down unless it reaches the bottom floor or other user actions are performed.
- **Stop Key (Custom Keystroke)**: Stop the elevator when it reaches the next floor.
- **Passenger Creation (Mouse Click or Keystroke)**: Simulate new passenger requests by clicking on a floor or using a specific keystroke. The passenger will request to go either up or down to another floor.

### Design Considerations
- **Animation**: Ensure that the elevator's movement between floors is smooth and visually appealing.
- **UI Design**: You are free to design and style the UI as creatively as you like. However, keep usability in mind for a pleasant user experience.

## Implementation Restrictions
- **ECGraphicViewImp**: You must use the provided `ECGraphicViewImp` class to interact with the Allegro5 graphics library. Direct use of Allegro5 functions is not allowed, as this project is focused on working within the provided codebase.

## Missing Files

There are some files missing from the project that were provided by the instructor. These files are essential for completing the project:
1. **ECGraphicViewImp**: This class is necessary for interacting with Allegro5 to render graphics and manage the elevator's visual representation.
2. **Helper Functions/Code**: Some additional helper functions or resources that should have been provided are missing. These are necessary for handling certain actions in the back-end and for proper integration.

Please contact your instructor or consult course materials for access to these missing files, as they


Passenger Count: Indicate whether there are passengers in the elevator and, if there are, show how many passengers are in the cabin.

Passenger Requests: Show which direction passengers are waiting for on each floor (up or down).

Interactive Controls
Up Key (UP): Move the elevator upwards. The elevator will continue moving up unless it reaches the top floor or other user actions are performed.

Down Key (DOWN): Move the elevator downwards. The elevator will continue moving down unless it reaches the bottom floor or other user actions are performed.

Stop Key (Custom Keystroke): Stop the elevator when it reaches the next floor.

Passenger Creation (Mouse Click or Keystroke): Simulate a new passenger request by clicking on a floor. The passenger will request to go either up or down to another floor.

Design Considerations
Animation: The movement of the elevator must be animated smoothly to give the impression of the elevator moving between floors.

UI Design: The UI can be customized and styled creatively. Think about usability and making it visually appealing.

Implementation Restrictions
ECGraphicViewImp: You must use the provided ECGraphicViewImp class to interact with the Allegro5 graphics library. Directly using Allegro5 functions will not be allowed, as the goal is to work within the provided codebase.

Missing Files
The following files are missing from the project, and were provided by the instructor. Without them, the project cannot be fully executed:

ECGraphicViewImp: The class used for all interactions with Allegro5. This class is essential for drawing graphics and managing the elevator's visual representation.

Helper Functions/Code: There are some missing helper functions or resources that should have been provided to handle certain actions in the back-end.

Please ensure you check with your instructor or course material for access to these missing files, as they are required to complete the project.

How to Use the UI
Run the application, and the elevator simulator window will appear.

Use the UP and DOWN arrow keys to control the elevator's movement.

To stop the elevator when it reaches the next floor, use the designated keystroke or button (check documentation).

Click on the floors to simulate passenger requests.

Good luck with building your interactive elevator simulator UI!