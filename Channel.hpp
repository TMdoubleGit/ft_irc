#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include "Server.hpp"

class User;

class Channel {

	private:
		const std::string 				_name;
		std::string						_password;
		std::string						_topic;
		std::string						_mode;
		int								_max_capacity;
		std::map<std::string, User*>	_users_list;
		std::vector<std::string>		_operators_list;
		std::vector<std::string>		_invited_list;

	public:
		Channel(std::string const &name);
		~Channel();

		//getter
		const std::string				getName() const;
		std::string						getPassword() const;
		std::string						getTopic() const;
		std::string						getMode() const;
		int								getMaxCapacity() const;
		std::map<std::string, User *>&	getUsersList();
		std::vector<std::string>&		getOperatorList();
		std::vector<std::string>&		getInvitedList();

		//setter
		void	setPassword(std::string password);
		void	setTopic(std::string topic);
		void	setMaxCapacity (int max_capacity);

		//methods

		void	addUser(std::string nickname, User *user);
		bool	isOperator(std::string nickname);
		void	addMode(std::string mode);
		void	removeMode(char mode);
	

	private:
		Channel();
		Channel(const Channel &copy);
		Channel& operator=(const Channel &obj);

};




#endif