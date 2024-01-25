
#ifndef SERVER_HPP
#define SERVER_HPP

#include <poll.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <cstring>
#include <csignal>
#include <map>
#include <string>
#include <vector>
#include <sstream>
#include <iostream>
#include <cctype>
#include <fstream>
#include <algorithm>
#include <fstream>
#include "User.hpp"
#include "Channel.hpp"
#include "File.hpp"
#include "Bot.hpp"
#include "macro.hpp"

extern bool	stop_server;

class User;
class Channel;
class File;
class Bot;

class Server {
	private:
		std::string							_server_name;
		std::string 						_password;
		int									_fd_socket;
		struct pollfd						*_poll_fd;
		int									_max_connections;
		int									_connection_numbers;
		std::map<int, User *>				_users;
		std::map<std::string, Channel *>	_channels;
		bool								_server_loop;
		std::map<std::string, File>			_files;
		Bot									*_bot;
	
	public:
		Server(std::string name, std::string port, std::string password);
		~Server();
		//method
		void								runServer();
		std::string							getName() const;
		int									getConnectionNumbers() const;
		std::map<int, User *>				getUsers() const;
		std::map<std::string, Channel *>	getChannels() const;

	private:
		Server();
		Server(const Server &copy);
		Server& operator=(const Server &obj);

		//network
		int			runSocket(std::string port);
		int			newConnection(void);
		void		addToPollFd(int newfd);
		void		removeFromPollFd(int i);
		int 		regularClient(int i);
		int			handlePolloutEvent(int i);
		int			handlePollerEvent(int i);

		//Parsing
		void		bufParsing(int const fd, std::string message);
		void		ft_split(std::vector<std::string> &commands, std::string message);
		int			ft_split_command(std::string cmd, std::vector<std::string> &cmd_parts);
		std::string	ft_strtok(const std::string &msg);

		//commands
		void		registration(int const fd, std::string cmd);
		void		runCommands(int const fd, std::string cmd);
			//nick
		void 		nickCommand(int const fd, std::vector<std::string> cmd_parts);
		bool 		validNick(std::string nick_name);
		bool		isDupNick(std::string nick_name);
			//pass
		void 		passCommand(int const fd, std::vector<std::string> cmd_parts);
			//user
		void 		userCommand(int const fd, std::vector<std::string> cmd_parts);
		std::string	findRealname(const std::string& msg);
		void		sendRegistration(int const fd);
			//ping
		void		pingCommand(int const fd, std::vector<std::string> cmd_parts);
			//join
		void		joinCommand(int const fd, std::vector<std::string> cmd_parts);
		void		split_name_and_keys(std::string message, std::vector<std::string> &names, std::vector<std::string> &keys);
		void		replyJoin(std::string channel_name, const int fd);
		std::string	joinUsersList(std::string channel_name);
			//privmsg
		void		privmsgCommand(int const fd, std::vector<std::string> cmd_parts);
		void		sendToChannel(int const fd, std::map<std::string, Channel *>::iterator it, std::string recipient, std::string text, std::string type);
			//notice
		void		noticeCommand(int const fd, std::vector<std::string> cmd_parts);
			//kick
		void		kickCommand(int const fd, std::vector<std::string> cmd_parts);
		void		parseKickCommand(std::string& to_parse, std::string& channel_name, std::string& user_to_kick, std::string& comment);
			//invite
		void		inviteCommand(int const fd, std::vector<std::string> cmd_parts);
		void 		parseInviteCommand(const std::string& input, std::string& nickname, std::string& channel);
			//quit
		void		quitCommand(int const fd, std::vector<std::string> cmd_parts);
		void		sendQuitToChannel(int const fd, std::map<std::string, Channel *>::iterator it, std::string reason);
			//part
		void		partCommand(int const fd, std::vector<std::string> cmd_parts);
		void		splitPartCommand(std::string message, std::vector<std::string> &channels, std::string &reason);
			//topic
		void		topicCommand(int const fd, std::vector<std::string> cmd_parts);
		bool		parseTopic(const std::string& input, std::string& channel_topic, std::string& topic);
			//mode
		void		modeCommand(int const fd, std::vector<std::string> cmd_parts);
		void		parseMode(const std::string& input, std::vector<std::string>& mode_parts);
		void		modeChannel(int const fd, std::vector<std::string> mode_parts);
		void		modeUser(int const fd, std::vector<std::string> mode_parts);
		void		addMode(int const fd, std::vector<std::string> mode_parts);
		void		removeMode(int const fd, std::vector<std::string> mode_parts);
		void		sendToChannel(std::map<std::string, Channel *>::iterator channel, std::string reply);
			//oper
		void		operCommand(int const fd, std::vector<std::string> cmd_parts);
			//send
		void		sendCommand(int const fd, std::vector<std::string> cmd_parts);
			//get
		void		getCommand(int const fd, std::vector<std::string> cmd_parts);
			//bot
		void		runBot(int const fd, std::string arguments);
		std::string getInfo();

};
		

#endif
