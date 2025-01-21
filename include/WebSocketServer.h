#pragma once
#include <string>
#include <vector>
#include <iostream>
#include <boost/asio.hpp>
#include <boost/beast.hpp>
#include <boost/asio/ssl.hpp>
#include <boost/beast/ssl.hpp>
#include <JsonResponse.h>
#include <json.hpp>

namespace websocket = boost::beast::websocket;
using tcp = boost::asio::ip::tcp;


/*
	this class relies heavly on boost libraries
	to communicate using websockets which are not
	implemented in standard c++ libraries
*/
class	WebSocketServer {
	/*
		asio I/O execution context
		- provides access to IO functionality
	*/
	boost::asio::io_context iox;
	/* websocket api address:port */
	std::string addr;
	std::string port;
	public:
		WebSocketServer();
		~WebSocketServer();
		WebSocketServer( const WebSocketServer& );
		WebSocketServer &operator = ( const WebSocketServer& );
		/* main methods */
		bool start();
		void init();
		/* channels that are subscriped to */
		std::vector<std::string>	channels;
};

std::string	get_env( const char* );

