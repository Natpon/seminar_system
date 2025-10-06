#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "operating_seminar_system.h"


typedef struct
{
    char *username;
    char *role;
} User;
typedef struct
{
    char *SeminarName;
    char *SeminarDate;    // YYYY-MM-DD
    char *Participants;
    char *Speaker;
    char *StartTime;      // HH:MM
    char *EndTime;        // HH:MM
    char *room;
} Seminar;

#define USER_FILE "operating_seminar_system_now/user.csv"
#define SEMINAR_FILE "operating_seminar_system_now/seminar.csv"
const char *ROOMS[5] = {"Room A", "Room B", "Room C", "Room D", "Room E"};
//ไม่สามารถแก้ไขห้องได้


 
// ฟังก์ชันช่วยอ่าน string จาก stdin แบบ dynamic
char *Dynamic_stdin()
{
    char Data[1024];
    if (fgets(Data, sizeof(Data), stdin) == NULL)
        return NULL;
    Data[strcspn(Data, "\n")] = 0;
    char *save = malloc(strlen(Data) + 1);
    strcpy(save, Data);
    return save;
}

int clearBuffer(void)
{
    int ch;
    while ((ch = getchar()) != '\n' && ch != EOF)
        ;
    return 0;
}

int timeToMinutes(const char *timeStr) {
    int h, m;
    sscanf(timeStr, "%d:%d", &h, &m);
    return h * 60 + m;
}
int checkRoomAvailability(const char *room, const char *date, const char *startTime, const char *endTime) {
    FILE *file = fopen(SEMINAR_FILE, "r");
    if (!file) return 1; // ถ้าไม่มีไฟล์ ห้องถือว่าว่าง

    char line[256];
    while (fgets(line, sizeof(line), file)) {
        line[strcspn(line, "\n")] = 0;
        char *sName = strtok(line, ",");
        char *sSpeaker = strtok(NULL, ",");
        char *sDate = strtok(NULL, ",");
        char *sStart = strtok(NULL, ",");
        char *sEnd = strtok(NULL, ",");
        char *sRoom = strtok(NULL, ",");

        if (!sName || !sSpeaker || !sDate || !sStart || !sEnd || !sRoom) continue;

        if (strcmp(room, sRoom) == 0 && strcmp(date, sDate) == 0) {
            int sStartMin = timeToMinutes(sStart);
            int sEndMin = timeToMinutes(sEnd);
            int newStartMin = timeToMinutes(startTime);
            int newEndMin = timeToMinutes(endTime);

            if (!(newEndMin <= sStartMin || newStartMin >= sEndMin)) {
                fclose(file);
                return 0; // ทับซ้อน
            }
        }
    }

    fclose(file);
    return 1; // ห้องว่าง
}

void addSeminar() {
    Seminar input;
    FILE *file = fopen(SEMINAR_FILE, "a");
    if (!file) {
        printf("Cannot open seminar.csv\n");
        return;
    }

    printf("Seminar name: ");
    input.SeminarName = Dynamic_stdin();

    printf("Speaker: ");
    input.Speaker = Dynamic_stdin();

    printf("Date (YYYY-MM-DD): ");
    input.SeminarDate = Dynamic_stdin();

    printf("Start Time (HH:MM): ");
    input.StartTime = Dynamic_stdin();

    printf("End Time (HH:MM): ");
    input.EndTime = Dynamic_stdin();

    // เลือกห้อง
    int roomChoice;
    do {
        printf("Select Room:\n");
        for (int i = 0; i < 5; i++)
            printf("%d. %s\n", i + 1, ROOMS[i]);
        scanf("%d", &roomChoice);
        clearBuffer();

        if (roomChoice < 1 || roomChoice > 5) {
            printf("Invalid room choice! Try again.\n");
            continue;
        }

        input.room = (char*)ROOMS[roomChoice - 1];

        if (!checkRoomAvailability(input.room, input.SeminarDate, input.StartTime, input.EndTime)) {
            printf("Room %s is already booked at this time. Choose another.\n", input.room);
            roomChoice = 0;
        }

    } while (roomChoice < 1 || roomChoice > 5);

    // จำนวนผู้เข้าร่วม
    int participants;
    do {
        printf("Number of participants: ");
        if (scanf("%d", &participants) != 1 || participants < 0) {
            printf("Invalid number! Try again.\n");
            clearBuffer();
            continue;
        }
        clearBuffer();
        break;
    } while (1);

    // บันทึกลง CSV
    fprintf(file, "%s,%s,%s,%s,%s,%s,%d\n",
            input.SeminarName, input.Speaker, input.SeminarDate,
            input.StartTime, input.EndTime, input.room, participants);
    fclose(file);

    printf("\n=== Seminar Added ===\n");
    printf("Name: %s\nSpeaker: %s\nDate: %s\nTime: %s - %s\nRoom: %s\nParticipants: %d\n",
           input.SeminarName, input.Speaker, input.SeminarDate, input.StartTime, input.EndTime,
           input.room, participants);

    // free memory
    free(input.SeminarName);
    free(input.Speaker);
    free(input.SeminarDate);
    free(input.StartTime);
    free(input.EndTime);
}

