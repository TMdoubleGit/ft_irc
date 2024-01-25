#include "../Server.hpp"

/*
	The QUIT command is used to terminate a client’s connection to the server.
	Command: QUIT
 	Parameters: [<reason>]
*/

void Server::quitCommand(int const fd, std::vector<std::string> cmd_parts)
{

	std::map<std::string, Channel *>::iterator  it;
	if(!cmd_parts[2].empty() && cmd_parts[2][0] == ':')
		cmd_parts[2].erase(0,1);

	for (it = _channels.begin(); it != _channels.end(); it++) {
		std::map<std::string, User *>& channel_users = it->second->getUsersList();
		std::map<std::string, User *>::iterator user;
		for (user = channel_users.begin(); user != channel_users.end(); user++) {
			if (user->second->getFd() == fd) {
				if (_users.find(fd) != _users.end()) 
					channel_users.erase(_users.find(fd)->second->getNickName());
				sendQuitToChannel(fd, it, cmd_parts[2]);
				break;
			}
		}
	}
}

void Server::sendQuitToChannel(int const fd, std::map<std::string, Channel *>::iterator it, std::string reason)
{
	std::map<std::string, User *>::iterator user = it->second->getUsersList().begin();
	while (user != it->second->getUsersList().end())
	{
		if (user->second->getFd() != fd)
			user->second->setSendBuff(_users[fd]->getPrefix() + " QUIT :Quit: " + reason + "\r\n");
		user++;
	}
}