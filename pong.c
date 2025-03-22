#include <ncurses.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>

#define DELAY 16000

void draw_ball(int x, int y, int r) {

    for (int i = 0; i <= r / 2; i++) {
        for (int j = 0; j <= r; j++) {
            mvprintw(y + i, x + j, " ");
        }
    }
}

void draw_paddle(int x, int y){
    for (int i = 0; i < 10; i++) {
        for (int j = 0; j < 4; j++) {
            mvprintw(y + i, x + j, " ");
        }
    }
}

int gameloop(int score_p1, int score_p2) {
    int y_p2 = 0;
    int y_p1 = 0;
    int max_x = 0;
    int max_y = 0;

    int ball_x = 0;
    int ball_y = 0;
    int radius = 4;
    int ball_frame = 0;

    getmaxyx(stdscr, ball_y, ball_x);
    ball_x = ball_x / 2;
    ball_y = ball_y / 2;

    y_p1 = ball_y - 4;
    y_p2 = ball_y - 4;

    int ball_dx = rand() % 2 == 0 ? 1 : -1;
    int ball_dy = rand() % 2 == 0 ? 1 : -1;

    int game_over = 0;
    int cpu_lag = 1;

    while(1) {
        clear();
        getmaxyx(stdscr, max_y, max_x);
        draw_paddle(10, y_p1);
        draw_paddle(max_x - 14, y_p2);
        draw_ball(ball_x, ball_y, radius);
        mvprintw(5, max_x / 4, "%d", score_p1);
        mvprintw(5, max_x - (max_x / 4), "%d", score_p2);
        refresh();

        int k = getch();

        if (y_p1 >= 0 && k == 'w') {
            y_p1--;
        }else if (y_p1 <= max_y - 10 && k == 's') {
            y_p1++;
        }

        if (ball_frame % 3 == 0 && ball_frame != 0) {

            if (ball_frame % 20 == 0) {
                cpu_lag += (rand() % 4) - 1;
            }

            y_p2 = ball_y - 4 + (ball_dy * cpu_lag);

            if (y_p2 < 0) {
                y_p2 = 0;
            } else if (y_p2 > max_y - 10) {
                y_p2 = max_y - 10;
            }

            if (ball_x < 6) {
                game_over = -1;
                break;
            } else if (ball_x > max_x - 6) {
                game_over = 1;
                break;
            }

            if (ball_y <= 0 || ball_y >= max_y - radius) {
                ball_dy = -ball_dy;
            }

            if (ball_x >= 10 && ball_x <= 10 + radius) {
                if ( (ball_y > y_p1 && ball_y < y_p1 + 10) || (ball_y + radius > y_p1 && ball_y + radius < y_p1 + 10)){
                    ball_dx = -ball_dx;
                }
            }

            if (ball_x >= max_x - 14 - radius && ball_x <= max_x - 10) {
                if ( (ball_y > y_p2 && ball_y < y_p2 + 10) || (ball_y + radius > y_p2 && ball_y + radius < y_p2 + 10)){
                    ball_dx = -ball_dx;
                }
            }

            ball_x += ball_dx;
            ball_y += ball_dy;
            ball_frame++;

        } else {
            ball_frame++;
        }

        usleep(DELAY);

    }

    endwin();

    return game_over;
}

int main() {
    srand(time(NULL));
    initscr();
    noecho();
    curs_set(FALSE);
    keypad(stdscr, TRUE);
    timeout(0);

    start_color();
    init_pair(2, COLOR_WHITE, COLOR_BLACK);
    bkgd(COLOR_PAIR(2));

    init_pair(1, COLOR_BLACK, COLOR_RED);
    attron(COLOR_PAIR(1));

    int score_p1 = 0;
    int score_p2 = 0;

    while(1) {
        int game_over = gameloop(score_p1, score_p2);

        if (game_over == 1) {
            score_p1++;
        } else if (game_over == -1) {
            score_p2++;
        }
    }

}


