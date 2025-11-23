#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include "auth.h"
#include "data.h"
#include "utils.h"

int main(void) {
    srand((unsigned)time(NULL));

    /* load DBs and leaves; note files are read on demand */
    load_users();
    load_sessions();
    load_attendance();
    load_leaves();

    while (1) {
        print_header("MAIN MENU");
        printf("1) Register Student\n");
        printf("2) Register Teacher\n");
        printf("3) Login\n");
        printf("4) Exit\n");

        char choicebuf[8];
        printf("Choose: ");
        input_line(choicebuf, sizeof(choicebuf));
        int choice = atoi(choicebuf);

        if (choice == 1) register_user_flow("student");
        else if (choice == 2) register_user_flow("teacher");
        else if (choice == 3) {
            char username[64], role[16];
            if (login_flow(username, role)) {
                if (strcmp(role, "admin") == 0) admin_dashboard_flow();
                else if (strcmp(role, "student") == 0) student_dashboard_flow(username);
                else teacher_dashboard_flow(username);
            }
        } else if (choice == 4) {
            printf("Goodbye.\n");
            break;
        } else {
            printf("Invalid option.\n");
        }
    }

    /* save on exit */
    save_users();
    save_sessions();
    save_attendance();
    save_leaves();

    return 0;
}