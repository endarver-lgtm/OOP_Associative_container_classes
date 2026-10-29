#pragma once
#include <cstdlib>
#include <iostream>
#include <sstream>
#include <string>

inline void inputClosed() {
    std::cout << "\nВвод завершён, выход из программы.\n";
    std::exit(0);
}

inline std::string readRaw(std::istream& in, const std::string& prompt) {
    std::cout << prompt;
    std::string s;
    if (!std::getline(in, s)) inputClosed();
    if (!s.empty() && s.back() == '\r') s.pop_back();
    return s;
}

inline std::string readLine(std::istream& in, const std::string& prompt, size_t maxLen = 100) {
    while (true) {
        std::string s = readRaw(in, prompt);
        size_t b = s.find_first_not_of(" \t");
        size_t e = s.find_last_not_of(" \t");
        s = (b == std::string::npos) ? "" : s.substr(b, e - b + 1);
        if (!s.empty() && s.size() <= maxLen) return s;
        std::cout << "Ошибка: строка не должна быть пустой и длиннее " << maxLen << " символов.\n";
    }
}

inline int readInt(std::istream& in, const std::string& prompt, int lo, int hi) {
    while (true) {
        std::istringstream ss(readRaw(in, prompt));
        int v;
        char rest;
        if (ss >> v && !(ss >> rest) && v >= lo && v <= hi) return v;
        std::cout << "Ошибка ввода: нужно целое число от " << lo << " до " << hi << ".\n";
    }
}

inline double readDouble(std::istream& in, const std::string& prompt, double lo, double hi) {
    while (true) {
        std::string s = readRaw(in, prompt);
        for (char& c : s) if (c == ',') c = '.';
        std::istringstream ss(s);
        double v;
        char rest;
        if (ss >> v && !(ss >> rest) && v >= lo && v <= hi) return v;
        std::cout << "Ошибка ввода: нужно число от " << lo << " до " << hi << ".\n";
    }
}

inline bool askYesNo(std::istream& in, const std::string& prompt) {
    while (true) {
        std::string s = readLine(in, prompt, 3);
        if (s == "y" || s == "Y" || s == "д" || s == "Д") return true;
        if (s == "n" || s == "N" || s == "н" || s == "Н") return false;
        std::cout << "Введите y (да) или n (нет).\n";
    }
}

inline void clearScreen() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

inline void pauseScreen() {
    std::cout << "\nНажмите Enter, чтобы продолжить...";
    std::cin.ignore(10000, '\n');
    std::cin.get();
}
