#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <vector>
#include <map>
#include <fstream>
#include <algorithm>

using namespace std;

namespace MazeLoader {
    int roomWidth = 20;
    int roomHeight = 10;
    int roomNumX = 5;
    int roomNumY = 4;

    void printMap(vector<vector<char>> map) {
        for (vector<char> line: map) {
            for (char c: line) {
                cout << c;
            }
            cout << endl;
        }
    }

    vector<vector<char>> getRoomTilemap(char* roomName) {
        ifstream roomFile(roomName);
        string line;

        if (!roomFile.is_open()){
            cout << "error while opening room map" << endl;
            exit(1);
        }

        vector<vector<char>> map;

        while (getline(roomFile, line)) {
            vector<char> mapLine(line.begin(), line.end());
            map.push_back(mapLine);
        }

        return map;
    }

    vector<vector<char>> getMazeTilemap() {
        vector<vector<char>> roomIn = getRoomTilemap("assets/map/room_in.txt");
        vector<vector<char>> roomA = getRoomTilemap("assets/map/room_A.txt");
        vector<vector<char>> roomB= getRoomTilemap("assets/map/room_B.txt");
        vector<vector<char>> roomC= getRoomTilemap("assets/map/room_C.txt");
        vector<vector<char>> roomOut = getRoomTilemap("assets/map/room_out.txt");

        map<char, vector<vector<char>>> rooms = {
            { 'I', roomIn },
            { 'A', roomA },
            { 'B', roomB },
            { 'C', roomC },
            { 'O', roomOut }
        };

        vector<vector<char>> mazeRooms = getRoomTilemap("assets/map/maze.txt");

        vector<vector<char>> maze(roomNumY * roomHeight, vector<char>(roomNumX * roomWidth, '.'));

        for (int i = 0; i < roomNumY; i++) {
            for (int j = 0; j < roomNumX; j++) {
                char room = mazeRooms[i][j];
                for (int n = 0; n < roomHeight; n++) {
                    copy(
                        rooms[room][n].begin(),
                        rooms[room][n].begin() + roomWidth,
                        maze[i * roomHeight + n].begin() + j * roomWidth
                    );
                }
            }
        }

        printMap(maze);

        return maze;
    }
}