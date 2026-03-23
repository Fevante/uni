#include <string>
#include <cstdlib>
#include <iostream>
#include <vector>
#include <chrono>
#include <thread>

void printMatrix(int rows, int cols, const std::vector<std::vector<char>>& matrix) {
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            std::cout << matrix[i][j];
        }
        std::cout << '\n';
    }
}

void clearTerminal() {
    std::cout << "\033[2J\033[H";
}

int main(int argc, char const *argv[])
{
    char ball = 'o';

    std::string xDir;
    std::string yDir;

    int width = 100;
    int height = 10;

    std::vector<std::vector<char>> matrix(height, std::vector<char>(width, ' '));

    int ball_x = rand() % width;
    int ball_y = rand() % height;

    matrix[ball_y][ball_x] = ball;

    xDir = "right";
    yDir = "down";

    while (true) {
        clearTerminal();
        printMatrix(height, width, matrix);
        if (xDir == "right") {
            ball_x++;
        } else {
            ball_x--;
        }

        if (yDir == "down") {
            ball_y++;
        } else {
            ball_y--;
        }

        if (ball_x <= 0) {
            xDir = "right";
        } else if (ball_x >= width - 1) {
            xDir = "left";
        }

        if (ball_y <= 0) {
            yDir = "down";
        } else if (ball_y >= height - 1) {
            yDir = "up";
        }

        for (int i = 0; i < height; i++) {
            for (int j = 0; j < width; j++) {
                matrix[i][j] = ' ';
            }
        }
        matrix[ball_y][ball_x] = ball;

        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    return 0;
}
