![Screenshot 2025-01-14 144021](https://github.com/user-attachments/assets/9cc1c813-9004-44a5-9473-e2c4c7ec0ad6)<br>

## Known issues

1. In some instances, the application may not generate the world as expected. Although the chunk logic is correct and functions properly under normal circumstances, this issue occasionally occurs for reasons that are currently unclear. If this happens, please relaunch the application to resolve the issue.<br>
2. The foliage, specifically the trees, does not generate leaves at all. This is because the logic for leaf generation has not been implemented. As a result, only the trunks are generated. Further development is needed to include the functionality for generating leaves.<br>

======================================================

## How to use<br>
#### *THE KEYBOARD CONTROLS:-*<br>
E key to thrust.<br> 
S and F key for rotating left or right.<br> SPACE key to shoot bullet.<br>
P for Pause.(toggle key)<br>
F8 for new game.(only works in game state and game restarts in game state).<br>

Debugger Options F1(toggle key)<br>
T for slow-Mo.(needs to be pressed down)<br>
N for respawn when you loose a life.<br>

## How to run the application:-<br>

Method 1(for new users): -

	1. Extract the zip folder to you desired location.
	2. Open the following path --> …\SimpleMiner\Run
	3. Double-click SimpleMiner_Release_x64.exe to start the program.

Method 2(for advanced users):-

	1. Extract the zip folder to you desired location.
	2. Open the following path --> …\SimpleMiner
	3. Open the Starship.sln using Visual Studio 2022 and make sure the solution config and platforms are "Release" and "x64".
	4. Press F7 key to build solution or go to Build --> Build Solution option using the Menu bar.
	5. Press Ctrl + F5 key to start the program without debugging or go to Debug --> Start without Debugging option using the Menu bar.

	NOTE:- 	
		* Visual Studio 2022 must be installed on the system.
		* In step 5 mentioned above, if you want you can execute the program with the debugger too.
