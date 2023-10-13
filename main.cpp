#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <algorithm>

using namespace std;

#define EMPTY 0
#define WHITE 1
#define BLACK 2

#define MAX_PLY 3

class GameManager;

class GameManager
{
private:
    vector<vector<int>> board;
    vector<int> value; // {non-use, value1, value2}
    string name;
    int user;     // index of accessing user value
    int opponent; // index of accessing opponent value
    int current;

public:
    GameManager();
    ~GameManager() {}

    void printBoard(void);
    int evaluate(void);
    void userMove(void);
    bool movePawn(int sx, int sy, int gx, int gy);

    int get_user(void) const { return user; }
};

int main(void)
{
    GameManager manager;
    int game_over = 0;

    // test///////////////////
    while (game_over == 0)
    {
        game_over = manager.evaluate();
        manager.printBoard();

        if (game_over)
        {
            cout << (game_over == manager.get_user() ? "HUMAN" : "COMPUTER") << " WIN" << endl;
            break;
        }

        manager.userMove();
    }
    //////////////////////////

    return 0;
}

// Get information and Initiate board staus
GameManager::GameManager() : board(3, vector<int>(3, 0)), value(3, 0)
{
    string temp;

    // Get info
    cout << "Type \"WHITE\" or \"BLACK\": ";
    cin >> temp;
    cout << "WHITE first (1) BLACK first (2): ";
    cin >> current;
    cout << "Input file name (default board=0): ";
    cin >> name;

    if (temp.compare("WHITE") == 0)
        user = WHITE;
    else if (temp.compare("BLACK") == 0)
        user = BLACK;
    opponent = (user == WHITE ? BLACK : WHITE);

    // Initiate board
    if (name.compare("0") != 0) // input file exist
    {
        string path("./");
        path.append(name);
        fstream input_file(path.c_str());
        string line;

        // Load board
        if (!input_file.is_open()) // wrong file name
            cout << "Cannot find input file!" << endl;
        else
        {
            // Load board
            int i = 0, j = 0;
            while (getline(input_file, line))
            {
                istringstream iss(line);
                int data;

                j = 0;
                while (iss >> data)
                    board[i][j++] = data;
                i++;
            }

            input_file.close();
            return;
        }
    }
    cout << "Initiate board as default." << endl;

    // Set default board
    board[0][0] = WHITE;
    board[0][1] = WHITE;
    board[0][2] = WHITE;

    board[2][0] = BLACK;
    board[2][1] = BLACK;
    board[2][2] = BLACK;
}

/* Print board state and value method */
void GameManager::printBoard(void)
{
    // Print board state
    cout << "忙式成式成式忖" << endl;
    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 3; j++)
            cout << "弛" << (board[i][j] ? (board[i][j] == WHITE ? "W" : "B") : " ");
        cout << "弛" << endl;
        cout << ((i != 2) ? "戍式托式托式扣\n" : "");
    }
    cout << "戌式扛式扛式戎" << endl;

    // Print value
    cout << "computer: " << value[opponent] << endl;
    cout << "human: " << value[user] << endl
         << endl;
    ;
}

/*  Evaluate method
    Evaluate board state and store in value vector
    return 1 if white win
    return 2 if black win
    return 0 if game in progress */
int GameManager::evaluate(void)
{
    // Check if game end
    // 1. No one's pawn on the board
    // 2. One's pawn reached on the other side

    // Check pawn existence
    bool user_exist = false;
    bool op_exist = false;

    for (int i = 0; i < 3; i++)
        for (int j = 0; j < 3; j++)
            if (board[i][j] == user)
                user_exist = true;
            else if (board[i][j] == opponent)
                op_exist = true;

    // Check reached pawn
    bool user_reached = false;
    bool op_reached = false;

    for (int j = 0; j < 3; j++)
        if (board[(user + 1) % 3][j] == user)
            user_reached = true;
        else if (board[(opponent + 1) % 3][j] == opponent)
            op_reached = true;

    // Evaluate with ended game
    if ((!op_exist) || user_reached) // user win
    {
        value[user] = 100;
        value[opponent] = -100;
        return user;
    }
    else if ((!user_exist) || op_reached) // opponent win
    {
        value[opponent] = 100;
        value[user] = -100;
        return opponent;
    }

    // Evaluate with board in progress
    int w_val = 0, b_val = 0;

    for (int i = 0; i < 3; i++)
    {
        w_val += count(board[i].begin(), board[i].end(), WHITE) * i * i;
        w_val -= count(board[i].begin(), board[i].end(), BLACK) * (4 - i) * (4 - i);

        b_val += count(board[i].begin(), board[i].end(), BLACK) * (2 - i) * (2 - i);
        b_val -= count(board[i].begin(), board[i].end(), WHITE) * (i + 2) * (i + 2);
    }

    // Store into value vector
    if (user == WHITE)
    {
        value[user] = w_val;
        value[opponent] = b_val;
    }
    else if (user == BLACK)
    {
        value[user] = b_val;
        value[opponent] = w_val;
    }

    return EMPTY;
}

