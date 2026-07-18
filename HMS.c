#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define NAME_LEN     50
#define PHONE_LEN    15
#define ADDR_LEN     100
#define TEXT_LEN     100
#define DATE_LEN     15
#define TIME_LEN     10
#define WARD_LEN     20
#define STATUS_LEN   15

#define GENERAL_BEDS 10
#define ICU_BEDS     10
#define PRIVATE_BEDS 10
#define TOTAL_BEDS   (GENERAL_BEDS + ICU_BEDS + PRIVATE_BEDS)

#define GENERAL_CHARGE 500.0
#define ICU_CHARGE     1500.0
#define PRIVATE_CHARGE 1000.0

#define PATIENT_FILE     "patients.dat"
#define DOCTOR_FILE      "doctors.dat"
#define BED_FILE         "beds.dat"
#define APPOINTMENT_FILE "appointments.dat"
#define EMERGENCY_FILE   "emergency.dat"

typedef struct {
    int   id;
    char  name[NAME_LEN];
    int   age;
    char  gender[10];
    char  disease[TEXT_LEN];
    char  phone[PHONE_LEN];
    char  address[ADDR_LEN];
    int   bedNo;
    float billAmount;
} PatientData;

typedef struct PatientNode {
    PatientData data;
    struct PatientNode *next;
} PatientNode;

typedef struct {
    int   id;
    char  name[NAME_LEN];
    char  specialization[NAME_LEN];
    char  phone[PHONE_LEN];
    float fee;
} DoctorData;

typedef struct DoctorNode {
    DoctorData data;
    struct DoctorNode *next;
} DoctorNode;

typedef struct {
    int   bedNo;
    char  ward[WARD_LEN];
    float chargePerDay;
    int   isAllocated;
    int   patientId;
} Bed;

typedef struct {
    int  id;
    int  patientId;
    int  doctorId;
    char date[DATE_LEN];
    char time[TIME_LEN];
    char status[STATUS_LEN];
} AppointmentData;

typedef struct AppointmentNode {
    AppointmentData data;
    struct AppointmentNode *next;
} AppointmentNode;

typedef struct {
    int  patientId;
    char name[NAME_LEN];
    int  age;
    char condition[TEXT_LEN];
    int  priority;
} EmergencyData;

typedef struct EmergencyNode {
    EmergencyData data;
    struct EmergencyNode *next;
} EmergencyNode;

PatientNode     *patientHead     = NULL;
DoctorNode      *doctorHead      = NULL;
AppointmentNode *appointmentHead = NULL;
EmergencyNode   *emergencyHead   = NULL;
Bed beds[TOTAL_BEDS];

void clearInputBuffer(void);
void trimNewline(char *s);
void pressEnterToContinue(void);
void toLowerStr(char *dest, const char *src);
int  strcicmp(const char *a, const char *b);
int  readMenuChoice(void);

void loadPatients(void);
void savePatients(void);
void loadDoctors(void);
void saveDoctors(void);
void loadBeds(void);
void saveBeds(void);
void loadAppointments(void);
void saveAppointments(void);
void loadEmergencyQueue(void);
void saveEmergencyQueue(void);

int  getNextPatientId(void);
void addPatient(void);
void editPatient(void);
PatientNode* findPatientById(int id);
void viewAllPatients(void);
void viewPatientById(void);

int  getNextDoctorId(void);
void addDoctor(void);
void editDoctor(void);
DoctorNode* findDoctorById(int id);
void viewAllDoctors(void);

void initBedsDefault(void);
int  countFreeBeds(void);
void viewBedStatus(void);
void allocateBed(void);
void releaseBed(void);

void generateBill(void);
void viewBill(void);

int  getNextAppointmentId(void);
void scheduleAppointment(void);
void editAppointment(void);
void cancelAppointment(void);
void viewAllAppointments(void);
AppointmentNode* findAppointmentById(int id);

void addEmergencyPatient(void);
void serveEmergencyPatient(void);
void viewEmergencyQueue(void);

void searchPatientByIdMenu(void);
void searchPatientByNameMenu(void);
void sortPatientsById(void);
void sortPatientsByName(void);
void searchDoctorByIdMenu(void);
void searchDoctorBySpecializationMenu(void);
void sortDoctorsByName(void);

void mainMenu(void);
void patientMenu(void);
void bedMenu(void);
void doctorMenu(void);
void billingMenu(void);
void appointmentMenu(void);
void emergencyMenu(void);
void searchSortMenu(void);

void freeAllMemory(void);

int main(void) {
    loadPatients();
    loadDoctors();
    loadBeds();
    loadAppointments();
    loadEmergencyQueue();

    mainMenu();

    savePatients();
    saveDoctors();
    saveBeds();
    saveAppointments();
    saveEmergencyQueue();
    freeAllMemory();

    printf("\nAll data saved. Thank you for using the Hospital Management System.\n");
    return 0;
}

void clearInputBuffer(void) {
    int c;
    while ((c = getchar()) != '\n' && c != EOF) { }
}

void trimNewline(char *s) {
    size_t len = strlen(s);
    if (len > 0 && s[len - 1] == '\n') s[len - 1] = '\0';
}

void pressEnterToContinue(void) {
    int c;
    printf("\nPress Enter to continue...");
    fflush(stdout);
    while ((c = getchar()) != '\n' && c != EOF) { }
}

void toLowerStr(char *dest, const char *src) {
    int i = 0;
    while (src[i]) {
        dest[i] = (char)tolower((unsigned char)src[i]);
        i++;
    }
    dest[i] = '\0';
}

int strcicmp(const char *a, const char *b) {
    while (*a && *b) {
        int ca = tolower((unsigned char)*a);
        int cb = tolower((unsigned char)*b);
        if (ca != cb) return ca - cb;
        a++; b++;
    }
    return (unsigned char)*a - (unsigned char)*b;
}

