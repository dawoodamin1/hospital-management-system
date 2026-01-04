// ============================================================================
// HOSPITAL EMERGENCY MANAGEMENT SYSTEM - MODULE 2 ONLY
// Doctor & Resource Allocation System
// FIXED VERSION - NO ENCODING ISSUES, CSV FILES WORK PROPERLY
// ============================================================================

#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <iomanip>

using namespace std;

// ============================================================================
// STRUCTURES
// ============================================================================

struct Patient {
    int id;
    string name;
    int age;
    int priority;
    string symptoms;
};

struct Doctor {
    int id;
    string name;
    string specialization;
    bool available;
};

struct Resource {
    int id;
    string name;
    bool available;
};

struct AllocNode {
    Patient patient;
    Doctor doctor;
    Resource resource;
    AllocNode* next;
};

// ============================================================================
// HELPER FUNCTION:  CREATE EMPTY CSV FILES IF THEY DON'T EXIST
// ============================================================================

void createDefaultCSVFiles() {
    ifstream checkDoctors("doctors.csv");
    if(! checkDoctors) {
        ofstream doctorsFile("doctors.csv");
        doctorsFile << "DoctorID,Name,Specialization,Available\n";
        doctorsFile. close();
        cout << "Created doctors.csv\n";
    }
    checkDoctors.close();

    ifstream checkResources("resources.csv");
    if(!checkResources) {
        ofstream resourcesFile("resources.csv");
        resourcesFile << "ResourceID,Name,Available\n";
        resourcesFile.close();
        cout << "Created resources. csv\n";
    }
    checkResources.close();

    ifstream checkAllocation("allocation_queue.csv");
    if(!checkAllocation) {
        ofstream allocationFile("allocation_queue.csv");
        allocationFile << "PatientID,PatientName,PatientAge,PatientPriority,PatientSymptoms,DoctorID,DoctorName,DoctorSpecialization,ResourceID,ResourceName\n";
        allocationFile. close();
        cout << "Created allocation_queue.csv\n";
    }
    checkAllocation.close();
}

// ============================================================================
// MODULE 2: ALLOCATION QUEUE - LINKED LIST FOR DOCTOR ALLOCATIONS
// ============================================================================

class AllocationQueue {
private:
    AllocNode* front;
    AllocNode* rear;

public:
    AllocationQueue() {
        front = nullptr;
        rear = nullptr;
    }

    void enqueue(Patient p, Doctor d, Resource r) {
        AllocNode* newNode = new AllocNode;
        newNode->patient = p;
        newNode->doctor = d;
        newNode->resource = r;
        newNode->next = nullptr;

        if(isEmpty()) {
            front = rear = newNode;
        } else {
            rear->next = newNode;
            rear = newNode;
        }

        cout << "Patient '" << p.name << "' allocated to Dr. " << d.name << " with " << r.name << "\n";
    }

    void dequeue() {
        if(isEmpty()) {
            cout << "Queue is empty.  Cannot dequeue.\n";
            return;
        }

        AllocNode* temp = front;
        cout << "Patient '" << temp->patient.name << "' treatment completed and released.\n";

        front = front->next;

        if(front == nullptr)
            rear = nullptr;

        delete temp;
    }

    bool isEmpty() const {
        return front == nullptr;
    }

    void display() const {
        cout << "\n========== CURRENT PATIENT-DOCTOR ALLOCATIONS ==========\n\n";

        if(isEmpty()) {
            cout << "No active allocations.\n\n";
            return;
        }

        AllocNode* temp = front;
        int count = 1;

        while(temp != nullptr) {
            cout << count << ". Patient: " << temp->patient.name
                 << " (ID: " << temp->patient.id
                 << ", Priority: " << temp->patient.priority << ")"
                 << "\n   Doctor: Dr. " << temp->doctor.name
                 << " (Specialization: " << temp->doctor.specialization << ")"
                 << "\n   Resource: " << temp->resource.name << "\n\n";
            temp = temp->next;
            count++;
        }
    }

    AllocNode* getFront() {
        return front;
    }

    int getQueueSize() const {
        int count = 0;
        AllocNode* temp = front;
        while(temp != nullptr) {
            count++;
            temp = temp->next;
        }
        return count;
    }

