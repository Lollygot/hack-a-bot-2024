#include <iostream>
#include <unistd.h>

using namespace std;

const int n = 3;

int maze[n][n][4] = {
    {{1, 1, 0, 0}, {1, 0, 0, 0}, {1, 0, 0, 1}},
    {{0, 1, 1, 1}, {0, 1, 0, 0}, {0, 0, 0, 1}},
    {{1, 1, 1, 0}, {0, 0, 1, 1}, {0, 1, 1, 1}}};

int visited[n][n] = {
    {0, 0, 0},
    {0, 0, 0},
    {0, 0, 0}};

bool isDone(int visited[n][n])
{
    for (int r = 0; r < n; r++)
    {
        for (int c = 0; c < n; c++)
        {
            if (visited[r][c] == 0)
                return false;
        }
    }
    return true;
}

bool printVisted(int visited[n][n])
{

    cout << "\n\n";
    for (int r = 0; r < n; r++)
    {
        for (int c = 0; c < n; c++)
        {
            cout << visited[r][c];
            cout << " ";
        }
        cout << '\n';
    }
    return true;
}

bool canNorth(int posX, int posY, int maze[n][n][4])
{
    return maze[posX][posY][0] == 0;
}

bool canEast(int posX, int posY, int maze[n][n][4])
{
    return maze[posX][posY][1] == 0;
}

bool canSouth(int posX, int posY, int maze[n][n][4])
{
    return maze[posX][posY][2] == 0;
}

bool canWest(int posX, int posY, int maze[n][n][4])
{
    return maze[posX][posY][3] == 0;
}

int main()
{
    int bot1PositionR = 2;
    int bot1PositionC = 0;
    int bot2PositionR = 0;
    int bot2PositionC = 2;

    int ctr = 0;
    while (!isDone(visited))
    {

        visited[bot1PositionR][bot1PositionC] = 1;
        visited[bot2PositionR][bot2PositionC] = 1;

        printVisted(visited);

        sleep(5);

        ctr++;

        // bot1
        if (canEast(bot1PositionR, bot1PositionC, maze) && (visited[bot1PositionR][bot1PositionC - 1] == 0))
        {
            bot1PositionC = bot1PositionC - 1;
        }
        else if (canNorth(bot1PositionR, bot1PositionC, maze) && (visited[bot1PositionR - 1][bot1PositionC] == 0))
        {
            bot1PositionR = bot1PositionR - 1;
        }
        else if (canWest(bot1PositionR, bot1PositionC, maze) && (visited[bot1PositionR][bot1PositionC + 1] == 0))
        {
            bot1PositionC = bot1PositionC + 1;
        }
        else if (canSouth(bot1PositionR, bot1PositionC, maze) && (visited[bot1PositionR + 1][bot1PositionC] == 0))
        {
            bot1PositionR = bot1PositionR + 1;
        }
        else if (canEast(bot1PositionR, bot1PositionC, maze))
        {
            bot1PositionC = bot1PositionC - 1;
        }
        else if (canNorth(bot1PositionR, bot1PositionC, maze))
        {
            bot1PositionR = bot1PositionR - 1;
        }
        else if (canWest(bot1PositionR, bot1PositionC, maze))
        {
            bot1PositionC = bot1PositionC + 1;
        }
        else if (canSouth(bot1PositionR, bot1PositionC, maze))
        {
            bot1PositionR = bot1PositionR + 1;
        }

        // bot2
        if (canWest(bot2PositionR, bot2PositionC, maze) && (visited[bot2PositionR][bot2PositionC + 1] == 0))
        {
            bot2PositionC = bot2PositionC + 1;
        }
        else if (canSouth(bot2PositionR, bot2PositionC, maze) && (visited[bot2PositionR + 1][bot2PositionC] == 0))
        {
            bot2PositionR = bot2PositionR + 1;
        }
        else if (canEast(bot2PositionR, bot2PositionC, maze) && (visited[bot2PositionR][bot2PositionC - 1] == 0))
        {
            cout << "\ncan east and not visited\n";
            bot2PositionC = bot2PositionC - 1;
        }
        else if (canNorth(bot2PositionR, bot2PositionC, maze) && (visited[bot2PositionR - 1][bot2PositionC] == 0))
        {
            bot2PositionR = bot2PositionR - 1;
        }
        else if (canWest(bot2PositionR, bot2PositionC, maze))
        {
            bot2PositionC = bot2PositionC + 1;
        }
        else if (canSouth(bot2PositionR, bot2PositionC, maze))
        {
            bot2PositionR = bot2PositionR + 1;
        }
        else if (canEast(bot2PositionR, bot2PositionC, maze))
        {
            cout << "\ncan east and visited\n";
            bot2PositionC = bot2PositionC - 1;
        }
        else if (canNorth(bot2PositionR, bot2PositionC, maze))
        {
            bot2PositionR = bot2PositionR - 1;
        }
    }
}