int readMenuChoice(void) {
    int choice;
    if (scanf("%d", &choice) != 1) {
        if (feof(stdin)) {
            printf("\nInput stream closed. Saving data and exiting...\n");
            savePatients();
            saveDoctors();
            saveBeds();
            saveAppointments();
            saveEmergencyQueue();
            freeAllMemory();
            exit(0);
        }
        clearInputBuffer();
        return -1;
    }
    clearInputBuffer();
    return choice;
}

void loadPatients(void) {
    FILE *fp = fopen(PATIENT_FILE, "rb");
    if (!fp) return;
    PatientData pd;
    PatientNode *tail = NULL;
    while (fread(&pd, sizeof(PatientData), 1, fp) == 1) {
        PatientNode *node = (PatientNode*)malloc(sizeof(PatientNode));
        node->data = pd;
        node->next = NULL;
        if (!patientHead) patientHead = node; else tail->next = node;
        tail = node;
    }
    fclose(fp);
}

void savePatients(void) {
    FILE *fp = fopen(PATIENT_FILE, "wb");
    if (!fp) { printf("Error: could not save patient data.\n"); return; }
    PatientNode *cur = patientHead;
    while (cur) { fwrite(&cur->data, sizeof(PatientData), 1, fp); cur = cur->next; }
    fclose(fp);
}

void loadDoctors(void) {
    FILE *fp = fopen(DOCTOR_FILE, "rb");
    if (!fp) return;
    DoctorData dd;
    DoctorNode *tail = NULL;
    while (fread(&dd, sizeof(DoctorData), 1, fp) == 1) {
        DoctorNode *node = (DoctorNode*)malloc(sizeof(DoctorNode));
        node->data = dd;
        node->next = NULL;
        if (!doctorHead) doctorHead = node; else tail->next = node;
        tail = node;
    }
    fclose(fp);
}

void saveDoctors(void) {
    FILE *fp = fopen(DOCTOR_FILE, "wb");
    if (!fp) { printf("Error: could not save doctor data.\n"); return; }
    DoctorNode *cur = doctorHead;
    while (cur) { fwrite(&cur->data, sizeof(DoctorData), 1, fp); cur = cur->next; }
    fclose(fp);
}

void initBedsDefault(void) {
    int i, bedNo = 1;
    for (i = 0; i < GENERAL_BEDS; i++, bedNo++) {
        beds[bedNo - 1].bedNo = bedNo;
        strcpy(beds[bedNo - 1].ward, "General");
        beds[bedNo - 1].chargePerDay = GENERAL_CHARGE;
        beds[bedNo - 1].isAllocated = 0;
        beds[bedNo - 1].patientId = -1;
    }
    for (i = 0; i < ICU_BEDS; i++, bedNo++) {
        beds[bedNo - 1].bedNo = bedNo;
        strcpy(beds[bedNo - 1].ward, "ICU");
        beds[bedNo - 1].chargePerDay = ICU_CHARGE;
        beds[bedNo - 1].isAllocated = 0;
        beds[bedNo - 1].patientId = -1;
    }
    for (i = 0; i < PRIVATE_BEDS; i++, bedNo++) {
        beds[bedNo - 1].bedNo = bedNo;
        strcpy(beds[bedNo - 1].ward, "Private");
        beds[bedNo - 1].chargePerDay = PRIVATE_CHARGE;
        beds[bedNo - 1].isAllocated = 0;
        beds[bedNo - 1].patientId = -1;
    }
}

void loadBeds(void) {
    FILE *fp = fopen(BED_FILE, "rb");
    if (!fp) {
        initBedsDefault();
        saveBeds();
        return;
    }
    size_t count = fread(beds, sizeof(Bed), TOTAL_BEDS, fp);
    fclose(fp);
    if (count != TOTAL_BEDS) {
        initBedsDefault();
        saveBeds();
    }
}

void saveBeds(void) {
    FILE *fp = fopen(BED_FILE, "wb");
    if (!fp) { printf("Error: could not save bed data.\n"); return; }
    fwrite(beds, sizeof(Bed), TOTAL_BEDS, fp);
    fclose(fp);
}

void loadAppointments(void) {
    FILE *fp = fopen(APPOINTMENT_FILE, "rb");
    if (!fp) return;
    AppointmentData ad;
    AppointmentNode *tail = NULL;
    while (fread(&ad, sizeof(AppointmentData), 1, fp) == 1) {
        AppointmentNode *node = (AppointmentNode*)malloc(sizeof(AppointmentNode));
        node->data = ad;
        node->next = NULL;
        if (!appointmentHead) appointmentHead = node; else tail->next = node;
        tail = node;
    }
    fclose(fp);
}

void saveAppointments(void) {
    FILE *fp = fopen(APPOINTMENT_FILE, "wb");
    if (!fp) { printf("Error: could not save appointment data.\n"); return; }
    AppointmentNode *cur = appointmentHead;
    while (cur) { fwrite(&cur->data, sizeof(AppointmentData), 1, fp); cur = cur->next; }
    fclose(fp);
}

void loadEmergencyQueue(void) {
    FILE *fp = fopen(EMERGENCY_FILE, "rb");
    if (!fp) return;
    EmergencyData ed;
    EmergencyNode *tail = NULL;
    while (fread(&ed, sizeof(EmergencyData), 1, fp) == 1) {
        EmergencyNode *node = (EmergencyNode*)malloc(sizeof(EmergencyNode));
        node->data = ed;
        node->next = NULL;
        if (!emergencyHead) emergencyHead = node; else tail->next = node;
        tail = node;
    }
    fclose(fp);
}

void saveEmergencyQueue(void) {
    FILE *fp = fopen(EMERGENCY_FILE, "wb");
    if (!fp) { printf("Error: could not save emergency queue data.\n"); return; }
    EmergencyNode *cur = emergencyHead;
    while (cur) { fwrite(&cur->data, sizeof(EmergencyData), 1, fp); cur = cur->next; }
    fclose(fp);
}

int getNextPatientId(void) {
    int maxId = 0;
    PatientNode *cur = patientHead;
    while (cur) { if (cur->data.id > maxId) maxId = cur->data.id; cur = cur->next; }
    return maxId + 1;
}

