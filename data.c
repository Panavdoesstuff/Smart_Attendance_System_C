#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "data.h"
#include "auth.h"
#include "utils.h"

const char *SESSIONS_DB  = "sessions.db";
const char *ATT_DB       = "attendance.db";
const char *LEAVES_DB    = "leaves.db";

/* Subjects */
char SUBJECTS[MAX_NOTES][64] = {"Coding", "Maths", "Economics", "Systems", "English"};
int SUBJECT_COUNT = 5;

/* Global Storage */
Session sessions[MAX_SESSIONS];
int total_sessions = 0;

AttRecord attendance[MAX_ATT_RECORDS];
int total_attendance = 0;

LeaveReq leaves[MAX_LEAVES];
int total_leaves = 0;

/* =============================================================
   SAVE / LOAD DBS
   ============================================================= */

/* sessions format: id|subject|branch|year|teacher|open\n */
void save_sessions() {
    FILE *f = fopen(SESSIONS_DB, "w");
    if (!f) return;
    for (int i = 0; i < total_sessions; ++i) {
        fprintf(f, "%s|%s|%s|%d|%s|%d\n",
                sessions[i].id,
                sessions[i].subject,
                sessions[i].branch,
                sessions[i].year,
                sessions[i].teacher,
                sessions[i].open);
    }
    fclose(f);
}

void load_sessions() {
    total_sessions = 0;
    FILE *f = fopen(SESSIONS_DB, "r");
    if (!f) return;
    char line[512];
    while (fgets(line, sizeof(line), f)) {
        trim(line);
        char *p = strtok(line, "|");
        if (!p) continue;
        strncpy(sessions[total_sessions].id, p, sizeof(sessions[total_sessions].id)-1);

        p = strtok(NULL, "|"); if (!p) p = "";
        strncpy(sessions[total_sessions].subject, p, sizeof(sessions[total_sessions].subject)-1);

        p = strtok(NULL, "|"); if (!p) p = "";
        strncpy(sessions[total_sessions].branch, p, sizeof(sessions[total_sessions].branch)-1);

        p = strtok(NULL, "|"); if (!p) p = "0";
        sessions[total_sessions].year = atoi(p);

        p = strtok(NULL, "|"); if (!p) p = "";
        strncpy(sessions[total_sessions].teacher, p, sizeof(sessions[total_sessions].teacher)-1);

        p = strtok(NULL, "|"); if (!p) p = "0";
        sessions[total_sessions].open = atoi(p);

        total_sessions++;
        if (total_sessions >= MAX_SESSIONS) break;
    }
    fclose(f);
}

/* attendance format: username|subject|count\n */
void save_attendance() {
    FILE *f = fopen(ATT_DB, "w");
    if (!f) return;
    for (int i = 0; i < total_attendance; ++i) {
        fprintf(f, "%s|%s|%d\n",
                attendance[i].username,
                attendance[i].subject,
                attendance[i].count);
    }
    fclose(f);
}

void load_attendance() {
    total_attendance = 0;
    FILE *f = fopen(ATT_DB, "r");
    if (!f) return;
    char line[512];
    while (fgets(line, sizeof(line), f)) {
        trim(line);
        char *p = strtok(line, "|");
        if (!p) continue;
        strncpy(attendance[total_attendance].username, p,
                sizeof(attendance[total_attendance].username)-1);

        p = strtok(NULL, "|"); if (!p) p = "";
        strncpy(attendance[total_attendance].subject, p,
                sizeof(attendance[total_attendance].subject)-1);

        p = strtok(NULL, "|"); if (!p) p = "0";
        attendance[total_attendance].count = atoi(p);

        total_attendance++;
        if (total_attendance >= MAX_ATT_RECORDS) break;
    }
    fclose(f);
}

/* leaves format: username|subject|reason|status\n */
void save_leaves() {
    FILE *f = fopen(LEAVES_DB, "w");
    if (!f) return;
    for (int i = 0; i < total_leaves; ++i) {
        fprintf(f, "%s|%s|%s|%s\n",
                leaves[i].username,
                leaves[i].subject,
                leaves[i].reason,
                leaves[i].status);
    }
    fclose(f);
}

