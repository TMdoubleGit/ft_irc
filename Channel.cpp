#include "Channel.hpp"

Channel::Channel(std::string const &name): _name(name),_mode("+"), _max_capacity(-1)
{
}

Channel::~Channel()
{
}

//getter

const std::string Channel::getName() const { return _name; }

std::string Channel::getPassword() const { return _password; }

std::string Channel::getTopic() const { return _topic; }

std::string Channel::getMode() const { return _mode; }

int Channel::getMaxCapacity() const { return _max_capacity; }

std::map<std::string, User *>& Channel::getUsersList() { return _users_list; }

std::vector<std::string>& Channel::getOperatorList() { return _operators_list; }

std::vector<std::string>& Channel::getInvitedList(){ return _invited_list; }

//setter

void Channel::setPassword(std::string password) { _password = password; }

void Channel::setTopic(std::string topic) { _topic = topic; }

void Channel::setMaxCapacity(int max_capacity) { _max_capacity = max_capacity; }

void Channel::addUser(std::string nickname, User *user)
{
	std::map <std::string, User *>::iterator it = _users_list.find(nickname);
	if (it == _users_list.end())
		_users_list[nickname] = user;
}

bool Channel::isOperator(std::string nickname)
{
	std::vector<std::string>::iterator user;
	if (_operators_list.empty())
		return (false);
	for (user = _operators_list.begin(); user != _operators_list.end(); user++) {
		if (*user == nickname)
			return (true);
	}
	return (false);
}

void Channel::addMode(std::string mode)
{
	_mode += mode;
}

void Channel::removeMode(char mode)
{
	std::string::size_type pos = _mode.find(mode);
	while (pos != std::string::npos) {
		_mode.erase(pos, 1);
		pos = _mode.find(mode);
	}
}

//private

Channel::Channel(): _name("")
{
	_max_capacity = -1;
}

Channel::Channel(const Channel &copy)
{
	*this = copy;
}

Channel& Channel::operator=(const Channel &obj)
{
	_password = obj._password;
	_topic = obj._topic;
	_mode = obj._mode;
	_max_capacity = obj._max_capacity;
	_users_list = obj._users_list;
	_operators_list = obj._operators_list;
	return (*this);
}
