#include <iostream>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <string>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <map>
#include "utils.hpp"
#include "Socket.hpp"
#include <vector>

#define BUF_SIZE 510

class IrcServer
{
private:
	Socket					*_listen_socket;
	std::vector<Socket *>	_socket_vector;

/*
	int					_connection_sock;
	int					_serv_sock;
	int					_client_sock;
	struct	sockaddr_in _serv_adr;
	struct	sockaddr_in _client_adr;
	socklen_t			_client_adr_size;
*/
	fd_set				_server_fds;
	fd_set				_client_fds;
	
	int					_fd_max;
	
	std::map<unsigned short, int>	_user_map;

public:

	IrcServer(int argc, char **argv);
	virtual ~IrcServer();

public:

	void	run(int argc);

private:

	void				echo_msg(int my_fd, char *buf, int len);
	void				client_msg(int fd);
	void				client_connect();
	struct sockaddr_in	parsing_host_info(char **argv);
	void				connect_to_server(char **argv);
	void				send_msg(int my_fd, int except_fd, const char *msg);
	void				send_map_data(int my_fd);
	void				show_map_data();

};

IrcServer::IrcServer(int argc, char **argv)
{
	// std::cout << "Irc Server Constructor called." << std::endl;
	FD_ZERO(&_client_fds);
	FD_ZERO(&_server_fds);

	_listen_socket = new Socket(htons(ft::atoi(argv[argc == 3 ? 2 : 1])));
	_listen_socket->set_type(SLEEP);
	FD_SET(_listen_socket->get_fd(), &_server_fds);
	_fd_max = _listen_socket->get_fd();
	_user_map.insert(std::pair<unsigned short, int>(ntohs(_listen_socket->get_port()), _listen_socket->get_fd()));

	_listen_socket->bind();
	_listen_socket->listen();
	
	if (argc == 3)
		connect_to_server(argv);
};

IrcServer::~IrcServer()
{
	delete _listen_socket;
	std::vector<Socket *>::iterator	iter;
	for (iter = _socket_vector.begin(); iter != _socket_vector.end(); iter++)
		delete (*iter);
};

void	 IrcServer::connect_to_server(char **argv)
{
	// std::cout << "connect to server function called\n";
	Socket			*new_socket;

	new_socket = _listen_socket->connect(argv[1]);
	new_socket->set_type(SERVER);
	FD_SET(new_socket->get_fd(), &_server_fds);
	_socket_vector.push_back(new_socket);
	_fd_max++;
	_user_map.insert(std::pair<unsigned short, int>(new_socket->get_port(), new_socket->get_fd()));

	// test
	std::cout << "connect_to_server\n";
	new_socket->show_info();

	// 서버 연결메시지 전송
	new_socket->write("S");

	// 서버 내부 map에 있는 데이터를 send_msg로 전송해야 함
	send_map_data(_listen_socket->get_fd());	
}

/*
** 1번 서버에 2번 서버가 연결 요청
** socket함수 호출
** sockaddr_in 구조체에 값 채워넣음
** 1번 서버와 2번 서버 connect로 연결
** 연결 되면 password 확인
** password 틀리면 연결 끊고 종료
*/

struct sockaddr_in	IrcServer::parsing_host_info(char **argv)
{
	std::string *		split_ret;
	std::string			string_host;
	std::string			string_port_network;
	// password 관련 처리 방법 필요함 
	std::string			string_password_network;
	struct sockaddr_in	host;
	struct addrinfo		*result;

	split_ret = ft::split(argv[1], ':');
	string_host = split_ret[0];
	string_port_network = split_ret[1];
	string_password_network = split_ret[2];
	host.sin_family = AF_INET;
	host.sin_addr.s_addr = inet_addr(string_host.c_str());
	host.sin_port = htons(ft::atoi(string_port_network.c_str()));
	if (host.sin_addr.s_addr == -1)
		Error("inet_addr() error");
	if (getaddrinfo(string_host.c_str(), string_port_network.c_str(), NULL, &result) != 0)
		Error("getaddrinfo() error");
	freeaddrinfo(result);
	delete[] split_ret;
	return (host);
};

void	IrcServer::client_connect()
{
	// std::cout << "client_connect function called." << std::endl;
	Socket		*new_socket;

	new_socket = _listen_socket->accept();
	new_socket->set_type(CLIENT);
	FD_SET(new_socket->get_fd(), &_client_fds);
	_socket_vector.push_back(new_socket);
	_user_map.insert(std::pair<unsigned short, int>(new_socket->get_port(), new_socket->get_fd()));
	if (_fd_max < new_socket->get_fd())
		_fd_max = new_socket->get_fd();

	// test
	std::cout << "client_sock:" << new_socket->get_fd() << std::endl;
	new_socket->show_info();
	
	// user create 전송
	std::string msg = "user create:" + std::to_string(new_socket->get_port());
	std::cout << "=================================" << std::endl;

	// 자신에게 연결하는 클라이언트/서버에 대한 정보를 자신에 연결되어 있는 서버에 공유하기 위함
	send_msg(_listen_socket->get_fd(), new_socket->get_fd(), msg.c_str());
}

void IrcServer::send_msg(int my_fd, int except_fd, const char *msg)
{
	int size = strlen(msg);

	for (int i = 3; i < _fd_max + 1; i++)
	{
		if (i != my_fd && i != except_fd && FD_ISSET(i, &_server_fds))
		{
			std::cout << "send msg: " << msg << std::endl;
			write(i, msg, size);
		}
	}
}

