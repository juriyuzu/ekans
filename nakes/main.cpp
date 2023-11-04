#include <iostream>
#include <string>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <limits>
#include <Windows.h>
using namespace std;

string x;
int fullX = 176,
	fullY = 56;

struct Player {
	int length = 5;
	int coords[3] = {3, 3, 1};
	int score = 0;
	int hiScore;
	string name;
};

struct Map {
	char mapp[300][700];
	int decayMap[300][700];
};

int rng(int min, int max) {
	return min + (rand() % (max - min + 1));
}

void gotoxy(int x, int y, bool abs = true) {
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	COORD newPosition;
	CONSOLE_SCREEN_BUFFER_INFO consoleInfo;
	GetConsoleScreenBufferInfo(hConsole, &consoleInfo);
	newPosition.X = x + (abs ? 0 : consoleInfo.dwCursorPosition.X);
	newPosition.Y = y + (abs ? 0 : consoleInfo.dwCursorPosition.Y);
	SetConsoleCursorPosition(hConsole, newPosition);
}

void fileReader(string fileName, vector<string> &arr) {
	arr.clear();
	ifstream inputFile(fileName);
    if (!inputFile.is_open()) {
        system("cls");
		std::cerr << "Failed to open the input file: " << fileName << std::endl;
    }
    string text, line;
    while (getline(inputFile, line)) arr.push_back(line);
	inputFile.close();
}

void fileWriter(string fileName, vector<string> &arr) {
	ofstream outputFile(fileName, ios::trunc);
    if (!outputFile.is_open()) {
        std::cerr << "Failed to open the input file: " << fileName << std::endl;
    }
    for (string s : arr) outputFile << s + '\n';
	outputFile.close();
}

void food(int x, int y, Map &map) {
	map.decayMap[y][x] = -1;
	gotoxy(x, y);
	cout << "\033[37m@";
}

void move(Player &player, Map &map, bool &dead, bool color = false) {
	if (map.decayMap[player.coords[1]][player.coords[0]] == -1) {
		player.score++;
		player.length += 5;
		int x, y;
		do {
			x = rng(0, fullX),
			y = rng(0, fullY);
		}
		while (map.decayMap[y][x] != 0);
		food(x, y, map);
	}
	if (map.decayMap[player.coords[1]][player.coords[0]] > 1) {
		dead = true;
	}
	gotoxy(player.coords[0], player.coords[1]);
	map.decayMap[player.coords[1]][player.coords[0]] = player.length;
	switch (player.coords[2]) {
	case 0:
		cout << (color ? "^\033[36m" : "^\033[33m");
		player.coords[1] = (player.coords[1] - 1 + fullY) % fullY;
		break;
	case 1:
		cout << (color ? ">\033[36m" : ">\033[33m");
		player.coords[0] = (player.coords[0] + 1) % fullX;
		break;
	case 2:
		cout << (color ? "v\033[36m" : "v\033[33m");
		player.coords[1] = (player.coords[1] + 1) % fullY;
		break;
	case 3:
		cout << (color ? "<\033[36m" : "<\033[33m");
		player.coords[0] = (player.coords[0] - 1 + fullX) % fullX;
		break;
	}
}

void decayer(Map &map) {
	for (int i = 0; i < fullY; i++) {
		for (int j = 0; j < fullX; j++) {
			if (map.decayMap[i][j] > 1) map.decayMap[i][j]--;
			else if (map.decayMap[i][j] == 1){
				gotoxy(j, i);
				cout << " ";
				map.decayMap[i][j]--;
			}
		}
	}
}

