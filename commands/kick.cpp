#include "../Server.hpp"

/*
	The KICK command can be used to request the forced removal of a user from a channel.
	It causes the <user> to be removed from the <channel> by force.
	If no <comment> is given, the server SHOULD use a default message instead.

	Command: KICK
   	Parameters: <channel> <user> [<comment>]
*/

void Server::kickCommand(int const fd, std::vector<std::string> cmd_parts)
{
	std::string channel_name, user_to_kick, comment;
	parseKickCommand(cmd_parts[2], channel_name, user_to_kick, comment);
	std::map<std::string, Channel *>::iterator it	= _channels.find(channel_name);
	

	if(comment.empty() || !comment[1])
		comment = ":Because why not ?";
	if (channel_name.empty() || user_to_kick.empty()) 
		_users[fd]->setSendBuff(ERR_NEEDMOREPARAMS(_users[fd]->getNickName(), cmd_parts[1]));
	else if (it == _channels.end())																	//check if the channel exist
		_users[fd]->setSendBuff(ERR_NOSUCHCHANNEL(_users[fd]->getNickName(), channel_name));
	else if (it->second->getUsersList().find(_users[fd]->getNickName()) == it->second->getUsersList().end()) //check if the kicker is in the right channel
		_users[fd]->setSendBuff(ERR_NOTONCHANNEL(_users[fd]->getNickName(), channel_name));
	else if (it->second->getUsersList().find(user_to_kick) == it->second->getUsersList().end()) //check if the kicker is in the right channel
		_users[fd]->setSendBuff(ERR_USERNOTINCHANNEL(_users[fd]->getNickName(), channel_name, user_to_kick));
	else if (it->second->isOperator(_users[fd]->getNickName()) == false)
		_users[fd]->setSendBuff(ERR_CHANOPRIVSNEEDED(_users[fd]->getNickName(), channel_name));
	else {
		std::map<std::string, User *>::iterator channel_user = _channels[channel_name]->getUsersList().begin();
		while (channel_user != _channels[channel_name]->getUsersList().end()) {
			channel_user->second->setSendBuff(_users[fd]->getPrefix() + " KICK #" + channel_name + " " + user_to_kick + " " + comment + "\r\n");
			channel_user++;
		}
		it->second->getUsersList().erase(user_to_kick);
	}

}

void Server::parseKickCommand(std::string& to_parse, std::string& channel_name, std::string& user_to_kick, std::string& comment)
{
	size_t colonPos = to_parse.find(':');
    size_t firstSpacePos = to_parse.find(' ');

    if (firstSpacePos != std::string::npos && to_parse[0] == '#') {
		channel_name = to_parse.substr(1, firstSpacePos - 1);
		if (colonPos != std::string::npos) {
			user_to_kick = to_parse.substr(firstSpacePos + 1, colonPos - firstSpacePos - 2);
			comment = to_parse.substr(colonPos);
		}
		else {
			user_to_kick = to_parse.substr(firstSpacePos + 1);
			comment.clear();
		}
	}
	else {
		if (to_parse[0] == '#')
			channel_name = to_parse.substr(1);
		else
			channel_name.clear();
		user_to_kick.clear();
		comment.clear();
	}
}