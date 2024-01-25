#include "../Server.hpp"

void Server::runCommands(int const fd, std::string cmd)
{
	std::vector<std::string> cmd_parts(3); //cmd_parts[0] is prefix, cmd_parts[1] is the command, cmd_parts[2] is the rest.
	if (ft_split_command(cmd, cmd_parts) == -1)
		return ;
	
	std::string cmd_name[] = {"NICK", "USER", "PING", "JOIN", "NOTICE", "KICK", "PRIVMSG", "INVITE", "QUIT", "PART", "TOPIC", "MODE", "OPER", "SEND", "GET"};
	size_t i;
	for (i = 0; i < sizeof(cmd_name) / sizeof(cmd_name[0]); i++) {
		if (cmd_name[i] == cmd_parts[1])
			break;
	}
	switch(i) {
		case 0:
			nickCommand(fd, cmd_parts);
			break;
		case 1:
			userCommand(fd, cmd_parts);
			break;
		case 2:
			pingCommand(fd, cmd_parts);
			break;
		case 3:
			joinCommand(fd, cmd_parts);
			break;
		case 4:
			noticeCommand(fd, cmd_parts);
			break;
		case 5:
			kickCommand(fd, cmd_parts);
			break;
		case 6:
			privmsgCommand(fd, cmd_parts);
			break;
		case 7:
			inviteCommand(fd, cmd_parts);
			break;
		case 8:
			quitCommand(fd, cmd_parts);
			break;
		case 9:
			partCommand(fd, cmd_parts);
			break;
		case 10:
			topicCommand(fd, cmd_parts);
			break;
		case 11:
			modeCommand(fd, cmd_parts);
			break;
		case 12:
			operCommand(fd, cmd_parts);
			break;
		case 13:
			sendCommand(fd, cmd_parts);
			break;
		case 14:
			getCommand(fd, cmd_parts);
			break;
		default:
			if (_users.find(fd) != _users.end())
				_users[fd]->setSendBuff(ERR_UNKNOWNCOMMAND(_users[fd]->getNickName(), cmd_parts[1]));
	}
}