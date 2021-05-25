#include "../../includes/commands.hpp"
#include "../../includes/IRCserv.hpp"
#include <cstring>

//does what it says
static void		check_empty_line(const std::vector<std::string> &arg, std::list<Client>::iterator client_it, const MyServ &serv)
{
	if (arg.size() < 2 || arg[1].find_first_not_of(' ') == arg[1].npos)
	{
		client_it->push_to_buffer(create_msg(431, client_it, serv));
		throw std::exception();
	}
}

//check username char and size
static std::string	check_username(const std::string &str, std::list<Client>::iterator client_it, const MyServ &serv)
{
	//check si la taille de l'username > 9 si oui envoie une erreur
	if (str.length() > 9)
	{
		client_it->push_to_buffer(create_msg(432, client_it, serv, str)); throw std::exception();
	}
	//check si la string contient des char interdit (les chars valides sont au dessus)
	for (size_t i = 0; i < str.length(); ++i)
	{
		if (!std::strchr(NICKNAME_VALID_CHAR, str[i]))
		{
			client_it->push_to_buffer(create_msg(432, client_it, serv, str)); throw std::exception();
		}
	}
	return str;
}

static void	check_username_ownership(std::string str, std::list<Client>::iterator client_it, const MyServ &serv)
{
	for (std::list<Client>::iterator it = g_all.g_aClient.begin(); it != g_all.g_aClient.end(); ++it)
	{
		if (&(*it) != &(*client_it))
		{
			if (it->get_nickname() == str)
			{
				client_it->push_to_buffer(create_msg(433, client_it, serv, str)); throw std::exception();
			}
		}
	}
}

void	nick_command(const std::string &line, std::list<Client>::iterator client_it, const MyServ &serv)
{
	std::vector<std::string> arg;

	//split arguments
	arg = ft_split(line, " ");

	try
	{
		//check if username exist and if its not blank or smth
		check_empty_line(arg, client_it, serv);
		//check username char and size
		std::string name = check_username(arg[1], client_it, serv);
		//check if any other user owns his username
		check_username_ownership(name, client_it, serv);

		// if all check has passed set his username
		if (client_it->is_registered() == true)
		{
			//Add nick to disconnected user deque
			add_disconnected_nick(client_it);
			//reply to him he changed his nick
			client_it->push_to_buffer(":" + client_it->get_nickname() + "!"
				+ client_it->get_username() + "@" + client_it->get_hostname() + " NICK " + name + "\r\n");
			//tell all channel he changed his nick
			send_to_all_channel("NICK " + name + "\r\n", client_it);
			send_to_all_server(":" + client_it->get_nickname() + " NICK " + ":" + name + "\r\n", g_all.g_aServer.begin(), true);
		}
		client_it->set_nickname(name);
		if (client_it->is_registered() == false && client_it->get_username().size() > 0
			&& client_it->get_hostname().size() > 0 && client_it->get_realname().size() > 0)
		{
			std::string		str_time;
			time_t			tmp = serv.get_start_time();

			str_time = get_created_time();
			str_time = ctime(&tmp);
			if (str_time[str_time.size() - 1] == '\n')
				str_time.resize(str_time.size() - 1);
			std::cout << "Lol" << std::endl;
			client_it->push_to_buffer(create_msg(1, client_it, serv, client_it->get_nickname()));
			std::cout << "Lolilol" << std::endl;
			client_it->push_to_buffer(create_msg(2, client_it, serv, serv.get_hostname(), SERV_VERSION));
			client_it->push_to_buffer(create_msg(3, client_it, serv, str_time));
			client_it->push_to_buffer(create_msg(4, client_it, serv, serv.get_hostname(), SERV_VERSION, USER_VALID_MODE, CHANNEL_VALID_MODE));
			client_it->push_to_buffer(create_msg(5, client_it, serv, serv.get_hostname(), serv.get_port()));

			motd_command("", client_it, serv);
			client_it->set_register(true);
			time(&client_it->get_last_activity());

			std::string		nick_msg = ":" + serv.get_hostname() + " NICK " + client_it->get_nickname() + " 1 ";

			nick_msg += client_it->get_username() + " " + client_it->get_hostname() + " 1 ";
			nick_msg += client_it->get_mode() + " :" + client_it->get_realname() + "\r\n";
			send_to_all_server(nick_msg, g_all.g_aServer.begin(), true);
		}
	}
	catch(const std::exception& e){ return ; }

	// ERR_NICKCOLLISION
	// ERR_UNAVAILABLE_RESSOURCE
	// ERR_RESTRICTED
}


