#include<iostream>
#include<vector>
#include<sstream>
#include<conio.h>
#include<time.h>
#include"console.h"
using namespace std;
#define ii pair<int,int>
#define const_prey -1

// direction : UP LEFT RIGHT DOWN
int cc[4] = {0 ,-1,1 ,0 };
int cr[4] = {-1,0 ,0 ,1 };
bool exit_game = false;
int screen[100][100];
int console_max_row, console_max_col;
string color_option[16] ={"black","dark blue","dark green","dark cyan","dark red","dark magenta","dark yellow","dark white","gray","blue",
"green","cyan","red","magenta","yellow","white"};

menu main_menu({20,1},colorToInt(console_yellow,console_black),colorToInt(console_red,console_black));
menu end_menu({1,1},colorToInt(console_yellow,console_black),colorToInt(console_red,console_black));
vector<COORD> console_size;

bool inrect(int x, int y, COORD top_left, COORD bot_right);
ii myrandom(COORD top_left, COORD bot_right);
int increase(int s);
void game_over();

void init_game(){
    COORD max_size;
    // menu begin game
    main_menu.add_option("play");
    main_menu.add_option("difficult");
    main_menu.add_option("window size");
    main_menu.add_option("snake color");
    main_menu.add_option("prey color");
    main_menu.add_option("background");
    main_menu.add_option("exit");
    // difficult button - option 2
    main_menu.add_option_value("easy",2);
    main_menu.add_option_value("medium",2);
    main_menu.add_option_value("hard",2);
    // windows size button - option 3
    string tmp_str;
    _console_get_max_window(max_size);
    console_size.push_back({45,30});
    console_size.push_back({60,40});
    console_size.push_back({75,50});
    int max_size_option = 1;
    for (int i = 0;i<console_size.size();i++){
        if (max_size.X>=console_size[i].X && max_size.Y>=console_size[i].Y){
                stringstream stream;
                stream << console_size[i].X << "x" << console_size[i].Y;
                tmp_str = stream.str();
                main_menu.add_option_value(tmp_str,3);
                max_size_option = i;
        }
    }
    main_menu.value_select[2] = max_size_option;
    // snake color button - option 4
    for (int i=0;i<16;i++){
        main_menu.add_option_value(color_option[i],4);
    }
    main_menu.value_select[3] = console_green+1;
    // prey color button - option 5
    for (int i=0;i<16;i++){
        main_menu.add_option_value(color_option[i],5);
    }
    main_menu.value_select[4] = console_red+1;
    // background button -option 6
    for (int i=0;i<16;i++){
        main_menu.add_option_value(color_option[i],6);
    }
    main_menu.value_select[5] = console_black+1;
    //---------------------------------------------------------------------
    // menu end game
    end_menu.add_option("restart");
    end_menu.add_option("exit");
    //-----------------------------------------------------------
}

void begin_game(){
    // resize console window
    _console_resize(45,30);
    _console_no_scroll();
    //change menu position
    main_menu.change_menu_position({11,15});

    //snake print
    _console_draw(9,1,"  ____              _        \n",10);
    _console_draw(9,2," / ___| _ __   __ _| | _____ \n",10);
    _console_draw(9,3," \\___ \\| \'_ \\ / _` | |/ / _ \\\n",10);
    _console_draw(9,4,"  ___) | | | | (_| |   <  __/\n",10);
    _console_draw(9,5," |____/|_| |_|\\__,_|_|\\_\\___|\n",10);

    _console_draw(15,10,"Author: Quan Luong",10);
    _console_set_cursor(false,0);
    //show menu
    main_menu.print();
    bool repeat = true;
    int now_option = 1;
    while (repeat && !exit_game){
        _console_set_cursor(false,0);
        if (_kbhit()){
            char c = _getch();
            string str;
            switch (c){
                case 'a':
                case 'A'://left
                    main_menu.change_option_value(now_option,'p',true);
                    break;
                case 'd':
                case 'D'://right
                    main_menu.change_option_value(now_option,'n',true);
                    break;
                case 's':
                case 'S'://down
                    if (now_option==main_menu.menu_option.size())
                        break;
                    main_menu.change_option('n',true);
                    now_option++;
                    break;
                case 'w':
                case 'W'://up
                    if (now_option==1)
                        break;
                    main_menu.change_option('p',true);
                    now_option--;
                    break;
                case VK_RETURN:
                    if (now_option == 1){
                        repeat  = false;
                    }
                    if (now_option == 7){
                        exit_game = true;
                    }
                    break;
                case 'x':
                case 'X':
                    exit_game = true;
                break;
            }
        }
    }
}

