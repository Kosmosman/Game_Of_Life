// Copyright 2022 joaquind
#include <stdio.h>
#include <unistd.h>
#include <termios.h>
#define HIGHT 27
#define WIDTH 82
#define clear() printf("\033[H\033[J")
int check_alive(char start[][WIDTH], char finish[][WIDTH], int i, int j);
void copy_matrix(char start[][WIDTH], char finish[][WIDTH]);
void printing(char finish[][WIDTH]);
void first_move(char start[][WIDTH], char finish[][WIDTH], int* coordinates);
void zero(char start[][WIDTH], char finish[][WIDTH]);
int change_field(char start[][WIDTH], char finish[][WIDTH]);
void make_buffer_zone(char start[][WIDTH]);
void set_keypress(void);
void reset_keypress(void);
void game_menu(void);

static struct termios with_buffer;

int main(void) {
    fd_set settings;
    struct timeval tv;
    int m = 100000, flag = 1;
    char start[HIGHT][WIDTH], finish[HIGHT][WIDTH], ch;
    int coordinates[HIGHT * WIDTH * 2];
    zero(start, finish);
    clear();
    printf("Добро пожаловать в Game Of Life!\n");
    first_move(start, finish, coordinates);
    make_buffer_zone(start);
    sleep(1);
    set_keypress();
    while (change_field(start, finish) > 0 && flag > 0) {
        FD_ZERO(&settings);
        FD_SET(0, &settings);
        tv.tv_sec = 0;
        tv.tv_usec = 0;
        int key = select(2, &settings, NULL, NULL, &tv);
        if (key) {
            ch = getc(stdin);
            if (ch == '+')
                m -= 5000;
            if (ch == '-')
                m += 5000;
            if (ch == 'q')
                flag = 0;
        }
        usleep(m);
        clear();
        game_menu();
        printing(finish);
        copy_matrix(start, finish);
        make_buffer_zone(start);
    }
    reset_keypress();
    printing(finish);
    printf("Вы закончили игру!\n");
    return 0;
}
// Проверка клетки на жизнеспособность
int check_alive(char start[][WIDTH], char finish[][WIDTH],
                int row, int column) {
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
        start[coordinates[y] + 1][coordinates[x] + 1] = '@';
        finish[coordinates[y] + 1][coordinates[x] + 1] = '@';
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
// Создание оболочки вокруг основного поля,
// где указываются значения соседних клеток
void make_buffer_zone(char start[][WIDTH]) {
    for (int i = 1; i < HIGHT - 1; i++) {
        start[i][0] = start[i][WIDTH - 2];
        start[i][WIDTH - 1] = start[i][1];
    }
    for (int j = 1; j < WIDTH - 1; j++) {
        start[0][j] = start[HIGHT - 2][j];
        start[HIGHT - 1][j] = start[1][j];
    }
    start[0][0] = start[HIGHT - 2][WIDTH - 2];
    start[HIGHT - 1][WIDTH - 1] = start[1][1];
    start[0][WIDTH - 1] = start[HIGHT - 2][1];
    start[HIGHT - 1][0] = start[1][WIDTH - 2];
}
// Перевод в неканонический режим для небуферизированного ввода символов
void set_keypress(void) {
    struct termios without_buffer;
    tcgetattr(0, &with_buffer);
    without_buffer = with_buffer;
    without_buffer.c_lflag &= (~ICANON & ~ECHO);
    without_buffer.c_cc[VTIME] = 0;
    without_buffer.c_cc[VMIN] = 1;
    tcsetattr(0, TCSANOW, &without_buffer);
}
// Возврак к каноническому режиму
void reset_keypress(void) {
    tcsetattr(0, TCSANOW, &with_buffer);
}
// Игровое меню, постоянно отображающееся во время игры
void game_menu(void) {
    printf("Для увеличения скорости введите \"+\","
           "для уменьшения - введите \"-\"\n");
    printf("Для выхода введите q\n");
}

/* 20 20 19 19 18 19 20 21 18 18 12 12 13
 12 13 13 14 13 14 14 15 15 14 15 13 15 15 16 a */
