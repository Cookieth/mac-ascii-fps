#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <ncurses.h>
#include <fstream>
#include <unistd.h>
#include <cstdlib>
#include <stdio.h>
#include <random>
#include <cstring>
#include <algorithm>
#include <unistd.h>

#include <chrono>

/*
* An attempt to get this tutorial https://www.youtube.com/watch?v=xW8skO7MFYw to work on mac.
*/

// Using https://github.com/k-vernooy/tetris

// Curses library history: use ncurses (new curses) https://en.wikipedia.org/wiki/Ncurses

/*
Outstanding questions
- What is being done in the setup?
- What is being done for cleanup (endwin)?
- What is stringstream?

*/

// Leaving the tetris screen thing here... just in case!
//const std::string screenstr = "  ┏━━k-vernooy/tetris━━┓\n  ┃                    ┃\n  ┃                    ┃   ┏━━next━━━┓\n  ┃                    ┃   ┃         ┃\n  ┃                    ┃   ┃         ┃\n  ┃                    ┃   ┃         ┃\n  ┃                    ┃   ┗━━━━━━━━━┛\n  ┃                    ┃\n  ┃                    ┃   ┏━━score━━┓\n  ┃                    ┃   ┃         ┃\n  ┃                    ┃   ┃  0      ┃\n  ┃                    ┃   ┃         ┃\n  ┃                    ┃   ┗━━━━━━━━━┛\n  ┃                    ┃\n  ┃                    ┃   ┏━━lines━━┓\n  ┃                    ┃   ┃         ┃\n  ┃                    ┃   ┃  0      ┃\n  ┃                    ┃   ┃         ┃\n  ┃                    ┃   ┗━━━━━━━━━┛\n  ┗━━━━━━━━━━━━━━━━━━━━┛\n                 ";

std::string readLine(std::string str, int n);
void printScreen(std::string screenstr);

int nScreenWidth = 120;
int nScreenHeight = 40;

float fPlayerX = 8.0f;
float fPlayerY = 8.0f;
float fPlayerA = 0.0f;

int nMapHeight = 16;
int nMapWidth = 16;

// FOV set to 45˚
float fFOV = 3.1415926 / 4.0;

float fDepth = 16.0f;

