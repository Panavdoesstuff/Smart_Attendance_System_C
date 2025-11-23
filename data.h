#ifndef DATA_H
#define DATA_H

#define MAX_SESSIONS    200
#define MAX_ATT_RECORDS 500
#define MAX_LEAVES      200
#define MAX_NOTES       20

typedef struct {
    char id[32];
    char subject[64];
    char branch[16];
    int  year;
    char teacher[64];
    int  open;         /* 1=open, 0=closed */
} Session;

typedef struct {
    char username[64];
    char subject[64];
    int  count;
} AttRecord;

typedef struct {
    char username[64];
    char subject[64];
    char reason[256];
    char status[16];   /* pending / approved / rejected */
} LeaveReq;

/* Config */
extern char SUBJECTS[MAX_NOTES][64];
extern int SUBJECT_COUNT;

/* Functions */
void load_sessions();
void save_sessions();
void load_attendance();
void save_attendance();
void load_leaves();
void save_leaves();

/* Dashboards */
void teacher_dashboard_flow(const char *username);
void student_dashboard_flow(const char *username);
void admin_dashboard_flow();

#endif