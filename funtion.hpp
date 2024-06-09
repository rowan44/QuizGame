#include <iostream>
#include <ctime>
#include <fstream>
#include <string>
#include <sstream>
using namespace std;


void printFileContent(const string& filename) {
    std::ifstream file(filename);
    std::string line;

    if (file.is_open()) {
        while (getline(file, line)) {
            cout << line << std::endl;
        }
        file.close();
    }
    else {
        cout << "Unable to open file: " << filename << std::endl;
    }
}
void writeUserToCSV(const string& filename, const string& username) {
    ifstream inFile(filename);
    string line;
    int maxUserID = 0; // Start from 0

    // Find the highest user ID in the file
    while (getline(inFile, line)) {
        size_t pos = line.find(',');
        int userID = stoi(line.substr(0, pos));
        if (userID > maxUserID) {
            maxUserID = userID;
        }
    }
    inFile.close();

    int newUserID = maxUserID + 1; // The new user ID is one more than the highest user ID

    ofstream outFile(filename, ios::app); // Open file in append mode

    if (outFile.is_open()) {
        outFile << newUserID << "," << username << "\n"; // Write newUserID and username to a new line in the file
        outFile.close();
    }
    else {
        cout << "Unable to open file: " << filename << endl;
    }
}
void deleteUser(const string& filename, const string& username) {
    ifstream file(filename);
    ofstream temp("temp.csv");

    if (!file.is_open() || !temp.is_open()) {
        std::cerr << "Could not open the file." << std::endl;
        return;
    }

    std::string line;
    while (std::getline(file, line)) {
        size_t pos = line.find(',');
        std::string user = (pos == std::string::npos) ? line : line.substr(0, pos);
        if (user != username) {
            temp << line << "\n";
        }
    }

    file.close();
    temp.close();

    // Replace the original file with the temporary file
    remove(filename.c_str());
    rename("temp.csv", filename.c_str());
}












