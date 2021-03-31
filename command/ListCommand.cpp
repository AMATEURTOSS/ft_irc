#include "ListCommand.hpp"

void ListCommand::run(IrcServer &irc)
{
    const int param_size = _msg.get_param_size();

    if (param_size == 0)
    {
        print_list(irc);
    }
    if (param_size == 1)
    {
        std::string *   channel_list;
        int             split_size;

        split_size = ft::split(_msg.get_param(0), ',', channel_list);
        print_list(irc, channel_list, split_size);
        delete[] channel_list;
    }
}

void ListCommand::print_list(IrcServer &irc)
{
	Socket										*socket			= irc.get_current_socket();
	std::map<std::string, Channel *>	        global_channel	= irc.get_global_channel();
	std::map<std::string, Channel *>::iterator  first			= global_channel.begin();
	std::map<std::string, Channel *>::iterator	last			= global_channel.end();

	socket->write(Reply(RPL::LISTSTART()).get_msg().c_str());
	while (first != last)
	{
		Channel * channel = first->second;
		if (channel->find_mode('p') || channel->find_mode('s'))
			socket->write(Reply(RPL::LIST(), channel->get_name(), "0", channel->get_topic()).get_msg().c_str());
		else
			socket->write(Reply(RPL::LIST(), channel->get_name(), "1", channel->get_topic()).get_msg().c_str());
		++first;
	}
	socket->write(Reply(RPL::LISTEND()).get_msg().c_str());
}

void ListCommand::print_list(IrcServer &irc, std::string *channel_list, int split_size)
{
    Socket	*socket = irc.get_current_socket();

	socket->write(Reply(RPL::LISTSTART()).get_msg().c_str());
	for (int i = 0; i < split_size; ++i)
	{
		Channel	*channel = irc.get_channel(channel_list[i]);
		if (channel == NULL)
			throw (Reply(ERR::NOSUCHCHANNEL(), channel->get_name()));
		if (channel->find_mode('p') || channel->find_mode('s'))
			socket->write(Reply(RPL::LIST(), channel->get_name(), "0", channel->get_topic()).get_msg().c_str());
		else
			socket->write(Reply(RPL::LIST(), channel->get_name(), "1", channel->get_topic()).get_msg().c_str());
	}
	socket->write(Reply(RPL::LISTEND()).get_msg().c_str());
}

ListCommand::ListCommand(): Command()
{
}