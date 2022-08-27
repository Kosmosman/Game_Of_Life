// Copyright 2022 joaquind
#include <stdio.h>
#include <unistd.h>
#define HIGHT 25
#define WIDTH 80
int check_alive(char start[][WIDTH], char finish[][WIDTH], int i, int j);
void copy_matrix(char start[][WIDTH], char finish[][WIDTH]);
void printing(char finish[][WIDTH]);
void first_move(char start[][WIDTH], char finish[][WIDTH], int* coordinates);
void zero(char start[][WIDTH], char finish[][WIDTH]);
int change_field(char start[][WIDTH], char finish[][WIDTH]);

int main(void) {
    char start[HIGHT][WIDTH], finish[HIGHT][WIDTH], ch;
    int coordinates[HIGHT * WIDTH * 2];
    printf("Добро пожаловать в Game Of Life! "
           "Для выхода введите q, для начал игры - любую другую клавишу.\n");
    while ((ch = getchar()) != 'q') {
        zero(start, finish);
        first_move(start, finish, coordinates);
        skip();
        while (change_field(start, finish) > 0) {
            skip();
            printing(finish);
            copy_matrix(start, finish);
        }
        if (getchar() == '\n') fflush(stdout);
        skip();
        printf("Вы закончили игру! Для начала нового хода введите любой символ, для выхода введите q.\n");
    }
    return 0;
}
// Проверка клетки на жизнеспособность
int check_alive(char start[][WIDTH], char finish[][WIDTH], int row, int column) {
    int count = 0, flag = 0;
    for (int m = row - 1; m <= row + 1; m++)
        for (int n = column - 1; n <= column + 1; n++)
            if ((start[m][n] == '@') && !(m == row && n == column))
                count++;
    if (count < 2 || count > 3)
        finish[row][column] = '.';
    if (count == 3) {
        finish[row][column] = '@';
        flag = 1;
    }
    if (count == 2 && start[row][column] == '@') {
        finish[row][column] = '@';
        flag = 1;
    }
    return flag;
}
// Копируем матрицу из финальной версии в стартовую на каждом новом ходу
void copy_matrix(char start[][WIDTH], char finish[][WIDTH]) {
    for (int i = 0; i < HIGHT; i++)
        for (int j = 0; j < WIDTH; j++)
            start[i][j] = finish[i][j];
}
// Печатаем поле со всеми клетками
void printing(char finish[][WIDTH]) {
    for (int i = 1; i < HIGHT - 1; i++) {
        for (int j = 1; j < WIDTH - 1; j++) {
            printf("%c", finish[i][j]);
        }
        printf("\n");
    }
    printf("\n\n");
}
// Заполняем поле пробелами перед стартом каждой игры
void zero(char start[][WIDTH], char finish[][WIDTH]) {
    for (int i = 0; i < HIGHT; i++)
        for (int j = 0; j < WIDTH; j++) {
            start[i][j] = '.';
            finish[i][j] = '.';
        }
}
// Вводим входные данные и печатаем новое поле
void first_move(char start[][WIDTH], char finish[][WIDTH], int* coordinates) {
    int count = 0;
    printf("Введите координаты начальных клеток в формате \"y x\"."
           "Для окончания ввода введите любой нечисловой символ.\n");
    while (scanf("%d%d", &coordinates[count], &coordinates[count + 1]) == 2)
        count += 2;
    for (int y = 0, x = 1; y < count; x += 2, y += 2) {
        start[coordinates[y]][coordinates[x]] = '@';
        finish[coordinates[y]][coordinates[x]] = '@';
    }
    printing(finish);
}
// Изменение клеток, отображаемое на конечном поле finish
int change_field(char start[][WIDTH], char finish[][WIDTH]) {
    int flag = 0;
    for (int i = 1; i < HIGHT - 1; i++)
        for (int j = 1; j < WIDTH - 1; j++)
            if (check_alive(start, finish, i, j))
                flag = 1;
    return flag;
}

void skip(void) {
    if (getchar() == '\n') fflush(stdout);
            else
                while (getchar() != '\n')
                    continue;
}