/*  User's move method
    Get user's move and update board state after move */
void GameManager::userMove(void)
{
    int sx, sy;
    int gx, gy;

    // Get starting coordinate
    do
    {
        cout << "Selet the starting coordinate of pawn want to move: ";
        cin >> sx >> sy;

        // Out of range
        if (sx < 0 || sx > 2 || sy < 0 || sy > 2)
        {
            cout << "Out Of Range!" << endl;
            continue;
        }

        // Empty
        if (board[sx][sy] == EMPTY)
            printf("There is NO pawn at (%d, %d)!\n", sx, sy);
        // Opponent's pawn
        else if (board[sx][sy] == opponent)
            cout << "You cannot move opponent's pawn!" << endl;
    } while (board[sx][sy] != user); // until user's pawn selected

    do
    {
        cout << "Selet the destination coordinate of pawn: ";
        cin >> gx >> gy;

        // Out of range
        if (gx < 0 || gx > 2 || gy < 0 || gy > 2)
        {
            cout << "Out Of Range!" << endl;
            continue;
        }
        else if (gx == sx && gy == sy)
        {
            cout << "You must move the pawn in your turn!" << endl;
            continue;
        }
    } while (!movePawn(sx, sy, gx, gy)); // until move succeed

    return;
}

/*  Move pawn method
    Input: start_x, start_y, goal_x, goal_y
    Move pawn at (start_x, start_y) to (goal_x, goal_y)
    return true if move succeed
    return false is move fail   */
bool GameManager::movePawn(int sx, int sy, int gx, int gy)
{
    // Move white pawn
    if (board[sx][sy] == WHITE)
    {
        if ((abs(gx - sx) > 1) || (abs(gy - sy) > 1)) // too far
            return false;
        else if (board[gx][gy] == WHITE) // blocked by white pawn
            return false;
        // BLACK pawn exist
        else if (board[gx][gy] == BLACK)
            if ((sx + 1 == gx) && (sy != gy)) // kill by diagonal move
            {
                board[gx][gy] = board[sx][sy];
                board[sx][sy] = EMPTY;
                return true;
            }
            else // blocked by black pawn
                return false;
        else if (board[gx][gy] == EMPTY) // destination is empty
            if ((gx - sx) * (gy - sy) == 0)
            {
                board[gx][gy] = board[sx][sy];
                board[sx][sy] = EMPTY;
                return true;
            }
            else // cannot diagonal move
                return false;
    }
    // Move black pawn
    else if (board[sx][sy] == BLACK)
    {
        if ((abs(gx - sx) > 1) || (abs(gy - sy) > 1)) // too far
            return false;
        else if (board[gx][gy] == BLACK) // blocked by black pawn
            return false;
        // WHITE pawn exist
        else if (board[gx][gy] == WHITE)
        {
            if ((sx - 1 == gx) && (sy != gy)) // kill by diagonal move
            {
                board[gx][gy] = board[sx][sy];
                board[sx][sy] = EMPTY;
                return true;
            }
            else // blocked by white pawn
                return false;
        }
        else if (board[gx][gy] == EMPTY) // destination is empty
            if ((gx - sx) * (gy - sy) == 0)
            {
                board[gx][gy] = board[sx][sy];
                board[sx][sy] = EMPTY;
                return true;
            }
            else // cannot diagonal move
                return false;
    }

    return false;
}
