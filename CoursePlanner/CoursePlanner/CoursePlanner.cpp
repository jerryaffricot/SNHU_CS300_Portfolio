//============================================================================
// Name        : CoursePlanner.cpp
// Author      : Jerry Affricot
// Version     : 1.0
// Copyright   : Copyright © 2017 SNHU COCE
// Description : Hello World in C++, Ansi-style
//============================================================================


#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <vector>
#include <stdexcept>
#include <cctype>

using namespace std;

// structure to hold course information
struct Course {
    string courseNumber;

    string courseName;

    vector<string> preRequisites;
    
};

// forward declaration
void displayCourse(Course course);

vector<vector<string>> readFile(string csvPath);

// internal node structure for Binary Search Tree
struct Node {
    Course course;
    Node* left;
    Node* right;

    Node() {
        left = nullptr;
        right = nullptr;
    }

    Node(Course c) : Node() {
        course = c;
    }
};


class BinarySearchTree {

private:
    Node* root;

    void addNode(Node* node, Course course);
    void displayInOrder(Node* node);
    void deleteTree(Node* node);

public:
    BinarySearchTree();
    virtual ~BinarySearchTree();
    void Insert(Course course);
    void DisplayInOrder();
    Course Search(string courseNumber);
};

/**
* Default constructor
*/
BinarySearchTree::BinarySearchTree() {
    // initialize root to null
    root = nullptr;
}

/**
* Destructor
*/
void BinarySearchTree::deleteTree(Node* node) {
    if (node == nullptr) {
        return;
    }

    // delete left sub tree
    deleteTree(node->left);

    // delete right sub tree
    deleteTree(node->right);

    // delete node
    delete node;

}

BinarySearchTree::~BinarySearchTree() {
    deleteTree(root);
}


/**
* Loop through tree and insert new node at the correct place
* 
* @param node starting node to loop through
* @param course new couse information
*/
void BinarySearchTree::addNode(Node* node, Course course) {
    Node* currNode = node;

    while (currNode != nullptr) {

        if (course.courseNumber.compare(currNode->course.courseNumber) < 0) {
            if (currNode->left == nullptr) {  // if left node is empty
                // assign new course to left node 
                currNode->left = new Node(course);
                break;
            }
            // assign current node to left node
            currNode = currNode->left;
        }
        else {
            // if right node is empty
            if (currNode->right == nullptr) {
                // assign new course to right node                 
                currNode->right = new Node(course);
                break;
            }
            // assign current node to right node
            currNode = currNode->right;
        }
    }
}


void BinarySearchTree::Insert(Course course) {
    if (root == nullptr) {  // tree is empty
        root = new Node(course);
    } else {
        // add course to tree
        addNode(root, course);
    }
}

/**
* Print node(s) in order
* 
* @param node starting node
*/
void BinarySearchTree::displayInOrder(Node* node) {

    if (node == nullptr) {
        return;
    }

    // print left sub tree
    displayInOrder(node->left);

    // display course information
    displayCourse(node->course);

    // print right sub tree
    displayInOrder(node->right);
}

/**
* Print the tree in order 
*/
void BinarySearchTree::DisplayInOrder(){

    // recursively print tree in order
    displayInOrder(root);
}

/**
* Search for a specific course number
* 
* @param courseNumber course to be searched
*/
Course BinarySearchTree::Search(string courseNumber) {
    Course course;

    // start with root node
    Node* currNode = root;

    // loop through tree
    while (currNode != nullptr) {
        if (courseNumber.compare(currNode->course.courseNumber) == 0) {
            // if found, return course
            course = currNode->course;
            break;
        }
        // if courseNumber < current node->couse.courseNumber,
        // loop through left sub tree
        else if (courseNumber.compare(currNode->course.courseNumber) < 0) {
            currNode = currNode->left;
        }
        else { // loop through right sub tree
            currNode = currNode->right;
        }
    }

    return course;
}

/**
* Display course information to console
* 
* @param course struct containing course information
*/
void displayCourse(Course course) {

    cout << "Course Number: " << course.courseNumber;
    cout << ", Course Name: " << course.courseName;
    cout << ", Pre-Requisite(s): ";

    if (course.preRequisites.size() == 0) {
        cout << "None.";
    }
    else {
        cout << course.preRequisites.at(0);

        for (unsigned int i = 1; i < course.preRequisites.size(); ++i) {
            cout << ", " << course.preRequisites.at(i);
        }
    }
    cout << endl;
}