void load_leaves() {
    total_leaves = 0;
    FILE *f = fopen(LEAVES_DB, "r");
    if (!f) return;
    char line[512];
    while (fgets(line, sizeof(line), f)) {
        trim(line);
        char *p = strtok(line, "|");
        if (!p) continue;
        strncpy(leaves[total_leaves].username, p,
                sizeof(leaves[total_leaves].username)-1);

        p = strtok(NULL, "|"); if (!p) p = "";
        strncpy(leaves[total_leaves].subject, p,
                sizeof(leaves[total_leaves].subject)-1);

        p = strtok(NULL, "|"); if (!p) p = "";
        strncpy(leaves[total_leaves].reason, p,
                sizeof(leaves[total_leaves].reason)-1);

        p = strtok(NULL, "|"); if (!p) p = "pending";
        strncpy(leaves[total_leaves].status, p,
                sizeof(leaves[total_leaves].status)-1);

        total_leaves++;
        if (total_leaves >= MAX_LEAVES) break;
    }
    fclose(f);
}

/* =============================================================
   NOTES
   ============================================================= */

void write_notes_to_file(const char *subject, const char *text) {
    char fname[128];
    note_filename_for_subject(subject, fname, sizeof(fname));
    FILE *f = fopen(fname, "w");
    if (!f) {
        printf("Failed to open notes file for writing: %s\n", fname);
        return;
    }
    fprintf(f, "%s\n", text);
    fclose(f);
}

void read_notes_from_file(const char *subject) {
    char fname[128];
    note_filename_for_subject(subject, fname, sizeof(fname));
    FILE *f = fopen(fname, "r");
    if (!f) {
        printf("No notes found for %s (file %s missing)\n", subject, fname);
        return;
    }
    printf("----- Notes for %s -----\n", subject);
    char line[1024];
    while (fgets(line, sizeof(line), f)) {
        printf("%s", line);
    }
    fclose(f);
    printf("\n-------------------------\n");
}

/* =============================================================
   TEACHER FEATURES
   ============================================================= */

void create_session_flow(const char *teacher) {
    char subject[64], branch[16], yrs[8];

    printf("Enter Subject (Coding/Maths/Economics/Systems/English): ");
    input_line(subject, sizeof(subject));

    printf("Enter Branch (e.g., CSE): ");
    input_line(branch, sizeof(branch));

    printf("Enter Year (number): ");
    input_line(yrs, sizeof(yrs));

    Session s = {0};
    snprintf(s.id, sizeof(s.id), "S%ld", time(NULL));
    strncpy(s.subject, subject, sizeof(s.subject)-1);
    strncpy(s.branch, branch, sizeof(s.branch)-1);
    s.year = atoi(yrs);
    strncpy(s.teacher, teacher, sizeof(s.teacher)-1);
    s.open = 1;

    sessions[total_sessions++] = s;
    save_sessions();
    printf("Session created: %s\n", s.id);
}

void close_session_flow(const char *teacher) {
    print_header("OPEN SESSIONS (Your Classes)");
    int printed = 0;
    for (int i = 0; i < total_sessions; ++i) {
        if (sessions[i].open && strcmp(sessions[i].teacher, teacher) == 0) {
            printf("%s | %s | %s | Year %d\n",
                   sessions[i].id, sessions[i].subject, sessions[i].branch, sessions[i].year);
            printed = 1;
        }
    }
    if (!printed) {
        printf("No open sessions to close.\n");
        return;
    }

    char id[32];
    printf("Enter Session ID to close: ");
    input_line(id, sizeof(id));

    for (int i = 0; i < total_sessions; ++i) {
        if (strcmp(sessions[i].id, id) == 0 && strcmp(sessions[i].teacher, teacher) == 0) {
            sessions[i].open = 0;
            save_sessions();
            printf("Session %s closed.\n", id);
            return;
        }
    }

    printf("Could not find a matching open session.\n");
}

void teacher_write_notes_flow(const char *teacher) {
    User *u = find_user(teacher);
    if (!u) {
        printf("Teacher record not found.\n");
        return;
    }

    char subject[64];
    if (strlen(u->subject) > 0) {
        strncpy(subject, u->subject, sizeof(subject)-1);
    } else {
        printf("Enter subject to write notes for: ");
        input_line(subject, sizeof(subject));
    }

    printf("Write the notes content (one line). This will overwrite existing notes file for the subject.\n");
    char content[1024];
    input_line(content, sizeof(content));
    write_notes_to_file(subject, content);
}

