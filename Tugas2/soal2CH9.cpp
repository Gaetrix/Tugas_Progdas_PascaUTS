#include <iostream>
#include <iomanip>
#include <string>
using namespace std;

const int NUM_STUDENTS = 20;

struct studentType {
    string studentFName;
    string studentLName;
    int testScore;
    char grade;
};

// Function prototypes
void getData(studentType students[], int size);
void assignGrade(studentType students[], int size);
int findHighestScore(const studentType students[], int size);
void printHighest(const studentType students[], int size, int highestScore);
void printAll(const studentType students[], int size);

int main() {
    studentType students[NUM_STUDENTS];
    int highestScore;

    // Read student data
    getData(students, NUM_STUDENTS);

    // Assign grades
    assignGrade(students, NUM_STUDENTS);

    // Find the highest score
    highestScore = findHighestScore(students, NUM_STUDENTS);

    // Print all students
    cout << "\nList of Students with Grades:\n";
    cout << "-----------------------------------------\n";
    printAll(students, NUM_STUDENTS);

    // Print highest scorers
    cout << "\nHighest Test Score: " << highestScore << endl;
    cout << "Student(s) with the highest score:\n";
    printHighest(students, NUM_STUDENTS, highestScore);

    return 0;
}

// Function to read students’ data
void getData(studentType students[], int size) {
    for (int i = 0; i < size; i++) {
        cout << "Enter first name, last name, and test score for student " << i + 1 << ": ";
        cin >> students[i].studentFName >> students[i].studentLName >> students[i].testScore;
    }
}

// Function to assign grade based on test score
void assignGrade(studentType students[], int size) {
    for (int i = 0; i < size; i++) {
        int score = students[i].testScore;
        if (score >= 90)
            students[i].grade = 'A';
        else if (score >= 80)
            students[i].grade = 'B';
        else if (score >= 70)
            students[i].grade = 'C';
        else if (score >= 60)
            students[i].grade = 'D';
        else
            students[i].grade = 'F';
    }
}

// Function to find the highest score
int findHighestScore(const studentType students[], int size) {
    int highest = students[0].testScore;
    for (int i = 1; i < size; i++) {
        if (students[i].testScore > highest)
            highest = students[i].testScore;
    }
    return highest;
}

// Function to print students with the highest score
void printHighest(const studentType students[], int size, int highestScore) {
    for (int i = 0; i < size; i++) {
        if (students[i].testScore == highestScore) {
            cout << left << setw(15)
                 << (students[i].studentLName + ", " + students[i].studentFName)
                 << " Score: " << students[i].testScore
                 << " Grade: " << students[i].grade << endl;
        }
    }
}

// Function to print all students’ data (one line per student)
void printAll(const studentType students[], int size) {
    cout << left << setw(20) << "Name"
         << setw(10) << "Score"
         << setw(10) << "Grade" << endl;
    cout << "-----------------------------------------\n";

    for (int i = 0; i < size; i++) {
        cout << left << setw(20)
             << (students[i].studentLName + ", " + students[i].studentFName)
             << setw(10) << students[i].testScore
             << setw(10) << students[i].grade << endl;
    }
}
