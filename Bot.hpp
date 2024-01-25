#ifndef BOT_HPP
#define BOT_HPP

#include "Server.hpp"
#include <ctime>

class Server;

class Bot {
    private:
        std::string _name;
        std::string _blue;
        std::string _green;
        std::string _reset;
        std::string _yellow;

    public:
        Bot();
        Bot(const Bot &copy);
        Bot& operator=(const Bot &obj);
        ~Bot();
        //getter
        std::string getName() const;
        //method
        std::string Introduction() const;
        std::string convertEuro(float euro, std::string s_euros) const;
        std::string getTrivia() const;
        std::string getRollDice(std::vector<std::string> &options);
        bool        isFloat(const char* s);
    private:
        bool        isUnsignedInt(const std::string &str);
        std::string intToString(int value);

};

#endif