void teacher_review_leaves_flow(const char *teacher) {
    User *u = find_user(teacher);
    if (!u) {
        printf("Teacher not found.\n");
        return;
    }

    const char *subject = u->subject;
    int found = 0;

    printf("Pending leave requests for %s:\n", subject);

    for (int i = 0; i < total_leaves; ++i) {
        if (strcmp(leaves[i].subject, subject) == 0 && strcmp(leaves[i].status, "pending") == 0) {
            printf("%d) %s - %s\n", i, leaves[i].username, leaves[i].reason);
            found = 1;
        }
    }

    if (!found) {
        printf("No pending requests.\n");
        return;
    }

    char idbuf[8];
    printf("Enter request number to handle (or blank to return): ");
    input_line(idbuf, sizeof(idbuf));
    if (strlen(idbuf) == 0) return;

    int id = atoi(idbuf);
    if (id < 0 || id >= total_leaves) {
        printf("Invalid id.\n");
        return;
    }

    char action[8];
    printf("Approve (a) / Reject (r): ");
    input_line(action, sizeof(action));
    if (strcmp(action, "a") == 0) strcpy(leaves[id].status, "approved");
    else if (strcmp(action, "r") == 0) strcpy(leaves[id].status, "rejected");
    else {
        printf("Unknown action.\n");
        return;
    }

    save_leaves();
    printf("Updated leave request %d.\n", id);
}

/* =============================================================
   STUDENT FEATURES
   ============================================================= */

void show_timetable() {
    print_header("STUDENT TIMETABLE");
    printf("09:00 - 10:00  | Coding\n");
    printf("10:00 - 11:00  | Maths\n");
    printf("11:00 - 12:00  | Economics\n");
    printf("13:00 - 14:00  | Systems\n");
    printf("14:00 - 15:00  | English\n");
}

void student_mark_attendance_flow(const char *username) {
    User *u = find_user(username);
    if (!u) {
        printf("User not found.\n");
        return;
    }

    int list_idx[64];
    int count = 0;

    for (int i = 0; i < total_sessions; ++i) {
        if (sessions[i].open && strcmp(sessions[i].branch, u->branch) == 0 && sessions[i].year == u->year) {
            list_idx[count++] = i;
            printf("%d) %s (Teacher: %s)\n", count, sessions[i].subject, sessions[i].teacher);
        }
    }

    if (count == 0) {
        printf("No open sessions for your branch/year.\n");
        return;
    }

    char choicebuf[8];
    printf("Select class number: ");
    input_line(choicebuf, sizeof(choicebuf));
    int choice = atoi(choicebuf);
    if (choice < 1 || choice > count) {
        printf("Invalid choice.\n");
        return;
    }

    int session_index = list_idx[choice - 1];
    int code = 1000 + (rand() % 9000);

    printf("Type this number to mark yourself present: %d\n", code);
    char typedbuf[16];
    input_line(typedbuf, sizeof(typedbuf));
    int typed = atoi(typedbuf);
    if (typed != code) {
        printf("Code mismatch. Attendance not recorded.\n");
        return;
    }

    AttRecord *r = NULL;
    for (int i = 0; i < total_attendance; ++i) {
        if (strcmp(attendance[i].username, username) == 0 &&
            strcmp(attendance[i].subject, sessions[session_index].subject) == 0) {
            r = &attendance[i];
            break;
        }
    }

    if (!r) {
        strncpy(attendance[total_attendance].username, username, sizeof(attendance[0].username)-1);
        strncpy(attendance[total_attendance].subject, sessions[session_index].subject, sizeof(attendance[0].subject)-1);
        attendance[total_attendance].count = 1;
        total_attendance++;
    } else {
        r->count++;
    }

    save_attendance();
    printf("Attendance recorded for %s in %s.\n", username, sessions[session_index].subject);
}

void student_view_notes_flow() {
    print_header("NOTES - Available Subjects");
    for (int i = 0; i < SUBJECT_COUNT; ++i) {
        printf("%d) %s\n", i+1, SUBJECTS[i]);
    }
    printf("0) Show all subjects\n");

    char choicebuf[8];
    printf("Choose subject number (or 0 to show all): ");
    input_line(choicebuf, sizeof(choicebuf));
    int choice = atoi(choicebuf);

    if (choice == 0) {
        for (int i = 0; i < SUBJECT_COUNT; ++i) read_notes_from_file(SUBJECTS[i]);
    } else if (choice >= 1 && choice <= SUBJECT_COUNT) {
        read_notes_from_file(SUBJECTS[choice-1]);
    } else {
        printf("Invalid choice.\n");
    }
}

