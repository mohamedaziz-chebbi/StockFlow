// Programmer: Mohamed Aziz Chebbi
#include "Utils.h"
#include <iostream>
#include <cstdlib>  
#include <cctype>   
#include <ctime>     
using namespace std;

string trim(const string& text) {
    int start = 0;
    int end = (int)text.length() - 1;
    while (start <= end && (text[start] == ' ' || text[start] == '\t' || text[start] == '\r'))
        start++;
    while (end >= start && (text[end] == ' ' || text[end] == '\t' || text[end] == '\r'))
        end--;
    return text.substr(start, end - start + 1);
}

string toLower(string text) {
    for (int i = 0; i < (int)text.length(); i++)
        text[i] = (char)tolower((unsigned char)text[i]);
    return text;
}

string shorten(const string& text, int maxLength) {
    if ((int)text.length() <= maxLength)
        return text;
    return text.substr(0, maxLength - 2) + "..";
}

int splitLine(const string& line, char separator, string fields[], int maxFields) {
    int count = 0;
    string current = "";
    for (int i = 0; i < (int)line.length(); i++) {
        if (line[i] == separator) {
            if (count < maxFields)
                fields[count] = current;
            count++;
            current = "";
        } else {
            current += line[i];
        }
    }
    if (count < maxFields)
        fields[count] = current;
    count++;
    return count;
}

bool isInteger(const string& text) {
    int length = (int)text.length();
    if (length == 0 || length > 9)          
        return false;
    int start = 0;
    if (text[0] == '-')
        start = 1;
    if (start == length)                  
        return false;
    for (int i = start; i < length; i++)
        if (!isdigit((unsigned char)text[i]))
            return false;
    return true;
}

bool isDecimal(const string& text) {
    int length = (int)text.length();
    if (length == 0 || length > 12)
        return false;
    int start = 0;
    if (text[0] == '-')
        start = 1;
    int digits = 0;
    int dots = 0;
    for (int i = start; i < length; i++) {
        if (isdigit((unsigned char)text[i]))
            digits++;
        else if (text[i] == '.')
            dots++;
        else
            return false;
    }
    return digits > 0 && dots <= 1;
}

int toInt(const string& text) {
    return atoi(text.c_str());
}

double toDouble(const string& text) {
    return atof(text.c_str());
}


string readLine(const string& prompt) {
    string line;
    cout << prompt;
    if (!getline(cin, line)) {
        cout << "\nInput closed. The program will stop.\n";
        exit(0);
    }
    return trim(line);
}

int readInt(const string& prompt, int minValue, int maxValue) {
    while (true) {
        string text = readLine(prompt);
        if (!isInteger(text)) {
            cout << "Error: please enter a whole number.\n";
            continue;
        }
        int value = toInt(text);
        if (value < minValue || value > maxValue) {
            cout << "Error: the value must be between " << minValue
                 << " and " << maxValue << ".\n";
            continue;
        }
        return value;
    }
}

double readDouble(const string& prompt, double minValue, double maxValue) {
    while (true) {
        string text = readLine(prompt);
        if (!isDecimal(text)) {
            cout << "Error: please enter a number (example: 12.50).\n";
            continue;
        }
        double value = toDouble(text);
        if (value < minValue || value > maxValue) {
            cout << "Error: the value must be between " << minValue
                 << " and " << maxValue << ".\n";
            continue;
        }
        return value;
    }
}

string readText(const string& prompt, int maxLength) {
    while (true) {
        string text = readLine(prompt);
        if (text.empty()) {
            cout << "Error: this field cannot be empty.\n";
            continue;
        }
        if ((int)text.length() > maxLength) {
            cout << "Error: maximum " << maxLength << " characters.\n";
            continue;
        }
        if (text.find('|') != string::npos) {
            cout << "Error: the character '|' is not allowed.\n";
            continue;
        }
        return text;
    }
}

bool confirm(const string& prompt) {
    while (true) {
        string answer = toLower(readLine(prompt + " (y/n): "));
        if (answer == "y" || answer == "yes")
            return true;
        if (answer == "n" || answer == "no")
            return false;
        cout << "Error: please answer y or n.\n";
    }
}

string currentDateTime() {
    time_t now = time(0);                 
    tm* localTime = localtime(&now);      
    char buffer[20];
    strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", localTime);
    return string(buffer);
}
