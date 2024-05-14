#include <iostream>
#include <queue>
#include <string>
#include <fstream>
#include <sstream>
#include <unordered_map>
#include <stdexcept>
#include <limits>

using namespace std;

unordered_map<string, int> diseasePriority = {
    {"coughing", 1},
    {"cold", 1},
    {"stomach ache", 2},
    {"headache", 3},
    {"vomiting", 4},
    {"diarrhea", 4},
    {"loss in consciousness", 5},
    {"bleeding", 7},
    {"concussion", 8},
    {"heart ache", 8}
};

// Struct to represent patient information
struct Patient {
    string name;
    string disease;
    int timeToReach;
    int appointmentNumber; 

    // Constructor
    Patient(string n, string d, int t, int apptNum) : name(n), disease(d), timeToReach(t), appointmentNumber(apptNum) {}

    // Comparison operator for priority queue
    bool operator<(const Patient& other) const {
        // Prioritize based on disease severity and time to reach
        if (diseasePriority[disease] == diseasePriority[other.disease])
            return timeToReach > other.timeToReach; // Lesser time to reach has higher priority
        return diseasePriority[disease] > diseasePriority[other.disease];
    }
};

// Function prototypes
void registerPatient(priority_queue<Patient>& pq);
bool loginPatient();
void bookAppointment(priority_queue<Patient>& pq);
void displayQueue(priority_queue<Patient> pq);
void saveQueueToFile(priority_queue<Patient> pq);

// Function to clear input buffer
void clearInputBuffer() {
    cin.clear();
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
}

// Function to validate integer input
int validateIntegerInput() {
    int input;
    while (true) {
        cin >> input;
        if (cin.fail()) {
            cout << "Invalid input. Please enter a valid integer: ";
            clearInputBuffer();
        } else {
            return input;
        }
    }
}

// Function to validate positive integer input
int validatePositiveIntegerInput() {
    int input;
    while (true) {
        input = validateIntegerInput();
        if (input <= 0) {
            cout << "Input must be a positive integer. Please enter again: ";
        } else {
            return input;
        }
    }
}

// Function to validate disease name input
string validateDiseaseInput() {
    string disease;
    unordered_map<string, int>::const_iterator it;
    while (true) {
        cin >> disease;
        it = diseasePriority.find(disease);
        if (it == diseasePriority.end()) {
            cout << "Invalid disease name. Please enter a valid disease: ";
        } else {
            return disease;
        }
    }
}

// Function to register a patient
void registerPatient(priority_queue<Patient>& pq) {
    string username, password, dob;
    cout << "Enter username: ";
    cin >> username;
    cout << "Enter password: ";
    cin >> password;
    cout << "Enter date of birth (DD/MM/YYYY): ";
    cin >> dob;

    // Save credentials to CSV file
    ofstream outFile("patients.csv", ios::app);
    if (!outFile.is_open()) {
        throw runtime_error("Unable to open file.");
    }
    outFile << username << "," << password << "," << dob << endl;
    outFile.close();

    cout << "Registration successful!" << endl;
}

// Function to login a patient
bool loginPatient() {
    string username, password, dob;
    cout << "Enter username: ";
    cin >> username;
    cout << "Enter password: ";
    cin >> password;
    cout << "Enter date of birth (DD/MM/YYYY): ";
    cin >> dob;

    // Check credentials from CSV file
    ifstream inFile("patients.csv");
    if (!inFile.is_open()) {
        throw runtime_error("Unable to open file.");
    }
    string line;
    while (getline(inFile, line)) {
        stringstream ss(line);
        string savedUsername, savedPassword, savedDob;
        getline(ss, savedUsername, ',');
        getline(ss, savedPassword, ',');
        getline(ss, savedDob, ',');

        if (username == savedUsername && password == savedPassword && dob == savedDob) {
            inFile.close();
            return true;
        }
    }
    inFile.close();
    return false;
}

// Function to book an appointment
void bookAppointment(priority_queue<Patient>& pq) {
    string name, disease;
    int timeToReach;
    cout << "Enter patient name: ";
    cin >> name;
    cout << "Enter disease: ";
    disease = validateDiseaseInput();
    cout << "Enter approximate time to reach hospital (in minutes): ";
    timeToReach = validatePositiveIntegerInput();

    // Create patient object
    static int appointmentCounter = 1; // New feature: Appointment number counter
    Patient patient(name, disease, timeToReach, appointmentCounter++);

    // Add patient to priority queue
    pq.push(patient);

    // Save patient queue to file
    saveQueueToFile(pq);

    cout << "Appointment booked successfully!" << endl;
}

// Function to save patient queue to file
void saveQueueToFile(priority_queue<Patient> pq) {
    ofstream outFile("patient_queue.csv");
    if (!outFile.is_open()) {
        throw runtime_error("Unable to open file.");
    }
    while (!pq.empty()) {
        Patient p = pq.top();
        outFile << p.appointmentNumber << "," << p.name << "," << p.disease << "," << p.timeToReach << endl;
        pq.pop();
    }
    outFile.close();
}

// Function to display the priority queue
void displayQueue(priority_queue<Patient> pq) {
    cout << "Priority Queue:" << endl;
    while (!pq.empty()) {
        Patient patient = pq.top();
        cout << "Appointment Number: " << patient.appointmentNumber << ", Name: " << patient.name << ", Disease: " << patient.disease << ", Time to reach: " << patient.timeToReach << endl;
        pq.pop();
    }
}

// Function to display menu
void displayMenu() {
    cout << "\nMenu:" << endl;
    cout << "1. Register patient" << endl;
    cout << "2. Login" << endl;
    cout << "3. Book appointment" << endl;
    cout << "4. Display patient queue" << endl;
    cout << "5. Exit" << endl;
    cout << "Enter your choice: ";
}

int main() {
    priority_queue<Patient> patientQueue;
    bool loggedIn = false;

    while (true) {
        displayMenu();
        int choice;
        cin >> choice;

        switch (choice) {
            case 1:
                try {
                    registerPatient(patientQueue);
                } catch (const runtime_error& e) {
                    cerr << "Error: " << e.what() << endl;
                }
                break;
            case 2:
                try {
                    loggedIn = loginPatient();
                    if (!loggedIn) {
                        cout << "Invalid username or password." << endl;
                    }
                } catch (const runtime_error& e) {
                    cerr << "Error: " << e.what() << endl;
                }
                break;
            case 3:
                if (!loggedIn) {
                    cout << "Please login first." << endl;
                    break;
                }
                try {
                    bookAppointment(patientQueue);
                } catch (const runtime_error& e) {
                    cerr << "Error: " << e.what() << endl;
                }
                break;
            case 4:
                displayQueue(patientQueue);
                break;
            case 5:
                cout << "Exiting program." << endl;
                return 0;
            default:
                cout << "Invalid choice. Please enter a number between 1 and 5." << endl;
                clearInputBuffer();
        }
    }

    return 0;
}
