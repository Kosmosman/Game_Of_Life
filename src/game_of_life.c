// Copyright 2022 joaquind
#include <stdio.h>
#include <unistd.h>
#define HIGHT 25
#define WIDTH 80
void check_alive(char start[][WIDTH], char finish[][WIDTH], int i, int j);
void copy_matrix(char start[][WIDTH], char finish[][WIDTH]);
int printing(char start[][WIDTH], char finish[][WIDTH]);
void first_printing(char start[][WIDTH], char finish[][WIDTH], int* coordinates);
void zero(char start[][WIDTH], char finish[][WIDTH]);

int main(void) {
    char start[HIGHT][WIDTH], finish[HIGHT][WIDTH], ch;
    int coordinates[HIGHT * WIDTH * 2];
    printf("Добро пожаловать в Game Of Life! "
           "Для выхода введите q, для начал игры - любую другую клавишу.\n");
    while ((ch = getchar()) != 'q') {
        first_printing(start, finish, coordinates);
        while (printing(start, finish) > 0) {
            getchar();
            copy_matrix(start, finish);
        }
    }
    return 0;
}
// Проверка клетки на жизнеспособность
void check_alive(char start[][WIDTH], char finish[][WIDTH], int row, int column) {
    int count = 0;
    for (int m = row - 1; m <= row + 1; m++)
        for (int n = column - 1; n <= column + 1; n++)
            if ((start[m][n] == '@') && !(m == row && n == column)) {
                count++;
            }
    if (count < 2 || count > 3)
        finish[row][column] = '.';
    if (count == 3)
        finish[row][column] = '@';
    if (count == 2 && start[row][column] == '@')
        finish[row][column] = '@';
}
// Копируем матрицу из финальной версии в стартовую на каждом новом ходу
void copy_matrix(char start[][WIDTH], char finish[][WIDTH]) {
    for (int i = 0; i < HIGHT; i++)
        for (int j = 0; j < WIDTH; j++)
            start[i][j] = finish[i][j];
}
// Печатаем поле со всеми клетками
int printing(char start[][WIDTH], char finish[][WIDTH]) {
    int i, j, cells = 0;
    for (i = 1; i < HIGHT - 1; i++) {
        for (j = 1; j < WIDTH - 1; j++) {
            check_alive(start, finish, i, j);
            printf("%c", finish[i][j]);
            if (finish[i][j] == '@')
                cells++;
        }
        printf("\n");
    }
    printf("cells = %d\n", cells);
    printf("\n\n");
    return cells > 0;
}
// Заполняем поле пробелами перед стартом каждой игры
void zero(char start[][WIDTH], char finish[][WIDTH]) {
    for (int i = 0; i < HIGHT; i++)
        for (int j = 0; j < WIDTH; j++) {
            start[i][j] = '.';
            finish[i][j] = '.';
        }
}
// Печатает поле с введенными входными данными
void first_printing(char start[][WIDTH], char finish[][WIDTH], int* coordinates) {
    int count = 0;
    zero(start, finish);
    printf("Введите координаты начальных клеток в формате \"y x\"."
           "Для окончания ввода введите любой нечисловой символ.\n");
    while (scanf("%d%d", &coordinates[count], &coordinates[count + 1]) == 2)
        count += 2;
    for (int y = 0, x = 1; y < count; x += 2, y += 2)
        start[coordinates[y]][coordinates[x]] = '@';
    for (int i = 0; i < HIGHT; i++) {
        for (int j = 1; j < WIDTH; j++) {
            printf("%c", start[i][j]);
        }
        printf("\n");
    }
    printf("\n\n");
}
