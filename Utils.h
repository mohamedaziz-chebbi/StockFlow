// Programmer: Mohamed Aziz Chebbi
#ifndef UTILS_H
#define UTILS_H

#include <string>

std::string trim(const std::string& text);              
std::string toLower(std::string text);                  
std::string shorten(const std::string& text, int maxLength); 
int splitLine(const std::string& line, char separator,
              std::string fields[], int maxFields);     

bool isInteger(const std::string& text);               
bool isDecimal(const std::string& text);               
int toInt(const std::string& text);
double toDouble(const std::string& text);

std::string readLine(const std::string& prompt);
int readInt(const std::string& prompt, int minValue, int maxValue);
double readDouble(const std::string& prompt, double minValue, double maxValue);
std::string readText(const std::string& prompt, int maxLength);
bool confirm(const std::string& prompt);

std::string currentDateTime();                          

#endif