// error handling
struct FileParserError : public exception {
    string  msg;
    FileParserError(string err) {
        msg = string("File Parser: ").append(err);
    }
    const char* what() const throw () {
        return msg.c_str();
    }
};



/**
* split string by ',' delimiter
*/
vector<string> parseLine(string line) {
    vector<string> row;
    string delimiter = ",";
    int startIndex = 0;
    int endIndex = line.find(delimiter);

    while (endIndex != -1) {
        row.push_back(line.substr(startIndex, endIndex - startIndex));
        startIndex = endIndex + delimiter.size();
        endIndex = line.find(delimiter, startIndex);
    }
    row.push_back(line.substr(startIndex, endIndex - startIndex));
    return row;
}

/**
* Read and parse CSV file
* 
* @param csvPath path to csv file
* @return file content in a vector
*/
vector<vector<string>> readFile(string csvPath) {
    vector<vector<string>> fileContents;
    fstream csvFile;

    // open file for reading
    csvFile.open(csvPath, ios::in);

    if (csvFile.is_open()) {
        string line;
        // read data from file, line by line
        while (getline(csvFile, line)) {
            if (line != "") {
                // string string by ','
                vector<string> row = parseLine(line);
                fileContents.push_back(row);
            }
        }

        // close file
        csvFile.close();
    }
    else {
        throw FileParserError(string("Failed to open file: ").append(csvPath));
    }

    return fileContents;
}

void loadCourses(string csvPath, BinarySearchTree* bst) {
    vector<string> courseTitles;
    try {
        // read file file
        vector<vector<string>> fileContents = readFile(csvPath);

        // extract course titles
        for (unsigned int i = 0; i < fileContents.size(); ++i) {
            vector<string> row = fileContents.at(i);
            if (row.size() >= 2) {
                courseTitles.push_back(row.at(0));
            }
        }

        for (unsigned int i =0; i < fileContents.size(); ++i) {
            vector<string> row = fileContents.at(i);
            if (row.size() >= 2) {
                Course course;

                // course number
                course.courseNumber = row.at(0);

                // course name
                course.courseName = row.at(1);

                // add pre-requisite(s)
                for (unsigned int j = 2; j < row.size(); ++j) {
                    if (find(courseTitles.begin(), courseTitles.end(), row.at(j)) != courseTitles.end()) {
                        course.preRequisites.push_back(row.at(j));
                    }
                }

                // insert into binary search tree
                bst->Insert(course);
            }
        }
    }
    catch (FileParserError &err) {
        cerr << err.what() << endl;
    }


}



int main()
{
    // binary search tree to hold all course information
    BinarySearchTree* searchTree = new BinarySearchTree();

    int choice = 0;
    cout << "Welcome to the course planner" << endl;
    
    
    while (choice != 9) {
        string csvPath, courseNumber;
        Course course;
        string number;

        // print menu
        cout << "1. Load data structure." << endl;
        cout << "2. Print Course List." << endl;
        cout << "3. Print Course. " << endl;
        cout << "9. Exit" << endl;
        cout << "What would you like to do? ";
        cin >> choice;
        switch (choice)
        {
        case 1:
            // load course into binary tree
            searchTree = new BinarySearchTree();

            cout << "Enter file path: ";            
            cin.ignore();
            getline(cin, csvPath);

            loadCourses(csvPath, searchTree);
            break;
        case 2:
            // display course in ascending course number
            searchTree->DisplayInOrder();
            break;
        case 3:
            // search for a specific course number
            cout << "Enter course number: ";
            cin.ignore();
            getline(cin, courseNumber);

            // check if input is empty
            number = courseNumber;

            number.erase(remove_if(number.begin(), number.end(), isspace), number.end());

            if (number == "") {
                cout << "Course number cannot be empty" << endl;                
            }
            else {
                course = searchTree->Search(courseNumber);
                if (!course.courseNumber.empty()) {
                    displayCourse(course);
                }
                else {
                    cout << "Course number " << courseNumber << " not found." << endl;
                }
            }
            break;
            
        default:
            if (choice != 9) {
                cout << choice << " is not a valid choice." << endl;
            }
            break;
        }
    }
    cout << "Thank you for using the course planner!" << endl;
    return 0;
}

