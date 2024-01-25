#include "../Server.hpp"

void Server::registration(int const fd, std::string cmd)
{
	std::vector<std::string> cmd_parts(3); //cmd_parts[0] is prefix, cmd_parts[1] is the command, cmd_parts[2] is the rest.
	if (ft_split_command(cmd, cmd_parts) == -1)
		return ;
	if (cmd_parts[1].find("NICK") != std::string::npos) {
		nickCommand(fd, cmd_parts);
	}	
	else if (cmd_parts[1].find("USER") != std::string::npos) {
		userCommand(fd, cmd_parts);
	}
	else if (cmd_parts[1].find("PASS") != std::string::npos) {
		passCommand(fd, cmd_parts);
		
	}
}

/* 
	The NICK command is used to give the client a nickname or change the previous one.
  	Command: NICK
	Parameters: <nickname>
*/

void Server::nickCommand(int const fd, std::vector<std::string> cmd_parts)
{
	std::string nick_name = ft_strtok(cmd_parts[2]);
	std::string old_name = "";
	if (nick_name.empty())
		_users[fd]->setSendBuff(ERR_NONICKNAMEGIVEN(_users[fd]->getNickName()));
	else if (!validNick(nick_name))
		_users[fd]->setSendBuff(ERR_ERRONEUSNICKNAME(_users[fd]->getNickName(), nick_name));
	else if (isDupNick(nick_name))
		_users[fd]->setSendBuff(ERR_NICKNAMEINUSE(_users[fd]->getNickName(), nick_name));
	else if (!_users[fd]->getIsRegistered())
		_users[fd]->setNickName(nick_name);
	else {
		if (_users[fd]->getIsRegistered()) {
			std::cout << "[S] Nickname changed." << std::endl;
			old_name = _users[fd]->getNickName();
			_users[fd]->setNickName(nick_name);
		}
	}
	_users[fd]->setSendBuff(":" + old_name + "!" + _users[fd]->getUserName() + "@localhost NICK " + nick_name + "\r\n");
}

bool Server::validNick(std::string nick_name)
{
	int i = 0;
	while (nick_name[i]) {
		if ((!isalnum(nick_name[i]) && (nick_name[i] != '-') && (nick_name[i] != '_')) || i > 8)
			return false;
		i++;
	}
	if (nick_name == "IRCSavvy")
		return false;
	return true;
}

bool Server::isDupNick(std::string nick_name)
{
	for (std::map<int, User *>::iterator it = _users.begin(); it != _users.end(); it++ ) {
		if (it->second->getNickName() == nick_name)
			return true;
	}
	return false;
}

/* 
	The PASS command is used to set a ‘connection password’. If set, the password must be set before any attempt to register the connection is made. 
	This requires that clients send a PASS command before sending the NICK / USER combination.
	Command: PASS
	Parameters: <password>
*/

void Server::passCommand(int const fd, std::vector<std::string> cmd_parts)
{
	if (cmd_parts[2].empty())
		_users[fd]->setSendBuff(ERR_NEEDMOREPARAMS(_users[fd]->getNickName(), cmd_parts[1]));
	else if (_password != cmd_parts[2]) {
		_users[fd]->setSendBuff(ERR_PASSWDMISMATCH(_users[fd]->getNickName()));
	}
	else if (_users[fd]->getIsRegistered())
		_users[fd]->setSendBuff(ERR_ALREADYREGISTRED(_users[fd]->getNickName()));
	else
		_users[fd]->setGoodPassword(true);
}

/*
	The USER command is used at the beginning of a connection to specify the username and realname of a new user.
	Command: USER
  	Parameters: <username> 0 * <realname>
*/

void Server::userCommand(int const fd, std::vector<std::string> cmd_parts)
{
	std::string username = ft_strtok(cmd_parts[2]);
	std::string realname = findRealname(cmd_parts[2]);
	if (username.empty() || realname.empty())
		_users[fd]->setSendBuff(ERR_NEEDMOREPARAMS(_users[fd]->getNickName(), cmd_parts[1]));
	else if (_users[fd]->getIsRegistered())
		_users[fd]->setSendBuff(ERR_ALREADYREGISTRED(_users[fd]->getNickName()));
	else if (!_users[fd]->getGoodPassword() || _users[fd]->getNickName() == "") {
		return ;
	}
	else {
		_users[fd]->setUserName(username);
		_users[fd]->setFullName(realname);
		_users[fd]->setIsRegistered(true);
		sendRegistration(fd);
	}

}

std::string Server::findRealname(const std::string& msg)
{
    std::string real;

    size_t firstNonSpacePos = msg.find_first_not_of(' ');

    if (firstNonSpacePos != std::string::npos)
    {
        size_t secondSpacePos = msg.find(' ', firstNonSpacePos);
        size_t thirdSpacePos = msg.find(' ', secondSpacePos + 1);
        size_t fourthSpacePos = msg.find(' ', thirdSpacePos + 1);

        if (fourthSpacePos != std::string::npos)
        {
            if (msg[fourthSpacePos + 1] == ':')
            {
                // Return everything after the ':' (including the space)
                real = msg.substr(fourthSpacePos + 1);
            }
            else
            {
                // Return the 5th text (excluding the space)
                real = msg.substr(fourthSpacePos + 1, msg.find(' ', fourthSpacePos + 1) - fourthSpacePos - 1);
            }
        }
    }
	if (!real.empty() && real[0] == ':') {
        real.erase(0, 1); // Erase the first character (colon)
    }
    return real;
}

void Server::sendRegistration(int const fd) {
	_users[fd]->setSendBuff(RPL_WELCOME(_users[fd]->getNickName(), _users[fd]->getUserName(), _users[fd]->getHostName()));
	_users[fd]->setSendBuff(RPL_YOURHOST(_users[fd]->getNickName(), _server_name, "1.0"));
	_users[fd]->setSendBuff(RPL_CREATED(_users[fd]->getNickName(), "06/10/2023"));
	_users[fd]->setSendBuff(RPL_MYINFO(_users[fd]->getNickName(), _server_name, "1.0", "o", "itkol"));

	std::ifstream data;
	char file[14] = "conf/motd.txt";
	data.open(file);
	if (!data) {
		_users[fd]->setSendBuff(ERR_NOMOTD(_users[fd]->getNickName()));
		return;
	}
	
	std::string motd;
	std::string line;
	motd = RPL_MOTDSTART(_users[fd]->getNickName(), _server_name);
	int linecount = 1;
	while (getline(data, line)) {
		if (linecount < 10)
			motd += RPL_MOTD(_users[fd]->getNickName(), line);
		else if (linecount < 12)
			motd += RPL_MOTD(_users[fd]->getNickName(), "\033[32m" + line + "\033[0m");
		else 
			motd += RPL_MOTD(_users[fd]->getNickName(), "\033[31m" + line + "\033[0m");
		linecount++;
	}
	motd += RPL_ENDOFMOTD(_users[fd]->getNickName());
	_users[fd]->setSendBuff(motd);
	data.close();
}
