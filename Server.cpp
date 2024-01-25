
#include "Server.hpp"
#include "Bot.hpp"

Server::Server()
{
	_server_name = "";
	_password = "";
	_fd_socket = -1;
	_max_connections = 0;
	_poll_fd = NULL;
	_connection_numbers = 0;
	_server_loop = true;
	
}

Server::Server(std::string name, std::string port, std::string password)
{
	_server_name = name;
	_password = password;
	_max_connections = 10;
	_connection_numbers = 1;
	_fd_socket = runSocket(port);
	_poll_fd = new struct pollfd[_max_connections]();
	_poll_fd[0].fd = _fd_socket;
	_poll_fd[0].events = POLLIN;
	_server_loop = true;
	_bot = new Bot();
}

Server::Server(const Server &copy)
{
	*this = copy;
}

Server& Server::operator=(const Server &obj)
{
	this->_server_name = obj._server_name;
	this->_password = obj._password;
	this->_fd_socket = obj._fd_socket;
	this->_max_connections = obj._max_connections;
	this->_server_loop = obj._server_loop;
	this->_poll_fd = new struct pollfd[_max_connections]();
	for(int i = 0; i < _max_connections; i++) {
		this->_poll_fd[i].fd = obj._poll_fd[i].fd;
		this->_poll_fd[i].events = obj._poll_fd[i].events;
	}
	this->_users = obj._users;
	this->_channels = obj._channels;
	this->_files = obj._files;
	this->_bot = obj._bot;
	return (*this);
}

Server::~Server()
{
	delete _bot;
	if (_poll_fd)
		delete [] _poll_fd;
	if (_fd_socket != -1)
		close (_fd_socket);
	if (!_users.empty()) {
		for (std::map<int, User *>::iterator it = _users.begin(); it != _users.end(); it++) {
		close(it->first);
		delete it->second;
		}
		_users.clear();
	}
	if (!_channels.empty()) {
		for (std::map<std::string, Channel *>::iterator it = _channels.begin(); it != _channels.end(); it++) {
			delete it->second;
	}
	_channels.clear();
	}
}

int Server::runSocket(std::string port)
{
	int listener;
	int yes = 1;
	int error_code;
	struct addrinfo hints, *ai, *tmp;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;
	hints.ai_protocol = getprotobyname("TCP")->p_proto;

	if ((error_code = getaddrinfo(NULL, port.c_str(), &hints, &ai)) != 0) {
		std::cerr << "problem with getaddrinfo(): " << gai_strerror(error_code) << std::endl;
		return (-1);
	}
	for (tmp = ai; tmp != NULL; tmp = tmp->ai_next) {
		listener = socket(tmp->ai_family, tmp->ai_socktype, tmp->ai_protocol);
		if (listener < 0)
			continue;
		if (setsockopt(listener, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) < 0) {
			close(listener);
			continue;
		}
		if (bind(listener, tmp->ai_addr, tmp->ai_addrlen) < 0) {
			close(listener);
			continue;
		}
		break;
	}

	freeaddrinfo(ai);

	if (tmp == NULL) {
		std::cerr << "problem with bind()" << std::endl;
		return (-1);
	}

	if (listen(listener, _max_connections) == -1) {
		std::cerr << "problem with listen()" << std::endl;
		return (-1);
	}
	return listener;
}

void Server::runServer()
{	if (_fd_socket == -1)
		return;
	while (!stop_server) {
		if (poll(_poll_fd, _connection_numbers, -1) == -1)
		{
			if (stop_server == true)
				break;
			std::cerr << "problem with poll()" << std::endl;
			break;
		}

		for (int i = 0; i < _connection_numbers; i++) {
			if (_poll_fd[i].revents & POLLIN)  { // Check if someone's ready to read
				if (_poll_fd[i].fd == _fd_socket) {
						if (newConnection() == -1)
							continue;
				}  // If listener is ready to read, handle new connection
				else  { 
					if (regularClient(i)== - 1) // If not the listener, we're just a regular client
						break;
				}
			}
			else if (_poll_fd[i].revents & POLLOUT) // = "can send() data to this socket without blocking."
			{
				if (handlePolloutEvent(i)== - 1) // If not the listener, we're just a regular client
						break;
			}
			else if (_poll_fd[i].revents & POLLERR) {
				if (handlePollerEvent(i) == 0)
					break;
				else
					return;
			}
		}
	}
}

std::string Server::getName() const { return _server_name; }

int Server::getConnectionNumbers() const { return _connection_numbers; }