PatientNode* findPatientById(int id) {
    PatientNode *cur = patientHead;
    while (cur) { if (cur->data.id == id) return cur; cur = cur->next; }
    return NULL;
}

void addPatient(void) {
    PatientNode *node = (PatientNode*)malloc(sizeof(PatientNode));
    node->data.id = getNextPatientId();
    node->data.bedNo = -1;
    node->data.billAmount = 0.0;

    printf("\n--- Add New Patient (ID: %d) ---\n", node->data.id);

    printf("Name: ");
    fgets(node->data.name, NAME_LEN, stdin);
    trimNewline(node->data.name);

    printf("Age: ");
    scanf("%d", &node->data.age);
    clearInputBuffer();

    printf("Gender (M/F/O): ");
    fgets(node->data.gender, sizeof(node->data.gender), stdin);
    trimNewline(node->data.gender);

    printf("Disease/Reason for admission: ");
    fgets(node->data.disease, TEXT_LEN, stdin);
    trimNewline(node->data.disease);

    printf("Phone: ");
    fgets(node->data.phone, PHONE_LEN, stdin);
    trimNewline(node->data.phone);

    printf("Address: ");
    fgets(node->data.address, ADDR_LEN, stdin);
    trimNewline(node->data.address);

    node->next = NULL;
    if (!patientHead) {
        patientHead = node;
    } else {
        PatientNode *cur = patientHead;
        while (cur->next) cur = cur->next;
        cur->next = node;
    }

    savePatients();
    printf("\nPatient registered successfully with ID %d.\n", node->data.id);
    pressEnterToContinue();
}

void editPatient(void) {
    int id;
    char buffer[ADDR_LEN];

    printf("\nEnter Patient ID to edit: ");
    scanf("%d", &id);
    clearInputBuffer();

    PatientNode *p = findPatientById(id);
    if (!p) {
        printf("Patient with ID %d not found.\n", id);
        pressEnterToContinue();
        return;
    }

    printf("\nEditing Patient ID %d (press Enter to keep current value)\n", id);

    printf("Name [%s]: ", p->data.name);
    fgets(buffer, sizeof(buffer), stdin); trimNewline(buffer);
    if (strlen(buffer) > 0) { strncpy(p->data.name, buffer, NAME_LEN - 1); p->data.name[NAME_LEN - 1] = '\0'; }

    printf("Age [%d]: ", p->data.age);
    fgets(buffer, sizeof(buffer), stdin); trimNewline(buffer);
    if (strlen(buffer) > 0) p->data.age = atoi(buffer);

    printf("Gender [%s]: ", p->data.gender);
    fgets(buffer, sizeof(buffer), stdin); trimNewline(buffer);
    if (strlen(buffer) > 0) { strncpy(p->data.gender, buffer, sizeof(p->data.gender) - 1); p->data.gender[sizeof(p->data.gender) - 1] = '\0'; }

    printf("Disease [%s]: ", p->data.disease);
    fgets(buffer, sizeof(buffer), stdin); trimNewline(buffer);
    if (strlen(buffer) > 0) { strncpy(p->data.disease, buffer, TEXT_LEN - 1); p->data.disease[TEXT_LEN - 1] = '\0'; }

    printf("Phone [%s]: ", p->data.phone);
    fgets(buffer, sizeof(buffer), stdin); trimNewline(buffer);
    if (strlen(buffer) > 0) { strncpy(p->data.phone, buffer, PHONE_LEN - 1); p->data.phone[PHONE_LEN - 1] = '\0'; }

    printf("Address [%s]: ", p->data.address);
    fgets(buffer, sizeof(buffer), stdin); trimNewline(buffer);
    if (strlen(buffer) > 0) { strncpy(p->data.address, buffer, ADDR_LEN - 1); p->data.address[ADDR_LEN - 1] = '\0'; }

    savePatients();
    printf("\nPatient details updated successfully.\n");
    pressEnterToContinue();
}

void viewAllPatients(void) {
    PatientNode *cur = patientHead;
    int count = 0;

    if (!patientHead) {
        printf("\nNo patients registered yet.\n");
        pressEnterToContinue();
        return;
    }

    printf("\n%-5s %-18s %-5s %-8s %-18s %-6s %-10s\n",
           "ID", "Name", "Age", "Gender", "Disease", "Bed", "Bill");
    printf("--------------------------------------------------------------------------\n");

    while (cur) {
        char bedStr[10];
        if (cur->data.bedNo == -1) strcpy(bedStr, "None");
        else sprintf(bedStr, "%d", cur->data.bedNo);

        printf("%-5d %-18s %-5d %-8s %-18s %-6s %-10.2f\n",
               cur->data.id, cur->data.name, cur->data.age, cur->data.gender,
               cur->data.disease, bedStr, cur->data.billAmount);
        cur = cur->next;
        count++;
    }
    printf("--------------------------------------------------------------------------\n");
    printf("Total Patients: %d\n", count);
    pressEnterToContinue();
}

void viewPatientById(void) {
    int id;
    printf("\nEnter Patient ID: ");
    scanf("%d", &id);
    clearInputBuffer();

    PatientNode *p = findPatientById(id);
    if (!p) { printf("Patient not found.\n"); pressEnterToContinue(); return; }

    printf("\n--- Patient Details ---\n");
    printf("ID         : %d\n", p->data.id);
    printf("Name       : %s\n", p->data.name);
    printf("Age        : %d\n", p->data.age);
    printf("Gender     : %s\n", p->data.gender);
    printf("Disease    : %s\n", p->data.disease);
    printf("Phone      : %s\n", p->data.phone);
    printf("Address    : %s\n", p->data.address);
    if (p->data.bedNo == -1) printf("Bed No     : Not Allotted\n");
    else printf("Bed No     : %d\n", p->data.bedNo);
    printf("Bill Amount: Rs. %.2f\n", p->data.billAmount);
    pressEnterToContinue();
}

