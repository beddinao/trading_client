#include <WebSocketServer.h>

WebSocketServer::WebSocketServer() {
	this->addr = get_env("WS_ADDR");
	if (this->addr.empty())
		this->addr = "test.deribit.com";
	this->port = "443";
	pthread_mutex_init(&this->channels_mutex, NULL);
	pthread_mutex_init(&this->halt_mutex, NULL);
}

WebSocketServer::~WebSocketServer() {
	if (this->active) {
		pthread_mutex_lock(&this->halt_mutex);
		this->halt = true;
		pthread_mutex_unlock(&this->halt_mutex);
		pthread_join(this->thread, NULL);
		pthread_detach(this->thread);
	}
	pthread_mutex_destroy(&this->channels_mutex);
	pthread_mutex_destroy(&this->halt_mutex);
}

WebSocketServer::WebSocketServer( const WebSocketServer &S ) {
	*this = S;
}

WebSocketServer	&WebSocketServer::operator = ( const WebSocketServer &S ) {
	(void)S;
	return (*this);
}

void	*WebSocketServer::routine(void *p) {
	WebSocketServer *ws_server = (WebSocketServer*)p;
	try {
		tcp::resolver resolver(ws_server->iox);
		boost::asio::ssl::context ssl_ctx(boost::asio::ssl::context::tlsv12_client);
		websocket::stream<boost::beast::ssl_stream<boost::beast::tcp_stream>> ws(ws_server->iox, ssl_ctx);

		auto const results = resolver.resolve(ws_server->addr.c_str(), ws_server->port.c_str());
		boost::beast::get_lowest_layer(ws).connect(results);
		ws.next_layer().handshake(boost::asio::ssl::stream_base::client);

		ws.handshake(ws_server->addr.c_str(), "/ws/api/v2");

		std::string msg = R"({"jsonrpc": "2.0", "method": "public/subscribe","params": {"channels": [")";
		for (std::vector<std::string>::iterator it = ws_server->channels.begin(); it != ws_server->channels.end(); ++it) {
			msg += *it;
			if ((it + 1) != ws_server->channels.end())
				msg += ",";
		}
		msg += R"("]},"id": 1337})";


		ws.write(boost::asio::buffer(msg));

		boost::beast::flat_buffer buffer;
		while (true) {
			pthread_mutex_lock(&ws_server->halt_mutex);
			if (ws_server->halt || !ws_server->ofile.is_open())
				break;
			pthread_mutex_unlock(&ws_server->halt_mutex);
			ws.read(buffer);
			nlohmann::json j = nlohmann::json::parse(boost::beast::buffers_to_string(buffer.data()));
			if(j["method"] == "subscription") {
				ws_server->ofile << j["params"]["channel"] << ": " << j["params"]["data"]["instrument_name"] << ":" << std::endl;
				ws_server->ofile << "\t[" << j["params"]["data"]["change_id"] << "]-->" << j["params"]["data"]["type"] << ":" << std::endl;
				ws_server->ofile << "\t\t" << "asks:" << j["params"]["data"]["asks"].dump(10) << std::endl;
				ws_server->ofile << "\t\t" << "bids:" << j["params"]["data"]["bids"].dump(10) << std::endl;
			}
			ws_server->ofile << "--------------------------------------------" << std::endl << std::endl;
			buffer.clear();
		}
		ws.close(websocket::close_code::normal);
	}
	catch (std::exception &e) {
		std::cout << "error: " << e.what() << std::endl;
		return NULL;
	}
	pthread_mutex_unlock(&ws_server->halt_mutex);
	return NULL;
}

void	WebSocketServer::start(std::string file_name) {
	if (this->channels.empty()) {
		std::cout << "no subsciptions are made yet" << std::endl;
		return;
	}
	if (this->active) {
		std::cout << "websocket streaming already running" << std::endl;
		return;
	}
	if (this->ofile.is_open())
		this->ofile.close();

	this->ofile.open(file_name, std::ios::out|std::ios::binary);
	if (!this->ofile.is_open()) {
		std::cout << "could not opend file for writing" << std::endl;
		return;
	}

	this->active = true;
	this->halt = false;
	pthread_create(&this->thread, NULL, WebSocketServer::routine, this);
	std::cout << "streaming must be started" << std::endl;
}

void	WebSocketServer::stop() {
	if (!this->active) {
		std::cout << "you must start streaming first" << std::endl;
		return;
	}
	this->active = false;
	pthread_mutex_lock(&this->halt_mutex);
	this->halt = true;
	pthread_mutex_unlock(&this->halt_mutex);
	pthread_join(this->thread, NULL);
	if (this->ofile.is_open())
		this->ofile.close();
	std::cout << "straming stoppd" << std::endl;
}