int main()
{
    // https://stackoverflow.com/questions/4804298/how-to-convert-wstring-into-string (keeping this here temporarily)
    // setlocale(LC_ALL, "-");

    // Setup from tetris (TODO: Find documentation to understand this)
    setlocale(LC_CTYPE, "");
    initscr();
    noecho();
    keypad(stdscr, TRUE);
    nodelay(stdscr, TRUE);
    curs_set(0);
    start_color();
    use_default_colors();

    char *screen = new char[nScreenWidth * nScreenHeight];
    //const std::string screenstr = "  ┏━━k-vernooy/tetris━━┓\n  ┃                    ┃\n  ┃                    ┃   ┏━━next━━━┓\n  ┃                    ┃   ┃         ┃\n  ┃                    ┃   ┃         ┃\n  ┃                    ┃   ┃         ┃\n  ┃                    ┃   ┗━━━━━━━━━┛\n  ┃                    ┃\n  ┃                    ┃   ┏━━score━━┓\n  ┃                    ┃   ┃         ┃\n  ┃                    ┃   ┃  0      ┃\n  ┃                    ┃   ┃         ┃\n  ┃                    ┃   ┗━━━━━━━━━┛\n  ┃                    ┃\n  ┃                    ┃   ┏━━lines━━┓\n  ┃                    ┃   ┃         ┃\n  ┃                    ┃   ┃  0      ┃\n  ┃                    ┃   ┃         ┃\n  ┃                    ┃   ┗━━━━━━━━━┛\n  ┗━━━━━━━━━━━━━━━━━━━━┛\n                 ";

    // TODO: Load map from file (?)
    std::string map;
    map += "################";
    map += "#..............#";
    map += "#..............#";
    map += "#..............#";
    map += "#..............#";
    map += "#...#..........#";
    map += "#..............#";
    map += "#..............#";
    map += "#..............#";
    map += "#...#..........#";
    map += "#..............#";
    map += "#..............#";
    map += "#.........######";
    map += "#..............#";
    map += "#..............#";
    map += "################";

    auto tp1 = std::chrono::system_clock::now();
    auto tp2 = std::chrono::system_clock::now();

    unsigned int microseconds = 10000;
    int count = 0;

    while (true)
    {
        // === Controls ===

        tp2 = std::chrono::system_clock::now();
        std::chrono::duration<float> elapsedTime = tp2 - tp1;
        tp1 = tp2;
        // My rendering print is not optimized, so I need to make it faster than the tutorial.
        float fElapsedTime = 0.25f; //elapsedTime.count();

        int ch = getch();
        if (ch)
        {
            if (ch == (char)'a')
            {
                fPlayerA -= (0.8f) * fElapsedTime;
            }
            else if (ch == (char)'d')
            {
                fPlayerA += (0.8f) * fElapsedTime;
            }
            else if (ch == (char)'w')
            {
                // Multiply by unit vector for coordinates
                fPlayerX += sinf(fPlayerA) * 5.0f * fElapsedTime;
                fPlayerY += cosf(fPlayerA) * 5.0f * fElapsedTime;

                //Convert location into integers, and check if the projected location hits a wall #
                if (map.c_str()[(int)fPlayerX * nMapWidth + (int)fPlayerY] == '#')
                {
                    //Undo what we just did
                    fPlayerX -= sinf(fPlayerA) * 5.0f * fElapsedTime;
                    fPlayerY -= cosf(fPlayerA) * 5.0f * fElapsedTime;
                }
            }
            else if (ch == (char)'s')
            {
                fPlayerX -= sinf(fPlayerA) * 5.0f * fElapsedTime;
                fPlayerY -= cosf(fPlayerA) * 5.0f * fElapsedTime;

                //Convert location into integers, and check if the projected location hits a wall #
                if (map.c_str()[(int)fPlayerX * nMapWidth + (int)fPlayerY] == '#')
                {
                    //Undo what we just did
                    fPlayerX += sinf(fPlayerA) * 5.0f * fElapsedTime;
                    fPlayerY += cosf(fPlayerA) * 5.0f * fElapsedTime;
                }
            }
        }

        // === Rendering ===

        // Horizontal calculations
        // TODO: Implement newline \n character to prevent overflow
        for (int x = 0; x < nScreenWidth; x++)
        {
            // 120 columns, means 120 rays to go across FOV
            // Start angle = (fPlayerA - fFOV / 2.0f) (FOV to the right by half) and incremented
            float fRayAngle = (fPlayerA - fFOV / 2.0f) + ((float)x / (float)nScreenWidth) * fFOV;

            // Track distance to wall, keep incrementing until land in cell
            float fDistanceToWall = 0;
            bool bHitWall = false;

            // unit vector in each direction
            float fEyeX = sinf(fRayAngle);
            float fEyeY = cosf(fRayAngle);

            while (!bHitWall && fDistanceToWall < fDepth)
            {
                fDistanceToWall += 0.1f;
                // Only require integer values
                int nTestX = (int)(fPlayerX + fEyeX * fDistanceToWall);
                int nTestY = (int)(fPlayerX + fEyeY * fDistanceToWall);

                // Test if ray is out of bounds
                if (nTestX < 0 || nTestX >= nMapWidth || nTestY < 0 || nTestY >= nMapHeight)
                {
                    bHitWall = true;
                    fDistanceToWall = fDepth;
                }
                else
                {
                    //Convert 3D coords to 2D array
                    if (map[nTestY * nMapWidth + nTestX] == '#')
                    {
                        bHitWall = true;
                        //fDistanceToWall will retain it's value
                    }
                }
            }

            // Ceiling and floor filling
            int nCeiling = (float)(nScreenHeight / 2.0) - nScreenHeight / ((float)fDistanceToWall);
            int nFloor = nScreenHeight - nCeiling;

            char nShade = ' ';

            //Shade appropriately http://www.asciitable.com/
            //Closest to furthest
            //TODO: Figure out extended ascii
            if (fDistanceToWall <= fDepth / 4.0f)
            {
                nShade = '#';
            }
            else if (fDistanceToWall < fDepth / 3.0f)
            {
                nShade = 'H';
            }
            else if (fDistanceToWall < fDepth / 2.0f)
            {
                nShade = '=';
            }
            else if (fDistanceToWall < fDepth)
            {
                nShade = '-';
            }
            else
            {
                nShade = '.';
            }

            // Draw Column
            for (int y = 0; y < nScreenHeight; y++)
            {
                if (y <= nCeiling)
                {
                    screen[y * nScreenWidth + x] = ' ';
                }
                else if (y > nCeiling && y <= nFloor)
                {
                    screen[y * nScreenWidth + x] = nShade;
                }
                else
                {
                    // Taking a portion of how far the floor can be seen.
                    float b = 1.0f - (((float)y - nScreenHeight / 2.0f) / ((float)nScreenHeight / 2.0f));
                    if (b < 0.25)
                    {
                        nShade = 'X';
                    }
                    else if (b < 0.5)
                    {
                        nShade = '>';
                    }
                    else if (b < 0.75)
                    {
                        nShade = '\'';
                    }
                    else if (b < 0.9)
                    {
                        nShade = '`';
                    }
                    else
                    {
                        nShade = ' ';
                    }
                    screen[y * nScreenWidth + x] = nShade;
                    //screen[y * nScreenWidth + x] = ' ';
                }
            }
        }

        screen[nScreenWidth * nScreenHeight - 1] = '\0';
        printScreen(std::string(screen));

        // Debugging things
        //usleep(microseconds);
        //count++;
    }

    // curses cleanup (TODO: Find documentation to understand this)
    endwin();
    return 0;
}

// Dependencies needed to print to the screen

std::string readLine(std::string str, int n)
{
    // returns the nth line of a string
    std::stringstream f(str);
    std::string s;

    for (int i = 0; i < n; i++)
    {
        getline(f, s);
    }

    getline(f, s);
    return s;
}

void printScreen(std::string screenstr)
{
    // ===== Setup Window =====
    std::vector<std::vector<std::string> > window;
    std::string line;
    std::stringstream f(screenstr);
    int lineNum = 0;

    while (getline(f, line))
    {
        // get number of lines in string
        lineNum++;
    }

    for (int x = 0; x < lineNum; x++)
    {
        std::vector<std::string> rowVec;
        std::string line = readLine(screenstr, x);
        for (int i = 0; i < line.length(); i++)
        {
            rowVec.push_back(line.substr(i, 1));
        }
        window.push_back(rowVec);
    }

    //https://stackoverflow.com/questions/54684870/printw-and-mvwprintw-do-not-print
    //int ch = getch();

    // loop through the window and print it
    for (int i = 0; i < window.size(); i++)
    {
        for (int j = 0; j < window[i].size(); j++)
        {
            if (((i >= 1 && i < 19) && (j > 4 && j < 25)) || ((i >= 3 && i < 6) && (j > 33 && j < 43)))
            {
                printw(std::string(" ").c_str());
            }
            else
            {
                printw(window[i][j].c_str());
            }
        }
        printw(std::string("\n").c_str());
    }
    wmove(stdscr, 0, 0);
}