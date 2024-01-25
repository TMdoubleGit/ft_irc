#include "../Server.hpp"

/*
	The INVITE command is used to invite a user to a channel.
	The parameter <nickname> is the nickname of the person to be invited to the target channel <channel>.

	Command: INVITE
  	Parameters: <nickname> <channel>
*/

void Server::inviteCommand(int const fd, std::vector<std::string> cmd_parts)
{
	std::string nickname_invite, channel_invite;
	parseInviteCommand(cmd_parts[2], nickname_invite, channel_invite);
	std::map<std::string, Channel *>::iterator channel = _channels.find(channel_invite);

	if (nickname_invite.empty() || channel_invite.empty())
		_users[fd]->setSendBuff(ERR_NEEDMOREPARAMS(_users[fd]->getNickName(), cmd_parts[1]));
	else if (channel == _channels.end()) {
		_users[fd]->setSendBuff(ERR_NOSUCHCHANNEL(_users[fd]->getNickName(), channel_invite));	
	}																							
	else if (channel->second->getUsersList().find(_users[fd]->getNickName()) == channel->second->getUsersList().end())
		_users[fd]->setSendBuff(ERR_NOTONCHANNEL(_users[fd]->getNickName(), channel_invite));
	else if (channel->second->getUsersList().find(nickname_invite) != channel->second->getUsersList().end())
		_users[fd]->setSendBuff(ERR_USERONCHANNEL(_users[fd]->getNickName(), nickname_invite, channel_invite));
	else if (!channel->second->isOperator(_users[fd]->getNickName()) && (channel->second->getMode().find("i") != std::string::npos)) //check if mode is invite-only.
			_users[fd]->setSendBuff(ERR_CHANOPRIVSNEEDED(_users[fd]->getNickName(), channel_invite));
	else {
		channel->second->getInvitedList().push_back(nickname_invite);
		_users[fd]->setSendBuff(RPL_INVITING(_users[fd]->getNickName(), nickname_invite, channel_invite));
		std::map<int, User *>::iterator user = _users.begin();
		while(user != _users.end()) {
			if (user->second->getNickName() == nickname_invite) {
				user->second->setSendBuff(_users[fd]->getPrefix() + " INVITE " + user->second->getNickName() + " #" + channel_invite + "\r\n");
				break;
			}
			user++;
		}
	}
}

void Server::parseInviteCommand(const std::string& input, std::string& nickname, std::string& channel)
{
	std::istringstream iss(input);

    nickname.clear();
    channel.clear();

    iss >> nickname;
    if (iss >> channel) {
        if (channel.find('#') != std::string::npos) {
            channel = channel.substr(channel.find('#') + 1);
        } else {
            channel.clear();
        }
    }
}