int getNextDoctorId(void) {
    int maxId = 0;
    DoctorNode *cur = doctorHead;
    while (cur) { if (cur->data.id > maxId) maxId = cur->data.id; cur = cur->next; }
    return maxId + 1;
}

DoctorNode* findDoctorById(int id) {
    DoctorNode *cur = doctorHead;
    while (cur) { if (cur->data.id == id) return cur; cur = cur->next; }
    return NULL;
}

void addDoctor(void) {
    DoctorNode *node = (DoctorNode*)malloc(sizeof(DoctorNode));
    node->data.id = getNextDoctorId();

    printf("\n--- Add New Doctor (ID: %d) ---\n", node->data.id);
    printf("Name: ");
    fgets(node->data.name, NAME_LEN, stdin); trimNewline(node->data.name);
    printf("Specialization: ");
    fgets(node->data.specialization, NAME_LEN, stdin); trimNewline(node->data.specialization);
    printf("Phone: ");
    fgets(node->data.phone, PHONE_LEN, stdin); trimNewline(node->data.phone);
    printf("Consultation Fee: ");
    scanf("%f", &node->data.fee);
    clearInputBuffer();

    node->next = NULL;
    if (!doctorHead) doctorHead = node;
    else { DoctorNode *cur = doctorHead; while (cur->next) cur = cur->next; cur->next = node; }

    saveDoctors();
    printf("\nDoctor added successfully with ID %d.\n", node->data.id);
    pressEnterToContinue();
}

void editDoctor(void) {
    int id;
    char buffer[ADDR_LEN];

    printf("\nEnter Doctor ID to edit: ");
    scanf("%d", &id);
    clearInputBuffer();

    DoctorNode *d = findDoctorById(id);
    if (!d) { printf("Doctor not found.\n"); pressEnterToContinue(); return; }

    printf("\nEditing Doctor ID %d (press Enter to keep current value)\n", id);

    printf("Name [%s]: ", d->data.name);
    fgets(buffer, sizeof(buffer), stdin); trimNewline(buffer);
    if (strlen(buffer) > 0) { strncpy(d->data.name, buffer, NAME_LEN - 1); d->data.name[NAME_LEN - 1] = '\0'; }

    printf("Specialization [%s]: ", d->data.specialization);
    fgets(buffer, sizeof(buffer), stdin); trimNewline(buffer);
    if (strlen(buffer) > 0) { strncpy(d->data.specialization, buffer, NAME_LEN - 1); d->data.specialization[NAME_LEN - 1] = '\0'; }

    printf("Phone [%s]: ", d->data.phone);
    fgets(buffer, sizeof(buffer), stdin); trimNewline(buffer);
    if (strlen(buffer) > 0) { strncpy(d->data.phone, buffer, PHONE_LEN - 1); d->data.phone[PHONE_LEN - 1] = '\0'; }

    printf("Consultation Fee [%.2f]: ", d->data.fee);
    fgets(buffer, sizeof(buffer), stdin); trimNewline(buffer);
    if (strlen(buffer) > 0) d->data.fee = (float)atof(buffer);

    saveDoctors();
    printf("\nDoctor details updated successfully.\n");
    pressEnterToContinue();
}

void viewAllDoctors(void) {
    DoctorNode *cur = doctorHead;
    if (!doctorHead) { printf("\nNo doctors registered yet.\n"); pressEnterToContinue(); return; }

    printf("\n%-5s %-20s %-20s %-15s %-10s\n", "ID", "Name", "Specialization", "Phone", "Fee");
    printf("--------------------------------------------------------------------------\n");
    while (cur) {
        printf("%-5d %-20s %-20s %-15s %-10.2f\n",
               cur->data.id, cur->data.name, cur->data.specialization, cur->data.phone, cur->data.fee);
        cur = cur->next;
    }
    pressEnterToContinue();
}

int countFreeBeds(void) {
    int i, count = 0;
    for (i = 0; i < TOTAL_BEDS; i++) if (!beds[i].isAllocated) count++;
    return count;
}

void viewBedStatus(void) {
    int i;
    printf("\n%-6s %-10s %-12s %-10s %-10s\n", "Bed#", "Ward", "Charge/Day", "Status", "PatientID");
    printf("--------------------------------------------------------------\n");
    for (i = 0; i < TOTAL_BEDS; i++) {
        if (beds[i].isAllocated)
            printf("%-6d %-10s %-12.2f %-10s %-10d\n", beds[i].bedNo, beds[i].ward, beds[i].chargePerDay, "Occupied", beds[i].patientId);
        else
            printf("%-6d %-10s %-12.2f %-10s %-10s\n", beds[i].bedNo, beds[i].ward, beds[i].chargePerDay, "Free", "-");
    }
    printf("\nTotal Beds: %d | Occupied: %d | Remaining Free Beds: %d\n",
           TOTAL_BEDS, TOTAL_BEDS - countFreeBeds(), countFreeBeds());
    pressEnterToContinue();
}

void allocateBed(void) {
    int id, choice, i, allocated = 0;
    char wantedWard[WARD_LEN];

    printf("\nEnter Patient ID to allocate a bed: ");
    scanf("%d", &id);
    clearInputBuffer();

    PatientNode *p = findPatientById(id);
    if (!p) { printf("Patient not found. Please register the patient first.\n"); pressEnterToContinue(); return; }
    if (p->data.bedNo != -1) { printf("This patient already has Bed No. %d allocated.\n", p->data.bedNo); pressEnterToContinue(); return; }

    printf("Select Ward Type:\n1. General (Rs.%.0f/day)\n2. ICU (Rs.%.0f/day)\n3. Private (Rs.%.0f/day)\nChoice: ",
           GENERAL_CHARGE, ICU_CHARGE, PRIVATE_CHARGE);
    scanf("%d", &choice);
    clearInputBuffer();

    if (choice == 1) strcpy(wantedWard, "General");
    else if (choice == 2) strcpy(wantedWard, "ICU");
    else if (choice == 3) strcpy(wantedWard, "Private");
    else { printf("Invalid choice.\n"); pressEnterToContinue(); return; }

    for (i = 0; i < TOTAL_BEDS; i++) {
        if (!beds[i].isAllocated && strcmp(beds[i].ward, wantedWard) == 0) {
            beds[i].isAllocated = 1;
            beds[i].patientId = id;
            p->data.bedNo = beds[i].bedNo;
            allocated = 1;
            printf("\nBed No. %d (%s ward) allocated to %s (ID: %d).\n", beds[i].bedNo, wantedWard, p->data.name, id);
            break;
        }
    }

    if (!allocated) {
        printf("\nSorry, no free beds available in %s ward right now.\n", wantedWard);
        pressEnterToContinue();
        return;
    }

    saveBeds();
    savePatients();
    printf("Remaining free beds overall: %d out of %d\n", countFreeBeds(), TOTAL_BEDS);
    pressEnterToContinue();
}