    void saveToFile(const string& filename) const {
        ofstream file(filename);
        if(!file) {
            cout << "Cannot open file " << filename << "\n";
            return;
        }

        file << "PatientID,PatientName,PatientAge,PatientPriority,PatientSymptoms,DoctorID,DoctorName,DoctorSpecialization,ResourceID,ResourceName\n";

        AllocNode* temp = front;

        while(temp != nullptr) {
            file << temp->patient. id << ","
                 << temp->patient.name << ","
                 << temp->patient.age << ","
                 << temp->patient.priority << ","
                 << temp->patient.symptoms << ","
                 << temp->doctor.id << ","
                 << temp->doctor.name << ","
                 << temp->doctor.specialization << ","
                 << temp->resource.id << ","
                 << temp->resource.name << "\n";
            temp = temp->next;
        }

        file.close();
        cout << "Allocation queue saved to " << filename << "\n";
    }

    void loadFromFile(const string& filename) {
        ifstream file(filename);
        if(!file) {
            cout << "File " << filename << " not found.\n";
            return;
        }

        string line;
        getline(file, line);

        while(getline(file, line)) {
            if(line.empty()) continue;

            stringstream ss(line);
            string temp;

            Patient p;
            Doctor d;
            Resource r;

            getline(ss, temp, ','); p.id = stoi(temp);
            getline(ss, p.name, ',');
            getline(ss, temp, ','); p.age = stoi(temp);
            getline(ss, temp, ','); p.priority = stoi(temp);
            getline(ss, p.symptoms, ',');

            getline(ss, temp, ','); d.id = stoi(temp);
            getline(ss, d.name, ',');
            getline(ss, d.specialization, ',');

            getline(ss, temp, ','); r.id = stoi(temp);
            getline(ss, r.name);
            r.available = true;

            enqueue(p, d, r);
        }

        file.close();
        if(front != nullptr) {
            cout << "Allocation queue loaded from " << filename << "\n";
        }
    }

    ~AllocationQueue() {
        while(! isEmpty()) {
            dequeue();
        }
    }
};

// ============================================================================
// MODULE 2: DOCTOR MANAGEMENT - DOCTORS & RESOURCES
// ============================================================================

class DoctorManagement {
private:
    vector<Doctor> doctors;
    vector<Resource> resources;
    AllocationQueue allocationQueue;
    int nextDoctorID;
    int nextResourceID;

public:
    DoctorManagement() : nextDoctorID(1), nextResourceID(1) {
        addDoctor("Dr. Ahmed Hassan", "Cardiology");
        addDoctor("Dr.  Fatima Khan", "Emergency Medicine");
        addDoctor("Dr. Muhammad Ali", "Orthopedics");
        addDoctor("Dr.  Aisha Malik", "General Surgery");
        addDoctor("Dr. Hassan Raza", "Cardiology");
        addDoctor("Dr. Zainab Ahmed", "Emergency Medicine");

        addResource("ICU Bed");
        addResource("Ventilator");
        addResource("Oxygen Tank");
        addResource("Surgical Equipment");
        addResource("Monitor");
        addResource("Defibrillator");
    }

    void addDoctor(const string& name, const string& specialization) {
        Doctor newDoctor;
        newDoctor.id = nextDoctorID++;
        newDoctor.name = name;
        newDoctor.specialization = specialization;
        newDoctor.available = true;

        doctors.push_back(newDoctor);
        cout << "Doctor added: " << name << " (ID: " << newDoctor.id << ")\n";
    }

    void removeDoctor(int doctorID) {
        for(size_t i = 0; i < doctors.size(); i++) {
            if(doctors[i]. id == doctorID) {
                cout << "Doctor removed: " << doctors[i].name << "\n";
                doctors.erase(doctors.begin() + i);
                return;
            }
        }
        cout << "Doctor ID " << doctorID << " not found.\n";
    }

    Doctor* findAvailableDoctor(const string& specialization = "") {
        if(! specialization.empty()) {
            for(auto& doc : doctors) {
                if(doc.available && doc.specialization == specialization) {
                    return &doc;
                }
            }
        }

        for(auto& doc : doctors) {
            if(doc. available) {
                return &doc;
            }
        }

        return nullptr;
    }

    Doctor* getDoctorByID(int doctorID) {
        for(auto& doc :  doctors) {
            if(doc.id == doctorID) {
                return &doc;
            }
        }
        return nullptr;
    }

    void displayAllDoctors() const {
        cout << "\n========== ALL DOCTORS (" << doctors.size() << ") ==========\n\n";

        if(doctors.empty()) {
            cout << "No doctors in system.\n";
            return;
        }

        for(size_t i = 0; i < doctors.size(); i++) {
            cout << (i + 1) << ". ID: " << doctors[i].id
                 << " | Name: " << doctors[i]. name
                 << " | Specialization: " << doctors[i].specialization
                 << " | Status: " << (doctors[i]. available ? "AVAILABLE" : "BUSY") << "\n";
        }
        cout << "\n";
    }

