#include "../Server.hpp"

/*
	The NOTICE message is used similarly to PRIVMSG. The difference between NOTICE and PRIVMSG
	is that automatic replies must never be sent in response to a NOTICE message.
	This rule also applies to servers – they must not send any error back to the client on receipt of a NOTICE command.
	
	Command: NOTICE
	Parameters: <target> <text to be sent>
*/

void Server::noticeCommand(int const fd, std::vector<std::string> cmd_parts)
{
	size_t	delim = cmd_parts[2].rfind(":");

	if (delim == std::string::npos)
		return;
	std::string recipient = cmd_parts[2].substr(0, delim - 1);
	if (recipient.empty())
		return;
	std::string text = cmd_parts[2].substr(delim);
	if ((delim = recipient.find(" ")) != std::string::npos)
		recipient.erase(delim);
	if (recipient[0] == '#') { //message send to channel
		std::map<std::string, Channel *>::iterator it = _channels.find(recipient.substr(1));							
		if (it == _channels.end())
			return ;
		else
			sendToChannel(fd, it, recipient, text, "NOTICE");
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
			return ;
		else {
			if (user == _users.end()) {
				if(it->second->getUsersList().find(user->second->getNickName()) != it->second->getUsersList().end()) {
					recipient.insert(1,"#");
					sendToChannel(fd, it, recipient, text, "NOTICE");
				} else
					return ;
			}
			else
				user->second->setSendBuff(_users[fd]->getPrefix() + " NOTICE " + recipient + " " + text + "\r\n");
		}
	}
}