void releaseBed(void) {
    int id, i;

    printf("\nEnter Patient ID to release/discharge bed: ");
    scanf("%d", &id);
    clearInputBuffer();

    PatientNode *p = findPatientById(id);
    if (!p) { printf("Patient not found.\n"); pressEnterToContinue(); return; }
    if (p->data.bedNo == -1) { printf("This patient does not have a bed allocated.\n"); pressEnterToContinue(); return; }

    for (i = 0; i < TOTAL_BEDS; i++) {
        if (beds[i].bedNo == p->data.bedNo) {
            beds[i].isAllocated = 0;
            beds[i].patientId = -1;
            break;
        }
    }

    printf("\nBed No. %d released for patient %s (ID: %d).\n", p->data.bedNo, p->data.name, id);
    p->data.bedNo = -1;

    saveBeds();
    savePatients();
    printf("Remaining free beds: %d out of %d\n", countFreeBeds(), TOTAL_BEDS);
    pressEnterToContinue();
}

void generateBill(void) {
    int id, i, days = 0;
    float bedCharge = 0.0, consultFee = 0.0, otherCharges = 0.0, total;
    char ward[WARD_LEN] = "N/A";
    char filename[40];
    FILE *fp;

    printf("\nEnter Patient ID to generate bill: ");
    scanf("%d", &id);
    clearInputBuffer();

    PatientNode *p = findPatientById(id);
    if (!p) { printf("Patient not found.\n"); pressEnterToContinue(); return; }

    if (p->data.bedNo != -1) {
        for (i = 0; i < TOTAL_BEDS; i++) {
            if (beds[i].bedNo == p->data.bedNo) {
                printf("Number of days admitted: ");
                scanf("%d", &days);
                clearInputBuffer();
                bedCharge = beds[i].chargePerDay * days;
                strcpy(ward, beds[i].ward);
                break;
            }
        }
    } else {
        printf("No bed allocated for this patient (outpatient billing).\n");
    }

    printf("Doctor consultation fee (0 if none): ");
    scanf("%f", &consultFee);
    clearInputBuffer();

    printf("Other charges (medicines/tests, 0 if none): ");
    scanf("%f", &otherCharges);
    clearInputBuffer();

    total = bedCharge + consultFee + otherCharges;
    p->data.billAmount = total;
    savePatients();

    printf("\n========== HOSPITAL BILL ==========\n");
    printf("Patient ID    : %d\n", p->data.id);
    printf("Patient Name  : %s\n", p->data.name);
    if (p->data.bedNo != -1) {
        printf("Ward          : %s\n", ward);
        printf("Bed No.       : %d\n", p->data.bedNo);
        printf("Days Admitted : %d\n", days);
        printf("Bed Charges   : Rs. %.2f\n", bedCharge);
    }
    printf("Consultation  : Rs. %.2f\n", consultFee);
    printf("Other Charges : Rs. %.2f\n", otherCharges);
    printf("------------------------------------\n");
    printf("TOTAL AMOUNT  : Rs. %.2f\n", total);
    printf("=====================================\n");

    sprintf(filename, "bill_%d.txt", id);
    fp = fopen(filename, "w");
    if (fp) {
        fprintf(fp, "HOSPITAL BILL RECEIPT\n");
        fprintf(fp, "Patient ID    : %d\n", p->data.id);
        fprintf(fp, "Patient Name  : %s\n", p->data.name);
        if (p->data.bedNo != -1) {
            fprintf(fp, "Ward          : %s\n", ward);
            fprintf(fp, "Bed No.       : %d\n", p->data.bedNo);
            fprintf(fp, "Days Admitted : %d\n", days);
            fprintf(fp, "Bed Charges   : Rs. %.2f\n", bedCharge);
        }
        fprintf(fp, "Consultation  : Rs. %.2f\n", consultFee);
        fprintf(fp, "Other Charges : Rs. %.2f\n", otherCharges);
        fprintf(fp, "TOTAL AMOUNT  : Rs. %.2f\n", total);
        fclose(fp);
        printf("\nReceipt saved as %s\n", filename);
    }

    pressEnterToContinue();
}

void viewBill(void) {
    int id;
    printf("\nEnter Patient ID to view bill: ");
    scanf("%d", &id);
    clearInputBuffer();

    PatientNode *p = findPatientById(id);
    if (!p) { printf("Patient not found.\n"); pressEnterToContinue(); return; }

    printf("\nPatient: %s (ID: %d)\nCurrent Bill Amount: Rs. %.2f\n", p->data.name, p->data.id, p->data.billAmount);
    pressEnterToContinue();
}

int getNextAppointmentId(void) {
    int maxId = 0;
    AppointmentNode *cur = appointmentHead;
    while (cur) { if (cur->data.id > maxId) maxId = cur->data.id; cur = cur->next; }
    return maxId + 1;
}

AppointmentNode* findAppointmentById(int id) {
    AppointmentNode *cur = appointmentHead;
    while (cur) { if (cur->data.id == id) return cur; cur = cur->next; }
    return NULL;
}

