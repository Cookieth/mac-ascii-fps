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

int main()
{
    // Setup from tetris (TODO: Find documentation to understand this)
    setlocale(LC_CTYPE, "");
    initscr();
    noecho();
    keypad(stdscr, TRUE);
    nodelay(stdscr, TRUE);
    curs_set(0);
    start_color();
    use_default_colors();

    //std::cout << "Hello" << std::endl;

    // ===== Setup Window =====
    std::vector<std::vector<std::string> > window;
    std::string line;
    const std::string screenstr = "  ┏━━k-vernooy/tetris━━┓\n  ┃                    ┃\n  ┃                    ┃   ┏━━next━━━┓\n  ┃                    ┃   ┃         ┃\n  ┃                    ┃   ┃         ┃\n  ┃                    ┃   ┃         ┃\n  ┃                    ┃   ┗━━━━━━━━━┛\n  ┃                    ┃\n  ┃                    ┃   ┏━━score━━┓\n  ┃                    ┃   ┃         ┃\n  ┃                    ┃   ┃  0      ┃\n  ┃                    ┃   ┃         ┃\n  ┃                    ┃   ┗━━━━━━━━━┛\n  ┃                    ┃\n  ┃                    ┃   ┏━━lines━━┓\n  ┃                    ┃   ┃         ┃\n  ┃                    ┃   ┃  0      ┃\n  ┃                    ┃   ┃         ┃\n  ┃                    ┃   ┗━━━━━━━━━┛\n  ┗━━━━━━━━━━━━━━━━━━━━┛\n                 ";
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

    // ===== Print Screen =====

    unsigned int microseconds = 10000;
    int count = 0;

    while (count < 500)
    {

        usleep(microseconds);
        //https://stackoverflow.com/questions/54684870/printw-and-mvwprintw-do-not-print
        int ch = getch();

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
        count++;
    }

    // curses cleanup (TODO: Find documentation to understand this)
    endwin();
    return 0;
}