void student_apply_leave_flow(const char *username) {
    char subject[64], reason[256];

    printf("Enter subject (e.g., Coding): ");
    input_line(subject, sizeof(subject));
    printf("Enter reason: ");
    input_line(reason, sizeof(reason));

    if (strlen(subject) == 0 || strlen(reason) == 0) {
        printf("Subject and reason required.\n");
        return;
    }

    strncpy(leaves[total_leaves].username, username, sizeof(leaves[0].username)-1);
    strncpy(leaves[total_leaves].subject, subject, sizeof(leaves[0].subject)-1);
    strncpy(leaves[total_leaves].reason, reason, sizeof(leaves[0].reason)-1);
    strncpy(leaves[total_leaves].status, "pending", sizeof(leaves[0].status)-1);
    total_leaves++;
    save_leaves();
    printf("Leave request submitted (status: pending).\n");
}

/* =============================================================
   DASHBOARDS
   ============================================================= */
   void student_view_leaves_flow(const char *username) {
    print_header("MY LEAVE REQUESTS");
    int found = 0;
    for (int i = 0; i < total_leaves; ++i) {
        if (strcmp(leaves[i].username, username) == 0) {
            printf("Subject: %s | Reason: %s | Status: [%s]\n", 
                   leaves[i].subject, leaves[i].reason, leaves[i].status);
            found = 1;
        }
    }
    if (!found) printf("No leave requests found.\n");
    printf("\n");
}

void student_dashboard_flow(const char *username) {
    while (1) {
        print_header("STUDENT DASHBOARD");
        printf("1) Mark Attendance\n");
        printf("2) View Attendance\n");
        printf("3) View Notes\n");
        printf("4) Apply Leave\n");
        printf("5) View Leave Status\n"); // <--- NEW OPTION
        printf("6) Timetable\n");
        printf("7) Logout\n"); // <--- Renumbered

        char choicebuf[8];
        printf("Choose: ");
        input_line(choicebuf, sizeof(choicebuf));
        int choice = atoi(choicebuf);

        if (choice == 1) student_mark_attendance_flow(username);
        else if (choice == 2) {
            // ... (existing code) ...
        } 
        else if (choice == 3) student_view_notes_flow();
        else if (choice == 4) student_apply_leave_flow(username);
        else if (choice == 5) student_view_leaves_flow(username); // <--- Call the new function
        else if (choice == 6) show_timetable();
        else if (choice == 7) break;
        else printf("Invalid option.\n");
    }
}

void teacher_dashboard_flow(const char *username) {
    User *u = find_user(username);
    if (!u) {
        printf("Teacher profile missing.\n");
        return;
    }

    while (1) {
        print_header("TEACHER DASHBOARD");
        printf("1) Create Session\n");
        printf("2) Close Session\n");
        printf("3) Write Notes (save to subject file)\n");
        printf("4) Review Leaves\n");
        printf("5) Logout\n");

        char choicebuf[8];
        printf("Choose: ");
        input_line(choicebuf, sizeof(choicebuf));
        int choice = atoi(choicebuf);

        if (choice == 1) create_session_flow(username);
        else if (choice == 2) close_session_flow(username);
        else if (choice == 3) teacher_write_notes_flow(username);
        else if (choice == 4) teacher_review_leaves_flow(username);
        else if (choice == 5) break;
        else printf("Invalid option.\n");
    }
}

void admin_dashboard_flow() {
    while (1) {
        print_header("ADMIN DASHBOARD");
        printf("1) View All Attendance\n");
        printf("2) Add New Subject\n");
        printf("3) Logout\n");

        char choicebuf[8];
        printf("Choose: ");
        input_line(choicebuf, sizeof(choicebuf));
        int choice = atoi(choicebuf);

        if (choice == 1) {
            printf("--- All Attendance Records ---\n");
            for (int i = 0; i < total_attendance; ++i) {
                printf("%s | %s | %d\n", attendance[i].username, attendance[i].subject, attendance[i].count);
            }
            printf("------------------------------\n");
        } else if (choice == 2) {
            char new_subject[64];
            printf("Enter new subject name: ");
            input_line(new_subject, sizeof(new_subject));
            if (strlen(new_subject) > 0) {
                if (SUBJECT_COUNT < MAX_NOTES) {
                    strncpy(SUBJECTS[SUBJECT_COUNT], new_subject, 63);
                    SUBJECT_COUNT++;
                    
                    /* Create file */
                    char fname[128];
                    note_filename_for_subject(new_subject, fname, sizeof(fname));
                    FILE *f = fopen(fname, "w");
                    if (f) {
                        fprintf(f, "Notes for %s\n", new_subject);
                        fclose(f);
                        printf("Subject added and %s created.\n", fname);
                    } else {
                        printf("Error creating file.\n");
                    }
                } else {
                    printf("Max subjects reached.\n");
                }
            }
        } else if (choice == 3) {
            break;
        } else {
            printf("Invalid option.\n");
        }
    }
}