void scheduleAppointment(void) {
    int pid, did;

    printf("\nEnter Patient ID: ");
    scanf("%d", &pid);
    clearInputBuffer();
    if (!findPatientById(pid)) { printf("Patient not found.\n"); pressEnterToContinue(); return; }

    printf("Enter Doctor ID: ");
    scanf("%d", &did);
    clearInputBuffer();
    if (!findDoctorById(did)) { printf("Doctor not found.\n"); pressEnterToContinue(); return; }

    AppointmentNode *node = (AppointmentNode*)malloc(sizeof(AppointmentNode));
    node->data.id = getNextAppointmentId();
    node->data.patientId = pid;
    node->data.doctorId = did;

    printf("Date (DD-MM-YYYY): ");
    fgets(node->data.date, DATE_LEN, stdin); trimNewline(node->data.date);
    printf("Time (HH:MM): ");
    fgets(node->data.time, TIME_LEN, stdin); trimNewline(node->data.time);
    strcpy(node->data.status, "Scheduled");

    node->next = NULL;
    if (!appointmentHead) appointmentHead = node;
    else { AppointmentNode *cur = appointmentHead; while (cur->next) cur = cur->next; cur->next = node; }

    saveAppointments();
    printf("\nAppointment scheduled successfully with ID %d.\n", node->data.id);
    pressEnterToContinue();
}

void editAppointment(void) {
    int id;
    char buffer[20];

    printf("\nEnter Appointment ID to edit: ");
    scanf("%d", &id);
    clearInputBuffer();

    AppointmentNode *a = findAppointmentById(id);
    if (!a) { printf("Appointment not found.\n"); pressEnterToContinue(); return; }

    printf("\nEditing Appointment ID %d (press Enter to keep current value)\n", id);

    printf("Date [%s]: ", a->data.date);
    fgets(buffer, sizeof(buffer), stdin); trimNewline(buffer);
    if (strlen(buffer) > 0) { strncpy(a->data.date, buffer, DATE_LEN - 1); a->data.date[DATE_LEN - 1] = '\0'; }

    printf("Time [%s]: ", a->data.time);
    fgets(buffer, sizeof(buffer), stdin); trimNewline(buffer);
    if (strlen(buffer) > 0) { strncpy(a->data.time, buffer, TIME_LEN - 1); a->data.time[TIME_LEN - 1] = '\0'; }

    printf("Status [%s] (Scheduled/Completed/Cancelled): ", a->data.status);
    fgets(buffer, sizeof(buffer), stdin); trimNewline(buffer);
    if (strlen(buffer) > 0) { strncpy(a->data.status, buffer, STATUS_LEN - 1); a->data.status[STATUS_LEN - 1] = '\0'; }

    saveAppointments();
    printf("\nAppointment updated successfully.\n");
    pressEnterToContinue();
}

void cancelAppointment(void) {
    int id;
    printf("\nEnter Appointment ID to cancel: ");
    scanf("%d", &id);
    clearInputBuffer();

    AppointmentNode *a = findAppointmentById(id);
    if (!a) { printf("Appointment not found.\n"); pressEnterToContinue(); return; }

    strcpy(a->data.status, "Cancelled");
    saveAppointments();
    printf("\nAppointment ID %d marked as Cancelled.\n", id);
    pressEnterToContinue();
}

void viewAllAppointments(void) {
    AppointmentNode *cur = appointmentHead;
    if (!appointmentHead) { printf("\nNo appointments scheduled.\n"); pressEnterToContinue(); return; }

    printf("\n%-5s %-10s %-10s %-12s %-8s %-12s\n", "ID", "PatientID", "DoctorID", "Date", "Time", "Status");
    printf("--------------------------------------------------------------------------\n");
    while (cur) {
        printf("%-5d %-10d %-10d %-12s %-8s %-12s\n",
               cur->data.id, cur->data.patientId, cur->data.doctorId, cur->data.date, cur->data.time, cur->data.status);
        cur = cur->next;
    }
    pressEnterToContinue();
}

void addEmergencyPatient(void) {
    EmergencyNode *node = (EmergencyNode*)malloc(sizeof(EmergencyNode));

    printf("\n--- Add Patient to Emergency Queue ---\n");
    printf("Patient ID (existing, or 0 if new/unregistered): ");
    scanf("%d", &node->data.patientId);
    clearInputBuffer();

    printf("Patient Name: ");
    fgets(node->data.name, NAME_LEN, stdin); trimNewline(node->data.name);

    printf("Age: ");
    scanf("%d", &node->data.age);
    clearInputBuffer();

    printf("Condition: ");
    fgets(node->data.condition, TEXT_LEN, stdin); trimNewline(node->data.condition);

    printf("Priority (1 = Critical, 2 = Serious, 3 = Stable): ");
    scanf("%d", &node->data.priority);
    clearInputBuffer();
    if (node->data.priority < 1 || node->data.priority > 3) node->data.priority = 3;

    node->next = NULL;
    if (!emergencyHead || emergencyHead->data.priority > node->data.priority) {
        node->next = emergencyHead;
        emergencyHead = node;
    } else {
        EmergencyNode *cur = emergencyHead;
        while (cur->next && cur->next->data.priority <= node->data.priority) cur = cur->next;
        node->next = cur->next;
        cur->next = node;
    }

    saveEmergencyQueue();
    printf("\nPatient added to Emergency Queue.\n");
    pressEnterToContinue();
}

void serveEmergencyPatient(void) {
    EmergencyNode *top;
    if (!emergencyHead) { printf("\nEmergency queue is empty.\n"); pressEnterToContinue(); return; }

    top = emergencyHead;
    printf("\n--- Serving Next Emergency Patient ---\n");
    printf("Patient ID : %d\n", top->data.patientId);
    printf("Name       : %s\n", top->data.name);
    printf("Age        : %d\n", top->data.age);
    printf("Condition  : %s\n", top->data.condition);
    printf("Priority   : %d\n", top->data.priority);

    emergencyHead = emergencyHead->next;
    free(top);
    saveEmergencyQueue();

    printf("\nPatient has been attended to and removed from the queue.\n");
    pressEnterToContinue();
}

