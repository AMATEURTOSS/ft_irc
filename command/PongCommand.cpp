#include "PongCommand.hpp"
#include "ft_irc.hpp"

PongCommand::PongCommand()
{}

PongCommand::PongCommand(PongCommand const &ref)
{
	_msg = ref._msg;
}

PongCommand::~PongCommand()
{}

PongCommand		&PongCommand::operator=(PongCommand const &ref)
{
	if (this != &ref)
		_msg = ref._msg;
	return (*this);
}

void			PongCommand::run(IrcServer &irc)
{
	Socket	*socket = irc.get_current_socket();

	if (socket->get_type() == UNKNOWN)
		throw (Reply(ERR::NOTREGISTERED()));
	else if (socket->get_type() == CLIENT)
	{
		if (_msg.get_param_size() == 0)
			throw (Reply(ERR::NOORIGIN()));
		else if (_msg.get_param_size() == 1)
		{
			// server1만 설정됨
			// 연결 시간같은거 만들어지면 그거 지금 시간으로 업데이트
		}
		else
		{
			// 목적지가 설정된 경우, 해당 서버로 명령어 전달
			Server	*server = irc.get_server(_msg.get_param(1));
			Member	*member = irc.get_member(_msg.get_param(1));
			if ((server == 0) && (member == 0))
				throw (Reply(ERR::NOSUCHSERVER(), _msg.get_param(1)));
			_msg.set_prefix(irc.get_serverinfo().SERVER_NAME);
			server->get_socket()->write(_msg.get_msg());
		}
	}
	else
	{
		std::string		servername = _msg.get_param(0);

		if (servername.at(0) == ':')
			servername = servername.substr(1);
		if (_msg.get_param_size() == 1)
		{
			//
		}
		else if (irc.get_serverinfo().SERVER_NAME == servername)
		{
			//
		}
		else
		{
			Server *server = irc.get_server(servername);
			Member *member = irc.get_member(servername);
			if (server)
				server->get_socket()->write(_msg.get_msg());
			else
			{
				
			}
			
		}
	}
}
