#include "common.h"
#include "referee.h"

void referee_process() {
    printf("Referee started!\n");
    signal(SIGINT, SIG_IGN);

    int team1_score = 0, team2_score = 0;
    int round_count = 0;
    int last_winner = -1;
    int consecutive_wins = 0;
    int team1_effort = 0, team2_effort = 0;

    time_t start_time = time(NULL);

    while (1) {
        printf("\n🏁 Round %d starts!\n", round_count + 1);

        if (round_count >= 8) {
            printf("🏁 Maximum number of rounds reached.\n");
            break;
        }

        time_t now = time(NULL);
        if (difftime(now, start_time) >= 120) {
            printf("⏰ Match time (2 minutes) is over.\n");
            break;
        }

        printf("\n🔔 Round %d is in progress...\n", round_count + 1);

        team1_effort = 0;
        team2_effort = 0;

        time_t round_start = time(NULL);
        while (difftime(time(NULL), round_start) < 10) {
            printf("⏳ Waiting for players to report their effort...\n");

            for (int i = 0; i < NUM_PLAYERS; i++) {
                kill(player_pids[i], SIGUSR1);
                sleep(2);
            }

            sleep(1);

            for (int i = 0; i < NUM_PLAYERS; i++) {
                int effort = 0;
                if (read(pipes[i][0], &effort, sizeof(int)) > 0) {
                    if (effort == 0)
                        printf("⚠️ Player %d is injured!\n", i);
                    else
                        printf("✅ Player %d effort: %d\n", i, effort);

                    if (i < TEAM_SIZE)
                        team1_effort += effort;
                    else
                        team2_effort += effort;
                } else {
                    printf("⚠️ Player %d didn't respond!\n", i);
                }
            }

            printf("📊 Total Effort So Far - Team1: %d | Team2: %d\n", team1_effort, team2_effort);
        }

        round_count++;
        if (team1_effort > team2_effort) {
            team1_score++;
            printf("🏆 Team 1 wins this round!\n");

            if (last_winner == 0)
                consecutive_wins++;
            else {
                consecutive_wins = 1;
                last_winner = 0;
            }
        } else if (team2_effort > team1_effort) {
            team2_score++;
            printf("🏆 Team 2 wins this round!\n");

            if (last_winner == 1)
                consecutive_wins++;
            else {
                consecutive_wins = 1;
                last_winner = 1;
            }
        } else {
            printf("🤝 It's a draw!\n");
            consecutive_wins = 0;
            last_winner = -1;
        }

        printf("📈 Score after Round %d: Team1 = %d | Team2 = %d\n", round_count, team1_score, team2_score);

        if (consecutive_wins == 2) {
            printf("🏁 Team %d wins the match with two consecutive rounds!\n", last_winner + 1);
            break;
        }

        if (round_count < 4) {
            printf("⏸️ Resting for 5 seconds before next round...\n");
            sleep(5);
        }
    }

    printf("\n🏆 Final Score: Team1 = %d | Team2 = %d\n", team1_score, team2_score);

    for (int i = 0; i < NUM_PLAYERS; i++) {
        kill(player_pids[i], SIGKILL);
    }
}
