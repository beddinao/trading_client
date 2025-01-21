#pragma once

#include <boost/asio.hpp>
#include <boost/beast.hpp>
#include <boost/asio/ssl.hpp>
#include <boost/beast/ssl.hpp>
#include <pthread.h>
#include <fstream>
#include <utils.h>

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
	/*
		thread worker implemented using c library functions
		.i don't have enough time left to figure out
		how to use the c++ ones
	*/
	bool active;
	pthread_t thread;
	/* output file for streaming */
	std::fstream ofile;
	public:
		WebSocketServer();
		~WebSocketServer();
		WebSocketServer( const WebSocketServer& );
		WebSocketServer &operator = ( const WebSocketServer& );
		/* main methods */
		void start(std::string);
		void stop();
		/* channels that are subscriped to */
		std::vector<std::string>	channels;
		/* thread routine */
		static void *routine(void*);
};

