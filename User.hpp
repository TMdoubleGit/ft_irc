#ifndef USER_HPP
#define USER_HPP

#include "Server.hpp"

class User {
	private:
		int 					_fd;
		std::string 			_user_name;
		std::string 			_nick_name;
		std::string 			_full_name;
		std::string 			_host_name;
		std::string				_read_buf;
		std::string				_send_buf;
		std::string				_prefix;
		std::string				_mode;
		bool 					_good_password;
		bool 					_is_registered;
		bool					_have_nick_pass_user;
		bool 					_is_operator;
		bool					_to_deconnect;
		//map of Channels
	
	public:
		User(int fd);
		~User();

		//getter
		int			getFd() const;
		std::string getUserName() const;
		std::string getNickName() const;
		std::string getFullName() const;
		std::string getHostName() const;
		std::string getMode() const;
		bool		getGoodPassword() const;
		bool		getHaveNickPassUser() const;
		bool		getIsRegistered() const;
		bool		getIsOperator() const;
		bool		getToDeconect() const;

		//special getter
		std::string		getPrefix() const;
		std::string&	getReadBuff();
		std::string&	getSendBuff();

		//setter
		void setUserName(std::string& user_name);
		void setNickName(std::string& nick_name);
		void setFullName(std::string& full_name);
		void setGoodPassword(bool good_password);
		void setHaveNickPassUser(bool have_nick_pass_user);
		void setIsRegistered(bool is_registered);
		void setIsOperator(bool is_operator);
		void setToDeconect(bool to_deconect);

		//special setter
		void setReadBuff(std::string const &read_buf);
		void setSendBuff(std::string const &send_buf);
		void setMode(std::string const &mode);

		//method
		void removeMode(char mode);


	private:
		User();
		User(const User &copy);
		User& operator=(const User &obj);

};

#endif