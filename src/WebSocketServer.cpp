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

void	WebSocketServer::init() {
	this->addr = get_env("WS_ADDR");
	if (this->addr.empty())
		this->addr = "test.deribit.com";
	this->port = "443";
}

bool	WebSocketServer::start() {
	if (this->channels.empty()) {
		std::cout << "no subsciptions are made yet" << std::endl;
		return false;
	}
	try {
		tcp::resolver resolver(this->iox);
		boost::asio::ssl::context ssl_ctx(boost::asio::ssl::context::tlsv12_client);
		websocket::stream<boost::beast::ssl_stream<boost::beast::tcp_stream>> ws(this->iox, ssl_ctx);

		auto const results = resolver.resolve(this->addr.c_str(), this->port.c_str());
		boost::beast::get_lowest_layer(ws).connect(results);
		ws.next_layer().handshake(boost::asio::ssl::stream_base::client);

		ws.handshake(this->addr.c_str(), "/ws/api/v2");
		std::cout << "connected to deribit websocket!" << std::endl;

		std::string msg = R"({"jsonrpc": "2.0", "method": "public/subscribe","params": {"channels": [")";
		for (std::vector<std::string>::iterator it = this->channels.begin(); it != this->channels.end(); ++it)
			msg += *it + ",";
		msg += R"("]},"id": 1337})";

		ws.write(boost::asio::buffer(msg));
		std::cout << "subscription request send!" << std::endl;

		boost::beast::flat_buffer buffer;
		while (true) {
			ws.read(buffer);
			nlohmann::json j = nlohmann::json::parse(boost::beast::buffers_to_string(buffer.data()));
			if(j["method"] == "subscription") {
				std::cout << j["params"]["channel"] << ": " << j["params"]["data"]["instrument_name"] << ":" << std::endl;
				std::cout << "\t[" << j["params"]["data"]["change_id"] << "]-->" << j["params"]["data"]["type"] << ":" << std::endl;
				std::cout << "\t\t" << "asks:" << j["params"]["data"]["asks"].dump(10) << std::endl;
				std::cout << "\t\t" << "bids:" << j["params"]["data"]["bids"].dump(10) << std::endl;
			}
			//std::cout << j.dump(5) << std::endl;
			std::cout << "--------------------------------------------" << std::endl << std::endl;
			buffer.clear();
		}
		ws.close(websocket::close_code::normal);
		return true;
	}
	catch (std::exception &e) {
		std::cout << "error: " << e.what() << std::endl;
		return false;
	}
}

