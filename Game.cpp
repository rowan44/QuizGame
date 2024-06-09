#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <algorithm>
#include <random>
#include <ctime>
#include <stdexcept>
#include <cstdio>
#include "funtion.hpp"
#include "tournament.hpp"

using namespace std;

// Define a maximum number of users
const int MAX_USERS = 100;

// Struct to store the question data
struct Question {
    int number;
    std::string question;
    std::string options[4];
    char correctAnswer;
    int points;
};

// Struct to store user data
struct User {
    int id;
    std::string name;
    int score;
};

// Node for the linked list to store discarded questions
struct DiscardedQuestionNode {
    Question question;
    DiscardedQuestionNode* next;
};

// Function to add a question to the discarded list
void addToDiscardList(DiscardedQuestionNode*& head, const Question& question) {
    DiscardedQuestionNode* newNode = new DiscardedQuestionNode{ question, nullptr };
    if (!head) {
        head = newNode;
    }
    else {
        DiscardedQuestionNode* temp = head;
        while (temp->next) {
            temp = temp->next;
        }
        temp->next = newNode;
    }
}

// Function to remove a question from the discarded list and return it
Question removeFromDiscardList(DiscardedQuestionNode*& head) {
    if (!head) {
        throw std::runtime_error("No questions in the discard pile");
    }

    DiscardedQuestionNode* temp = head;
    Question question = temp->question;
    head = head->next;
    delete temp;
    return question;
}

// Function to check if discard list is empty
bool isDiscardListEmpty(DiscardedQuestionNode* head) {
    return head == nullptr;
}

// Function to load questions from a CSV file
Question* loadQuestions(const std::string& filename, int& questionCount) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Could not open the file: " << filename << std::endl;
        questionCount = 0;
        return nullptr;
    }

    std::string line;
    std::getline(file, line); // Skip the header

    Question* questions = new Question[100]; // Assume max 100 questions initially
    int capacity = 100;
    questionCount = 0;

    while (std::getline(file, line)) {
        if (questionCount == capacity) {
            // Resize array if capacity is reached
            capacity *= 2;
            Question* newQuestions = new Question[capacity];
            for (int i = 0; i < questionCount; ++i) {
                newQuestions[i] = questions[i];
            }
            delete[] questions;
            questions = newQuestions;
        }

        std::stringstream ss(line);
        std::string item;
        Question& q = questions[questionCount];

        try {
            std::getline(ss, item, ',');
            q.number = std::stoi(item);
            std::getline(ss, q.question, ',');
            for (int i = 0; i < 4; ++i) {
                std::getline(ss, q.options[i], ',');
            }
            std::getline(ss, item, ',');
            q.correctAnswer = item[0];
            std::getline(ss, item, ',');
            q.points = std::stoi(item);
        }
        catch (const std::invalid_argument& e) {
            continue; // Skip invalid question
        }
        catch (const std::out_of_range& e) {
            continue; // Skip invalid question
        }

        ++questionCount;
    }

    file.close();
    return questions;
}

// Function to check if a file exists
bool fileExists(const std::string& filename) {
    std::ifstream file(filename);
    return file.is_open();
}

// Function to load the list of users from player.csv
int loadUsers(const std::string& filename, User* users) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Could not open the file: " << filename << std::endl;
        return 0;
    }

    int count = 0;
    std::string line;
    while (std::getline(file, line) && count < MAX_USERS) {
        std::stringstream ss(line);
        std::string idStr, name;
        std::getline(ss, idStr, ',');
        std::getline(ss, name);
        try {
            users[count].id = std::stoi(idStr);
        }
        catch (const std::invalid_argument& e) {
            continue; // Skip invalid user data
        }
        catch (const std::out_of_range& e) {
            continue; // Skip invalid user data
        }
        users[count].name = name;
        users[count].score = 0; // Initialize score to 0
        ++count;
    }

    file.close();
    return count;
}

// Function to save the results to a CSV file
void saveResults(User* users, int userCount, const std::string& filename) {
    std::ofstream outFile(filename);
    if (!outFile.is_open()) {
        std::cerr << "Could not open the file: " << filename << std::endl;
        return;
    }

    outFile << "ID,Name,Score\n";
    for (int i = 0; i < userCount; ++i) {
        outFile << users[i].id << "," << users[i].name << "," << users[i].score << "\n";
    }

    outFile.close();
}

