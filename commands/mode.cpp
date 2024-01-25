#include "../Server.hpp"
/*
	The MODE command is used to set or remove options (or modes) from a given target.
	Command: MODE
 	Parameters: <target> [<modestring> [<argument>]]
*/

static bool StringisAlnum(std::string str);
static bool containsUnknownMode(const std::string& str);

void Server::modeCommand(int const fd, std::vector<std::string> cmd_parts)
{
	std::vector<std::string> mode_parts(3); //[0] is target, [1] is modestring, [2] is arguments.
	parseMode(cmd_parts[2], mode_parts);
	if (mode_parts[0][0] == '#')
		modeChannel(fd, mode_parts);
	else
		modeUser(fd, mode_parts);
}

void Server::parseMode(const std::string& input, std::vector<std::string>& mode_parts)
{
	std::istringstream iss(input);
    std::string word;
    mode_parts.clear();
	
    iss >> word;
    mode_parts.push_back(word);

    if (iss >> word) {
        mode_parts.push_back(word);
    } else {
        mode_parts.push_back("");
    }

    std::string remaining;
    while (iss >> word) {
        if (!remaining.empty()) {
            remaining += " ";
        }
        remaining += word;
    }
    mode_parts.push_back(remaining);
}

void Server::modeChannel(int const fd, std::vector<std::string> mode_parts) //channel mode
{
	mode_parts[0].erase(0,1);
	std::map<std::string, Channel *>::iterator channel = _channels.find(mode_parts[0]);
	if (channel == _channels.end())
		_users[fd]->setSendBuff(ERR_NOSUCHCHANNEL(_users[fd]->getNickName(), mode_parts[0]));
	else if (mode_parts[1].empty()) {
		_users[fd]->setSendBuff(RPL_CHANNELMODEIS(_users[fd]->getNickName(), mode_parts[0], channel->second->getMode()));
	}
	else {
		std::vector<std::string>::iterator ops = channel->second->getOperatorList().begin(); 
		while (ops != channel->second->getOperatorList().end()) {
			if (*ops == _users[fd]->getNickName())
				break;
			ops++;
		}
		if (ops == channel->second->getOperatorList().end())
			_users[fd]->setSendBuff(ERR_CHANOPRIVSNEEDED(_users[fd]->getPrefix(), mode_parts[0]));
		else if (mode_parts[1][0] == '+')
			addMode(fd, mode_parts);
		else if (mode_parts[1][0] == '-')
			removeMode(fd, mode_parts);
	}
}

void Server::addMode(int const fd, std::vector<std::string> mode_parts) //when channel mod is '+'
{
	std::map<std::string, Channel *>::iterator channel = _channels.find(mode_parts[0]);
	if (mode_parts[1].find("i")!= std::string::npos) {
		if (channel->second->getMode().find("i") == std::string::npos) {
			channel->second->addMode("i");
			sendToChannel(channel, RPL_MODE(mode_parts[0], "+i"));
		}
	}
	if (mode_parts[1].find("t")!= std::string::npos) {
		if (channel->second->getMode().find("t") == std::string::npos) {
			channel->second->addMode("t");
			sendToChannel(channel, RPL_MODE(mode_parts[0], "+t"));
		}
	}
	if (mode_parts[1].find("k")!= std::string::npos) {
		if (mode_parts[2].empty() == true)
			return;
		if (channel->second->getMode().find("k") == std::string::npos) {
			 if (!StringisAlnum(mode_parts[2])) {
				_users[fd]->setSendBuff(ERR_INVALIDMODEPARAM(_users[fd]->getNickName(), mode_parts[0], "+k", mode_parts[2]));
				return;
			 }
			 else {
				channel->second->addMode("k");
				channel->second->setPassword(mode_parts[2]);
				sendToChannel(channel, RPL_MODE_PARAM(mode_parts[0], "+k", mode_parts[2]));
			 }
		}
	}
	if (mode_parts[1].find("o")!= std::string::npos) {
		if (mode_parts[2].empty() == true)
			return;
		std::map<const int, User *>::iterator it = _users.begin();
		while (it != _users.end()) {
			if (it->second->getNickName() == mode_parts[2])
				break;
			it++;
		}
		if (it == _users.end()) {
			_users[fd]->setSendBuff(ERR_NOSUCHNICK(_users[fd]->getNickName(), mode_parts[2]));
			return;
		}
		if (channel->second->getUsersList().find(mode_parts[2]) == channel->second->getUsersList().end()) {
			_users[fd]->setSendBuff(ERR_USERNOTINCHANNEL(_users[fd]->getNickName(), mode_parts[0],mode_parts[2]));
			return;
		}
		std::vector<std::string>::iterator ops = channel->second->getOperatorList().begin();
		while (ops != channel->second->getOperatorList().end()) {
			if (*ops == mode_parts[2])
				break;
			ops++;
		}
		if (ops != channel->second->getOperatorList().end())
			return;
		channel->second->getOperatorList().push_back(mode_parts[2]);
		sendToChannel(channel, RPL_MODE_PARAM(mode_parts[0], "+o", mode_parts[2]));
	}
	if (mode_parts[1].find("l")!= std::string::npos) {

		if (channel == _channels.end()) {
			_users[fd]->setSendBuff(ERR_NOSUCHCHANNEL(_users[fd]->getNickName(), mode_parts[0]));
			return ;
		}
		if (mode_parts[2].empty() == true)
			return;
		int limit = atoi(mode_parts[2].c_str());
		if (limit < 0 || (int)channel->second->getUsersList().size() > limit)
			return ;
		channel->second->setMaxCapacity(limit);
		if (channel->second->getMode().find("l") == std::string::npos) {
			channel->second->addMode("l");
			sendToChannel(channel, RPL_MODE_PARAM(mode_parts[0], "+l", mode_parts[2]));
		}
	}
}

