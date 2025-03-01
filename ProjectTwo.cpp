/***************************************************************
 * ProjectTwo.cpp
 * CS 300 - ABCU Advising Assistance Program
 *
 * This program reads a CSV file containing course information, stores the data in a binary search tree (BST), and provides a menu-driven interface for:
 *   1) Loading the data,
 *   2) Printing an alphanumeric course list,
 *   3) Printing details for a specific course (title + prerequisites),
 *   9) Exiting the program.
 *
 * Author: Christopher Davidson
 * Date: 2/22/25
 ***************************************************************/

#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <fstream>
#include <algorithm>
#include <cctype>
// For numeric_limits
#include <limits>

using namespace std;

/***************************************************************
 * Course Struct
 *
 * Represents a single course, storing:
 * - courseNumber
 * - courseName
 * - prerequisites (list of courseNumbers)
 ***************************************************************/
struct Course {
    string courseNumber;
    string courseName;
    vector<string> prerequisites;

    // Helper method to print just the course number and course name
    void printCourseBasic() const {
        cout << courseNumber << ", " << courseName << endl;
    }
};

/***************************************************************
 * Node Struct
 *
 * A node in the binary search tree (BST). It contains one Course, plus pointers to its left and right child nodes.
 ***************************************************************/
struct Node {
    // The actual course data
    Course course;
    // Pointer to left child in the BST
    Node* left;
    // Pointer to right child in the BST
    Node* right;

    // Constructor for convenience
    Node(const Course& c) : course(c), left(nullptr), right(nullptr) {}
};

/***************************************************************
 * CourseBST Class
 *
 * A binary search tree keyed by courseNumber in alphanumeric order. Provides:
 *   - insert (Course)
 *   - printAll() (in-order traversal)
 *   - search (courseNumber)
 *   - destructor for cleanup
 ***************************************************************/
class CourseBST {
private:
    // The root node of the BST
    Node* root;

    // Recursively inserts a course into the BST by courseNumber
    void addNode(Node*& node, const Course& course) {
        // If position is empty, place the new course here
        if (!node) {
            node = new Node(course);
        }
        // Or compare courseNumber to decide left or right subtree
        else if (course.courseNumber < node->course.courseNumber) {
            addNode(node->left, course);
        }
        else {
            addNode(node->right, course);
        }
    }

    // Recursively performs an in order traversal and prints each course as the user visits it
    void inOrder(Node* node) const {
        // Base case: null node
        if (!node) return;
        // Traverse left subtree
        inOrder(node->left);
        // Print current node
        node->course.printCourseBasic();
        // Traverse right subtree
        inOrder(node->right);
    }

    // Recursively searches for a course by its courseNumber
    Course* searchNode(Node* node, const string& courseNumber) const {
        // If node is null, the course doesn't exist in this subtree
        if (!node) return nullptr;

        // Compare target courseNumber with current node's courseNumber
        if (node->course.courseNumber == courseNumber) {
            return &(node->course);
        }
        else if (courseNumber < node->course.courseNumber) {
            return searchNode(node->left, courseNumber);
        }
        else {
            return searchNode(node->right, courseNumber);
        }
    }

    // Recursively destroys all nodes to free memory
    void destroy(Node* node) {
        // Base case
        if (!node) return;
        destroy(node->left);
        destroy(node->right);
        delete node;
    }

public:
    // Constructor initializes an empty BST
    CourseBST() : root(nullptr) {}

    // Destructor calls recursive destroy to free all nodes
    ~CourseBST() {
        destroy(root);
    }

    // Inserts a new course into the BST
    void insert(const Course& course) {
        addNode(root, course);
    }

    // Prints all courses in sorted order by courseNumber
    void printAll() const {
        inOrder(root);
    }

    // Returns a pointer to the course if found, otherwise nullptr
    Course* search(const string& courseNumber) const {
        return searchNode(root, courseNumber);
    }
};

/***************************************************************
 * Utility: toUpperTrim
 *
 * Trims leading/trailing whitespace, then converts string to uppercase. Ensures consistent matching of course numbers like "csci100" -> "CSCI100".
 ***************************************************************/
