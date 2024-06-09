#include <iostream>
#include <string>
#include <random>
#include <queue>
#include <fstream>
#include <sstream>

// User structure
struct Userresult {
    int id;
    std::string name;
    int points;
    Userresult* left;
    Userresult* right;
};

// Double linked list class
class DoubleLinkedList {
public:
    DoubleLinkedList() : head(nullptr), tail(nullptr) {}
    ~DoubleLinkedList() {
        Userresult* current = head;
        while (current != nullptr) {
            Userresult* next = current->right;
            delete current;
            current = next;
        }
    }

    void insert(Userresult* user) {
        if (head == nullptr) {
            head = tail = user;
            user->left = user->right = nullptr;
        }
        else if (user->points >= head->points) {
            user->right = head;
            user->left = nullptr;
            head->left = user;
            head = user;
        }
        else if (user->points <= tail->points) {
            user->right = nullptr;
            user->left = tail;
            tail->right = user;
            tail = user;
        }
        else {
            Userresult* current = head->right;
            while (current != nullptr && current->points >= user->points) {
                current = current->right;
            }
            user->right = current;
            user->left = current->left;
            current->left->right = user;
            current->left = user;
        }
    }

    void printTop(int n) {
        Userresult* current = head;
        int count = 0;
        while (current != nullptr && count < n) {
            std::cout << current->id << " - " << current->name << " - " << current->points << std::endl;
            current = current->right;
            ++count;
        }
    }

    Userresult* getHead() const {
        return head;
    }

private:
    Userresult* head;
    Userresult* tail;
};

// Function to build binary tree
Userresult* buildbinaryTree(Userresult** users, int start, int end) {
    if (start > end) {
        return nullptr;
    }

    int mid = (start + end) / 2;
    Userresult* root = users[mid];

    root->left = buildbinaryTree(users, start, mid - 1);
    root->right = buildbinaryTree(users, mid + 1, end);

    return root;
}

// Function to search user by ID in the binary tree
Userresult* searchUser(Userresult* root, int id) {
    if (root == nullptr) {
        return nullptr;
    }

    if (root->id == id) {
        return root;
    }

    Userresult* foundUser = searchUser(root->left, id);
    if (foundUser != nullptr) {
        return foundUser;
    }

    return searchUser(root->right, id);
}

// Function to read CSV file
int readCSV(const std::string& filename, Userresult users[], int maxSize) {
    std::ifstream file(filename);
    std::string line;
    int lineCount = 0;
    int userCount = 0;

    if (!file.is_open()) {
        std::cerr << "Failed to open file: " << filename << std::endl;
        return 0;
    }

    while (std::getline(file, line) && userCount < maxSize) {
        lineCount++;
        if (lineCount < 2) continue; // Skip the first line (header)

        std::istringstream ss(line);
        std::string token;
        if (std::getline(ss, token, ',')) users[userCount].id = std::stoi(token);
        if (std::getline(ss, token, ',')) users[userCount].name = token;
        if (std::getline(ss, token, ',')) users[userCount].points = std::stoi(token);

        users[userCount].left = nullptr;
        users[userCount].right = nullptr;

        userCount++;
    }

    file.close();
    return userCount;
}

// Function to display the tournament
void displayTournament(Userresult** users, int size, const int spacesPerLevel[], const int spacesBetweenNumbers[]) {
    int count = 0;
    int level = 0;

    while (count < size && level < 5) {
        // Print leading spaces for the current level
        for (int i = 0; i < spacesPerLevel[level]; ++i) {
            std::cout << " ";
        }

        // Print nodes at the current level
        for (int i = 0; i < (1 << level); ++i) {
            if (count < size) {
                std::cout << users[count]->id << " " << users[count]->name;
                ++count;
                if (count < size) {
                    // Print spaces between nodes at the current level
                    for (int j = 0; j < spacesBetweenNumbers[level]; ++j) {
                        std::cout << " ";
                    }
                }
            }
            else {
                break;
            }
        }

        std::cout << std::endl << std::endl; // Newline for the next level and extra line spacing
        ++level;
    }
}

int showtournament() {
    DoubleLinkedList list;
    std::string filename = "results.csv";
    const int maxUsers = 100;
    Userresult usersresult[maxUsers];

    int userCount = readCSV(filename, usersresult, maxUsers);

    for (int i = 0; i < userCount; ++i) {
        list.insert(&usersresult[i]);
    }

    // Display sorted list (top 31 only)
    std::cout << "Top 31 Sorted List:" << std::endl;
    list.printTop(31);
    std::cout << std::endl;

    // Build binary tree consists of 31 players because only the top 31 members should be displayed
    const int size = 31;
    Userresult* users[size];
    Userresult* current = list.getHead();
    for (int i = 0; i < size; ++i) {
        users[i] = current;
        current = current->right;
    }
    Userresult* root = buildbinaryTree(users, 0, size - 1);

    // Display tournament
    std::cout << "Tournament:" << std::endl;
    const int spacesPerLevel[5] = { 85, 70, 50, 35, 3 };
    const int spacesBetweenNumbers[5] = { 1, 25, 14, 6, 3 };
    displayTournament(users, size, spacesPerLevel, spacesBetweenNumbers);

    // Search for a user by ID
    int searchId;
    std::cout << "Enter user ID to search: ";
    std::cin >> searchId;
    Userresult* targetUser = searchUser(root, searchId);
    // When the user is found, display their name and points
    if (targetUser != nullptr) {
        std::cout << "User " << targetUser->name << " found with points: " << targetUser->points << std::endl;
    }
    else {
        std::cout << "User is not in the top 31" << std::endl;
    }

    return 0;
}