void IrcServer::echo_msg(int my_fd, char *buf, int len)
{
	// std::cout << "echo_msg function called." << std::endl;
	for (int  i = 3; i < _fd_max + 1; i++)
	{
		// my_fd가 server인지 client인지 확인 후 정보 수정해서 전송
		// 현재 서버의 이름을 메시지의 경로에 추가
		if (FD_ISSET(i, &_client_fds) && i != my_fd)
		{
			write(i, buf, len);
		}
		else if (FD_ISSET(i, &_server_fds) && i != my_fd)
		{
			write(i, buf, len);
		}
	}
	write(1, buf, len);
}

void	IrcServer::send_map_data(int my_fd)
{
	std::map<unsigned short, int>::iterator begin;
	std::map<unsigned short, int>::iterator end;

	begin = _user_map.begin();
	end = _user_map.end();
	while (begin != end)
	{
		// 전송하려는 포트 번호를 가진 fd에는 메시지를 보내지 않음
		std::string msg = "user create:" + std::to_string(begin->first);
		send_msg(my_fd, begin->second, msg.c_str());
		begin++;
	}
}

void	IrcServer::show_map_data()
{
	std::map<unsigned short, int>::iterator begin = _user_map.begin();
	std::map<unsigned short, int>::iterator end = _user_map.end();

	std::cout << "============MAP DATA=============\n";
	while (begin != end)
	{
		std::cout << "key(port): " << begin->first << std::endl;
		std::cout << "value(fd): " << begin->second << std::endl;
		std::cout << "-----------------\n";
		begin++;
	}
	std::cout << "=================================\n";
}

void	IrcServer::client_msg(int fd)
{
	// std::cout << "client_msg function called." << std::endl;
	char			buf[BUF_SIZE];
	int				str_len = read(fd, buf, BUF_SIZE);
	bool			is_digit = false;
	std::string *	split_ret = ft::split(std::string(buf), ':');
	std::string		user_port = split_ret[1];
	const char *	tmp = user_port.c_str();

	// check server msg
	std::cout << "user_port:" << user_port << std::endl;
	for (int i = 0; i < user_port.length(); i++)
	{
		if (std::isdigit(tmp[i]) == false)
		{
			std::cout << "false:" << tmp[i] << std::endl;
			break ;
		}
		is_digit = true;
	}
	std::cout << "is_digit: " << is_digit << std::endl;
	if (is_digit)
	{
		_user_map.insert(std::pair<unsigned short, int>((unsigned short)ft::atoi(user_port.c_str()), fd));
		std::map<unsigned short, int>::iterator it = _user_map.find((unsigned short)ft::atoi(user_port.c_str()));
		//std::cout << "new user_map.port: " << it->first << std::endl;
		//std::cout << "new user_map.value: " << it->second << std::endl;
		show_map_data();
	}
	if (str_len == 0)
	{
		FD_CLR(fd, &_client_fds);
		close(fd);
		std::cout << "closed client: " << fd << std::endl;
	}
	else if (str_len == 1 && buf[0] == 'S')
	{
		FD_CLR(fd, &_client_fds);
		FD_SET(fd, &_server_fds);

		std::vector<Socket *>::iterator		iter;
		for (iter = _socket_vector.begin(); iter != _socket_vector.end(); iter++)
		{
			if ((*iter)->get_fd() == fd)
			{
				(*iter)->set_type(SERVER);
			}
		}
		// 새로운 서버가 연결을 시도하는 경우(이미 연결은 됐고 서버로 인증받는 단계)
		// fd는 새롭게 연결되는 서버고, 이 서버에는 기존 서버들에 대한 정보가 필요함
		// map에 있는 데이터들을 전송해줘서 해당 서버가 연결하기 위한 통로를 알 수 있도록 메시지 전송
		send_map_data(_listen_socket->get_fd());
		// Member에서 제거도 해야 됨
	}
	else
		echo_msg(fd, buf, str_len);
	delete[] split_ret;
}

void	IrcServer::run(int argc)
{
	// std::cout << "run function called." << std::endl;
	struct timeval	timeout;
	int				fd_num;
	int				i;
	fd_set			tmp;

	while (1)
	{
		timeout.tv_sec = 5;
		timeout.tv_usec = 5000;

		tmp = _server_fds;
		if ((fd_num = select(_fd_max + 1, &tmp, 0, 0, &timeout)) == -1)
			throw (Error("select return -1. server_fds"));
		if (fd_num != 0)
		{
			for (i = 0; i < _fd_max + 1; i++)
			{
				if (FD_ISSET(i, &tmp))
				{
					if (i == _listen_socket->get_fd())
					{
						// 새로운 연결 요청이 온 경우 -> 새로운 유저 생성 (맵에 추가) -> 다른 서버에 유저 생성 메세지 전송
						// (다른 서버가 처음 요청 한 경우 (argc == 3) -> 일단 클라이언트라고 가정, client_msg에서 검증하고 변경함)
						client_connect();
					}
					else
					{
						// 메세지 처리 (서버) -> (유저 생성 메세지?) (Y) -> 유저 맵에 추가 -> 다른 서버로 전파
						//									 (N) -> echo_msg

						client_msg(i);
					}
				}
			}
		}
		tmp = _client_fds;
		if((fd_num = select(_fd_max + 1, &tmp, 0 ,0, &timeout)) == -1)
			throw (Error("select return -1. client fd"));
		if (fd_num != 0)
		{
			for (i = 0; i < _fd_max + 1; i++)
			{
				if (FD_ISSET(i, &tmp))
				{
					// 메세지 처리 (클라이언트 인 경우) -> 서버 검증 (Y) -> 유저맵 전송 
					//                                      (N) -> echo_msg
					client_msg(i);
				}
			}
		}
	}
};

int main(int argc, char **argv)
{
	try
	{
		IrcServer server(argc, argv);
		server.run(argc);
	}
	catch(Error &e)
	{
		std::cerr << e.what() << '\n';
	}
}