//console.h ver 1
#include"console.h"
//console function

_console console;

void _console_get_max_window(COORD &max_size){
    max_size.X = console.max_column;
    max_size.Y = console.max_row;
}

void _console_init(void){
    console.column = 80;
    console.row = 25;
    system("mode 650");
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(hOut,&csbi);
    console.max_column = csbi.srWindow.Right - csbi.srWindow.Left + 1;
    console.max_row    = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;

    _console_resize(80,25);
}

void _console_pixel_resize(int width, int height){
    //resize window
    HWND conso = GetConsoleWindow();
    RECT re;
    GetWindowRect(conso, &re);
    MoveWindow(conso,re.left,re.top,width,height,TRUE);
    //  get window size
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(hOut,&csbi);
    console.column = csbi.srWindow.Right - csbi.srWindow.Left + 1;
    console.row    = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
}
void _console_no_scroll(){
    int column, row;
    //  get window size
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(hOut,&csbi);
    column = csbi.srWindow.Right - csbi.srWindow.Left + 1;
    row = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;

    //    no scroll
    COORD new_size = {column,row};
    SetConsoleScreenBufferSize(hOut,new_size);
}

void _console_resize(int column, int row){
    char tmp_string[256];
    sprintf(tmp_string,"mode %d,%d",column,row);
    system(tmp_string);
    console.column = column;
    console.row    = row;
}

void _console_clr(){
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    COORD dwWriteCoord = {0,0};
    DWORD dwSize = console.column*console.row;
    DWORD charWriten;
    FillConsoleOutputCharacter(hOut,(TCHAR) ' '    ,dwSize,dwWriteCoord,&charWriten);
    FillConsoleOutputAttribute(hOut,console_default,dwSize,dwWriteCoord,&charWriten);
}

void _console_gotoxy(int x, int y){
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    COORD position = {x-1,y-1};
    SetConsoleCursorPosition(hOut,position);
}

int _console_where_x(){
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    if (GetConsoleScreenBufferInfo(hOut,&csbi)){
        return csbi.dwCursorPosition.X;
    }
    return -1;
}

int _console_where_y(){
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    if (GetConsoleScreenBufferInfo(hOut,&csbi)){
        return csbi.dwCursorPosition.Y;
    }
    return -1;
}

void _console_text_color(int color){
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hOut,color);
}

void _console_set_cursor(bool visible, int cursor_size){
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    if (cursor_size<=0){
        cursor_size = 20; // default size of cursor
    }
    CONSOLE_CURSOR_INFO cci;
    cci.bVisible = visible;
    cci.dwSize = cursor_size;
    SetConsoleCursorInfo(hOut, &cci);
}

void _console_draw(int x, int y,string str,int color){
    int background_color, text_color;
    //print text
    _console_gotoxy(x,y);

    text_color = color % 16;
    _console_text_color(text_color);
    std::cout << str;

    //fill background
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    COORD dwWriteCoord = {x-1,y-1};
    DWORD charWriten;
    if (!FillConsoleOutputAttribute(hOut,(DWORD)color,str.length(),dwWriteCoord,&charWriten)){
            return;
    }

    //reset to default
    _console_text_color(console_default);
}
void _console_write(string str){
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(hOut,&csbi);
    _console_draw(_console_where_x()+1,_console_where_y()+1,str,csbi.wAttributes);
}

void _console_print_buffer(){
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD dwSize;
    char* c = (char*)malloc(dwSize*sizeof(char));
    COORD coord_read = {0,0};
    DWORD charWriten;
    ReadConsoleOutputCharacter(hOut,(char*)&c,dwSize,coord_read,&charWriten);
}

int colorToInt(int background, int text){
    return (background<<4)|text;// bg*16+text
}
//-----------------------------------------------------------------------

//time function

void my_time::time_save(){
    SYSTEMTIME ST;
    GetSystemTime(&ST);
    time_array[0] = ST.wHour;
    time_array[1] = ST.wMinute;
    time_array[2] = ST.wSecond;
    time_array[3] = ST.wMilliseconds;
}
long long diff_time(my_time a, my_time b){
    int tmp = 0;
    for (int i=0;i<=3;i++){
        tmp += (b.time_array[i]-a.time_array[i])*time_val[i];
    }
    if (tmp < 0)
        tmp = time_val[0] + tmp;
    return tmp;
}
//--------------------------------------------------------------------------

//menu function

int max(int a, int b){
    if (a>b) return a;
    return b;
}

