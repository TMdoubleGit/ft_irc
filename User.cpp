
#include "User.hpp"

User::User(int fd)
{
	_fd = fd;
	_nick_name = "";
	_user_name = "";
	_full_name = "";
	_host_name = "localhost";
	_prefix = "";
	_mode = "+";
	_good_password = false;
	_is_operator = false;
	_is_registered = false;
	_have_nick_pass_user = false;
	_to_deconnect = false;
}

User::~User()
{
	
}
//Getter

int User::getFd() const { return _fd; }

std::string User::getUserName() const { return _user_name; }

std::string User::getNickName() const { return _nick_name; }

std::string User::getFullName() const { return _full_name; }

std::string User::getHostName() const { return _host_name; }

std::string User::getMode() const { return _mode; }

bool User::getGoodPassword() const { return _good_password; }

bool User::getHaveNickPassUser() const	{ return _have_nick_pass_user; }

bool User::getIsRegistered() const { return _is_registered; }

bool User::getIsOperator() const { return _is_operator; }

bool User::getToDeconect() const { return _to_deconnect; }

std::string User::getPrefix() const{ return ":" + _nick_name + "!" + _user_name + "@" + _host_name; }

std::string& User::getReadBuff() { return _read_buf; }

std::string& User::getSendBuff() { return _send_buf; }


//Setter

void User::setUserName(std::string& user_name) { _user_name = user_name; }

void User::setNickName(std::string& nick_name) { _nick_name = nick_name; }

void User::setFullName(std::string& full_name) { _full_name = full_name; }

void User::setGoodPassword(bool good_password) { _good_password = good_password; }

void User::setHaveNickPassUser(bool have_nick_pass_user) {_have_nick_pass_user = have_nick_pass_user; }

void User::setIsRegistered(bool is_registered)  { _is_registered = is_registered; }

void User::setIsOperator(bool is_operator) { _is_operator = is_operator; }

void User::setToDeconect(bool to_deconect) { _to_deconnect = to_deconect; }

void User::setReadBuff(std::string const &read_buf) { _read_buf += read_buf; }

void User::setSendBuff(std::string const &send_buf) { _send_buf += send_buf; }

void User::setMode(std::string const &mode) { _mode += mode; }

//Method

void User::removeMode(char mode)
{
	std::string::size_type pos = _mode.find(mode);
	while (pos != std::string::npos) {
		_mode.erase(pos, 1);
		pos = _mode.find(mode);
	}
}

//Private

User::User() {}

User::User(const User &copy)
{
	*this = copy;
}

User& User::operator=(const User &obj)
{
	_fd = obj._fd;
	_nick_name = obj._nick_name;
	_user_name = obj._user_name;
	_full_name = obj._full_name;
	_host_name = obj._host_name;
	_read_buf = obj._read_buf;
	_send_buf = obj._send_buf;
	_prefix = obj._prefix;
	_mode = obj._mode;
	_good_password = obj._good_password;
	_is_operator = obj._is_operator;
	_is_registered = obj._is_registered;
	_to_deconnect = obj._to_deconnect;
	_have_nick_pass_user = obj._have_nick_pass_user;
	return (*this);
}
