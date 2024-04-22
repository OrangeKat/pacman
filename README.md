# S4 Project

Installation and user manual

1./Installation

  To run the C game on your computer, you will need one of the following:
Windows Subsystem for Linux (WSL):
If you are using a Windows computer, you can install WSL, which allows you to run a Linux distribution such as Ubuntu alongside Windows. You will need WSL and Ubuntu installed on your Windows machine. Please refer to the official Microsoft documentation for instructions on how to install WSL and Ubuntu.

Unix-based Operating System:
If you have a Unix-based operating system, such as Ubuntu, Linux Mint, or any other Linux  distribution, you can directly run the game without using WSL.

Open a terminal.
Run the following command to update the package lists:  sudo apt update
Install GCC and related build tools with : 
sudo apt install build-essential
sudo apt install libsdl2-dev libsdl2-ttf-dev libsdl2-image-dev

Then download the zip file and move it to the Desktop
Run the make command to build the game:  make
Then execute the program : ./game
To uninstall : make clean then delete it by using the rm -r pacman.zip
2./User

Welcome to the game! This user manual will guide you through the gameplay and provide instructions on how to navigate the user-friendly interface.

1. Launching the Game:
   - Ensure that you have successfully installed and built the game following the installation instructions.
   - Open a terminal or command prompt.
   - Navigate to the game directory using the `cd` command.
   - Execute the game by running the following command: ./game
 
2. Main Menu:
   - Upon launching the game, you will be presented with the main menu.
   - The main menu displays several options, such as “Play”, “SelectMap’’, “HighScore” , ”About”,”Help”  and "Quit."
   - To select an option, simply click on it using your mouse or trackpad. 

3. Playing the Game:
   - If you select the "Play" option from the main menu, you’ll have to select the options.
   - During gameplay, you will control the character using the arrow keys.

4. Exiting the Game:
   - To exit the game, either select the "Quit" option from the main menu or close the game window using the standard window controls (e.g., clicking on the close button in the corner).

Note: The user interface is designed to be intuitive and user-friendly, allowing you to interact with the game elements easily. If you encounter any difficulties or have any questions, please refer to the in-game help or consult the game's documentation.

Enjoy playing the game and have a great time exploring its features and challenges!!!




