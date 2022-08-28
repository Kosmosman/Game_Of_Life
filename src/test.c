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
int first_move(char start[][WIDTH], char finish[][WIDTH], int* coordinates);
void zero(char start[][WIDTH], char finish[][WIDTH]);
int change_field(char start[][WIDTH], char finish[][WIDTH]);
void make_buffer_zone(char start[][WIDTH]);
void set_keypress(void);
void reset_keypress(void);
void game_menu(void);
int speed_setting(char* ch, int* m);
void read_file(int* coordinates, FILE* stream);
int first_choice(char start[][WIDTH], char finish[][WIDTH], int* coordinates, int res, int try, int count);
int sec_choice(char start[][WIDTH], char finish[][WIDTH], int* coordinates, int res, int count);

static struct termios with_buffer;

int main(void) {
    fd_set settings;
    struct timeval tv;
    int m = 100000, flag = 1;
    char start[HIGHT][WIDTH], finish[HIGHT][WIDTH], ch;
    int coordinates[HIGHT * WIDTH * 2];
    zero(start, finish);
    clear();
    if (first_move(start, finish, coordinates) == 1) {
    make_buffer_zone(start);
    set_keypress();
    while (change_field(start, finish) > 0 && flag > 0) {
        FD_ZERO(&settings);
        FD_SET(0, &settings);
        tv.tv_sec = 0;
        tv.tv_usec = 0;
        int key = select(2, &settings, NULL, NULL, &tv);
        if (key)
            flag = speed_setting(&ch, &m);
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
    }
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
int first_move(char start[][WIDTH], char finish[][WIDTH], int* coordinates) {
    int in;
    char ch;
    int res = 1;
    int try = 0;
    int count = 0;
    printf("Добро пожаловать в Game Of Life!\n");
    printf("Выберите режим: \n");
    while (scanf("%d", &in) != 1 || ((in != 1) && (in != 2)) || (ch = getchar()) != '\n') {
           printf("Попробуйте еще раз!\n");
        while (getchar() != '\n')
            continue;
    }
    if (in == 1) {
        res = first_choice(start, finish, coordinates, res, try, count);
    } else if (in == 2) {
        res = sec_choice(start, finish, coordinates, res, count);
    }
    return res;
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
// Функция, контролирующая изменение скорости и выход из игры
int speed_setting(char* ch, int* m) {
    int flag = 1;
    *ch = getc(stdin);
    if (*ch == '+' && *m > 5000)
        *m -= 5000;
    if (*ch == '-')
        *m += 5000;
    if (*ch == 'q')
        flag = 0;
    return flag;
}
/* 20 20 19 19 18 19 20 21 18 18 12 12 13
 12 13 13 14 13 14 14 15 15 14 15 13 15 15 16 a */
int first_choice(char start[][WIDTH], char finish[][WIDTH], int* coordinates, int res, int try, int count) {
    int c;
    printf("Введите координаты начальных клеток в формате \"y x\"."
           "Для окончания ввода введите 'g'.\n");
    while (res == 1 && try == 0) {
    c = scanf("%d%d", &coordinates[count], &coordinates[count + 1]);
    if ((c == 2) && coordinates[count] <= 25 && coordinates[count] >= 0 && coordinates[count + 1] <= 80 && coordinates[count + 1] >= 0) {
        count += 2;
        res = 1;
    } else if (getchar() == 'g') {
        res = 1;
        try = 1;
    } else {
        printf("Некорректный ввод! Попробуйте еще раз. Вводимые числа по У не должны быть отрицательными и быть больше 25. По Х числа не могут быть отрицательными и быть больше 80. Вводимые значения должны быть целыми.\n");
        while (getchar() != '\n')
            continue;
        res = 1;
        try = 0;
        count = 0;
    }
    }
    if ((res == 1)) {
for (int y = 0, x = 1; y < count; x += 2, y += 2) {
    start[coordinates[y]][coordinates[x]] = '@';
    finish[coordinates[y]][coordinates[x]] = '@';
}
printing(finish);
    }
return res;
}

int sec_choice(char start[][WIDTH], char finish[][WIDTH], int* coordinates, int res, int count) {
    char pattern[100];
    res = 0;
    printf("Введите путь до файла вместе с ним самим: ");
    while (res == 0) {
    scanf("%s", pattern);
    FILE* stream = fopen(pattern, "r");
    if (stream != NULL) {
    while (fscanf(stream, "%d%d", &coordinates[count],
                  &coordinates[count + 1]) == 2) {
        count += 2;
        res = 1;
    }
    fclose(stream);
    } else {
        printf("Некорректное имя файла, попробуйте еще раз. В формате: './files/file.txt'\n");
               res = 0;
    }
    if (res == 1) {
for (int y = 0, x = 1; y < count; x += 2, y += 2) {
    start[coordinates[y]][coordinates[x]] = '@';
    finish[coordinates[y]][coordinates[x]] = '@';
}
printing(finish);
    }
    }
return res;
}
