#include <ncurses.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>

#define DELAY 16000

typedef struct game_object {
    int x, y;
    int dx, dy;
    int width;
    int height;
} game_object;

void draw_object(game_object obj) {
    for (int i = 0; i < obj.width - 1; i++) {
        for (int j = 0; j < obj.height - 1; j++) {
            mvprintw(obj.y + j, obj.x + i, " ");
        }
    }
}

int checkCollision(game_object a, game_object b) {
    int left = (a.x + a.width) - b.x;   // How much A is inside B on the left
    int right = (b.x + b.width) - a.x;  // A is inside B from the right
                                                  //
    int top = (a.y + a.height) - b.y;  // How much A is inside B on the top
    int bottom = (b.y + b.height) - a.y; 


    if (a.x < b.x + b.width &&
        a.x + a.width > b.x &&
        a.y < b.y + b.height &&
        a.y + a.height > b.y) {

        int dx = left < right ? left : right;
        int dy = top < bottom ? top : bottom;

        if (dx == dy) {
            return 3;
        }

        return dx < dy ? 1 : 2;
    }

    return 0;
}

int gameloop(int score_p1, int score_p2) {
    int max_x = 0;
    int max_y = 0;
    int ball_frame = 0;
    int game_over = 0;
    int cpu_lag = 1;
    
    game_object ball = {0, 0, 0, 0, 5, 3};
    game_object paddle1 = {0, 0, 0, 0, 5, 11};
    game_object paddle2 = {0, 0, 0, 0, 5, 11};
    game_object arena_top = {0, 0, 0, 0, 0, 1};
    game_object arena_bottom = {0, 0, 0, 0, 0, 1};


    getmaxyx(stdscr, max_y, max_x);

    arena_top.width = max_x;

    arena_bottom.y = max_y;
    arena_bottom.width = max_x;
    arena_bottom.height = 1;

    ball.x = max_x / 2;
    ball.y = max_y / 2;

    paddle1.x = 10;
    paddle1.y = ball.y - (paddle1.height / 2) - ball.height;

    paddle2.x = max_x - 10;
    paddle2.y = ball.y - (paddle1.height / 2) - ball.height;

    ball.dx = rand() % 2 == 0 ? 1 : -1;
    ball.dy = rand() % 2 == 0 ? 1 : -1;

    while(1) {
        clear();
        getmaxyx(stdscr, max_y, max_x);
        draw_object(ball);
        draw_object(paddle1);
        draw_object(paddle2);
        mvprintw(5, max_x / 4, "%d", score_p1);
        mvprintw(5, max_x - (max_x / 4), "%d", score_p2);
        mvprintw(0, 0, "CPU LAG: %d", cpu_lag);
        refresh();

        int k = getch();

        if (paddle1.y >= 0 && k == 'w') {
            paddle1.y--;
        }else if (paddle1.y <= max_y - paddle1.height && k == 's') {
            paddle1.y++;
        }

        int paddle2_center = paddle2.y + (paddle2.height / 2);

        if (paddle2_center + abs(cpu_lag) < ball.y) {
            paddle2.y += 1;
        } else if (paddle2_center - abs(cpu_lag) > ball.y) {
            paddle2.y -= 1;
        }


        if (paddle2.y < 0) {
            paddle2.y = 0;
        } else if (paddle2.y > max_y - paddle2.height + 1) {
            paddle2.y = max_y - paddle2.height + 1;
        }

        if (ball_frame % 3 == 0 && ball_frame != 0) {

            if (ball_frame % 20 == 0) {
                cpu_lag += (rand() % 3) - 1;
            }

            if (ball.x < 6) {
                game_over = -1;
                break;
            } else if (ball.x > max_x - 6) {
                game_over = 1;
                break;
            }

            int collision = checkCollision(ball, paddle1);
            if (collision == 1) ball.dx = -ball.dx;
            if (collision == 2) ball.dy = -ball.dy;
            if (collision == 3) {
                ball.dx = -ball.dx;
                ball.dy = -ball.dy;
            }

            collision = checkCollision(ball, paddle2);
            if (collision == 1) ball.dx = -ball.dx;
            if (collision == 2) ball.dy = -ball.dy;
            if (collision == 3) {
                ball.dx = -ball.dx;
                ball.dy = -ball.dy;
            }

            if (checkCollision(ball, arena_top) == 2 || checkCollision(ball, arena_bottom) == 2) {
                ball.dy = -ball.dy;
            }

            ball.x += ball.dx;
            ball.y += ball.dy;
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