void viewEmergencyQueue(void) {
    EmergencyNode *cur = emergencyHead;
    int pos = 1;

    if (!emergencyHead) { printf("\nEmergency queue is empty.\n"); pressEnterToContinue(); return; }

    printf("\n%-5s %-10s %-20s %-5s %-20s %-10s\n", "S.No", "PatientID", "Name", "Age", "Condition", "Priority");
    printf("--------------------------------------------------------------------------\n");
    while (cur) {
        printf("%-5d %-10d %-20s %-5d %-20s %-10d\n",
               pos, cur->data.patientId, cur->data.name, cur->data.age, cur->data.condition, cur->data.priority);
        cur = cur->next;
        pos++;
    }
    pressEnterToContinue();
}

void searchPatientByIdMenu(void) {
    int id;
    printf("\nEnter Patient ID to search: ");
    scanf("%d", &id);
    clearInputBuffer();

    PatientNode *p = findPatientById(id);
    if (!p) printf("No patient found with ID %d.\n", id);
    else printf("\nFound: ID %d | Name: %s | Age: %d | Disease: %s\n",
                p->data.id, p->data.name, p->data.age, p->data.disease);
    pressEnterToContinue();
}

void searchPatientByNameMenu(void) {
    char query[NAME_LEN], queryLower[NAME_LEN], nameLower[NAME_LEN];
    PatientNode *cur;
    int found = 0;

    printf("\nEnter patient name (or part of it) to search: ");
    fgets(query, NAME_LEN, stdin); trimNewline(query);
    toLowerStr(queryLower, query);

    cur = patientHead;
    printf("\nSearch Results:\n");
    while (cur) {
        toLowerStr(nameLower, cur->data.name);
        if (strstr(nameLower, queryLower)) {
            printf("ID %d | Name: %s | Age: %d | Disease: %s\n",
                   cur->data.id, cur->data.name, cur->data.age, cur->data.disease);
            found = 1;
        }
        cur = cur->next;
    }
    if (!found) printf("No matching patients found.\n");
    pressEnterToContinue();
}

void sortPatientsById(void) {
    int swapped;
    PatientNode *cur;

    if (!patientHead || !patientHead->next) { printf("\nNothing to sort.\n"); pressEnterToContinue(); return; }

    do {
        swapped = 0;
        cur = patientHead;
        while (cur->next) {
            if (cur->data.id > cur->next->data.id) {
                PatientData temp = cur->data;
                cur->data = cur->next->data;
                cur->next->data = temp;
                swapped = 1;
            }
            cur = cur->next;
        }
    } while (swapped);

    savePatients();
    printf("\nPatients sorted by ID successfully.\n");
    viewAllPatients();
}

void sortPatientsByName(void) {
    int swapped;
    PatientNode *cur;

    if (!patientHead || !patientHead->next) { printf("\nNothing to sort.\n"); pressEnterToContinue(); return; }

    do {
        swapped = 0;
        cur = patientHead;
        while (cur->next) {
            if (strcicmp(cur->data.name, cur->next->data.name) > 0) {
                PatientData temp = cur->data;
                cur->data = cur->next->data;
                cur->next->data = temp;
                swapped = 1;
            }
            cur = cur->next;
        }
    } while (swapped);

    savePatients();
    printf("\nPatients sorted by name successfully.\n");
    viewAllPatients();
}

void searchDoctorByIdMenu(void) {
    int id;
    printf("\nEnter Doctor ID to search: ");
    scanf("%d", &id);
    clearInputBuffer();

    DoctorNode *d = findDoctorById(id);
    if (!d) printf("No doctor found with ID %d.\n", id);
    else printf("\nFound: ID %d | Name: %s | Specialization: %s | Fee: Rs.%.2f\n",
                d->data.id, d->data.name, d->data.specialization, d->data.fee);
    pressEnterToContinue();
}

void searchDoctorBySpecializationMenu(void) {
    char query[NAME_LEN], queryLower[NAME_LEN], specLower[NAME_LEN];
    DoctorNode *cur;
    int found = 0;

    printf("\nEnter specialization to search: ");
    fgets(query, NAME_LEN, stdin); trimNewline(query);
    toLowerStr(queryLower, query);

    cur = doctorHead;
    printf("\nSearch Results:\n");
    while (cur) {
        toLowerStr(specLower, cur->data.specialization);
        if (strstr(specLower, queryLower)) {
            printf("ID %d | Name: %s | Specialization: %s | Fee: Rs.%.2f\n",
                   cur->data.id, cur->data.name, cur->data.specialization, cur->data.fee);
            found = 1;
        }
        cur = cur->next;
    }
    if (!found) printf("No matching doctors found.\n");
    pressEnterToContinue();
}

void sortDoctorsByName(void) {
    int swapped;
    DoctorNode *cur;

    if (!doctorHead || !doctorHead->next) { printf("\nNothing to sort.\n"); pressEnterToContinue(); return; }

    do {
        swapped = 0;
        cur = doctorHead;
        while (cur->next) {
            if (strcicmp(cur->data.name, cur->next->data.name) > 0) {
                DoctorData temp = cur->data;
                cur->data = cur->next->data;
                cur->next->data = temp;
                swapped = 1;
            }
            cur = cur->next;
        }
    } while (swapped);

    saveDoctors();
    printf("\nDoctors sorted by name successfully.\n");
    viewAllDoctors();
}

