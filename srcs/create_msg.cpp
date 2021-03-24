#include <iostream>
#include "../includes/Server.hpp"
#include "../includes/IRCserv.hpp"
#include <string>
#include "../includes/commands.hpp"

std::string create_msg(const int &code, const size_t &client_idx, const Server &serv, const std::string &arg1, const std::string &arg2)
{
	std::string sample;
	std::string true_code;

	true_code = std::to_string(code);
	if (code < 10)
		true_code = std::string(2, '0').append(std::to_string(code));
	else if (code < 100)
		true_code = std::string(1, '0').append(std::to_string(code));
	if (g_aClient[client_idx].second.get_nickname().empty())
		sample = std::string(":" + serv.get_hostname() + " " + true_code);
	else
		sample = std::string(":" + serv.get_hostname() + " " + true_code + " " + g_aClient[client_idx].second.get_nickname());

	switch (code)
	{
		case 1:
			return sample + RPL_WELCOME(arg1);
		case 410:
			return sample + ERR_INVALIDCAP(arg1);
		case 421:
			return sample + ERR_UNKNOWNCOMMAND(arg1);
		case 431:
			return sample + ERR_NONICKNAMEGIVEN();
		case 432:
			return sample + ERR_ERRONEUSNICKNAME(arg1);
		case 433:
			return sample + ERR_NICKNAMEINUSE(arg1);
		case 461:
			return sample + ERR_NEEDMOREPARAMS(arg1);
		case 462:
			return sample + ERR_ALREADYREGISTRED();
		case 501:
			return sample + ERR_UMODEUNKNOWNFLAG();
		case 502:
			return sample + ERR_USERSDONTMATCH();
		default:
			return std::string("");
	}
}
