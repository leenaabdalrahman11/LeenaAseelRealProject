#include "common.h"
#include "player.h"
#include "referee.h"

int pipes[NUM_PLAYERS][2];
pid_t player_pids[NUM_PLAYERS];
int ready_pipes[NUM_PLAYERS][2];

int main() {
    for (int i = 0; i < NUM_PLAYERS; i++) {
        pipe(pipes[i]);
        pipe(ready_pipes[i]);
        fcntl(pipes[i][0], F_SETFL, O_NONBLOCK);
        setbuf(stdout, NULL);

        pid_t pid = fork();
        if (pid == 0) {
            player_process(i);
            exit(0);
        } else {
            player_pids[i] = pid;
            sleep(1);
            close(pipes[i][1]);
            close(ready_pipes[i][1]);
        }
    }

    for (int i = 0; i < NUM_PLAYERS; i++) {
        int dummy;
        sleep(1);
        if (read(ready_pipes[i][0], &dummy, sizeof(int)) != -1)
            printf("Player %d is ready!\n", i);
        else
            printf("Player %d is not ready!\n", i);
        close(ready_pipes[i][0]);
    }

    referee_process();

    for (int i = 0; i < NUM_PLAYERS; i++) {
        wait(NULL);
    }

    return 0;
}