// ฟังก์ชัน login แบบอ่านจาก user.csv
int login(User *user)
{
    char *inputUser, *inputPass;
    FILE *user_file = fopen(USER_FILE, "r");
    if (!user_file)
    {
        printf("Error: cannot open %s\n", USER_FILE);
        return 0;
    }

    printf("Enter username: ");
    inputUser = Dynamic_stdin();
    printf("Enter password: ");
    inputPass = Dynamic_stdin();

    char line[256];
    while (fgets(line, sizeof(line), user_file))
    {
        line[strcspn(line, "\n")] = 0;
        char *fileUser = strtok(line, ",");
        char *filePass = strtok(NULL, ",");
        char *fileRole = strtok(NULL, ",");
        if (fileUser && filePass && fileRole)
        {
            if (strcmp(inputUser, fileUser) == 0 && strcmp(inputPass, filePass) == 0)
            {
                user->username = malloc(strlen(fileUser) + 1);
                strcpy(user->username, fileUser);
                user->role = malloc(strlen(fileRole) + 1);
                strcpy(user->role, fileRole);
                fclose(user_file);
                free(inputUser);
                free(inputPass);
                return 1; // login สำเร็จ
            }
        }
    }

    fclose(user_file);
    free(inputUser);
    free(inputPass);
    return 0; // login ล้มเหลว
}

// ================== Dummy Functions ===================
void addUser() { printf("[Dummy] Add User called\n"); }
void removeUser() { printf("[Dummy] Remove User called\n"); }
void editUser() { printf("[Dummy] Edit User called\n"); }
void ViewbackupData() { printf("[Dummy] Backup Data called\n"); }
void restoreData() { printf("[Dummy] Restore Data called\n"); }

void searchSeminar() { printf("[Dummy] Search Seminar called\n"); }
void updateSeminar() { printf("[Dummy] Update Seminar called\n"); }
void deleteSeminar() { printf("[Dummy] Delete Seminar called\n"); }

void approveParticipants() { printf("[Dummy] Approve Participants called\n"); }
void viewMySeminars() { printf("[Dummy] View My Seminars called\n"); }
void uploadSlides() { printf("[Dummy] Upload Slides called\n"); }
void viewFeedback() { printf("[Dummy] View Feedback called\n"); }
void viewSchedule() { printf("[Dummy] View Schedule called\n"); }
void giveFeedback() { printf("[Dummy] Give Feedback called\n"); }