void play_game(){
    bool repeat = true;
    my_time now,last;
    int column,row;
    int begin_snake_length;
    int snake_length, eated_prey;
    snake_length = 2;
    eated_prey = 0;
    int snake_direction = 2; // right
    ii prey;
    ii snake_head, snake_back;

    _console_clr();
    _console_resize(console_size[main_menu.value_select[2]-1].X,console_size[main_menu.value_select[2]-1].Y);
    //  get window size
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(hOut,&csbi);
    column = csbi.srWindow.Right - csbi.srWindow.Left + 1;
    row = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
    row--;
    COORD top_left  = {2,2};
    COORD bot_right = {column-1,row-1};
    //init value game
    int ms_step, grow;
    int const_snake;
    int color_prey, color_snake, background_color;
    bool change_direction;
    switch (main_menu.value_select[1]){
        case 1:
            grow = 5;
            begin_snake_length = (column)*0.1;
            ms_step = 150;
            break;
        case 2:
            grow = 2;
            begin_snake_length = (column)*0.1;
            ms_step = 100;
            break;
        case 3:
            grow = 1;
            begin_snake_length = (column)*0.1;
            ms_step = 50;
            break;
    }
    for (int i=1;i<=column;i++)
        for (int j=1;j<=row;j++){
            screen[i][j] = 0;
        }
    color_prey  = (main_menu.value_select[4]-1)<<4;
    color_snake = (main_menu.value_select[3]-1)<<4;
    background_color = (main_menu.value_select[5]-1)<<4;
    // print table
    string str = " ";
    str[0] = char(201); // top left
    _console_draw(1,1,str,colorToInt(15,0));
    str[0] = char(200); // bot left
    _console_draw(1,row,str,colorToInt(15,0));
    str[0] = char(187); // top right
    _console_draw(column,1,str,colorToInt(15,0));
    str[0] = char(188); // bot right
    _console_draw(column,row,str,colorToInt(15,0));
    //-------------------------------------------------
    str[0] = char(205);
    for (int i=2;i<=column-1;i++){
        _console_draw(i,1,str,colorToInt(15,0));
        _console_draw(i,row,str,colorToInt(15,0));
    }
    //|||||||||||||||||||||||||||||||||||||||||||||||||
    str[0] = char(186);
    for (int i=2;i<=row-1;i++){
        _console_draw(1,i,str,colorToInt(15,0));
        _console_draw(column,i,str,colorToInt(15,0));
    }
    // space space space space space space space space space
    for (int i=2;i<=column-1;i++){
        for (int j=2;j<=row-1;j++){
            _console_draw(i,j," ",background_color);
        }
    }
    // draw snake
    int snake_x = column/begin_snake_length+1;
    int snake_y = row/2;
    const_snake = 1;
    snake_back = ii(snake_x,snake_y);
    snake_head = ii(snake_x+begin_snake_length-1,snake_y);
    for (int i=snake_x;i<=snake_x+begin_snake_length-1;i++){
        _console_draw(i,snake_y," ",color_snake);
        screen[i][snake_y] = const_snake;
        const_snake = increase(const_snake);
    }
    snake_length = begin_snake_length;
    // draw prey
    prey = myrandom(top_left,bot_right);
    screen[prey.first][prey.second] = const_prey;
    _console_draw(prey.first,prey.second," ",color_prey);
    // print length snake
    _console_gotoxy(10,row+1); cout << "length of snake: "<<snake_length;
    //game play
    last.time_save();
    while (repeat && !exit_game){
        _console_set_cursor(false,0);
        now.time_save();
        if (diff_time(last,now)>ms_step){// let snake go
            change_direction = false;
            //inserst head of snake
            snake_head.first  += cc[snake_direction];
            snake_head.second += cr[snake_direction];
            if (inrect(snake_head.first,snake_head.second,top_left,bot_right)==false){
                repeat = false;
            }
            if (screen[snake_head.first][snake_head.second]>0){// snake hit his tail
                repeat = false;
            }
            if (screen[snake_head.first][snake_head.second]==-1){
                eated_prey ++;

                snake_length ++;
                _console_gotoxy(10,row+1); cout << "length of snake: "<<snake_length;
                if (ms_step>=30){
                    if (eated_prey%5==0){
                        ms_step -= 2;
                    }
                }

                prey = myrandom(top_left,bot_right);
                screen[prey.first][prey.second] = const_prey;
                _console_draw(prey.first,prey.second," ",color_prey);
                last.time_save();
                // the snake is longer
                screen[snake_head.first][snake_head.second] = const_snake;
                const_snake = increase(const_snake);
                _console_draw(snake_head.first,snake_head.second," ",color_snake);
                continue;
            }
            screen[snake_head.first][snake_head.second] = const_snake;
            const_snake = increase(const_snake);
            _console_draw(snake_head.first,snake_head.second," ",color_snake);

            // delete back of snake
            if (snake_length-begin_snake_length < (eated_prey*grow)){
                snake_length=snake_length+1;
                _console_gotoxy(10,row+1); cout << "length of snake: "<<snake_length;
                last.time_save();
                continue;
            }
            int tmp = increase(screen[snake_back.first][snake_back.second]);
            for (int k=0;k<4;k++){
                int newc = snake_back.first + cc[k];
                int newr = snake_back.second + cr[k];
                if (screen[newc][newr]==tmp){
                    screen[snake_back.first][snake_back.second] = 0;
                    _console_draw(snake_back.first,snake_back.second," ",background_color);
                    snake_back = make_pair(newc,newr);
                    break;
                }
            }

            last.time_save();
        }
        if (_kbhit()){
            char button = _getch();
          if (change_direction==false){
            change_direction = true;
            switch (button){
                case 'x':
                case 'X':
                    exit_game = true;
                    break;
//                case VK_LEFT://1
                case 'a':
                case 'A':
                    if (snake_direction == 0 || snake_direction == 3)
                        snake_direction = 1;
                    break;
//                case VK_RIGHT://2
                case 'd':
                case 'D':
                    if (snake_direction == 0 || snake_direction == 3)
                        snake_direction = 2;
                    break;
//                case VK_UP://0
                case 'w':
                case 'W':
                    if (snake_direction == 1 || snake_direction == 2)
                        snake_direction = 0;
                    break;
//                case VK_DOWN://3
                case 's':
                case 'S':
                    if (snake_direction == 1 || snake_direction == 2)
                        snake_direction = 3;
                    break;
            }
          }
          Sleep(10);
        }
        if (!repeat){
            game_over();
        }
    }
}

