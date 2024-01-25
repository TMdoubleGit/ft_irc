#include "../Server.hpp"

/*
	The TOPIC command is used to change or view the topic of the given channel.
	If <topic> is not given, either RPL_TOPIC or RPL_NOTOPIC is returned specifying
	the current channel topic or lack of one. If <topic> is an empty string,
	the topic for the channel will be cleared.

    Command: TOPIC
  	Parameters: <channel> [<topic>]
*/

void Server::topicCommand(int const fd, std::vector<std::string> cmd_parts)
{
	std::string channel_topic, topic;
	bool	clear;
	clear = parseTopic(cmd_parts[2], channel_topic, topic);
	if (channel_topic.empty()) {
		_users[fd]->setSendBuff(ERR_NEEDMOREPARAMS(_users[fd]->getNickName(), cmd_parts[1]));
		return ;
	}
	std::map<std::string, Channel *>::iterator channel = _channels.find(channel_topic);
	if (channel == _channels.end())
		_users[fd]->setSendBuff(ERR_NOSUCHCHANNEL(_users[fd]->getNickName(), channel_topic));
	else if (channel->second->getUsersList().find(_users[fd]->getNickName()) == channel->second->getUsersList().end())
		_users[fd]->setSendBuff(ERR_NOTONCHANNEL(_users[fd]->getNickName(), channel_topic));
	else if (!clear) {
		if (channel->second->getTopic().empty() == false)
			_users[fd]->setSendBuff(RPL_TOPIC(_users[fd]->getNickName(), channel_topic, channel->second->getTopic()));
		else
			_users[fd]->setSendBuff(RPL_NOTOPIC(_users[fd]->getNickName(), channel_topic));	
	}
	else {
		if (channel->second->getMode().find('t') != std::string::npos && !channel->second->isOperator(_users[fd]->getNickName()))
			_users[fd]->setSendBuff(ERR_CHANOPRIVSNEEDED(_users[fd]->getNickName(), channel_topic));
		else {
			channel->second->setTopic(topic);
			std::map<std::string, User *>::iterator member = channel->second->getUsersList().begin();
			while (member != channel->second->getUsersList().end()) {
				member->second->setSendBuff(RPL_TOPIC(_users[fd]->getNickName(), channel_topic, channel->second->getTopic()));
				member++;
			}
		}
	}
}

bool Server::parseTopic(const std::string& input, std::string& channel_topic, std::string& topic) {
    channel_topic.clear();
    topic.clear();

    if (input.empty())
        return false;

    size_t colonPos = input.find(':');
    if (colonPos != std::string::npos) {
        if (input[0] == '#')
            channel_topic = input.substr(1, colonPos - 2);
        topic = input.substr(colonPos + 1);
        return true;
    }

    if (input[0] == '#') {
        size_t spacePos = input.find(' ');

        if (spacePos == std::string::npos)
            channel_topic = input.substr(1);
		else {
            channel_topic = input.substr(1, spacePos - 1);
            topic = input.substr(spacePos + 1);
        }
    }
    return false;
}