std::map<int, User *> Server::getUsers() const { return _users; }

std::map<std::string, Channel *>	Server::getChannels() const { return _channels; }

int Server::newConnection(void)
{
	struct sockaddr_storage remoteaddr;
	socklen_t addrlen = sizeof remoteaddr;

	int newfd = accept(_fd_socket,(struct sockaddr *)&remoteaddr, &addrlen);
	if (newfd == -1) {
		std::cerr << "Problem with accept()" << std::endl;
		return (-1);
	}
	addToPollFd(newfd);
	std::cout << "server: new connection from " << inet_ntoa(((struct sockaddr_in*)&remoteaddr)->sin_addr) << " on socket " << newfd << std::endl;
	return (0);
}

void Server::addToPollFd(int newfd)
{
	if (_connection_numbers == _max_connections) {
		_max_connections *= 2;
		struct pollfd	*temp_fd = new struct pollfd[_max_connections]();
		memcpy(temp_fd, _poll_fd, sizeof(struct pollfd));
		delete [] _poll_fd;
		_poll_fd = temp_fd;
	}
	_poll_fd[_connection_numbers].fd = newfd;
	_poll_fd[_connection_numbers].events = POLLIN | POLLOUT;
	_users.insert(std::pair<int, User *>(newfd, new User(newfd)));
	_connection_numbers++;
}

void Server::removeFromPollFd(int i)
{
	for (std::map<std::string, Channel*>::iterator channelIt = _channels.begin(); channelIt != _channels.end(); ++channelIt) {
        std::map<std::string, User*>& usersList = channelIt->second->getUsersList();
        std::map<std::string, User*>::iterator userListIt = usersList.begin();

        while (userListIt != usersList.end()) {
            if (userListIt->second->getFd() == _poll_fd[i].fd) {
                usersList.erase(userListIt++);  // Erase the user from the _users_list
            } else {
                ++userListIt;
            }
        }
    }

	std::map<int, User*>::iterator it = _users.find(_poll_fd[i].fd);
	if (it != _users.end()) {
		delete it->second;
		_users.erase(it);
	} else {
		std::cerr << "Error : can't find the fd: " << _poll_fd[i].fd << std::endl;
	}
	close (_poll_fd[i].fd);
	_poll_fd[i] = _poll_fd[_connection_numbers - 1];
	_connection_numbers--;
}

int Server::regularClient(int i)
{
	char buf[256];
	memset(&buf, 0, 256);
	int fd =_poll_fd[i].fd;
	int nbytes = recv(fd, buf, sizeof buf, 0);

	if (nbytes <= 0) {
		if (nbytes == 0)
			std::cout << "server: socket " << fd << " hung up" << std::endl;
		else
			std::cout << "Problem with recv()" << std::endl;
		removeFromPollFd(i);
		return (-1);
	}
	else { // else deal with commands
		std::cout << "\033[1;33m[C] Message received from client #" << fd << ": " << buf << "\033[0m";

		if (_users.find(fd) == _users.end())
			return (-1);
		else 
			_users.find(fd)->second->setReadBuff(buf);
		
		if (_users.find(fd) == _users.end())
			return (-1);
		else {
			if (_users.find(fd)->second->getReadBuff().find("\r\n") != std::string::npos)
			{
				bufParsing(fd ,_users.find(fd)->second->getReadBuff());
				if (_users.find(fd)->second->getReadBuff().find("\r\n"))
					_users.find(fd)->second->getReadBuff().clear();
				
			}
		}
	}
	return (0);
}

int Server::handlePolloutEvent(int i)
{
	int fd =_poll_fd[i].fd;
	if (_users.find(fd) != _users.end()) {
		std::istringstream buf(_users[fd]->getSendBuff());
		std::string reply;

		if (send(fd, _users[fd]->getSendBuff().c_str(), _users[fd]->getSendBuff().size(), MSG_NOSIGNAL)== -1)
			std::cout << "Problem with send()" << std::endl;
		while(getline(buf, reply))
			std::cout << "\033[1;34m[S] Message sent to client #" << fd << " >> " << reply << "\033[0m" << std::endl;
		_users[fd]->getSendBuff().clear();
	}
	return 0;
}

int Server::handlePollerEvent(int i)
{
	if (_poll_fd[i].fd == _fd_socket)
	{
		std::cerr << "Problem with listen()" << std::endl;
		return (-1);
	}
	else {
		removeFromPollFd(i);
		return (0);
	}
}