void game_over(){
    _console_resize(45,30);
    _console_no_scroll();
    _console_set_cursor(false,0);
    _console_clr();
    _console_draw(1,1,"  /$$$$$$                                   \n",12);
    _console_draw(1,2," /$$__  $$                                  \n",12);
    _console_draw(1,3,"| $$  \\__/  /$$$$$$  /$$$$$$/$$$$   /$$$$$$ \n",12);
    _console_draw(1,4,"| $$ /$$$$ |____  $$| $$_  $$_  $$ /$$__  $$\n",12);
    _console_draw(1,5,"| $$|_  $$  /$$$$$$$| $$ \\ $$ \\ $$| $$$$$$$$\n",12);
    _console_draw(1,6,"| $$  \\ $$ /$$__  $$| $$ | $$ | $$| $$_____/\n",12);
    _console_draw(1,7,"|  $$$$$$/|  $$$$$$$| $$ | $$ | $$|  $$$$$$$\n",12);
    _console_draw(1,8," \\______/  \\_______/|__/ |__/ |__/ \\_______/\n",12);
    _console_draw(1,9,"\n",12);
    _console_draw(1,10,"\n",12);
    _console_draw(1,11,"  /$$$$$$                               \n",12);
    _console_draw(1,12," /$$__  $$                              \n",12);
    _console_draw(1,13,"| $$  \\ $$ /$$    /$$ /$$$$$$   /$$$$$$ \n",12);
    _console_draw(1,14,"| $$  | $$|  $$  /$$//$$__  $$ /$$__  $$\n",12);
    _console_draw(1,15,"| $$  | $$ \\  $$/$$/| $$$$$$$$| $$  \\__/\n",12);
    _console_draw(1,16,"| $$  | $$  \\  $$$/ | $$_____/| $$      \n",12);
    _console_draw(1,17,"|  $$$$$$/   \\  $/  |  $$$$$$$| $$      \n",12);
    _console_draw(1,18," \\______/     \\_/    \\_______/|__/       \n",12);
}

void end_game(){
    if (exit_game){
        _console_clr();
    }
    end_menu.change_menu_position({20,25});
    end_menu.print();
    int now_option = 1;
    bool repeat = true;
    while (repeat && !exit_game){
        _console_set_cursor(false,0);
        if (_kbhit()){
            char button = _getch();
                switch (button){
                    case 'x':
                    case 'X':
                        exit_game = true;
                        break;
//                case VK_UP://0
                    case 'w':
                    case 'W':
                        if (now_option == 1)
                            break;
                        now_option--;
                        end_menu.change_option('p',true);
                        break;
//                case VK_DOWN://3
                    case 's':
                    case 'S':
                        if (now_option == 2)
                            break;
                        now_option++;
                        end_menu.change_option('n',true);
                        break;
                    case VK_RETURN:
                        if (now_option == 2)
                            exit_game = true;
                        if (now_option == 1)
                            repeat = false;
                        break;
                }
        }
    }
    end_menu.clr();
}

int main(){
    _console_init();
    init_game();
    while (!exit_game){
        _console_set_cursor(false,0);
        begin_game();
        play_game();
        end_game();
    }
    system("pause");
}

bool inrect(int x, int y, COORD top_left, COORD bot_right){
    if (top_left.X<=x && x<=bot_right.X && top_left.Y<=y && y<=bot_right.Y){
        return true;
    }
    return false;
}

ii myrandom(COORD top_left, COORD bot_right){
    int c,r;
    do{
        srand(time(NULL));
        c=rand()%(bot_right.X-top_left.X+1)+top_left.X;
        srand(time(NULL));
        r=rand()%(bot_right.Y-top_left.Y+1)+top_left.Y;
        if (!inrect(c,r,top_left,bot_right)){
            continue;
        }
    }
    while (screen[c][r]>0); // prey in snake
    return make_pair(c,r);
}

increase(int s){
    int tmp = (s+1)%10000;
    if (tmp == 0){
        return 10000;
    }
    return tmp;
}
