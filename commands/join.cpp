#include "../Server.hpp"

/*
	The JOIN command indicates that the client wants to join the given channel(s), each channel using the given key for it.
	The server receiving the command checks whether or not the client can join the given channel, and processes the request.
	Servers MUST process the parameters of this command as lists on incoming commands from clients, with the first <key> 
	being used for the first <channel>, the second <key> being used for the second <channel>, etc.

	While a client is joined to a channel, they receive all relevant information about that channel including the JOIN,
	PART, KICK, and MODE messages affecting the channel. They receive all PRIVMSG and NOTICE messages sent to the channel,
	and they also receive QUIT messages from other clients joined to the same channel (to let them know those users have
	left the channel and the network). This allows them to keep track of other channel members and channel modes.

	Command: JOIN
	Parameters: <channel>{,<channel>} [<key>{,<key>}]
*/

void Server::joinCommand(int const fd, std::vector<std::string> cmd_parts)
{
	std::vector<std::string> names;
	std::vector<std::string> keys;

	split_name_and_keys(cmd_parts[2], names, keys);
	
	if (names.empty())
		_users[fd]->setSendBuff(ERR_NEEDMOREPARAMS(_users[fd]->getNickName(), cmd_parts[1]));

	for (size_t i = 0; i < names.size(); i++) {
		if (names[i][0] == '#')
			names[i].erase(0,1);
		std::map<std::string, Channel *>::iterator it = _channels.find(names[i]);

		if (it == _channels.end())
			_channels.insert(std::pair<std::string, Channel *>(names[i], new Channel(names[i])));	//Create a new Channel, insert it in the map
		else if (it->second->getMode().find('k') != std::string::npos) {
			bool key_is_good = false;
			for (std::vector<std::string>::iterator key = keys.begin(); key != keys.end(); ++key) {
				if (*key == it->second->getPassword()) {
					key_is_good = true;
					break;
				}
			}
			if (!key_is_good) {							
				_users[fd]->setSendBuff(ERR_BADCHANNELKEY(_users[fd]->getNickName(), names[i]));	
				continue;																			
			}
		}
		it = _channels.find(names[i]);

		if (it->second->getMaxCapacity() != -1 && (int)it->second->getUsersList().size() == it->second->getMaxCapacity()) {
			_users[fd]->setSendBuff(ERR_CHANNELISFULL(_users[fd]->getNickName(), names[i]));	
			continue;
		}

		if (it->second->getMode().find('i') != std::string::npos) {
			std::vector<std::string>::iterator invited = it->second->getInvitedList().begin();
			while (invited != it->second->getInvitedList().end()) {
				if (*invited == _users[fd]->getNickName())
					break;
				invited++;
			}
			if (invited == it->second->getInvitedList().end()) {
				_users[fd]->setSendBuff(ERR_INVITEONLYCHAN(_users[fd]->getNickName(),names[i]));
				continue;
			}
		}

		_channels[names[i]]->addUser(_users[fd]->getNickName(), _users[fd]); //Add the user to the channel users list
		if (it->second->getOperatorList().empty())
			it->second->getOperatorList().push_back(_users[fd]->getNickName()); //If he's the one creating the channel, make him operator.
		replyJoin(names[i], fd); //reply to the clients
	}
}

void Server::split_name_and_keys(std::string message, std::vector<std::string> &names, std::vector<std::string> &keys) {

	if (message[0] == ' ')
		message.erase(0, 1);
	
	std::istringstream iss(message);
    std::string token;
    bool isName = true;  // Start with names

    while (std::getline(iss, token, ' ')) {
        std::istringstream tokenStream(token);
        std::string subToken;

        while (std::getline(tokenStream, subToken, ',')) {
            if (isName) {
                names.push_back(subToken);
            } else {
                keys.push_back(subToken);
            }
        }
        isName = !isName;  // Toggle between names and keys
    }
}

void Server::replyJoin(std::string channel_name, const int fd)
{
	std::map<std::string, User *>::iterator it = _channels[channel_name]->getUsersList().begin();

	while (it != _channels[channel_name]->getUsersList().end()) {
		it->second->setSendBuff(_users[fd]->getPrefix() + " JOIN :#" + channel_name + "\r\n");
		if (!_channels[channel_name]->getTopic().empty())
			it->second->setSendBuff(RPL_TOPIC(_users[fd]->getNickName(), channel_name, _channels[channel_name]->getTopic()));
		it->second->setSendBuff(RPL_NAMREPLY(_users[fd]->getNickName(), channel_name, joinUsersList(channel_name)));
		it->second->setSendBuff(RPL_ENDOFNAMES(_users[fd]->getNickName(), channel_name));
		it++;
	}
}

std::string Server::joinUsersList(std::string channel_name)
{
	std::string ret;
	std::map<std::string, User *>::iterator it = _channels[channel_name]->getUsersList().begin();
	while (it != _channels[channel_name]->getUsersList().end()) {
		if (_channels[channel_name]->isOperator(it->second->getNickName()))
			ret += "@";
		ret += it->second->getNickName();
		ret += " ";
		it++;
	}
	if (!ret.empty() && ret[ret.size() - 1] == ' ')
		ret.erase(ret.end() - 1);

	return (ret);
}

