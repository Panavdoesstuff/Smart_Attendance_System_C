#ifndef AUTH_H
#define AUTH_H

#define MAX_USERS 200

typedef struct {
    char username[64];
    char password[64];
    char role[16];     /* "student" or "teacher" */
    char subject[64];  /* for teacher */
    char branch[16];   /* for student */
    int  year;         /* for student */
} User;

/* Global storage exposed for internal lookups if needed */
extern User users[MAX_USERS];
extern int total_users;

/* Functions */
void save_users();
void load_users();
User *find_user(const char *name);
void register_user_flow(const char *role);
int login_flow(char *out_username, char *out_role);

#endif