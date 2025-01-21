#include <WebSocketServer.h>

/* default constructor
 * .check if a custom websocket endpoint is set, if no set the deribit default
 * .initiate mutexes */
WebSocketServer::WebSocketServer() {
	this->addr = get_env("WS_ADDR");
	if (this->addr.empty())
		this->addr = "test.deribit.com";
	this->port = "443";
	this->active = false;
}

/*
 * .if the thread is running set the halt flag for it to exit
 * .clean mutexes */
WebSocketServer::~WebSocketServer() {
	if (this->active) {
		pthread_cancel(this->thread);
		pthread_join(this->thread, NULL);
	}
}

WebSocketServer::WebSocketServer( const WebSocketServer &S ) {
	*this = S;
}

WebSocketServer	&WebSocketServer::operator = ( const WebSocketServer &S ) {
	(void)S;
	return (*this);
}

/*
	websocket client thread routine
	.resolve the address/port sat at constructor
	.connect
	.send subscription request to previously added channels
	.enters an infinite loop reading streams from those channels until shut 
	by the parent thread or an error occure
*/
void	*WebSocketServer::routine(void *p) {
	WebSocketServer *ws_server = (WebSocketServer*)p;
	try {
		/* creating a resolver class instance to resolve address */
		tcp::resolver resolver(ws_server->iox);
		/* SSL context to create an ssl websocket */
		boost::asio::ssl::context ssl_ctx(boost::asio::ssl::context::tlsv12_client);
		websocket::stream<boost::beast::ssl_stream<boost::beast::tcp_stream>> ws(ws_server->iox, ssl_ctx);
		/* enabling timeouts on the websocket; it is stuff like this is why i prefer C */
		//ws.set_option(boost::beast::websocket::stream_base::timeout::suggested(boost::beast::role_type::server));
		/* resolving address */
		auto const results = resolver.resolve(ws_server->addr.c_str(), ws_server->port.c_str());
		/* connecting and performing handshake */
		boost::beast::get_lowest_layer(ws).connect(results);
		ws.next_layer().handshake(boost::asio::ssl::stream_base::client);
		ws.handshake(ws_server->addr.c_str(), "/ws/api/v2");
		/* subscription message formation by adding all the channels registered previously by code 6 */
		std::string msg = R"({"jsonrpc": "2.0", "method": "public/subscribe","params": {"channels": [")";
		for (std::vector<std::string>::iterator it = ws_server->channels.begin(); it != ws_server->channels.end(); ++it) {
			msg += *it;
			if ((it + 1) != ws_server->channels.end())
				msg += ",";
		}
		msg += R"("]},"id": 1337})";
		/* printing the formed subscrition msg, always usefull */
		std::cout << "\nSubscription-request message:[" << CYN << msg << RST << "]" << std::endl;
		/* sending the message */
		ws.write(boost::asio::buffer(msg));
		/* special dynamic buffer provided by beast to store the response */
		boost::beast::flat_buffer buffer;
		while (true) {
			/* reading response from socket */
			buffer.clear();
			ws.read(buffer);
			/* parsing response json string using the nlohmann/json library */
			nlohmann::json j = nlohmann::json::parse(boost::beast::buffers_to_string(buffer.data()));
			/* basic check if endpoint responded with an error, and canceling streaming if so */
			ws_server->ofile << j.dump(2) << std::endl;
			if (j.contains("error")) {
				ws_server->ofile << "websocket endpoint responded with error: " << RED << j["error"].dump(3) << RST << std::endl;
				break;
			}
			/* a bit strict but some channels would hang on that */
			else if (!j.contains("result")/* || j["result"].empty()*/) {
				std::cout << "\ninvalid websocket response, " << RED << "quiting.." << RST << std::endl;
				break;
			}
			/* printing special stream data if usefull method */
			else if(j["method"] == "subscription") {
				ws_server->ofile << j["params"]["channel"] << ": " << j["params"]["data"]["instrument_name"] << ":" << std::endl;
				ws_server->ofile << "\t[" << j["params"]["data"]["change_id"] << "]-->" << j["params"]["data"]["type"] << ":" << std::endl;
				ws_server->ofile << "\t\t" << "asks:" << j["params"]["data"]["asks"].dump(10) << std::endl;
				ws_server->ofile << "\t\t" << "bids:" << j["params"]["data"]["bids"].dump(10) << std::endl;
			}
			ws_server->ofile << "--------------------------------------------" << std::endl << std::endl;
		}
		/* closing the websocket */
		ws.close(websocket::close_code::normal);
	}
	catch (std::exception &e) {
		std::cout << "error: " << e.what() << std::endl;
	}
	if (ws_server->ofile.is_open())
		ws_server->ofile.close();
	ws_server->active = false;
	pthread_exit(NULL);
}

/*
	start the websocket stream
	.check if not already running
	.open output file stream
	.creates a thread to run the stream and 
	assign it's routine to it

*/
void	WebSocketServer::start(std::string file_name) {
	/* check if there is channels to subscribe to first */
	if (this->channels.empty()) {
		std::cout << RED "error: " << RST << "must subscribe to at least one channel, use code 6" << std::endl;
		return;
	}
	/* if thread is already running */
	if (this->active) {
		std::cout << RED << "error: " << RST << "websocket streaming already running, use code 8 to stop it" << std::endl;
		return;
	}
	/* closing file stream if already opened to be reused */
	if (this->ofile.is_open())
		this->ofile.close();

	/* checking if the provided path is valid */
	this->ofile.open(file_name, std::ios::out|std::ios::binary);
	if (!this->ofile.is_open()) {
		std::cout << RED << "error: " << RST <<  "could not open/create file for writing, check path permissions" << std::endl;
		return;
	}
	/* creating thread and assigning it it's routing */
	pthread_create(&this->thread, NULL, WebSocketServer::routine, this);
	pthread_detach(this->thread);
	this->active = true;
	std::cout << GRN << "--> WebSocket streaming launched in background > " << RST << file_name << std::endl;
}

/*
	stop the websocket stream
	.check if one is running
	.set halt flag and wait for the thread to exit
	.close the file stream
*/
void	WebSocketServer::stop() {
	/* check if there is an active stream first */
	if (!this->active) {
		std::cout << RED << "error: " << RST << "no streaming is active" << std::endl;
		return;
	}
	pthread_cancel(this->thread);
	pthread_join(this->thread, NULL);
	this->active = false;
	/* closing file stream if not already close by thread */
	if (this->ofile.is_open())
		this->ofile.close();
	std::cout << GRN << "--> WebSocket straming stopped successfully" << RST << std::endl;
}

