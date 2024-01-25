#include "../Server.hpp"

/*
	The SEND command is used to send a file to another client on the server.
	The parameter <nickname> is the nickname of the person to possibly receive the file at the path <filepath>.

	Command: QUOTE SEND
  	Parameters: <filepath> <nickname>
*/

static std::string parseSend(std::string str, std::vector<std::string> &arguments);

void Server::sendCommand(int const fd, std::vector<std::string> cmd_parts)
{
    std::vector<std::string> arguments;
    std::string filename = parseSend(cmd_parts[2], arguments);
    
    if (filename.empty()) {
        _users[fd]->setSendBuff(ERR_NEEDMOREPARAMS(_users[fd]->getNickName(), cmd_parts[1]));
        return;
    }
    std::map< int, User *>::iterator user = _users.begin();
    while (user != _users.end())
    {
        if (user->second->getNickName() == arguments[1])
            break;
        user++;
    }
    if (user == _users.end()) {
        _users[fd]->setSendBuff(ERR_NOSUCHNICK(_users[fd]->getNickName(),arguments[1]));
        return;
    }
    std::fstream file(arguments[0].c_str(), std::fstream::in);
    if (file.fail()) {
        _users[fd]->setSendBuff(ERR_FILEERROR(_users[fd]->getNickName(), "open", arguments[0]));
        return;
    }
    file.close();
    if (_files.find(filename) != _files.end()) {
        _users[fd]->setSendBuff(":localhost " + _users[fd]->getNickName() + " :The filename " + filename + " is already used \r\n");
        return;
    }
    File f(filename, arguments[0], _users[fd]->getNickName(), arguments[1]);
    _files.insert(std::pair<std::string, File>(filename, f));
    user->second->setSendBuff(_users[fd]->getPrefix() + " " + "NOTICE" + " " + user->second->getNickName() + " want to send you this file " + filename + "\r\n");
}

static std::string parseSend(std::string str, std::vector<std::string> &arguments) {
    size_t spacePos = str.find(' ');

    if (spacePos == std::string::npos) {
        arguments.clear();
        return "";
    }
    std::string firstWord = str.substr(0, spacePos);
    std::string secondWord = str.substr(spacePos + 1);

    size_t lastSlashPos = firstWord.find_last_of('/');

    arguments.push_back(firstWord);
    arguments.push_back(secondWord);
    if (lastSlashPos != std::string::npos)
        return firstWord.substr(lastSlashPos + 1);
    else
        return (firstWord);
}