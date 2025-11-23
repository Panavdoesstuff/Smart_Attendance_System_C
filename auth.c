#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "auth.h"
#include "utils.h"

const char *USERS_DB = "users.db";

User users[MAX_USERS];
int total_users = 0;

/* =============================================================
   UTILITIES (Auth specific)
   ============================================================= */

User *find_user(const char *name) {
    for (int i = 0; i < total_users; ++i) {
        if (strcmp(users[i].username, name) == 0) return &users[i];
    }
    return NULL;
}

/* =============================================================
   SAVE / LOAD USERS
   ============================================================= */

/* users format: username|password|role|subject|branch|year\n */
void save_users() {
    FILE *f = fopen(USERS_DB, "w");
    if (!f) return;
    for (int i = 0; i < total_users; ++i) {
        fprintf(f, "%s|%s|%s|%s|%s|%d\n",
                users[i].username,
                users[i].password,
                users[i].role,
                users[i].subject,
                users[i].branch,
                users[i].year);
    }
    fclose(f);
}

void load_users() {
    total_users = 0;
    FILE *f = fopen(USERS_DB, "r");
    if (!f) return;
    char line[512];
    while (fgets(line, sizeof(line), f)) {
        trim(line);
        char *p = strtok(line, "|");
        if (!p) continue;
        strncpy(users[total_users].username, p, sizeof(users[total_users].username)-1);

        p = strtok(NULL, "|"); if (!p) continue;
        strncpy(users[total_users].password, p, sizeof(users[total_users].password)-1);

        p = strtok(NULL, "|"); if (!p) continue;
        strncpy(users[total_users].role, p, sizeof(users[total_users].role)-1);

        p = strtok(NULL, "|"); if (!p) p = "";
        strncpy(users[total_users].subject, p, sizeof(users[total_users].subject)-1);

        p = strtok(NULL, "|"); if (!p) p = "";
        strncpy(users[total_users].branch, p, sizeof(users[total_users].branch)-1);

        p = strtok(NULL, "|"); if (!p) p = "0";
        users[total_users].year = atoi(p);

        total_users++;
        if (total_users >= MAX_USERS) break;
    }
    fclose(f);
}

/* =============================================================
   REGISTRATION / LOGIN
   ============================================================= */

void register_user_flow(const char *role) {
    char username[64], password[64];

    printf("Enter Username: ");
    input_line(username, sizeof(username));
    if (strlen(username) == 0) {
        printf("Username cannot be empty.\n");
        return;
    }
    if (find_user(username)) {
        printf("User already exists.\n");
        return;
    }

    printf("Enter Password: ");
    input_line(password, sizeof(password));

    strncpy(users[total_users].username, username,
            sizeof(users[0].username)-1);
    strncpy(users[total_users].password, password,
            sizeof(users[0].password)-1);
    strncpy(users[total_users].role, role,
            sizeof(users[0].role)-1);

    if (strcmp(role, "student") == 0) {
        printf("Enter Branch: ");
        input_line(users[total_users].branch, sizeof(users[0].branch));
        printf("Enter Year (number): ");
        char yrs[8];
        input_line(yrs, sizeof(yrs));
        users[total_users].year = atoi(yrs);
    } else { /* teacher */
        printf("Enter Subject you teach (choose one of: Coding/Maths/Economics/Systems/English): ");
        input_line(users[total_users].subject, sizeof(users[0].subject));
    }

    total_users++;
    save_users();
    printf("Registration successful.\n");
}

int login_flow(char *out_username, char *out_role) {
    char username[64], password[64];

    printf("Enter Username: ");
    input_line(username, sizeof(username));
    printf("Enter Password: ");
    input_line(password, sizeof(password));

    /* Admin Hardcoded Check */
    if (strcmp(username, "admin") == 0 && strcmp(password, "admin123") == 0) {
        strncpy(out_username, "admin", 63);
        strncpy(out_role, "admin", 15);
        return 1;
    }

    User *u = find_user(username);
    if (!u) {
        printf("No such user.\n");
        return 0;
    }

    if (strcmp(u->password, password) != 0) {
        printf("Incorrect password.\n");
        return 0;
    }

    strncpy(out_username, username, 63);
    strncpy(out_role, u->role, 15);
    return 1;
}