// ================== Menu ตาม Role ======================
void manageSeminar() { 
    int choice;
    while (1)
    {
        printf("\nSeminar Management:\n1. Add Seminar\n2. Search Seminar\n3. Update Seminar\n4. Delete Seminar\n0. Back to main menu\nChoice: ");
        scanf("%d", &choice);
        clearBuffer();
        if (choice == 1 || choice == 2 || choice == 3 || choice == 4 || choice == 0)
        {
            switch (choice)
            {
            case 1:
                addSeminar();
                break;
            case 2:
                searchSeminar();
                break;
            case 3:
                updateSeminar();
                break;
            case 4:
                deleteSeminar();
                break;
            case 0:
                return;
            default:
                printf("Invalid choice.\n");
                break;
            }
        }
        else
        {
            printf("Invalid choice. Please try again.\n");
            continue;
        }
    }
}
void showMenu(User *user)
{
    int choice;
    while (1)
    {
        printf("\nLogged in as %s (%s)\n", user->username, user->role);
        if (strcmp(user->role, " Admin") == 0)
        {
            printf("1. User Management\n2. Data Backup/Restore\n3. View schedule\n4. View Feedback\n0. Exit\nChoice: ");
            scanf("%d", &choice);
            clearBuffer();
            if (choice == 1 || choice == 2 || choice == 3 || choice == 4 || choice == 0)
            {
                switch (choice)
                {
                case 1:
                    printf("1. Add User\n2. Remove User\n3. Edit User\nChoice: ");
                    int userChoice;
                    scanf("%d", &userChoice);
                    clearBuffer();
                    if (userChoice == 1)
                        addUser();
                    else if (userChoice == 2)
                        removeUser();
                    else if (userChoice == 3)
                        editUser();
                    else
                        printf("Invalid choice.\n");
                    break;
                case 2:
                    printf("1.View Backup Data\n2. Restore Data\nChoice: ");
                    int dataChoice;
                    scanf("%d", &dataChoice);
                    clearBuffer();
                    if (dataChoice == 1)
                        ViewbackupData();
                    else if (dataChoice == 2)
                        restoreData();
                    else
                        printf("Invalid choice.\n");
                    break;
                case 3:
                    viewSchedule();
                    break;
                case 4:
                    viewFeedback();
                    break;
                case 0:
                    return;
                default:
                    printf("Invalid choice.\n");
                    break;
                }
            }
            else
            {
                printf("Invalid choice. Please try again.\n");
                continue;
            }
        }
        else if (strcmp(user->role, "Organizer") == 0)
        {
            printf("1. Manage Seminar\n2. Approve Participants\n3. View schedule\n0. Exit\nChoice: ");
            scanf("%d", &choice);
            clearBuffer();
            if (choice == 1 || choice == 2 || choice == 3 || choice == 0)
            {
                switch (choice)
                {
                case 1:
                    manageSeminar();
                    break;
                case 2:
                    approveParticipants();
                    break;
                case 3:
                    viewSchedule();
                    break;
                case 0:
                    return;
                default:
                    printf("Invalid choice.\n");
                    break;
                }
            }
            else
            {
                printf("Invalid choice. Please try again.\n");
                continue;
            }
        }
        else if (strcmp(user->role, "Speaker") == 0)
        {
            printf("1. View My Seminars\n2. Upload Slides\n3. View Feedback\n0. Exit\nChoice: ");
            scanf("%d", &choice);
            clearBuffer();
            if (choice == 1 || choice == 2 || choice == 3 || choice == 0)
            {
                switch (choice)
                {
                case 1:
                    viewMySeminars();
                    break;
                case 2:
                    uploadSlides();
                    break;
                case 3:
                    viewFeedback();
                    break;
                case 0:
                    return;
                default:
                    printf("Invalid choice.\n");
                    break;
                }
            }
            else
            {
                printf("Invalid choice. Please try again.\n");
                continue;
            }
        }
        else if (strcmp(user->role, "Participant") == 0)
        {
            printf("1. View Schedule\n2. Give Feedback\n0. Exit\nChoice: ");
            scanf("%d", &choice);
            clearBuffer();
            if (choice == 1 || choice == 2 || choice == 0)
            {
                switch (choice)
                {
                case 1:
                    viewSchedule();
                    break;
                case 2:
                    giveFeedback();
                    break;
                case 0:
                    return;
                default:
                    printf("Invalid choice.\n");
                    break;
                }
            }
            else
            {
                printf("Invalid choice. Please try again.\n");
                continue;
            }
        }
        else if (strcmp(user->role, "Viewer") == 0)
        {
            printf("1. View Schedule\n0. Exit\nChoice: ");
            scanf("%d", &choice);
            clearBuffer();
            if (choice == 1 || choice == 0)
            {
                switch (choice)
                {
                case 1:
                    viewSchedule();
                    break;
                case 0:
                    return;
                default:
                    printf("Invalid choice.\n");
                    break;
                }
            }
            else
            {
                printf("Invalid choice. Please try again.\n");
                continue;
            }
        }
        else
        {
            printf("Unknown role. Exiting menu.\n");
            return;
        }
    }
}

// ================== int =============================

int main()
{
    User currentUser;
    if (!login(&currentUser))
    {
        printf("Login failed. Exiting...\n");
        return 0;
    }

    showMenu(&currentUser);

    // free memory
    free(currentUser.username);
    free(currentUser.role);

    printf("Goodbye!\n");
    return 0;
    
}
