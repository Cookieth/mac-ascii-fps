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
Outstanding questions
- What is being done in the setup?
- What is being done for cleanup (endwin)?
- What is stringstream?

*/

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
    // Setup for ncurses
    setlocale(LC_CTYPE, "");
    initscr();
    noecho();
    keypad(stdscr, TRUE);
    nodelay(stdscr, TRUE);
    curs_set(0);
    start_color();
    use_default_colors();

    //char *screen = new char[nScreenWidth * nScreenHeight];
    std::vector<char> screen(nScreenWidth * nScreenHeight);

    // TODO: Load map from file (?)
    std::string map;
    map += "################";
    map += "#..............#";
    map += "#..............#";
    map += "#..............#";
    map += "#..............#";
    map += "#..............#";
    map += "#..............#";
    map += "#..............#";
    map += "#..............#";
    map += "#..............#";
    map += "#..............#";
    map += "#..............#";
    map += "#..............#";
    map += "#..............#";
    map += "#..............#";
    map += "################";

    auto tp1 = std::chrono::system_clock::now();
    auto tp2 = std::chrono::system_clock::now();

    unsigned int microseconds = 10000;
    int count = 0;

    // Reduce screen width by 1 to allow for newline character
    nScreenWidth -= 1;

    while (true)
    {
        // Constantly resize to the size of the window
        nScreenWidth = getmaxx(stdscr);
        nScreenHeight = getmaxy(stdscr);
        screen.resize(nScreenWidth * nScreenHeight);

        // === Controls ===

        tp2 = std::chrono::system_clock::now();
        std::chrono::duration<float> elapsedTime = tp2 - tp1;
        tp1 = tp2;
        // My rendering print is not optimized, so I need to make it faster than the tutorial.
        float fElapsedTime = elapsedTime.count();

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

            // Hit a boundary (delineate the walls, where the corner of the blocks are)
            bool bBoundary = false;

            // unit vector in each direction
            float fEyeX = sinf(fRayAngle);
            float fEyeY = cosf(fRayAngle);

            // --- Ray Casting Rendering ---

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
                    // Case for hitting the wall
                    // Convert 3D coords to 2D array
                    if (map[nTestY * nMapWidth + nTestX] == '#')
                    {
                        bHitWall = true;
                        //fDistanceToWall will retain it's value

                        // --- Edge handling mechanism ---

                        // Essentially, draw a line from corners of cells to the player,
                        // If the ray has a small angle to that line, then it must be an edge!
                        std::vector<std::pair<float, float> > p; // distance, dot product (angle)

                        //4 corners to try
                        for (int tx = 0; tx < 2; tx++)
                        {
                            for (int ty = 0; ty < 2; ty++)
                            {
                                // Create vectors from perfect corners to the player
                                float vy = (float)nTestY + ty - fPlayerY;
                                float vx = (float)nTestX + tx - fPlayerX;

                                // Get the magnitude of that vector
                                float d = sqrt(vx * vx + vy * vy);
                                // Get the dot product of the vector
                                float dot = (fEyeX * vx / d) + (fEyeY * vy / d);
                                p.push_back(std::make_pair(d, dot));
                            }
                        }

                        // Sort Pairs from closest to farthest
                        // Check if the first element is less than the other first element (magnitude of the vector)
                        sort(p.begin(), p.end(), [](const std::pair<float, float> &left, const std::pair<float, float> &right) { return left.first < right.first; });

                        // The threshold of the angle such that if it is less than this value,
                        // Then the angle is considered "acute", and it must be a boundary.
                        float fBound = 0.01;

                        // If you take the arccos of the dot product, you get the angle.
                        if (acos(p.at(0).second) < fBound)
                        {
                            bBoundary = true;
                        }
                        // Only need to test for the first two/three corners
                        if (acos(p.at(1).second) < fBound)
                        {
                            bBoundary = true;
                        }
                        /*
                        if (acos(p.at(2).second) < fBound)
                        {
                            bBoundary = true;
                        }*/

                        // ------
                    }
                }
            }

            // ------

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

            // (See boundary before)
            // If it is a boundary, then outline it
            if (bBoundary)
            {
                nShade = ' ';
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

        // === Rendering the Map ===
        // Display Stats (from the tutorial)
        // TODO: Find out the equivalent of this.
        //swprintf_s(screen, 40, L"X=%3.2f, Y=%3.2f, A=%3.2f FPS=%3.2f ", fPlayerX, fPlayerY, fPlayerA, 1.0f / fElapsedTime);

        // Display Map

        // Iterate through the coordinates, fill in the map
        for (int nx = 0; nx < nMapWidth; nx++)
            for (int ny = 0; ny < nMapWidth; ny++)
            {

                screen[(ny + 1) * nScreenWidth + nx] = map[ny * nMapWidth + nx];
            }

        // Marker for the player
        screen[((int)fPlayerX + 1) * nScreenWidth + (int)fPlayerY] = 'P';

        // Insert newline at the end of each line
        for (int i = 0; i < nScreenHeight; i++)
        {
            screen[i * nScreenWidth] = '\n';
        }

        screen[nScreenWidth * nScreenHeight - 1] = '\0';
        printScreen(std::string(screen.data()));

        // Debugging things
        //usleep(microseconds);
        //count++;
    }

    // curses cleanup
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
    /*
    wprintw(stdscr, screenstr.data());
    wmove(stdscr, 0, 0);*/

    // loop through the window and print it
    for (int i = 0; i < screenstr.length(); i++)
    {
        printw((const char *)(&screenstr[i]));
    }
    wmove(stdscr, 0, 0);

    /*
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

    // loop through the window and print it
    for (int i = 0; i < window.size(); i++)
    {
        for (int j = 0; j < window[i].size(); j++)
        {
            printw(window[i][j].c_str());
        }
        printw("\n");
    }
    wmove(stdscr, 0, 0);*/
}
