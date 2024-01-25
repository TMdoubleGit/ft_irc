#include "../Server.hpp"

/*
	The PING command is used to test the presence of an active client or
	server at the other end of the connection. 
	When a PING message is received, the appropriate PONG message MUST be
	sent as reply.

	Command: PING
	Parameters: <token>
*/


void Server::pingCommand(int const fd, std::vector<std::string> cmd_parts)
{
	if (cmd_parts[2].empty())
		_users[fd]->setSendBuff(ERR_NEEDMOREPARAMS(_users[fd]->getNickName(), cmd_parts[1]));

	if (cmd_parts[2][0] == ' ')
		cmd_parts[2].erase(0,1);
	
	cmd_parts[2].insert(0, ":");
	_users[fd]->setSendBuff(_users[fd]->getPrefix() + " PONG " + cmd_parts[2] + "\r\n");
}