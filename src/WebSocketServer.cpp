#include <WebSocketServer.h>

WebSocketServer::WebSocketServer() { };
WebSocketServer::~WebSocketServer() { };

WebSocketServer::WebSocketServer( const WebSocketServer &S ) {
	*this = S;
}

WebSocketServer	&WebSocketServer::operator = ( const WebSocketServer &S ) {
	(void)S;
	return (*this);
}

void	WebSocketServer::start( WebSocketServer &Server ) {
	(void)Server;
	std::cout << "starting server" << std::endl;
}