// Function to quiz a user
void quizUser(User* users, int userCount, Question* questions, int questionCount, DiscardedQuestionNode*& discardList) {
    std::srand(std::time(nullptr));
    int roundsPerUser = 3;

    for (int round = 0; round < roundsPerUser; ++round) {
        for (int i = 0; i < userCount; ++i) {
            User& currentUser = users[i];
            std::cout << "Question for " << currentUser.name << " (ID: " << currentUser.id << "):\n";

            Question selectedQuestion;
            char choice;
            if (discardList) {
                std::cout << "Do you want to answer a skipped question? (Y/N): ";
                std::cin >> choice;
                if (choice == 'Y' || choice == 'y') {
                    try {
                        selectedQuestion = removeFromDiscardList(discardList);
                    }
                    catch (const std::runtime_error& e) {
                        std::cerr << e.what() << std::endl;
                        continue; // Skip to next iteration if there are no discarded questions
                    }
                }
                else {
                    int questionIndex = std::rand() % questionCount;
                    selectedQuestion = questions[questionIndex];
                }
            }
            else {
                int questionIndex = std::rand() % questionCount;
                selectedQuestion = questions[questionIndex];
            }

            std::cout << selectedQuestion.number << ". " << selectedQuestion.question << std::endl;
            std::cout << "A. " << selectedQuestion.options[0] << std::endl;
            std::cout << "B. " << selectedQuestion.options[1] << std::endl;
            std::cout << "C. " << selectedQuestion.options[2] << std::endl;
            std::cout << "D. " << selectedQuestion.options[3] << std::endl;

            std::cout << "Enter your answer (A, B, C, D) or S to skip: ";
            char answer;
            std::cin >> answer;

            if (answer == 'S' || answer == 's') {
                addToDiscardList(discardList, selectedQuestion);
            }
            else if (answer == selectedQuestion.correctAnswer) {
                currentUser.score += selectedQuestion.points;
                std::cout << "Yeah you got it! You earned " << selectedQuestion.points << " points.\n";
            }
            else {
                std::cout << "Sorry, wrong answer. Better luck next time.\n";
            }
        }
    }

    std::cout << "\nResults:\n";
    for (int i = 0; i < userCount; ++i) {
        std::cout << users[i].name << " (ID: " << users[i].id << "): " << users[i].score << " points\n";
    }

    saveResults(users, userCount, "results.csv");
}

int play() {
    Question* questions;
    int questionCount;

    // Load questions from the CSV file
    questions = loadQuestions("C:\\Users\\TCM\\source\\repos\\Project1\\Project1\\questions.csv", questionCount);
    if (questions == nullptr) {
        return 1;
    }

    DiscardedQuestionNode* discardList = nullptr;

    // User initialization
    User users[MAX_USERS];
    int userCount = loadUsers("C:\\Users\\TCM\\source\\repos\\Project1\\Project1\\player.csv", users);

    // Sort users by ID to start with the user having ID 1
    std::sort(users, users + userCount, [](const User& a, const User& b) {
        return a.id < b.id;
        });

    // Quiz the users
    quizUser(users, userCount, questions, questionCount, discardList);

    // Clean up
    delete[] questions;
    while (discardList) {
        DiscardedQuestionNode* temp = discardList;
        discardList = discardList->next;
        delete temp;
    }
}

void startgame() {
    cout << "============================Welcome to APU Card Game============================" << endl;
    cout << "1. Rules " << endl;
    cout << "2. List of Player" << endl;
    cout << "3. Player Register" << endl;
    cout << "4. Start Game" << endl;
    cout << "5. Tournament" << endl;
    cout << "6. Delete Player" << endl;
    cout << "7. Exit" << endl;
    cout << "================================================================================" << endl;
    int option;
    int n;
    int questionCount = 0;
    int numPlayers;

    string username;
    cout << "Please Enter your option (1-7) : ";
    cin >> option;
    switch (option) {
    case 1:
        printFileContent("Rules.csv");
        startgame();
        break;
    case 2:
        printFileContent("player.csv");
        startgame();
        break;
    case 3:

        cout << "How many player wants to register ?" << endl;
        cin >> n;
        for (int i = 0; i < n; i++) {
            cout << "Please enter your name : ";
            cin >> username;
            writeUserToCSV("player.csv", username);
        }
        startgame();
        break;
    case 4:
        play();
        break;
    case 5:
        showtournament();
    case 6:
        cout << "Enter the ID of player you want to remove :";
        cin >> username;
        deleteUser("player.csv", username);
        startgame();
        break;
    case 7:
        break;

    default:
        cout << "Invalid Input ..........." << endl;
        startgame();
    }


}


int main() {
	startgame();
	return 0;
}