string toUpperTrim(const string& str) {
    // Find first and last non-whitespace characters
    size_t start = str.find_first_not_of(" \t\r\n");
    size_t end = str.find_last_not_of(" \t\r\n");

    // Extract substring if the user found real content
    string trimmed;
    if (start != string::npos && end != string::npos) {
        trimmed = str.substr(start, end - start + 1);
    }

    // Convert to uppercase
    for (auto& c : trimmed) {
        c = toupper(static_cast<unsigned char>(c));
    }
    return trimmed;
}

/***************************************************************
 * loadCourses
 *
 * Reads a CSV file (named filename) line by line, splitting each line on commas to get:
 *    [courseNumber, courseName, prereq1, prereq2]
 *
 * Then it creates a Course object and inserts it into the BST.
 * If the file can't be opened, an error is displayed.
 ***************************************************************/
void loadCourses(const string& filename, CourseBST& bst) {
    ifstream file(filename);
    if (!file.is_open()) {
        // If file isn't found or can't be opened, print an error
        cout << "ERROR: Could not open file: " << filename << endl;
        return;
    }

    cout << "Loading courses from " << filename << "..." << endl;

    string line;
    while (getline(file, line)) {
        // Skip any blank lines (just in case)
        if (line.empty()) continue;

        // Split the line by commas to get tokens
        vector<string> tokens;
        string token;
        stringstream ss(line);

        while (getline(ss, token, ',')) {
            tokens.push_back(token);
        }

        // Expect at least 2 tokens: courseNumber and courseName
        if (tokens.size() < 2) {
            cout << "WARNING: Invalid course line (skipped): " << line << endl;
            continue;
        }

        // Create a Course object from these tokens
        Course course;
        course.courseNumber = toUpperTrim(tokens[0]);
        course.courseName = tokens[1];

        // Any remaining tokens are prerequisites
        for (size_t i = 2; i < tokens.size(); ++i) {
            course.prerequisites.push_back(toUpperTrim(tokens[i]));
        }

        // Insert the course into the BST
        bst.insert(course);
    }

    file.close();
    cout << "Courses loaded into data structure." << endl;
}

/***************************************************************
 * printCourseInfo
 *
 * Prompts the user for a courseNumber, searches for that course in the BST, then prints its name and prerequisites (if any).
 * If a prerequisite is also in the BST, prints its name too.
 ***************************************************************/
void printCourseInfo(const CourseBST& bst) {
    cout << "What course do you want to know about? ";
    string userInput;
    getline(cin, userInput);
    string courseKey = toUpperTrim(userInput);

    // Search for the course in the BST
    Course* course = bst.search(courseKey);
    if (!course) {
        // If the course can't be found, inform the user
        cout << "Course not found." << endl;
        return;
    }

    // Print the main course info: number + title
    cout << course->courseNumber << ", " << course->courseName << endl;

    // If this course has prerequisites, display them
    if (!course->prerequisites.empty()) {
        cout << "Prerequisites: ";
        bool firstPrinted = false;

        // For each prerequisite ID, check if it's in the BST to get the full name
        for (const auto& prereqID : course->prerequisites) {
            if (firstPrinted) {
                cout << ", ";
            }
            else {
                firstPrinted = true;
            }

            Course* prereqCourse = bst.search(prereqID);
            if (prereqCourse) {
                // Print "CSCI101: Introduction to Programming in C++"
                cout << prereqCourse->courseNumber << ": " << prereqCourse->courseName;
            }
            else {
                // If not found, show only the ID
                cout << prereqID << ": None Required";
            }
        }
        cout << endl;
    }
    else {
        // This course has no prerequisites
        cout << "Prerequisites: None" << endl;
    }
}

/***************************************************************
 * main
 *
 * Main entry point. Presents a menu-driven UI that allows users to:
 *   - Load courses from a file (Option 1)
 *   - Print all courses in sorted order (Option 2)
 *   - Search for a single course (Option 3)
 *   - Exit (Option 9)
 *
 * If the user attempts to print or search before loading, they are prompted to load data first.
 ***************************************************************/
