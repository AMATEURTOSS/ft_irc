#include "RehashCommand.hpp"

void    RehashCommand::run(IrcServer & irc)
{
	Socket		*socket = irc.get_current_socket();
	Member		*member = irc.find_member(socket->get_fd());
	ReadConf	rc;

	if (!member->check_mode('o', false))
		throw (Reply(ERR::NOPRIVILEGES()));
	rc.open_file("ft_irc.conf");
	rc.read_config(irc.get_serverinfo());
	socket->write(Reply(RPL::REHASHING(), "ft_irc.conf").get_msg().c_str());
}

RehashCommand::RehashCommand(): Command()
{
}

RehashCommand::~RehashCommand()
{
}