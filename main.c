#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <windows.h>

#define mapYsize 13
#define mapXsize 70

//Written on the job in a few hours. To optimize and rearrange later.
//Could fit easily in less than 150 lines of code.

unsigned char m_map[mapXsize][mapYsize];
unsigned char map_buffer[mapXsize][mapYsize];
DWORD WINAPI    GetTickCount(void);

int i;
int j;
int sector;
int v_line;
int h_line;
int v_pos;
int v_pos1;
int h_pos;
int points = 0;

typedef struct Player Player;
typedef struct Teleport Teleport;

struct Player
{
    int x;
    int y;
    boolean teleport;
    int p_size;
    int a_state_size;
    int old_x;
    int old_y;
};

struct Teleport
{
    int x;
    int y;
    int sector;
    int v_pos;
    int h_pos;
    int created;
};

unsigned int fps = ( 1000 / 8 );
uint32_t      tl_start    = 0;
uint32_t      tl_end      = 0;
uint32_t      tl_elapsed  = 0;

/*
====================
MapBuffering
    -> Copy the map at the end of the loop for
    mapUpdate optimization by changes comparison

    @param      map_buff    reference on previous map
    @param      map_        reference on actual map
====================
*/
void MapBuffering(unsigned char map_buff[][mapYsize], unsigned char map_[][mapYsize]){
    int x, y;
    for(y = 0; y < mapYsize; y++)
    {
        for( x = 0; x < mapXsize; x++)
        {
            map_buff[x][y] = map_[x][y];
        }
    }
}

