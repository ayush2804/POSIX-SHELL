#include <iostream>
#include <string>
#include <vector>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <pwd.h>
#include <grp.h>
#include <ctime>
#include <cstring>
using namespace std;

const string PROMPT = "@:>";

string getCurrentDirectory() {
    char cwd[PATH_MAX];
    if (getcwd(cwd, sizeof(cwd)) != nullptr) {
        return string(cwd);
    } else {
        perror("getcwd");
        exit(EXIT_FAILURE);
    }
}

// Global constant for the current working directory
const string CURRENT_DIRECTORY = getCurrentDirectory();
string PREVIOUS_DIRECTORY = getCurrentDirectory();

// Function to tokenize user input
vector<string> tokenizeInput(const string& input) {
    vector<string> tokens;
    int start = 0, end = 0;

    while ((end = input.find(" ", start)) != string::npos) {
        if (end != start) {
            tokens.push_back(input.substr(start, end - start));
        }
        start = end + 1;
    }

    if (start != input.length()) {
        tokens.push_back(input.substr(start));
    }

    return tokens;
}

// Function to execute the ls command
void executeLsCommand(const vector<string>& tokens) {
    bool showHidden = false;
    bool longFormat = false;
    string directory = ".";

    for (size_t i = 1; i < tokens.size(); ++i) {
        if (tokens[i] == "-a") {
            showHidden = true;
        } else if (tokens[i] == "-l") {
            longFormat = true;
        } else if (tokens[i].find("-") == 0) {
            cerr << "Invalid option: " << tokens[i] << endl;
            return;
        } else {
            directory = tokens[i];
        }
    }

    if (directory == "~") {
        directory = getenv("HOME");
    }

    if (chdir(directory.c_str()) != 0) {
        perror("cd");
        return;
    }

    char* prevDir = getcwd(nullptr, 0);
    PREVIOUS_DIRECTORY = string(prevDir);
    free(prevDir);

    DIR* dir;
    struct dirent* entry;

    if ((dir = opendir(".")) != nullptr) {
        while ((entry = readdir(dir)) != nullptr) {
            string fileName = entry->d_name;

            if (!showHidden && fileName[0] == '.') {
                continue; // Skip hidden files
            }

            if (longFormat) {
                struct stat fileStat;
                if (stat(fileName.c_str(), &fileStat) == 0) {
                    struct passwd* pw = getpwuid(fileStat.st_uid);
                    struct group* gr = getgrgid(fileStat.st_gid);

                    // Display file permissions in a human-readable format
                    string permissions = "";
                    permissions += (S_ISDIR(fileStat.st_mode)) ? "d" : "-";
                    permissions += (fileStat.st_mode & S_IRUSR) ? "r" : "-";
                    permissions += (fileStat.st_mode & S_IWUSR) ? "w" : "-";
                    permissions += (fileStat.st_mode & S_IXUSR) ? "x" : "-";
                    permissions += (fileStat.st_mode & S_IRGRP) ? "r" : "-";
                    permissions += (fileStat.st_mode & S_IWGRP) ? "w" : "-";
                    permissions += (fileStat.st_mode & S_IXGRP) ? "x" : "-";
                    permissions += (fileStat.st_mode & S_IROTH) ? "r" : "-";
                    permissions += (fileStat.st_mode & S_IWOTH) ? "w" : "-";
                    permissions += (fileStat.st_mode & S_IXOTH) ? "x" : "-";

                    cout << permissions << " ";
                    cout << fileStat.st_nlink << " ";
                    cout << pw->pw_name << " ";
                    cout << gr->gr_name << " ";
                    cout << fileStat.st_size << " ";
                    struct tm* timeinfo = localtime(&fileStat.st_mtime);
                    char timeString[80];
                    strftime(timeString, sizeof(timeString), "%b %d %H:%M", timeinfo);
                    cout << timeString << " ";
                }
            }

            cout << fileName << " ";
        }
        cout << endl;
        closedir(dir);
    } else {
        perror("opendir");
    }
}

// Function to execute a command
void executeCommand(const vector<string>& tokens) {
    if (tokens.empty()) {
        return;
    }

    string command = tokens[0];

    if (command == "cd") {
        if (tokens.size() < 2) {
            cerr << "Usage: cd <directory> or cd ~ or cd -" << endl;
        } else {
            string directory = tokens[1];

            if (directory == "~") {
                directory = getenv("HOME");
            } else if (directory == "-") {
                directory = PREVIOUS_DIRECTORY;
            }

            char* prevDir = getcwd(nullptr, 0);
            if (chdir(directory.c_str()) != 0) {
                perror("cd");
            } else {
                PREVIOUS_DIRECTORY = string(prevDir);
                free(prevDir);
            }
        }
    } else if (command == "echo") {
        for (int i = 1; i < tokens.size(); ++i) {
            cout << tokens[i] << ' ';
        }
        cout << endl;
    } else if (command == "pwd") {
        cout << getCurrentDirectory() << endl;
    } else if (command == "ls") {
        executeLsCommand(tokens);
    }
}

int main() {
    char* username = getenv("USER");
    char hostname[256];
    gethostname(hostname, sizeof(hostname));
    char cwd[1024];

    while (true) {
        if (getcwd(cwd, sizeof(cwd)) == nullptr) {
            perror("getcwd");
            exit(EXIT_FAILURE);
        }

        cout << username << "@" << hostname << ":" << cwd << PROMPT;
        string input;
        getline(cin, input);

        // Tokenize and execute the input
        vector<string> tokens = tokenizeInput(input);
        executeCommand(tokens);
    }

    return 0;
}
