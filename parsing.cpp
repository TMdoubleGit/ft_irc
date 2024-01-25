#include "Server.hpp"

void Server::bufParsing(int const fd, std::string message) 
{
	std::vector<std::string> commands;
	std::map<int, User *>::iterator it = _users.find(fd);

	ft_split(commands, message);
	for (size_t i = 0; i != commands.size(); i++) {
		if (it->second->getIsRegistered() == false) {
			if (it->second->getHaveNickPassUser() == false) {
				registration(fd, commands[i]);
			}
		}
		else {
			runCommands(fd, commands[i]);
		}
	}

}

void Server::ft_split(std::vector<std::string> &commands, std::string message) //split all commands inside a vector using '\n' as delimiter
{
	std::string::size_type i = 0;
	std::string delim = "\n";
	std::string substring;

	while ((i = message.find(delim)) != std::string::npos) {
		substring = message.substr(0, i);
		commands.push_back(substring);
		message.erase(0, i + delim.length());
	}
}

int Server::ft_split_command(std::string cmd, std::vector<std::string> &cmd_parts) //split one command inside a vector, [0]: prefix, [1]: name, [2]: rest
{
	if (cmd.empty())
		return -1;
	
	std::string copy = cmd;
	if (cmd[0] == ':')
	{
		if (cmd.find_first_of(' ') != std::string::npos)
			copy.erase(0, copy.find_first_of(' ') + 1);
	}
	if (copy.find_first_of(' ') == std::string::npos)
	{
		cmd_parts[1].insert(0, copy, 0, std::string::npos);
		if (cmd_parts[1].find('\r') != std::string::npos)
			cmd_parts[1].erase(cmd_parts[1].find('\r'), 1);
	}
	else { // Else we insert everything until the space
		cmd_parts[1].insert(0, copy, 0, copy.find_first_of(' '));
	}

	cmd_parts[0].assign(cmd, 0, cmd.find(cmd_parts[1], 0));

	size_t msg_start = cmd.find(cmd_parts[1], 0) + cmd_parts[1].length();
	cmd_parts[2] = cmd.substr(msg_start + 1, std::string::npos);
	if (cmd_parts[2].find('\r') != std::string::npos)
		cmd_parts[2].erase(cmd_parts[2].find('\r'), 1);

	for (size_t i = 0; i < cmd_parts[1].size(); i++)
		cmd_parts[1][i] = std::toupper(cmd_parts[1][i]);
	return (0);
}


std::string Server::ft_strtok(const std::string &msg) // return everything until space character
{
	std::string nickname;

    size_t firstNonSpacePos = msg.find_first_not_of(' ');

    if (firstNonSpacePos != std::string::npos)
    {
        size_t spacePos = msg.find(' ', firstNonSpacePos);
        if (spacePos != std::string::npos)
        {
            nickname = msg.substr(firstNonSpacePos, spacePos - firstNonSpacePos);
        }
        else
        {
            nickname = msg.substr(firstNonSpacePos);
        }
    }
    return nickname;
}

