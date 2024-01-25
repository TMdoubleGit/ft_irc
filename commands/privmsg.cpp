#include "../Server.hpp"

/*
	The PRIVMSG command is used to send private messages between users, as well as to send messages to channels.
	<target> is the nickname of a client or the name of a channel.

	Command: PRIVMSG
  	Parameters: <target> <text to be sent>
*/

void Server::privmsgCommand(int const fd, std::vector<std::string> cmd_parts)
{
	size_t	delim = cmd_parts[2].rfind(":");

	if (delim == std::string::npos) {
		_users[fd]->setSendBuff(ERR_NOTEXTTOSEND(_users[fd]->getNickName()));
		return;
	}
	std::string recipient = cmd_parts[2].substr(0, delim - 1);
	if (recipient.empty()) {
		_users[fd]->setSendBuff(ERR_NORECIPIENT(_users[fd]->getNickName(), cmd_parts[1]));
		return;
	}
	std::string text = cmd_parts[2].substr(delim);
	if ((delim = recipient.find(" ")) != std::string::npos)
		recipient.erase(delim);
	if (recipient[0] == '#') { //message send to channel
		std::map<std::string, Channel *>::iterator it = _channels.find(recipient.substr(1));							
		if (it == _channels.end())
			_users[fd]->setSendBuff(ERR_NOSUCHNICK(_users[fd]->getNickName(), recipient));
		else
			sendToChannel(fd, it, recipient, text, "PRIVMSG");
	}
	else {						//message send to user
		std::map<std::string, Channel *>::iterator it = _channels.find(recipient);
		std::map<int, User *>::iterator user = _users.begin();
		while(user != _users.end()) {
			if (user->second->getNickName() == recipient)
				break;
			user++;
		}
		if (it  == _channels.end() && user == _users.end())
		{
			if (recipient == "IRCSavvy"){
					runBot(fd, text);
			}
			else
				_users[fd]->setSendBuff(ERR_NOSUCHNICK(_users[fd]->getNickName(), recipient));
		}
		else {
			if (user == _users.end()) {
				if(it->second->getUsersList().find(user->second->getNickName()) != it->second->getUsersList().end()) {
					recipient.insert(1,"#");
					sendToChannel(fd, it, recipient, text, "PRIVMSG");
				} else
					_users[fd]->setSendBuff(ERR_NOSUCHNICK(_users[fd]->getNickName(), recipient));
			}
			else
				user->second->setSendBuff(_users[fd]->getPrefix() + " PRIVMSG " + recipient + " " + text + "\r\n");
		}
	}
}

void Server::sendToChannel(int const fd, std::map<std::string, Channel *>::iterator it, std::string recipient, std::string text, std::string type)
{
	std::map<std::string, User *>::iterator user = it->second->getUsersList().begin();
	while (user != it->second->getUsersList().end())
	{
		if (user->second->getFd() != fd)
			user->second->setSendBuff(_users[fd]->getPrefix() + " " + type + " " + recipient + " " + text + "\r\n");
		user++;
	}
}

