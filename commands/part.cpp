#include "../Server.hpp"

/*
	The PART command removes the client from the given channel.
	On sending a successful PART command, the user will receive a PART
	message from the server for the channel they have been removed from.
	<reason> is the reason that the client has left the channel.

	Command: PART
  	Parameters: <channel>[<reason>]

*/

void Server::partCommand(int const fd, std::vector<std::string> cmd_parts)
{
	std::vector<std::string> channels_to_part;
	std::string reason;
	if (cmd_parts[2].empty() || cmd_parts[2][0] != '#') {
		_users[fd]->setSendBuff(ERR_NEEDMOREPARAMS(_users[fd]->getNickName(), cmd_parts[1]));
		return ;
	}
	splitPartCommand(cmd_parts[2], channels_to_part, reason);
	for (size_t i = 0; i < channels_to_part.size(); i++) {
		if (channels_to_part[i][0] == '#')
			channels_to_part[i].erase(0,1);
		std::map<std::string, Channel *>::iterator it = _channels.find(channels_to_part[i]);
		if (it == _channels.end()) {
			_users[fd]->setSendBuff(ERR_NOSUCHCHANNEL(_users[fd]->getNickName(), channels_to_part[i]));
			continue;
		}
		else if (it->second->getUsersList().find(_users[fd]->getNickName()) == it->second->getUsersList().end()) {
			_users[fd]->setSendBuff(ERR_NOTONCHANNEL(_users[fd]->getNickName(), channels_to_part[i]));
			continue;
		}
		else {
			it->second->getUsersList().erase(_users[fd]->getNickName());
			_users[fd]->setSendBuff(_users[fd]->getPrefix() + " PART #" + channels_to_part[i] +  " " + reason + "\r\n");
			std::map<std::string, User *>::iterator member = it->second->getUsersList().begin();
			while (member != it->second->getUsersList().end()) {
				member->second->setSendBuff(_users[fd]->getPrefix() + " PART #" + channels_to_part[i] +  " " + reason + "\r\n");
				member++;
			}
		}	
	}
}
	
void Server::splitPartCommand(std::string message, std::vector<std::string> &channels, std::string &reason)
{
	if (message[0] == ' ')
		message.erase(0, 1);

	channels.clear();
    size_t spacePos = message.find(' ');
    
    std::string channelsStr = message.substr(0, spacePos);
    
    if (spacePos != std::string::npos) {
        reason = message.substr(spacePos + 1);
    } else {
        reason.clear();
    }
    std::stringstream ss(channelsStr);
    std::string channel;
    
    while (std::getline(ss, channel, ',')) {
        if (!channel.empty() && channel[0] == '#') {
            channels.push_back(channel);
        }
    }
}