    void updateDoctorAvailability(int doctorID, bool available) {
        Doctor* doc = getDoctorByID(doctorID);
        if(doc) {
            doc->available = available;
        }
    }

    void addResource(const string& resourceName) {
        Resource newResource;
        newResource.id = nextResourceID++;
        newResource.name = resourceName;
        newResource.available = true;

        resources.push_back(newResource);
        cout << "Resource added: " << resourceName << " (ID: " << newResource. id << ")\n";
    }

    void removeResource(int resourceID) {
        for(size_t i = 0; i < resources.size(); i++) {
            if(resources[i].id == resourceID) {
                cout << "Resource removed: " << resources[i].name << "\n";
                resources.erase(resources.begin() + i);
                return;
            }
        }
        cout << "Resource ID " << resourceID << " not found.\n";
    }

    Resource* findAvailableResource() {
        for(auto& res : resources) {
            if(res.available) {
                return &res;
            }
        }
        return nullptr;
    }

    Resource* getResourceByID(int resourceID) {
        for(auto& res : resources) {
            if(res.id == resourceID) {
                return &res;
            }
        }
        return nullptr;
    }

    void displayAllResources() const {
        cout << "\n========== ALL RESOURCES (" << resources.size() << ") ==========\n\n";

        if(resources. empty()) {
            cout << "No resources in system.\n";
            return;
        }

        for(size_t i = 0; i < resources.size(); i++) {
            cout << (i + 1) << ". ID: " << resources[i]. id
                 << " | Name: " << resources[i].name
                 << " | Status: " << (resources[i]. available ? "AVAILABLE" : "IN USE") << "\n";
        }
        cout << "\n";
    }

    void updateResourceAvailability(int resourceID, bool available) {
        Resource* res = getResourceByID(resourceID);
        if(res) {
            res->available = available;
        }
    }

    void allocatePatientToDoctor(const Patient& p, const string& specialization = "") {
        Doctor* availableDoctor = findAvailableDoctor(specialization);
        if(! availableDoctor) {
            cout << "No available doctors for patient " << p.name << "\n";
            return;
        }

        Resource* availableResource = findAvailableResource();
        if(!availableResource) {
            cout << "No available resources for patient " << p.name << "\n";
            return;
        }

        updateDoctorAvailability(availableDoctor->id, false);
        updateResourceAvailability(availableResource->id, false);

        allocationQueue.enqueue(p, *availableDoctor, *availableResource);
    }

    void releasePatient() {
        if(allocationQueue.isEmpty()) {
            cout << "No patients to release.\n";
            return;
        }

        AllocNode* front = allocationQueue.getFront();

        updateDoctorAvailability(front->doctor.id, true);
        updateResourceAvailability(front->resource.id, true);

        allocationQueue.dequeue();
    }

    void displayAllocations() const {
        allocationQueue.display();
    }

    void saveDoctorsToFile(const string& filename) const {
        ofstream file(filename);
        if(!file) {
            cout << "Cannot open file " << filename << "\n";
            return;
        }

        file << "DoctorID,Name,Specialization,Available\n";

        for(const auto& doc : doctors) {
            file << doc.id << ","
                 << doc.name << ","
                 << doc.specialization << ","
                 << (doc.available ? "1" : "0") << "\n";
        }

        file.close();
        cout << "Doctors saved to " << filename << "\n";
    }

    void loadDoctorsFromFile(const string& filename) {
        ifstream file(filename);
        if(!file) {
            cout << "File " << filename << " not found.  Using default doctors.\n";
            return;
        }

        doctors.clear();
        string line;
        getline(file, line);

        while(getline(file, line)) {
            if(line.empty()) continue;

            stringstream ss(line);
            string temp;

            Doctor doc;
            getline(ss, temp, ','); doc.id = stoi(temp);
            getline(ss, doc.name, ',');
            getline(ss, doc.specialization, ',');
            getline(ss, temp); doc.available = (stoi(temp) == 1);

            doctors.push_back(doc);
            if(doc.id >= nextDoctorID) nextDoctorID = doc.id + 1;
        }

        file.close();
        if(! doctors.empty()) {
            cout << doctors. size() << " doctors loaded from " << filename << "\n";
        }
    }

