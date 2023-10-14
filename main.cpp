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

struct Node;
class GameManager;

int isTerminate(const vector<vector<int>> board);
int value(const vector<vector<int>> board, int user);
Node *generateGameTree(int depth, Node *parent = nullptr);

// Game-Tree node
struct Node
{
    vector<vector<int>> board;
    Node *parent = nullptr;
    vector<Node *> children;
    int sx, sy, gx, gy; // last moved pawn's coordinate
    int v;              // evaluation value

    Node(vector<vector<int>> board, int sx, int sy, int gx, int gy)
        : board(board), sx(sx), sy(sy), gx(gx), gy(gy) {}
};

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
    void comMove(void);
    bool movePawn(int sx, int sy, int gx, int gy);

    int get_user(void) const { return user; }
    int get_com(void) const { return opponent; }
    vector<vector<int>> get_board(void) const { return board; }
};

GameManager manager;

int main(void)
{
    int game_over = 0;

    // test///////////////////
    while (game_over == 0)
    {
        game_over = isTerminate(manager.get_board());
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
    int winner = isTerminate(board);

    // Evaluate with ended game
    if (winner == user) // user win
    {
        value[user] = 100;
        value[opponent] = -100;
        return user;
    }
    else if (winner == opponent) // opponent win
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

/*  Computer's move method
    Get computer's move by min-max algorithm with Alpha-Beta pruing and update board state after move */
void GameManager::comMove(void)
{
    Node *root = new Node(board, 0, 0, 0, 0);

    // Generate tree
    generateGameTree(MAX_PLY + 1);
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

/*  Check game is terminated
    inspect the input game board
    return 1 if WHITE win
    return 2 if BLACK win
    return 0 if game in progress */
int isTerminate(const vector<vector<int>> board)
{
    // Check if game end
    // 1. No one's pawn on the board
    // 2. One's pawn reached on the other side

    // Check pawn existence
    bool w_exist = false;
    bool b_exist = false;

    for (int i = 0; i < 3; i++)
        for (int j = 0; j < 3; j++)
            if (board[i][j] == WHITE)
                w_exist = true;
            else if (board[i][j] == BLACK)
                b_exist = true;

    // Check reached pawn
    bool w_reached = false;
    bool b_reached = false;

    for (int j = 0; j < 3; j++)
        if (board[2][j] == WHITE)
            w_reached = true;
        else if (board[0][j] == BLACK)
            b_reached = true;

    // Inspect winner
    if ((!b_exist) || w_reached) // WHITE win
        return WHITE;
    else if ((!w_exist) || b_reached) // BLACK win
        return BLACK;
    else
        return 0; // gaem in progress
}

/*  Return user's value of board state */
int value(const vector<vector<int>> board, int user)
{
    int winner = isTerminate(board);
    int v;

    // Evaluate with ended game
    if (winner)
        if (winner == user) // user win
            return 100;
        else // opponent win
            return -100;

    // Evaluate with board in progress
    int w_val = 0, b_val = 0;

    for (int i = 0; i < 3; i++)
    {
        w_val += count(board[i].begin(), board[i].end(), WHITE) * i * i;
        w_val -= count(board[i].begin(), board[i].end(), BLACK) * (4 - i) * (4 - i);

        b_val += count(board[i].begin(), board[i].end(), BLACK) * (2 - i) * (2 - i);
        b_val -= count(board[i].begin(), board[i].end(), WHITE) * (i + 2) * (i + 2);
    }

    return (user == WHITE ? w_val : b_val);
}

/*  Generate game tree using DFS */
Node *generateGameTree(int depth, Node *parent)
{
    Node *new_node;

    if (depth == 0)
        return nullptr;

    // Generate node if possible move
    if (parent == nullptr) // game tree root
        new_node = new Node(manager.get_board(), 0, 0, 0, 0);
    else
    {
        vector<vector<int>> board(parent->board);

        for (int sx = 0; sx < 3; sx++)
            for (int sy = 0; sy < 3; sy++)
                // Computer's pawn selected
                if (board[sx][sy] == manager.get_com())
                    for (int gx = 0; gx < 3; gx++)
                        for (int gy = 0; gy < 3; gy++)
                            // Move white pawn
                            if (board[sx][sy] == WHITE)
                            {
                                if ((abs(gx - sx) > 1) || (abs(gy - sy) > 1)) // too far
                                {
                                }
                                else if (board[gx][gy] == WHITE) // blocked by white pawn
                                {
                                }
                                // BLACK pawn exist
                                else if (board[gx][gy] == BLACK)
                                    if ((sx + 1 == gx) && (sy != gy)) // kill by diagonal move
                                    {
                                        board[gx][gy] = board[sx][sy];
                                        board[sx][sy] = EMPTY;
                                        new_node = new Node(board, sx, sy, gx, gy);
                                    }
                                    else // blocked by black pawn
                                    {
                                    }
                                else if (board[gx][gy] == EMPTY) // destination is empty
                                    if ((gx - sx) * (gy - sy) == 0)
                                    {
                                        board[gx][gy] = board[sx][sy];
                                        board[sx][sy] = EMPTY;
                                        new_node = new Node(board, sx, sy, gx, gy);
                                    }
                                    else // cannot diagonal move
                                    {
                                    }
                            }
                            // Move black pawn
                            else if (board[sx][sy] == BLACK)
                            {
                                if ((abs(gx - sx) > 1) || (abs(gy - sy) > 1)) // too far
                                {
                                }
                                else if (board[gx][gy] == BLACK) // blocked by black pawn
                                {
                                }
                                // WHITE pawn exist
                                else if (board[gx][gy] == WHITE)
                                {
                                    if ((sx - 1 == gx) && (sy != gy)) // kill by diagonal move
                                    {
                                        board[gx][gy] = board[sx][sy];
                                        board[sx][sy] = EMPTY;
                                        new_node = new Node(board, sx, sy, gx, gy);
                                    }
                                    else // blocked by white pawn
                                    {
                                    }
                                }
                                else if (board[gx][gy] == EMPTY) // destination is empty
                                    if ((gx - sx) * (gy - sy) == 0)
                                    {
                                        board[gx][gy] = board[sx][sy];
                                        board[sx][sy] = EMPTY;
                                        new_node = new Node(board, sx, sy, gx, gy);
                                    }
                                    else // cannot diagonal move
                                    {
                                    }
                            }
    }

    // Set parent-child connection
    new_node->parent = parent;
    new_node->children.push_back(generateGameTree(depth - 1, new_node));

    // Terminal node
    if (depth == 1)
        new_node->v = value(new_node->board, manager.get_com()); // evaluate value

    return new_node;
}