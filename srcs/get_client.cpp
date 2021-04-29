/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_client.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jvaquer <jvaquer@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/03/18 09:49:00 by lucas             #+#    #+#             */
/*   Updated: 2021/04/29 23:28:02 by lucas            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include "../includes/IRCserv.hpp"
# include "../includes/commands.hpp"
# include <algorithm>

bool		sort_dec(const std::pair<SOCKET,Client> &a,  const std::pair<SOCKET,Client> &b) 
{ 
  return (a.first > b.first); 
}

std::string	custom_ntoa(uint32_t in)
{
	std::string		buffer;
	unsigned char	*bytes = (unsigned char *)&in;

	buffer = ft_to_string((int)bytes[0]) + "." + ft_to_string((int)bytes[1]) + "." + ft_to_string((int)bytes[2]) + "." + ft_to_string((int)bytes[3]);
	return buffer;
}

void		accept_connection(MyServ &serv, t_sock &sock)
{
	Connection		new_connection;
	SOCKET			new_fd;
	SOCKADDR_IN6	clSock6;
	socklen_t		clSock_len = sizeof(clSock6);

	FD_CLR(sock.sockfd, &serv.get_readfs());
	memset(&clSock6, 0, sizeof(clSock6));
	new_fd = accept(sock.sockfd, (sockaddr*)&clSock6, &clSock_len);
	if (fcntl(new_fd, F_SETFL, O_NONBLOCK) < 0)
	{
		#ifdef DEBUG
			std::cout <<"fcntl error: failed to set nonblock fd\n";
		#endif
		closesocket(new_fd);
		return ;
	}
	#ifdef __APPLE__
		std::cout << "* New connection from: " << custom_ntoa(clSock6.sin6_addr.__u6_addr.__u6_addr32[3]) << ":"
			<< ntohs(clSock6.sin6_port) << (sock.is_tls ? " (tls)" : "") << std::endl;
	#endif
	#ifdef __linux__
		std::cout << "* New connection from: " << custom_ntoa(clSock6.sin6_addr.__in6_u.__u6_addr32[3]) << ":"
			<< ntohs(clSock6.sin6_port) << (sock.is_tls ? " (tls)" : "") << std::endl;
	#endif
	if (sock.is_tls)
	{
		if (!(new_connection._sslptr = SSL_new(serv.sslctx)))
			std::cerr << "Error: SSL_NEW\n";
		if (SSL_set_fd(new_connection._sslptr, new_fd) < 1)
			std::cerr << "Error: SSL_fd_set\n";
	}
	new_connection.set_tls(sock.is_tls);
	new_connection._fd = new_fd;
	new_connection.sock_addr = clSock6;
	time(&new_connection.get_last_activity());
	//push de <fd, User> sur le vecteur
	g_aUnregistered.push_back(std::make_pair(new_fd, new_connection));
	//send motd a l'arrivee du client sur le server
	// motd_command("", g_aClient.size() - 1, serv);
	//sort en ordre decroissant en fonction de la key(ou first)
	// std::sort(g_aClient.begin(), g_aClient.end(), sort_dec);
}

void		try_accept_connection(MyServ &serv)
{
	for (std::deque<t_sock>::iterator it = g_serv_sock.begin(); it < g_serv_sock.end(); ++it)
	{
		if (FD_ISSET(it->sockfd, &serv.get_readfs()))
		{
			accept_connection(serv, *it);
		}
	}
}
