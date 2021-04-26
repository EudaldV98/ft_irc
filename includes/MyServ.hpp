/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   MyServ.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jvaquer <jvaquer@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/03/16 15:30:03 by jvaquer           #+#    #+#             */
/*   Updated: 2021/04/26 16:36:45 by jvaquer          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MYSERV
# define MYSERV

# include <unistd.h>
# include <iostream>
# include <string>
# include <vector>
# include <sys/select.h>
# include <sys/time.h>
# include <map>
# include <cstring>
# include <openssl/ssl.h>

class MyServ;
std::map<std::string, void	(*)(const std::string &line, const size_t &client_idx, const MyServ &serv)> fill_command(void);

class MyServ
{
	private:
		std::string						_hostname;
		std::string						_port;
		int								_listen_limit;

		fd_set		_readfs;
		fd_set		_writefs;
		fd_set		_exceptfs;
		int			_max_fd;
		timeval		_timeout;
		time_t		_start_time;
        const std::map<std::string, void (*)(const std::string &line, const size_t &client_idx, const MyServ &serv)> _command;
		unsigned char	_password[32];
		unsigned char	_oper_password[32];
		bool			_pass_for_connection;
		bool			_pass_oper;

	public:
		SSL_CTX		*sslctx;
		SSL			*sslptr;

		MyServ() : _listen_limit(0), _max_fd(0), _command(fill_command()),
					_pass_for_connection(false), _pass_oper(false)
		{
			time(&_start_time);
			set_timeout(3);
			bzero(_password, 32);
			bzero(_oper_password, 32);
		}

		~MyServ()
		{
			;
		}
		//class coplienne

		/*
		** getter
		*/
		std::string	get_hostname() const	{ return _hostname; }
		std::string	get_port() const		{ return _port; }
		int			get_listen_limit() const		{ return _listen_limit; }
		const unsigned char *get_password() const { return _password; }
		const unsigned char *get_oper_password() const { return _oper_password; }
		fd_set	&get_readfs() { return _readfs; }
		fd_set	&get_writefs() { return _writefs; }
		fd_set	&get_exceptfs() { return _exceptfs; }
		int		get_max_fd() { return _max_fd; }
		timeval	&get_timeout() { return _timeout; }
		time_t	get_start_time() const { return _start_time; }
        const std::map<std::string,  void	(*)(const std::string &line, const size_t &client_idx, const MyServ &serv)>	&get_command() const { return _command; }
		bool	get_need_pass() const { return _pass_for_connection; }
		bool	get_pass_oper() const { return _pass_oper; }
		
		/*
		 ** setter
		 */
		void	set_hostname(const std::string hostname) { _hostname = hostname; }
		void	set_port(const std::string port) { _port = port; }
		void	set_listen_limit(int listen_limit) { _listen_limit = listen_limit; }
		void	set_password(const unsigned char *password) { std::memcpy(_password, password, 32); }
		void	set_oper_password(const unsigned char *password) { std::memcpy(_oper_password, password, 32); }
		void	set_max_fd(int value) { _max_fd = value; }
		void	set_timeout(int sec = int(), int usec = int())
		{
			_timeout.tv_sec = sec;
			_timeout.tv_usec = usec;
		}
		void	set_need_pass(bool need) { _pass_for_connection = need; }
		void	set_pass_oper(bool need) { _pass_oper = need; }
};

#endif
