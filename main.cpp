#include <iostream>
#include <unistd.h>
#include <term.h>
#include <vector>

// https://www.cplusplus.com/articles/4z18T05o/#POSIX
void ClearScreen()
{
    if (!cur_term)
    {
        int result;
        setupterm(NULL, STDOUT_FILENO, &result);
        if (result <= 0)
            return;
    }

    // Get char * of a string literal in c (I could be overcomplicating this)
    std::string str = "clear";
    std::vector<char> cstr(str.c_str(), str.c_str() + str.size() + 1);

    putp(tigetstr(cstr.data()));
}

int main()
{
    std::cout << "Hello" << std::endl;
    sleep(2);
    ClearScreen();
    std::cout << "Hello" << std::endl;
    return 0;
}