    void saveResourcesToFile(const string& filename) const {
        ofstream file(filename);
        if(!file) {
            cout << "Cannot open file " << filename << "\n";
            return;
        }

        file << "ResourceID,Name,Available\n";

        for(const auto& res : resources) {
            file << res. id << ","
                 << res.name << ","
                 << (res.available ? "1" : "0") << "\n";
        }

        file.close();
        cout << "Resources saved to " << filename << "\n";
    }

    void loadResourcesFromFile(const string& filename) {
        ifstream file(filename);
        if(!file) {
            cout << "File " << filename << " not found. Using default resources.\n";
            return;
        }

        resources.clear();
        string line;
        getline(file, line);

        while(getline(file, line)) {
            if(line.empty()) continue;

            stringstream ss(line);
            string temp;

            Resource res;
            getline(ss, temp, ','); res.id = stoi(temp);
            getline(ss, res.name, ',');
            getline(ss, temp); res.available = (stoi(temp) == 1);

            resources.push_back(res);
            if(res.id >= nextResourceID) nextResourceID = res.id + 1;
        }

        file.close();
        if(!resources.empty()) {
            cout << resources.size() << " resources loaded from " << filename << "\n";
        }
    }

    void saveAllocationsToFile(const string& filename) const {
        allocationQueue.saveToFile(filename);
    }

    void loadAllocationsFromFile(const string& filename) {
        allocationQueue.loadFromFile(filename);
    }

    int getTotalDoctors() const {
        return doctors.size();
    }

    int getTotalResources() const {
        return resources.size();
    }

    int getActiveAllocations() const {
        return allocationQueue.getQueueSize();
    }
};

// ============================================================================
// MAIN PROGRAM - INTERACTIVE MENU
// ============================================================================

void displayMenu() {
    cout << "\n========== MODULE 2 - DOCTOR & RESOURCE ALLOCATION MENU ==========\n\n";
    cout << "DOCTOR OPERATIONS:\n";
    cout << "1. Add Doctor\n";
    cout << "2. Remove Doctor\n";
    cout << "3. Display All Doctors\n";
    cout << "\nRESOURCE OPERATIONS:\n";
    cout << "4. Add Resource\n";
    cout << "5. Remove Resource\n";
    cout << "6. Display All Resources\n";
    cout << "\nALLOCATION OPERATIONS:\n";
    cout << "7. Allocate Patient to Doctor\n";
    cout << "8. Release Patient (Treatment Complete)\n";
    cout << "9. Display Current Allocations\n";
    cout << "\nFILE OPERATIONS:\n";
    cout << "10. Save All Data\n";
    cout << "11. Load All Data\n";
    cout << "\nTEST & DEMO:\n";
    cout << "12. Run Demo\n";
    cout << "13. Exit\n";
}

void runDemo(DoctorManagement& doctorMgmt) {
    cout << "\n========== RUNNING MODULE 2 DEMO ==========\n\n";

    Patient p1 = {1, "Ali Hassan", 45, 1, "Heart Attack"};
    Patient p2 = {2, "Fatima Khan", 32, 2, "High Fever & Fracture"};
    Patient p3 = {3, "Ahmed Ali", 28, 3, "Minor Injury"};

    doctorMgmt.displayAllDoctors();
    doctorMgmt.displayAllResources();

    cout << "\n--- ALLOCATING PATIENTS ---\n\n";
    doctorMgmt.allocatePatientToDoctor(p1, "Cardiology");
    doctorMgmt.allocatePatientToDoctor(p2, "Emergency Medicine");
    doctorMgmt.allocatePatientToDoctor(p3);

    doctorMgmt. displayAllocations();

    cout << "\n--- RELEASING FIRST PATIENT ---\n";
    doctorMgmt. releasePatient();

    doctorMgmt.displayAllocations();

    cout << "\n========== DEMO STATISTICS ==========\n";
    cout << "Total Doctors: " << doctorMgmt.getTotalDoctors() << "\n";
    cout << "Total Resources: " << doctorMgmt.getTotalResources() << "\n";
    cout << "Active Allocations: " << doctorMgmt.getActiveAllocations() << "\n";

    cout << "\nDemo completed!\n";
}

