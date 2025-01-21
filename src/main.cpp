#include <trader.h>

bool	initiate_trader(APIClient &api_client) {
	std::string client_id;
	std::string client_secret;
	std::map<std::string, std::string> endpoints;
	/* client cridentials setup */
	if (!get_client_info(client_id, client_secret))
		return false;
	api_client.setup_client(client_id, client_secret);
	/* endpoints setup */
	get_endpoints_info(endpoints);
	api_client.setup_endpoints(endpoints);
	return true;
}

std::string read_stdin_str(std::string out) {
	std::string line;
	while (1) {
		std::cout << out;
		std::cin.clear();
		std::getline(std::cin, line);
		if (!std::cin)
			throw std::runtime_error("stdin error");
		if (line.empty())
			continue;
		return line;
	}
}

double read_stdin_int(std::string out) {
	std::string line;
	while (1) {
		std::cout << out;
		std::getline(std::cin, line);
		if (!std::cin)
			throw std::runtime_error("stdin error");
		if (line.empty())
			continue;
		return std::stod(line);
	}
}

void	collect_order_params(APIClient &api_client) {
	std::string action, instrument, type;
	double price;
	int amount;

	try {
		action = read_stdin_str("action: ");
		instrument = read_stdin_str("instrument name: ");
		type = read_stdin_str("type: ");
		amount = read_stdin_int("amount: ");
		price = read_stdin_int("price: ");
	}
	catch (std::exception &e) {
		std::cout << "invalid params" << std::endl;
		return;
	}

	api_client.place_order(action, instrument, type, amount, price);
}

void	collect_cancelation_params(APIClient &api_client) {
	std::string order_id;

	try {
		order_id = read_stdin_str("order id: ");
	}
	catch (std::exception &e) {
		std::cout << "invalid params" << std::endl;
		return;
	}

	api_client.cancel_order(order_id);
}

void	collect_edit_params(APIClient &api_client) {
	std::string order_id;
	double price;
	int amount;

	try {
		order_id = read_stdin_str("order id: ");
		amount = read_stdin_int("amount: ");
		price = read_stdin_int("price: ");
	}
	catch (std::exception &e) {
		std::cout << "invalid params" << std::endl;
		return;
	}

	api_client.modify_order(order_id, amount, price);
}

void	collect_position_params(APIClient &api_client) {
	std::string currency;
	std::string kind;

	try {
		currency = read_stdin_str("currency: ");
		kind = read_stdin_str("kind: ");
	}
	catch (std::exception &e) {
		std::cout << "invalid params" << std::endl;
		return;
	}

	api_client.get_position(currency, kind);
}

void	collect_book_params(APIClient &api_client) {
	std::string instrument;

	try {
		instrument = read_stdin_str("instrument: ");
	}
	catch (std::exception &e) {
		std::cout << "invalid params" << std::endl;
		return;
	}

	api_client.get_order_book(instrument);
}

void	collect_subscription_params(WebSocketServer &ws_server) {
	std::string action;
	std::string channel;

	try {
		channel = read_stdin_str("channel: ");
		action = read_stdin_str("action: ");
	}
	catch (std::exception &e) {
		std::cout << "invalid params" << std::endl;
		return;
	}

	std::vector<std::string>::iterator it;
	if (action == "add")
		ws_server.channels.push_back(channel);
	else if (action == "remove") {
		std::vector<std::string>::iterator it = ws_server.channels.begin();
		while (it != ws_server.channels.end()) {
			it++;
			if (channel == *it) {
				ws_server.channels.erase(it);
				break;
			}
		}
	}
}

void	collect_streaming_params(WebSocketServer &ws_server) {
	std::string file_name;

	try {
		file_name = read_stdin_str("stream output file path: ");
	}
	catch (std::exception &e) {
		std::cout << "invalid params" << std::endl;
		return;
	}

	ws_server.start(file_name);
}

int	main() {
	APIClient		api_client;
	WebSocketServer	ws_server;
	std::string	cin_input;
	int		choice;


	/* retrieving info required for data transfer */
	if (!initiate_trader(api_client)) {
		std::cout << "initiation failed" << std::endl;
		return 1;
	}

	/* using those data to identify user */
	if (!api_client.authenticate()) {
		std::cout << "authentication failed" << std::endl;
		return 1;
	}

	/* first time getting current time from last auth */
	api_client.snap_time();

	//"book.BTC-PERPETUAL.100ms" 
	while (true) {
		show_menu();
		std::cout << "your choice > ";
		std::getline(std::cin, cin_input);

		if (!std::cin) break;
		clear_terminal();

		try {
			choice = std::stoi(cin_input);
			switch (choice) {
				case 1: collect_order_params(api_client); continue;
				case 2: collect_cancelation_params(api_client); continue;
				case 3: collect_edit_params(api_client); continue;
				case 4: collect_book_params(api_client); continue;
				case 5: collect_position_params(api_client); continue;
				case 6: collect_subscription_params(ws_server); continue;
				case 7: collect_streaming_params(ws_server); continue;
				case 8: ws_server.stop(); continue;
				case 0:
					std::cout << "Exiting.." << std::endl;
					return (0);
				default:	throw std::runtime_error("");
			}
		}
		catch (std::exception &e) {
			std::cout << "Invalid Option\"" + cin_input + "\"" << std::endl;
			continue;
		}
	}
}


