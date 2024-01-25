#include "../Server.hpp"

/*
	The Get command is used to get a file from another client on the server.
	The parameter <filepath> is the path were you want receive the file named <filename>.

	Command: QUOTE GET
  	Parameters: <filepath> <filename>
*/

static void parseGet(std::string str, std::vector<std::string> &arguments);

void Server::getCommand(int const fd, std::vector<std::string> cmd_parts)
{
    std::vector<std::string> arguments;
    parseGet(cmd_parts[2], arguments);

	if (arguments.size() < 2) {
        _users[fd]->setSendBuff(ERR_NEEDMOREPARAMS(_users[fd]->getNickName(), cmd_parts[1]));
        return;
    }
    if (_files.find(arguments[1]) == _files.end()) {
        _users[fd]->setSendBuff(":localhost " + _users[fd]->getNickName() + " :The filename " + arguments[1] + " doesn't exist \r\n");
        return;
    }
    File file(_files.find(arguments[1])->second);
    if (file.getReceiver() != _users[fd]->getNickName()) {
        _users[fd]->setSendBuff(":localhost " + _users[fd]->getNickName() + " :You don't have the permission to get this file \r\n");
        return;
    }
    std::string path = arguments[0] + "/" + arguments[1];
    std::fstream	output_file(path.c_str(), std::fstream::out);
    if (output_file.fail()) {
        _users[fd]->setSendBuff(ERR_FILEERROR(_users[fd]->getNickName(), "open output", arguments[1]));
        return;
    }
	std::fstream	input_file(file.getFilePath().c_str(), std::fstream::in);
    if (input_file.fail()) {
        output_file.close();
        _users[fd]->setSendBuff(ERR_FILEERROR(_users[fd]->getNickName(), "open input", arguments[1]));
        return;
    }
    if (output_file.is_open())
        output_file << input_file.rdbuf();
    _files.erase(file.getFileName());
    output_file.close();
    input_file.close();
    _users[fd]->setSendBuff(":localhost " + _users[fd]->getNickName() + " :You successfully got the file" + arguments[1] + " at the path " + path + "\r\n");
}

static void parseGet(std::string str, std::vector<std::string> &arguments) {
    std::istringstream iss(str);
    std::string word;

    while (iss >> word) {
        arguments.push_back(word);
    }
}