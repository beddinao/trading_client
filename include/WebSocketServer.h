#pragma once
#include	<string>
#include	<iostream>

class	WebSocketServer {
	public:
		WebSocketServer();
		~WebSocketServer();
		WebSocketServer( const WebSocketServer& );
		WebSocketServer &operator = ( const WebSocketServer& );

		void start( WebSocketServer& );
};

