#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

using namespace std;

#define EMPTY 0
#define WHITE 1
#define BLACK 2

class Board
{
private:
    vector<vector<int>> board;
    string name;
    int user;
    int current;

public:
    Board();
    ~Board() {}

    void printBoard(void) const;
};

int main(void)
{
    Board game_board;

    // test
    game_board.printBoard();

    return 0;
}

// Get information and Initiate board staus
Board::Board() : board(3, vector<int>(3, 0))
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

/* Print board state */
void Board::printBoard(void) const
{
    cout << "忙式成式成式忖" << endl;
    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 3; j++)
            cout << "弛" << board[i][j];
        cout << "弛" << endl;
        cout << ((i != 2) ? "戍式托式托式扣\n" : "");
    }
    cout << "戌式扛式扛式戎" << endl;
}