void mainMenu(void) {
    int choice;
    do {
        printf("\n==================================================\n");
        printf("        HOSPITAL MANAGEMENT SYSTEM - MAIN MENU\n");
        printf("==================================================\n");
        printf(" 1. Patient Registration\n");
        printf(" 2. Bed Allocation\n");
        printf(" 3. Doctor Records\n");
        printf(" 4. Billing\n");
        printf(" 5. Appointment Scheduling\n");
        printf(" 6. Emergency Queue\n");
        printf(" 7. Search & Sort\n");
        printf(" 8. Exit\n");
        printf("--------------------------------------------------\n");
        printf("Enter your choice: ");
        choice = readMenuChoice();

        switch (choice) {
            case 1: patientMenu(); break;
            case 2: bedMenu(); break;
            case 3: doctorMenu(); break;
            case 4: billingMenu(); break;
            case 5: appointmentMenu(); break;
            case 6: emergencyMenu(); break;
            case 7: searchSortMenu(); break;
            case 8: printf("\nExiting...\n"); break;
            default: printf("\nInvalid choice. Try again.\n");
        }
    } while (choice != 8);
}

void patientMenu(void) {
    int choice;
    do {
        printf("\n------ Patient Registration Menu ------\n");
        printf("1. Add New Patient\n");
        printf("2. Edit Patient Details\n");
        printf("3. View All Patients\n");
        printf("4. View Patient by ID\n");
        printf("5. Back to Main Menu\n");
        printf("Enter your choice: ");
        choice = readMenuChoice();
        switch (choice) {
            case 1: addPatient(); break;
            case 2: editPatient(); break;
            case 3: viewAllPatients(); break;
            case 4: viewPatientById(); break;
            case 5: break;
            default: printf("\nInvalid choice.\n");
        }
    } while (choice != 5);
}

void bedMenu(void) {
    int choice;
    do {
        printf("\n------ Bed Allocation Menu ------\n");
        printf("1. View Bed Status (and remaining beds)\n");
        printf("2. Allocate Bed to Patient\n");
        printf("3. Release/Discharge Bed\n");
        printf("4. Back to Main Menu\n");
        printf("Enter your choice: ");
        choice = readMenuChoice();
        switch (choice) {
            case 1: viewBedStatus(); break;
            case 2: allocateBed(); break;
            case 3: releaseBed(); break;
            case 4: break;
            default: printf("\nInvalid choice.\n");
        }
    } while (choice != 4);
}

void doctorMenu(void) {
    int choice;
    do {
        printf("\n------ Doctor Records Menu ------\n");
        printf("1. Add New Doctor\n");
        printf("2. Edit Doctor Details\n");
        printf("3. View All Doctors\n");
        printf("4. Back to Main Menu\n");
        printf("Enter your choice: ");
        choice = readMenuChoice();
        switch (choice) {
            case 1: addDoctor(); break;
            case 2: editDoctor(); break;
            case 3: viewAllDoctors(); break;
            case 4: break;
            default: printf("\nInvalid choice.\n");
        }
    } while (choice != 4);
}

void billingMenu(void) {
    int choice;
    do {
        printf("\n------ Billing Menu ------\n");
        printf("1. Generate Bill for Patient\n");
        printf("2. View Current Bill of Patient\n");
        printf("3. Back to Main Menu\n");
        printf("Enter your choice: ");
        choice = readMenuChoice();
        switch (choice) {
            case 1: generateBill(); break;
            case 2: viewBill(); break;
            case 3: break;
            default: printf("\nInvalid choice.\n");
        }
    } while (choice != 3);
}

void appointmentMenu(void) {
    int choice;
    do {
        printf("\n------ Appointment Menu ------\n");
        printf("1. Schedule New Appointment\n");
        printf("2. Edit Appointment\n");
        printf("3. Cancel Appointment\n");
        printf("4. View All Appointments\n");
        printf("5. Back to Main Menu\n");
        printf("Enter your choice: ");
        choice = readMenuChoice();
        switch (choice) {
            case 1: scheduleAppointment(); break;
            case 2: editAppointment(); break;
            case 3: cancelAppointment(); break;
            case 4: viewAllAppointments(); break;
            case 5: break;
            default: printf("\nInvalid choice.\n");
        }
    } while (choice != 5);
}

void emergencyMenu(void) {
    int choice;
    do {
        printf("\n------ Emergency Queue Menu ------\n");
        printf("1. Add Patient to Emergency Queue\n");
        printf("2. Serve Next Emergency Patient\n");
        printf("3. View Emergency Queue\n");
        printf("4. Back to Main Menu\n");
        printf("Enter your choice: ");
        choice = readMenuChoice();
        switch (choice) {
            case 1: addEmergencyPatient(); break;
            case 2: serveEmergencyPatient(); break;
            case 3: viewEmergencyQueue(); break;
            case 4: break;
            default: printf("\nInvalid choice.\n");
        }
    } while (choice != 4);
}

void searchSortMenu(void) {
    int choice;
    do {
        printf("\n------ Search & Sort Menu ------\n");
        printf("1. Search Patient by ID\n");
        printf("2. Search Patient by Name\n");
        printf("3. Sort Patients by ID\n");
        printf("4. Sort Patients by Name\n");
        printf("5. Search Doctor by ID\n");
        printf("6. Search Doctor by Specialization\n");
        printf("7. Sort Doctors by Name\n");
        printf("8. Back to Main Menu\n");
        printf("Enter your choice: ");
        choice = readMenuChoice();
        switch (choice) {
            case 1: searchPatientByIdMenu(); break;
            case 2: searchPatientByNameMenu(); break;
            case 3: sortPatientsById(); break;
            case 4: sortPatientsByName(); break;
            case 5: searchDoctorByIdMenu(); break;
            case 6: searchDoctorBySpecializationMenu(); break;
            case 7: sortDoctorsByName(); break;
            case 8: break;
            default: printf("\nInvalid choice.\n");
        }
    } while (choice != 8);
}

void freeAllMemory(void) {
    PatientNode *p = patientHead;
    while (p) { PatientNode *t = p; p = p->next; free(t); }

    DoctorNode *d = doctorHead;
    while (d) { DoctorNode *t = d; d = d->next; free(t); }

    AppointmentNode *a = appointmentHead;
    while (a) { AppointmentNode *t = a; a = a->next; free(t); }

    EmergencyNode *e = emergencyHead;
    while (e) { EmergencyNode *t = e; e = e->next; free(t); }
}