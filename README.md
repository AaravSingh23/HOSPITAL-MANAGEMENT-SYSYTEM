🏥 Hospital Management System (HMS)

A terminal-based Hospital Management System written in pure C, designed to manage core hospital operations including patient registration, bed allocation, doctor records, billing, appointment scheduling, and emergency triage — all backed by persistent binary file storage.


📋 Table of Contents


Features
Data Structures
Getting Started
Usage
File Structure
Bed Configuration
Screenshots
Contributing



✨ Features

👤 Patient Management


Register new patients with full details (name, age, gender, disease, phone, address)
Edit existing patient records
View all patients in a formatted table
Look up a patient by ID


🛏️ Bed Allocation


30 beds across three wards: General, ICU, and Private
Allocate a bed to a registered patient by ward type
Release/discharge a bed when a patient leaves
Real-time view of bed availability and occupancy status


👨‍⚕️ Doctor Records


Add and edit doctor profiles (name, specialization, phone, consultation fee)
View the full list of registered doctors


💰 Billing


Auto-calculate bills based on ward type and number of days admitted
View a patient's current outstanding bill


📅 Appointment Scheduling


Schedule appointments linking a patient to a doctor with date and time
Edit or cancel existing appointments
View all scheduled appointments


🚨 Emergency Queue


Add emergency patients with a priority level
Serve patients in priority order (priority queue via linked list)
View the current emergency queue


🔍 Search & Sort


Search patients by ID or Name (case-insensitive)
Search doctors by ID or Specialization
Sort patients by ID or Name
Sort doctors by Name


💾 Persistent Storage


All data is automatically saved to binary .dat files on exit and after every write operation
Data is reloaded from disk on the next program launch — no data loss between sessions



🗂️ Data Structures

The system uses singly linked lists for dynamic, heap-allocated records:

EntityStructureStorage FilePatientsPatientNodepatients.datDoctorsDoctorNodedoctors.datAppointmentsAppointmentNodeappointments.datEmergencyEmergencyNodeemergency.datBedsBed[] (array)beds.dat


🚀 Getting Started

Prerequisites


A C compiler: gcc, clang, or equivalent
Works on Linux, macOS, and Windows (MinGW/MSYS2)


Compilation

bashgcc HMS.c -o hms

Run

bash./hms


📖 Usage

On launch, the program loads all existing data and presents the main menu:

==================================================
        HOSPITAL MANAGEMENT SYSTEM - MAIN MENU
==================================================
 1. Patient Registration
 2. Bed Allocation
 3. Doctor Records
 4. Billing
 5. Appointment Scheduling
 6. Emergency Queue
 7. Search & Sort
 8. Exit
--------------------------------------------------
Enter your choice:

Navigate using numeric menu choices. All data is saved automatically on every change and on exit.


📁 File Structure

.
├── HMS.c               # Full source code (single-file project)
├── patients.dat        # Binary: patient records (auto-generated)
├── doctors.dat         # Binary: doctor records (auto-generated)
├── beds.dat            # Binary: bed allocation state (auto-generated)
├── appointments.dat    # Binary: appointment records (auto-generated)
└── emergency.dat       # Binary: emergency queue (auto-generated)


⚠️ The .dat files are generated automatically at runtime. Do not manually edit them.




🛏️ Bed Configuration

WardBedsCharge/DayGeneral10₹500ICU10₹1,500Private10₹1,000Total30—

These values can be adjusted via the #define constants at the top of HMS.c.