void Server::removeMode(int const fd, std::vector<std::string> mode_parts) //when channel mod is '-'
{
	std::map<std::string, Channel *>::iterator channel = _channels.find(mode_parts[0]);

	if (mode_parts[1].find("i") != std::string::npos) {
		if (channel->second->getMode().find("i") != std::string::npos) {
			channel->second->removeMode('i');
			sendToChannel(channel, RPL_MODE(mode_parts[0], "-i"));
		}
	}
	if (mode_parts[1].find("t")!= std::string::npos) {
		if (channel->second->getMode().find("t") != std::string::npos) {
			channel->second->removeMode('t');
			sendToChannel(channel, RPL_MODE(mode_parts[0], "-t"));
		}
	}
	if (mode_parts[1].find("k")!= std::string::npos) {
		if (channel->second->getMode().find("k") != std::string::npos) {
			channel->second->removeMode('k');
			channel->second->setPassword("");
			sendToChannel(channel, RPL_MODE(mode_parts[0], "-k"));
		}
	}
	if (mode_parts[1].find("o")!= std::string::npos) {
		if (mode_parts[2].empty() == true)
			return;
		std::map<const int, User *>::iterator it = _users.begin();
		while (it != _users.end()) {
			if (it->second->getNickName() == mode_parts[2])
				break;
			it++;
		}
		if (it == _users.end()) {
			_users[fd]->setSendBuff(ERR_NOSUCHNICK(_users[fd]->getNickName(), mode_parts[2]));
			return;
		}
		if (channel->second->getUsersList().find(mode_parts[2]) == channel->second->getUsersList().end()) {
			_users[fd]->setSendBuff(ERR_USERNOTINCHANNEL(_users[fd]->getNickName(), mode_parts[0],mode_parts[2]));
			return;
		}
		std::vector<std::string>::iterator ops = channel->second->getOperatorList().begin();
		while (ops != channel->second->getOperatorList().end()) {
			if (*ops == mode_parts[2])
				break;
			ops++;
		}
		if (ops == channel->second->getOperatorList().end())
			return;
		channel->second->getOperatorList().erase(ops);
		sendToChannel(channel, RPL_MODE_PARAM(mode_parts[0], "-o", mode_parts[2]));
	}
	if (mode_parts[1].find("l")!= std::string::npos) {
		if (channel == _channels.end()) {
			_users[fd]->setSendBuff(ERR_NOSUCHCHANNEL(_users[fd]->getNickName(), mode_parts[0]));
			return ;
		}
		int limit = -1;
		channel->second->setMaxCapacity(limit);
		if (channel->second->getMode().find("l") != std::string::npos) {
			channel->second->removeMode('l');
			sendToChannel(channel, RPL_MODE(mode_parts[0], "-l"));
		}
	}
}

void Server::sendToChannel(std::map<std::string, Channel *>::iterator channel, std::string reply)
{
	std::map<std::string, User *>::iterator member = channel->second->getUsersList().begin();
	while (member != channel->second->getUsersList().end()) {
		member->second->setSendBuff(reply);
		member++;
	}
}

void Server::modeUser(int const fd, std::vector<std::string> mode_parts) //MODE when target is User
{
	std::map<const int, User *>::iterator user = _users.begin();
	while (user != _users.end()) {
		if (user->second->getNickName() == mode_parts[0])
			break;
		user++;
	}
	if (user == _users.end())
		_users[fd]->setSendBuff(ERR_NOSUCHNICK(_users[fd]->getNickName(), mode_parts[0]));
	else if (user->second->getNickName() != _users.find(fd)->second->getNickName())
		_users[fd]->setSendBuff(ERR_USERSDONTMATCH(_users[fd]->getNickName()));
	else if (mode_parts[1].empty())
		_users[fd]->setSendBuff(RPL_UMODEIS(_users[fd]->getNickName(),_users[fd]->getMode()));
	else if (mode_parts[1][0] == '-' || mode_parts[1][0] == '+') {
		if (mode_parts[1][0] == '-' && mode_parts[1].find('o') != std::string::npos && _users[fd]->getMode().find("o") != std::string::npos) {
			_users[fd]->removeMode('o');
			_users[fd]->setSendBuff(RPL_MODE_USER(_users[fd]->getNickName(), "-o"));
			_users[fd]->setIsOperator(false);
		}
		else if(containsUnknownMode(mode_parts[1]))
			_users[fd]->setSendBuff(ERR_UMODEUNKNOWNFLAG(_users[fd]->getNickName()));
	}
}

//utils fonction:

static bool StringisAlnum(std::string str) 
{
    for (size_t i = 0; i < str.length(); i++) 
	{
        if (!isalnum(str[i])) 
            return (false);
    }
    return (true);
}

static bool containsUnknownMode(const std::string& str) {
    for (std::string::const_iterator it = str.begin(); it != str.end(); ++it) {
        if (*it != '+' && *it != 'o' && *it != '-') {
            return true;
        }
    }
    return false;
}