int main()
{
    Player p_player;
    Teleport t_enter;
    Teleport t_exit;

    p_player.teleport = false;
    p_player.p_size = 4;
    p_player.a_state_size = p_player.p_size;

    srand(time(NULL));

    for(j = 0; j < mapYsize; j++){
        for(i = 0; i < mapXsize; i++){
            if (j == 0 || j == 12){
                m_map[i][j] = '=';
            }
            if(i == 0 || i == 69){
                m_map[i][j] = '|';
            }
            if(i == 10 || i == 50){
                m_map[i][j] = '|';
            }
            if(i != 0 && i != 69 && j%2 == 0 && j != 0 && j != 12 && i != 10 && i != 50){
                m_map[i][j] = '-';
            }
        }
    }

    //FIRST POSITION
    do{
        t_enter.h_pos =  rand() % 10 + 1;
    }while(t_enter.h_pos%2 != 0);
    t_enter.v_pos =  rand() % (5 - 2) + 2;

    switch(t_enter.v_pos){
    case 2:
        t_enter.x = 10;
        t_enter.y = t_enter.h_pos;
        sector  = 1;
        break;
    case 3:
        t_enter.x = 50;
        t_enter.y = t_enter.h_pos;
        sector = 2;
        break;
    case 4:
        t_enter.x = 69;
        t_enter.y = t_enter.h_pos;
        sector = 3;
        break;
    default:
        t_enter.x = 10;
        t_enter.y = t_enter.h_pos;
        sector = 1;
        break;
    }

    //Set enter position teleporter
    p_player.x = t_enter.x;
    p_player.y = t_enter.y;
    m_map[t_enter.x][t_enter.y] = 'O';

    //Exit teleport creation:
    do{
        t_exit.h_pos =  rand() % 10 + 1;
    }while(t_exit.h_pos%2 != 0);

    t_exit.v_pos = t_enter.v_pos - 1;

    switch(t_exit.v_pos){
    case 1:
        t_exit.x = 0;
        t_exit.y = t_exit.h_pos;
        break;
    case 2:
        t_exit.x = 10;
        t_exit.y = t_exit.h_pos;
        break;
    case 3:
        t_exit.x = 50;
        t_exit.y = t_exit.h_pos;
        break;
    }

    //Set exit position teleporter
    m_map[t_exit.x][t_exit.y] = 'O';

    while(1){
        tl_start = GetTickCount();

        //Vertical movement
        if(GetAsyncKeyState(VK_DOWN)){
            if(p_player.y < 10){
                m_map[p_player.x+3][p_player.y] = '-';
                m_map[p_player.x+2][p_player.y] = '-';
                m_map[p_player.x+1][p_player.y] = '-';
                m_map[p_player.x][p_player.y] = '-';
                p_player.y  += 2;

            }
        }
        if(GetAsyncKeyState(VK_UP)){
            if(p_player.y > 2){
                m_map[p_player.x+3][p_player.y] = '-';
                m_map[p_player.x+2][p_player.y] = '-';
                m_map[p_player.x+1][p_player.y] = '-';
                m_map[p_player.x][p_player.y] = '-';
                p_player.y -= 2;
            }
        }
        //+VITE

        //Horizontal movement(auto)
        if(m_map[p_player.x-1][p_player.y] != '|'){
            p_player.x--;
            m_map[p_player.x][p_player.y] = 167;

            if(m_map[p_player.x+4][p_player.y] == 'O'){
                m_map[p_player.x+4][p_player.y] = 'O';
            }

            if(m_map[p_player.x+4][p_player.y] != '|' && m_map[p_player.x+4][p_player.y] != 'O')
            m_map[p_player.x+4][p_player.y] = '-';
        }



        //Wall in face, but should be here a 'O
        if(m_map[p_player.x-1][p_player.y] == 'O'){
            p_player.teleport = true;
            m_map[t_enter.x][t_enter.y] = '|';
            p_player.old_x = p_player.x;
            p_player.old_y = p_player.y;

            switch(sector){
            case 1: points += 20;
                break;
            case 2: points += 5;
                break;
            case 3: points += 10;
                break;
            }

            COORD cPos;
            cPos.X = 25;
            cPos.Y = 16;
            SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), cPos);
            printf("Pts:[");
            SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 2);
            printf("%d", points);
            SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
            printf("]");
             do{
                t_enter.h_pos =  rand() % 10 + 1;
            }while(t_enter.h_pos%2 != 0);
            t_enter.v_pos =  rand() % (5 - 2) + 2;

                switch(t_enter.v_pos){
                    case 2:
                        t_enter.x = 10;
                        t_enter.y = t_enter.h_pos;
                        sector = 1;
                        break;
                    case 3:
                        t_enter.x = 50;
                        t_enter.y = t_enter.h_pos;
                        sector = 2;
                        break;
                    case 4:
                        t_enter.x = 69;
                        t_enter.y = t_enter.h_pos;
                        sector = 3;
                        break;
                    default:
                        t_enter.x = 10;
                        t_enter.y = t_enter.h_pos;
                        break;
                }

                //Set enter position teleporter
                p_player.x = t_enter.x;
                p_player.y = t_enter.y;
                m_map[t_enter.x][t_enter.y] = 'O';
        }

        //Teleport?
        if(p_player.teleport){
            if(p_player.a_state_size != 0){
                m_map[p_player.old_x + p_player.a_state_size - 1][p_player.old_y] = '-';
                p_player.a_state_size--;
            }
            if(p_player.a_state_size == 0){
                p_player.teleport = false;
                p_player.a_state_size = p_player.p_size;
                m_map[t_exit.x][t_exit.y] = '|';
                //Exit teleport creation:
                do{
                    t_exit.h_pos =  rand() % 10 + 1;
                }while(t_exit.h_pos%2 != 0);

                t_exit.v_pos = t_enter.v_pos - 1;

                switch(t_exit.v_pos){
                    case 1:
                        t_exit.x = 0;
                        t_exit.y = t_exit.h_pos;
                        break;
                    case 2:
                        t_exit.x = 10;
                        t_exit.y = t_exit.h_pos;
                        break;
                    case 3:
                        t_exit.x = 50;
                        t_exit.y = t_exit.h_pos;
                        break;
                }

                //Set exit position teleporter
                m_map[t_exit.x][t_exit.y] = 'O';
            }
        }

        //Display:
        for(j = 0; j < mapYsize; j++){
            for(i = 0; i < mapXsize; i++){
                if(map_buffer[i][j] != m_map[i][j]){
                    COORD cPos;
                    cPos.X = i;
                    cPos.Y = j;
                    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), cPos);
                    if(m_map[i][j] == 'O'){
                        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 9);
                    }else if(m_map[i][j] == 167){
                        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 11);
                    }
                    printf("%c", m_map[i][j]);
                    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
                }
            }
        }


        //FPS Timer:
        tl_end      = GetTickCount();
        tl_elapsed  = ( tl_end - tl_start );

        //If we need we stop a moment the Fps:
        if(tl_elapsed < fps)
            Sleep(fps - tl_elapsed);

        MapBuffering(map_buffer, m_map);
    }

    return 0;
}