void menu::change_menu_position(COORD position){
    int x,y;
    x = position.X-top_left.X;
    y = position.Y-top_left.Y;
    top_left = position;
    bot_right.X += x;
    bot_right.Y += y;
}

void menu::add_option(string name_option){
    menu_option.push_back(name_option);
    this->bot_right.X = max(this->bot_right.X,name_option.length()-1+(this->top_left.X));
    this->bot_right.Y ++;
    value_select.push_back(-1);
    n_value_select.push_back(0);
}

int real_length_option(string str){
    for (int i=0;i<str.length();i++){
        if (str[i]=='>'){
            return i;//(i-1)+1
        }
    }
}

void menu::add_option_value(string name_value,int option){
    menu_option[option-1] = menu_option[option-1]+">"+name_value;
    int real_length = real_length_option(menu_option[option-1]);
    this->bot_right.X = max(bot_right.X,top_left.X+real_length-1+name_value.length()+2);//name_option<name_value>
    if (value_select[option-1]<0){
        value_select[option-1] = 1;
    }
    n_value_select[option-1]++;
}

string menu::option_str(int option){
    int len = menu_option[option-1].length();
    string option_tmp = "";
    string value_option = "";
    for (int i=0;i<len;i++){
        if (menu_option[option-1][i]=='>'){
            int n=0;
            for (int j=i;j<len;j++){
                if (menu_option[option-1][j]=='>'){
                    n++;
                }
                if (n>=value_select[option-1]){
                    for (int k = j+1;k<len;k++){
                        if (menu_option[option-1][k]=='>'){
                            break;
                        }
                        value_option+=menu_option[option-1][k];
                    }
                    break;
                }
            }
            break;
        }
        option_tmp += menu_option[option-1][i];
    }
    int menu_width = this->bot_right.X-this->top_left.X+1;
    if (value_option!=""){
        int n = (menu_width-(option_tmp.length()+value_option.length()+2)) ;
        for (int i=1;i<=n;i++){
            option_tmp=option_tmp+" ";
        }
//        cout << value_option << "\n";
        option_tmp= option_tmp+"<"+value_option+">";
    }
    while (option_tmp.length()<menu_width) option_tmp+=" ";
    return option_tmp;
}

void menu::print_select_option(int option){
    int x,y;
    x = top_left.X;
    y = top_left.Y+option-1;
    _console_gotoxy(x,y);
    _console_text_color(color_select);
    _console_write(option_str(option));
}

void menu::print_unselect_option(int option){
    int x,y;
    x = top_left.X;
    y = top_left.Y+option-1;
    _console_gotoxy(x,y);
    _console_text_color(color_unselect);
    _console_write(option_str(option));
}

void menu::print(){
    // i+1 because option = 1.. size
    for (int i=0;i<menu_option.size();i++){
        if (i+1==select_option){
            print_select_option(i+1);
        }else{
            print_unselect_option(i+1);
        }
    }
}

void menu::clr(){
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD charWriten;
    DWORD dwSize = bot_right.X-top_left.X+1;
    COORD dwWriteCoord;
    for (int i=1;i<=menu_option.size();i++){
        dwWriteCoord = {top_left.X-1,top_left.Y+i-2};
        FillConsoleOutputCharacter(hOut,(TCHAR) ' '    ,dwSize,dwWriteCoord,&charWriten);
        FillConsoleOutputAttribute(hOut,console_default,dwSize,dwWriteCoord,&charWriten);
    }
}

void menu::change_option(char c,bool print){
    int new_option;
    if (c=='n'){//next
        new_option = (select_option+1)% menu_option.size();
        if (new_option==0)
            new_option = menu_option.size();
    }
    if (c=='p'){//previous
        new_option = (select_option+menu_option.size()-1) % menu_option.size();
        if (new_option==0){
            new_option = menu_option.size();
        }
    }
    if (print){
        print_select_option(new_option);
        print_unselect_option(select_option);
    }
    select_option = new_option;
}

void menu::change_option_value(int option, char c, bool print){
    int tmp = value_select[option-1];
    if (tmp<0){
        return;
    }
    int new_value_option;
    if (c=='n'){
        new_value_option = (tmp+1)%n_value_select[option-1];
        if (new_value_option == 0){
            new_value_option = n_value_select[option-1];
        }
    }
    if (c=='p'){
        new_value_option = (tmp+n_value_select[option-1]-1)%n_value_select[option-1];
        if (new_value_option == 0){
            new_value_option = n_value_select[option-1];
        }
    }
    value_select[option-1] = new_value_option;
    if (print){
        if (select_option==option){
            print_select_option(option);
        }else{
            print_unselect_option(option);
        }
    }
}