int main() {
    // Chosen data structure (BST)
    CourseBST bst;
    // Tracks whether data has been loaded
    bool loaded = false; 

    cout << "Welcome to the course planner." << endl << endl;

    int choice = 0;
    while (choice != 9) {
        // Display menu options
        cout << "  1. Load Data Structure." << endl;
        cout << "  2. Print Course List." << endl;
        cout << "  3. Print Course." << endl;
        cout << "  9. Exit" << endl;
        cout << endl << "What would you like to do? ";

        cin >> choice;

        // Check if cin failed to parse an integer
        if (!cin) {
            // Clear error flags
            cin.clear();
            // Discard any leftover input up to a newline (prevents infinite loop)
            cin.ignore(numeric_limits<streamsize>::max(), '\n');

            // Show a single error message
            cout << "Input is not a valid option." << endl << endl;

            // Go back to top of the while loop (re-display the menu)
            continue;
        }

        // Clear leftover newline from input buffer
        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        // Respond to user's choice
        switch (choice) {
        // Added tools to improve user experience
        case 1: {
            cout << "Enter the file name to load (case-insensitive, with or without .csv): ";
            string inputName;
            getline(cin, inputName);

            // The *correct base name* in exact case
            const string correctBase = "CS 300 ABCU_Advising_Program_Input";
            // Use all-caps of the base for easy ignoring-case checks
            const string correctBaseCaps = "CS 300 ABCU_ADVISING_PROGRAM_INPUT";

            // Convert user input to uppercase for comparison
            string inputCaps = inputName;
            for (auto& c : inputCaps) {
                c = toupper(static_cast<unsigned char>(c));
            }

            // STEP 1: Check if the user typed .csv at the end (in any case).
            //         If yes, remove it before comparing to the correct base.
            bool hasCsvExtension = false;
            if (inputCaps.size() >= 4 &&
                inputCaps.substr(inputCaps.size() - 4) == ".CSV")
            {
                hasCsvExtension = true;
                // Remove the last 4 characters (".csv") so the base is left
                inputCaps = inputCaps.substr(0, inputCaps.size() - 4);
            }

            // STEP 2: Compare what's left to the expected base name, ignoring case
            if (inputCaps == correctBaseCaps) {
                // If it matches ignoring case, accept it.
                // Build the final filename in the exact case desired:
                string finalFilename = correctBase + ".csv";

                // Inform the user to how their input is being resolved
                cout << "Using file: " << finalFilename << endl;

                // Now actually load courses from that file
                loadCourses(finalFilename, bst);
                loaded = true;
            }
            else {
                // The input didn't match "CS 300 ABCU_Advising_Program_Input" ignoring case
                cout << "ERROR: The file name does not match \""
                    << correctBase << "\" (ignoring case)." << endl;
                cout << "Please re-check your spelling and try again." << endl;
            }

            break;
        }
        case 2:
            if (!loaded) {
                // If no data loaded yet, ask user to load first
                cout << "Please load courses before printing the list." << endl;
            }
            else {
                // Print all courses in sorted (in-order) order
                cout << "Here is the course schedule:" << endl << endl;
                bst.printAll();
                cout << endl;
            }
            break;
        case 3:
            if (!loaded) {
                // If no is data loaded, it is impossible to search
                cout << "Please load courses before searching for a course." << endl;
            }
            else {
                printCourseInfo(bst);
                cout << endl;
            }
            break;
        case 9:
            // Exit the loop => end program
            cout << "Thank you for using the course planner!" << endl;
            cout << "Press ENTER to close the program..." << endl;

            // Clear any leftover characters from the buffer
            cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            // Wait for the user to press ENTER
            cin.get();

            // After the user presses ENTER the program ends.
            break;
        default:
            // Invalid menu option
            cout << choice << " is not a valid option." << endl << endl;
            break;
        }
    }

    // End of program
    return 0;  
}
