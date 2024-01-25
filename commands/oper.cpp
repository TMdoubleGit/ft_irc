#include "../Server.hpp"

/* 
	The OPER command is used by a normal user to obtain IRC operator privileges.
	Both parameters are required for the command to be successful.
	Command: OPER
  	Parameters: <name> <password>

*/

void Server::operCommand(int const fd, std::vector<std::string> cmd_parts)
{
	std::string name, password;

	std::istringstream iss(cmd_parts[2]);
	iss >> name;
	iss >> password;

	if (name.empty() || password.empty())
		_users[fd]->setSendBuff(ERR_NEEDMOREPARAMS(_users[fd]->getNickName(), cmd_parts[1]));
	else if (name != "Harry")
		_users[fd]->setSendBuff(ERR_NOOPERHOST(_users[fd]->getNickName()));
	else if (password != "Alohomora")
		_users[fd]->setSendBuff(ERR_PASSWDMISMATCH(_users[fd]->getNickName()));
	else if (!_users[fd]->getIsOperator()) {
		_users[fd]->setIsOperator(true);
		_users[fd]->setSendBuff(RPL_YOUREOPER(_users[fd]->getNickName()));
		_users[fd]->setMode("o");
		_users[fd]->setSendBuff(RPL_MODE_USER(_users[fd]->getNickName(), "+o"));
	}
}