void snake() {
	// prologue
	bool butt[5] = {0, 0, 0, 0, 0};
	Map map;
	while (true) {
		system("cls");
		cout << "+";
		for (int i = 0; i < fullX - 2; i++) cout << "-";
		for (int i = 0; i < fullY - 1; i++) cout << "|\n";
		cout << "-";
		gotoxy(0, 0);
		cout << "\n\n\tPress [F11] to toggle fullscreen and resize the border:"
			 << "\n\n\tWidth [LEFT / RIGHT]: " << fullX
			 << "\n\tHeight [UP / DOWN]: " << fullY
			 << "\n\n\tPress [ENTER] to continue.";
		while (true) {
			if (GetAsyncKeyState(VK_UP) & 0x8000 && !butt[0]) {
				butt[0] = true;
				break;
	        }
	        else if (!(GetAsyncKeyState(VK_UP) & 0x8000)) butt[0] = false;
			if (GetAsyncKeyState(VK_RIGHT) & 0x8000 && !butt[1]) {
				butt[1] = true;
				break;
	        }
	        else if (!(GetAsyncKeyState(VK_RIGHT) & 0x8000)) butt[1] = false;
			if (GetAsyncKeyState(VK_DOWN) & 0x8000 && !butt[2]) {
				butt[2] = true;
				break;
	        }
	        else if (!(GetAsyncKeyState(VK_DOWN) & 0x8000)) butt[2] = false;
			if (GetAsyncKeyState(VK_LEFT) & 0x8000 && !butt[3]) {
				butt[3] = true;
				break;
	        }
	        else if (!(GetAsyncKeyState(VK_LEFT) & 0x8000)) butt[3] = false;
	        if (GetAsyncKeyState(VK_RETURN) & 0x8000 && !butt[4]) {
				butt[4] = true;
				break;
	        }
	        else if (!(GetAsyncKeyState(VK_RETURN) & 0x8000)) butt[4] = false;
		}
        if (butt[0]) fullY--;
        if (butt[1]) fullX++;
        if (butt[2]) fullY++;
        if (butt[3]) fullX--;
        if (butt[4]) break;
	}
	cin.clear();
	cin.ignore(numeric_limits<streamsize>::max(), '\n');
	// prologue end
	
	
	// main menu loop
	bool menu = false;
	while (true) {
		// introduction
		system("cls");
		bool multiplayer = false;
		Player user, user2;
		cout << "\n\n\tSNAKE:"
			 << "\n\n\tPlayer 1 (Up, Down, Left, Right)"
			 << "\n\n\t\tName: ";
		getline(cin, x);
		user.name = x;
		vector<string> saveFile;
		fileReader("saveFile.txt", saveFile);
		bool hasSave = false;
		for (int i = 0; i < saveFile.size(); i += 2) {
			if (saveFile[i] == user.name) {
	    		hasSave = true;
	    		user.hiScore = stoi(saveFile[i + 1]);
	    		break;
			}
		}
		if (hasSave) {
			cout << "\n\t\tSave file detected."
				 << "\n\n\t\tWelcome back " << user.name << "!"
				 << "\n\n\t\tHigh Score: " << user.hiScore;
		}
		else {
			cout << "\n\t\tNo save file detected."
				 << "\n\n\t\tType [Create] to create a save file."
				 << "\n\n\t\t > ";
			getline(cin, x);
			if (x == "Create") {
				saveFile.push_back(user.name);
				saveFile.push_back(to_string(user.score));
				fileWriter("saveFile.txt", saveFile);
				cout << "\n\t\tSave file successfully created.";
			}
			else {
				cout << "\n\tGuest mode enabled.";
			}
		}
		cout << "\n\n\n\n\n\n\tType [Multiplayer] to enable Player 2."
			 << "\n\n\t > ";
		getline(cin, x);
		if (x == "Multiplayer") {
			multiplayer = true;
			user2.coords[0] = fullX - 3;
			user2.coords[1] = fullY - 3;
			user2.coords[2] = 3;
			cout << "\n\n\n\n\n\tPlayer 2 (W, A, S, D)"
				 << "\n\n\t\tName: ";
			getline(cin, x);
			user2.name = x;
			vector<string> saveFile;
			fileReader("saveFile.txt", saveFile);
			bool hasSave = false, forcedGuest = false;
			for (int i = 0; i < saveFile.size(); i += 2) {
				if (saveFile[i] == user2.name) {
		    		hasSave = true;
		    		if (user2.name == user.name) forcedGuest = true;
		    		user2.hiScore = stoi(saveFile[i + 1]);
		    		break;
				}
			}
			if (hasSave && !forcedGuest) {
				cout << "\n\t\tSave file detected."
					 << "\n\n\t\tWelcome back " << user2.name << "!"
					 << "\n\n\t\tHigh Score: " << user2.hiScore;
			}
			else {
				if (forcedGuest) {
					cout << "\n\t\tSimilar players detected."
						 << "\n\n\t\tForcing guest mode.";
				}
				else {
					cout << "\n\t\tNo save file detected."
						 << "\n\n\t\tType [Create] to create a save file."
						 << "\n\n\t\t > ";
					getline(cin, x);
					if (x == "Create") {
						saveFile.push_back(user2.name);
						saveFile.push_back(to_string(user2.score));
						fileWriter("saveFile.txt", saveFile);
						cout << "\n\t\tSave file successfully created.";
					}
					else {
						cout << "\n\tGuest mode enabled.";
					}
				}
			}
		}
		cout << "\n\n\n\n\n\n\tThe game will start immediately."
			 << "\n\n\t";
		system("pause");
		// introduction end
		
		
		// retry loop
		while (true) {
			// climax
			for (int i = 0; i < fullY; i++) {
				for (int j = 0; j < fullX; j++) {
					map.mapp[i][j] = ' ';
					map.decayMap[i][j] = 0;
				}
			}
			system("cls");
			bool dead = false, dead2 = true;
			if (multiplayer) dead2 = false;
			food(rng(0, fullX), rng(0, fullY), map);
			while (true) {
				if (dead && dead2) break;
				decayer(map);
				if (!dead) {
					move(user, map, dead, false);
					if (GetAsyncKeyState(VK_UP) & 0x8000 && user.coords[2] != 2) {
						user.coords[2] = 0;
			        }
					if (GetAsyncKeyState(VK_RIGHT) & 0x8000 && user.coords[2] != 3) {
						user.coords[2] = 1;
			        }
					if (GetAsyncKeyState(VK_DOWN) & 0x8000 && user.coords[2] != 0) {
						user.coords[2] = 2;
			        }
					if (GetAsyncKeyState(VK_LEFT) & 0x8000 && user.coords[2] != 1) {
						user.coords[2] = 3;
			        }
				}
		        if (multiplayer && !dead2) {
					move(user2, map, dead2, true);
					if (GetAsyncKeyState(0x57) & 0x8000 && user2.coords[2] != 2) {
						user2.coords[2] = 0;
			        }
					if (GetAsyncKeyState(0x44) & 0x8000 && user2.coords[2] != 3) {
						user2.coords[2] = 1;
			        }
					if (GetAsyncKeyState(0x53) & 0x8000 && user2.coords[2] != 0) {
						user2.coords[2] = 2;
			        }
					if (GetAsyncKeyState(0x41) & 0x8000 && user2.coords[2] != 1) {
						user2.coords[2] = 3;
			        }
				}
			        
		        Sleep(25);
			}
			cout << "\n\n\033[37m";
			gotoxy(user.coords[0], 0, false);
			system("pause");
			system("cls");
			// climax end
			
			
			// epilogue
			while (true) {
				cout << "\n\n\tGame Over"
					 << "\n\n\t" << (multiplayer ? (user.score != user2.score ? (user.score > user2.score ? user.name + " wins!" : user2.name + " wins!") : "Draw.") : "")
					 << "\n\n\n\n\n\t" << user.name << " got " << user.score << ".";
				if (user.score > user.hiScore) {
					cout << "\n\n\tNew High Score!";
					user.hiScore = user.score;
					fileReader("saveFile.txt", saveFile);
					for (int i = 0; i < saveFile.size(); i += 2) {
						if (saveFile[i] == user.name) {
				    		saveFile[i + 1] = to_string(user.hiScore);
				    		break;
						}
					}
					fileWriter("saveFile.txt", saveFile);
				}
				if (multiplayer) {
					cout << "\n\n\n\n\n\t" << user2.name << " got " << user2.score << ".";
					if (user2.score > user2.hiScore) {
						cout << "\n\n\tNew High Score!";
						user2.hiScore = user2.score;
						fileReader("saveFile.txt", saveFile);
						for (int i = 0; i < saveFile.size(); i += 2) {
							if (saveFile[i] == user2.name) {
					    		saveFile[i + 1] = to_string(user2.hiScore);
					    		break;
							}
						}
						fileWriter("saveFile.txt", saveFile);
					}
				}
				cout << "\n\n\n\n\n\tType [Back] to go back to the home screen."
					 << "\n\n\t     [Retry] to play again."
					 << "\n\n\t      > ";
				getline(cin, x);
				if (x == "Back") {
					menu = true;
					break;
				}
				else if (x == "Retry") {
					cout << "\n\n\t\tThe game will start immediately."
						 << "\n\n\t\t";
					user.score = 0;
					user2.score = 0;
					user.length = 4;
					user2.length = 4;
					user.coords[0] = 3;
					user.coords[1] = 3;
					user.coords[2] = 1;
					user.coords[0] = fullX - 3;
					user.coords[1] = fullY - 3;
					user.coords[2] = 3;
					system("pause");
					system("cls");
					break;
				}
				system("cls");
			}
			if (!menu) continue;
			menu = false;
			break;
			// epilogue end
			
			
		}
		// retry loop end
		
		
	}
	// main menu loop end
	
	
}

int main() {
	srand(time(0));
	snake();
	
	return 0;
}
