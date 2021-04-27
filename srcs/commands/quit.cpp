/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   quit.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jvaquer <jvaquer@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/03/25 12:27:36 by jvaquer           #+#    #+#             */
/*   Updated: 2021/04/27 21:16:32 by jvaquer          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/commands.hpp"
#include "../../includes/IRCserv.hpp"

void	quit_command(const std::string &line, const size_t &client_idx, const MyServ &serv)
{
	std::vector<std::string>	args;
	std::string					output;
	std::string					part_string;
	t_discon_id					new_id;

	new_id.nickname = g_aClient[client_idx].second.get_nickname();
	new_id.username = g_aClient[client_idx].second.get_username();
	new_id.hostname = g_aClient[client_idx].second.get_hostname();
	new_id.realname = g_aClient[client_idx].second.get_realname();
	args = ft_split(line, " ");
	for (size_t chann_idx = 0; chann_idx < g_vChannel.size(); chann_idx++)
	{
		for (std::vector<Client*>::iterator it = g_vChannel[chann_idx]._users.begin();
			it != g_vChannel[chann_idx]._users.end(); it++)
		{
			if ((*it)->get_nickname() == g_aClient[client_idx].second.get_nickname())
			{
				part_string += g_vChannel[chann_idx].get_name() + ",";
			}
		}
	}
	if (part_string.size() > 0)
		part_string.resize(part_string.size() - 1);
	if (args.size() == 1)
	{
		part_command("PART " + part_string, client_idx, serv);
		throw QuitCommandException();
	}
	else
	{
		for (size_t i = 1; i < args.size() ; i++)
			output += args[i] + " ";
		if (output.size() > 0)
			output.resize(output.size() - 1);
		part_command("PART " + part_string + " " + output, client_idx, serv);
		throw QuitCommandException();
	}
}