bool	check_valid_nickname(const std::string &nick)
{
	if (nick.size() > 9)
		return (false);
	for (size_t i = 0; i < nick.size(); i++)
	{
		if (!std::strchr(NICKNAME_VALID_CHAR, nick[i]))
			return (false);
	}
	return (true);
}

bool	check_valid_usermode(const std::string &umode)
{
	if (umode.size() > 8 || umode.size() == 0)
		return (false);
	if (umode[0] != '+')
		return (false);
	for (size_t i = 1; i < umode.size(); i++)
	{
		if (!std::strchr(USER_VALID_MODE, umode[i]))
			return (false);
	}
	return (true);
}

bool	check_valid_hostname(const std::string hostname)
{
	std::vector<std::string>	shortname = ft_split(hostname, ".");

	for (size_t i = 0; i < shortname.size(); i++)
	{
		if (shortname[i][0] == '-' || shortname[i][shortname[i].size() -1] == '-')
			return (false);
		for (size_t k = 0; k < shortname[i].size(); k++)
		{
			if (!isalnum(shortname[i][k]) && shortname[i][k] != '-')
				return (false);
		}
	}
	return (true);
}

void	introduce_user(std::vector<std::string> params, std::list<Server>::iterator server_it, const MyServ &serv)
{
	Client							cli;
	std::list<Client>::iterator		client_it;
	std::string						full_msg;

	if (!check_valid_nickname(params[2]))
		return ;
	if (!is_number(params[3]) || !is_number(params[6]))
		return ;
	if (!check_valid_usermode(params[7]))
		return ;
	if (!check_valid_hostname(params[5]))
		return ;
	if (ft_atoi(params[3]) <= 0)
		return ;
	if ((client_it = find_client_by_iterator(params[2])) != g_all.g_aClient.end())
	{
		server_it->push_to_buffer(create_msg(433, server_it, serv, params[2]));
		return ;
	}
	cli.set_nickname(params[2]);
	cli.set_mode(params[7]);
	cli.set_hopcount(ft_atoi(params[3]));
	cli.set_username(params[4]);
	cli.set_hostname(params[5]);
	cli.set_realname(&params[8][1]);
	cli._fd = server_it->_fd;
	cli.set_server_token(ft_atoi(params[6]));
	cli.set_server_host(&(*server_it));

	g_all.g_aClient.push_back(cli);

	full_msg = ":" + serv.get_hostname();
	for (size_t i = 1; i < params.size(); i++)
		full_msg += " " + params[i];
	full_msg += "\r\n";
	send_to_all_server(full_msg, server_it);
}

void	change_nick(std::vector<std::string> params, std::list<Server>::iterator server_it, const MyServ &serv)
{
	std::string					nick;
	std::string					new_nick;
	std::list<Client>::iterator	client_it;

	(void)server_it;
	(void)serv;
	if (params[0].find(':') != 0 || params.size() < 3)
		return ;
	if ((client_it = find_client_by_iterator(&params[0][1])) == g_all.g_aClient.end())
		return ;
	if (find_client_by_iterator(&params[2][1]) != g_all.g_aClient.end())
		return ;
	nick = &params[0][1];
	new_nick = &params[2][1];
	if (!check_valid_nickname(nick) || !check_valid_nickname(new_nick))
		return ;
	send_to_all_server(":" + client_it->get_nickname() + " NICK " + new_nick + "\r\n", server_it);
	std::string		rpl = create_full_name_msg(client_it) + " NICK " + new_nick + "\r\n";
	for (size_t i = 0; i < g_vChannel.size(); i++)
	{
		if (g_vChannel[i].is_user_in_chan(*client_it))
		{
			g_vChannel[i].send_to_all_interne(rpl);
		}
	}
	client_it->set_nickname(new_nick);
}

void	nick_command(const std::string &line, std::list<Server>::iterator server_it, const MyServ &serv)
{
	std::vector<std::string>	params = ft_split(line, " ");
	size_t						pos;

	if (params.size() < 3)
	{
		server_it->push_to_buffer(create_msg(461, server_it, serv, params[1]));
		return ;
	}
	if (params[0].size() <= 1)
		return ;
	if (is_servername_exist(&params[0][1]))
	{
		if (params.size() < 9)
		{
			server_it->push_to_buffer(create_msg(461, server_it, serv, params[1]));
			return ;
		}
		pos = line.find(':', 1);
		if (pos == std::string::npos)
			return ;
		params.resize(8);
		params.push_back(line.substr(pos));
		introduce_user(params, server_it, serv);
	}
	else
	{
		change_nick(params, server_it, serv);
	}
}
