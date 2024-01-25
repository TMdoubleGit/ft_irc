#include "Server.hpp"

void Server::runBot(int const fd, std::string arguments)
{
	std::vector<std::string> options;
    std::istringstream iss(arguments);
    std::string token_option;
    while (iss >> token_option)
        options.push_back(token_option);
    
    options[0].erase(0, 1);
    if (options[0] == "1" || options[0] == "[1]") {
        _users[fd]->setSendBuff(_users[fd]->getPrefix() + " NOTICE " + _users[fd]->getNickName() + " " + getInfo() + "\r\n");
    }
    else if (options[0] == "2" || options[0] == "[2]") {
        _users[fd]->setSendBuff(_users[fd]->getPrefix() + " NOTICE " + _users[fd]->getNickName() + " " + _bot->getTrivia() + "\r\n");
    }
    else if (options[0] == "3" || options[0] == "[3]") {
        if (options.size() < 2)
            _users[fd]->setSendBuff(_users[fd]->getPrefix() + " NOTICE " + _users[fd]->getNickName() + " " + "Bad numbers of arguments!" + "\r\n");
        else {
            if (!_bot->isFloat(options[1].c_str()))
                _users[fd]->setSendBuff(_users[fd]->getPrefix() + " NOTICE " + _users[fd]->getNickName() + " " + options[1] + " is not a float!" + "\r\n");
            else {
                float euro = static_cast<float>(strtod(options[1].c_str(), NULL));
                _users[fd]->setSendBuff(_users[fd]->getPrefix() + " NOTICE " + _users[fd]->getNickName() + " " + _bot->convertEuro(euro, options[1]) + "\r\n");
            }
        }
    }
    else if (options[0] == "4" || options[0] == "[4]") {
        _users[fd]->setSendBuff(_users[fd]->getPrefix() + " NOTICE " + _users[fd]->getNickName() + " " + _bot->getRollDice(options) + "\r\n");
    }
    else
        _users[fd]->setSendBuff(_users[fd]->getPrefix() + " NOTICE " + _users[fd]->getNickName() + " " + _bot->Introduction() + "\r\n");
}

std::string Server::getInfo()
{
	std::stringstream connection_number;
    std::string blue = "\033[34m";
    std::string green = "\033[32m";
    std::string reset = "\033[0m";
    connection_number << getConnectionNumbers() - 1;

    std::string s = blue + "You are currently using the server " + getName() + " and there is " + connection_number.str() + " users online.\n" + reset;
    std::map<int, User *>::iterator user = _users.begin();
    s += "Here are the current users loged on the server: ";
    while (user != _users.end()) {
        s += green + user->second->getNickName() + reset + ", ";
        user++;
    }
    s.erase(s.length() - 2);
    s  += "\n";
    std::map<std::string, Channel *>::iterator channel = _channels.begin();
    if (channel != _channels.end()) {
    s += "And here are the current channels opened on the server: ";
        while (channel != _channels.end()) {
            s += green + "#" + channel->second->getName() + reset + ", ";
            channel++;
    }
    s.erase(s.length() - 2);
    s  += "\n";
    }
    return s;
}