int main() {
    DoctorManagement doctorMgmt;
    bool running = true;

    cout << "\n========== HOSPITAL EMERGENCY MANAGEMENT SYSTEM - MODULE 2 ==========\n";
    cout << "Doctor & Resource Allocation System\n";
    cout << "==================================================================\n\n";

    cout << "Initializing CSV files...\n";
    createDefaultCSVFiles();

    cout << "\nLoading data from files...\n";
    doctorMgmt.loadDoctorsFromFile("doctors.csv");
    doctorMgmt.loadResourcesFromFile("resources.csv");
    doctorMgmt.loadAllocationsFromFile("allocation_queue.csv");

    while(running) {
        displayMenu();
        cout << "\nEnter your choice:  ";
        int choice;
        cin >> choice;
        cin.ignore(10000, '\n');

        switch(choice) {
            case 1: {
                cout << "\n--- ADD NEW DOCTOR ---\n";
                string name, specialization;
                cout << "Enter doctor name: ";
                getline(cin, name);
                cout << "Enter specialization (Cardiology/Emergency Medicine/Orthopedics/General Surgery): ";
                getline(cin, specialization);
                doctorMgmt.addDoctor(name, specialization);
                break;
            }

            case 2: {
                cout << "\n--- REMOVE DOCTOR ---\n";
                int doctorID;
                cout << "Enter doctor ID to remove: ";
                cin >> doctorID;
                doctorMgmt. removeDoctor(doctorID);
                break;
            }

            case 3:
                doctorMgmt. displayAllDoctors();
                break;

            case 4: {
                cout << "\n--- ADD NEW RESOURCE ---\n";
                string resourceName;
                cout << "Enter resource name: ";
                getline(cin, resourceName);
                doctorMgmt.addResource(resourceName);
                break;
            }

            case 5: {
                cout << "\n--- REMOVE RESOURCE ---\n";
                int resourceID;
                cout << "Enter resource ID to remove: ";
                cin >> resourceID;
                doctorMgmt.removeResource(resourceID);
                break;
            }

            case 6:
                doctorMgmt.displayAllResources();
                break;

            case 7: {
                cout << "\n--- ALLOCATE PATIENT ---\n";
                Patient p;
                cout << "Enter patient ID: ";
                cin >> p. id;
                cin.ignore();
                cout << "Enter patient name: ";
                getline(cin, p.name);
                cout << "Enter patient age: ";
                cin >> p.age;
                cin.ignore();
                cout << "Enter symptoms:  ";
                getline(cin, p.symptoms);
                cout << "Enter priority (1=Critical, 2=Urgent, 3=Standard): ";
                cin >> p. priority;
                cin.ignore();
                cout << "Enter required doctor specialization (or press Enter for any): ";
                string spec;
                getline(cin, spec);

                doctorMgmt.allocatePatientToDoctor(p, spec);
                break;
            }

            case 8:
                doctorMgmt. releasePatient();
                break;

            case 9:
                doctorMgmt.displayAllocations();
                break;

            case 10: {
                cout << "\n--- SAVING DATA ---\n";
                doctorMgmt.saveDoctorsToFile("doctors.csv");
                doctorMgmt. saveResourcesToFile("resources.csv");
                doctorMgmt. saveAllocationsToFile("allocation_queue.csv");
                cout << "All data saved successfully!\n";
                break;
            }

            case 11: {
                cout << "\n--- LOADING DATA ---\n";
                doctorMgmt.loadDoctorsFromFile("doctors.csv");
                doctorMgmt.loadResourcesFromFile("resources.csv");
                doctorMgmt.loadAllocationsFromFile("allocation_queue.csv");
                cout << "All data loaded successfully!\n";
                break;
            }

            case 12:
                runDemo(doctorMgmt);
                break;

            case 13:
                cout << "\n--- SAVING DATA BEFORE EXIT ---\n";
                doctorMgmt.saveDoctorsToFile("doctors.csv");
                doctorMgmt. saveResourcesToFile("resources. csv");
                doctorMgmt.saveAllocationsToFile("allocation_queue.csv");

                cout << "\n========== SYSTEM STATISTICS ==========\n";
                cout << "Total Doctors: " << doctorMgmt.getTotalDoctors() << "\n";
                cout << "Total Resources:  " << doctorMgmt. getTotalResources() << "\n";
                cout << "Active Allocations: " << doctorMgmt.getActiveAllocations() << "\n";

                cout << "\nThank you for using Module 2!\n";
                cout << "Files saved to:\n";
                cout << "  - doctors.csv\n";
                cout << "  - resources. csv\n";
                cout << "  - allocation_queue.csv\n";
                cout << "Goodbye!\n\n";
                running = false;
                break;

            default:
                cout << "Invalid choice. Please try again.\n";
        }

        if(running && choice != 13) {
            cout << "\nPress Enter to continue...  ";
            cin.get();
        }
    }

    return 0;
}