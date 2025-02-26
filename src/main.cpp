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
		action = read_stdin_str("action[sell/buy]: ");
		instrument = read_stdin_str("required, instrument name: ");
		type = read_stdin_str("optional, type[\n\tlimit/stop_limit/take_limit/market/stop_market/\n\ttake_market/market_limit/trailing_stop]: ");
		amount = read_stdin_int("required, amount: ");
		price = read_stdin_int("optional, price: ");
	}
	catch (std::exception &e) {
		std::cout << "invalid params" << std::endl;
		return;
	}

	if (action != "sell" && action != "buy") {
		std::cout << "invalid action"  << std::endl;
		return;
	}

	api_client.place_order(action, instrument, type, amount, price);
}

void	collect_cancelation_params(APIClient &api_client) {
	std::string order_id;

	try {
		order_id = read_stdin_str("required, order id: ");
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
		order_id = read_stdin_str("required, order id: ");
		amount = read_stdin_int("optional, new amount: ");
		price = read_stdin_int("optional, new price: ");
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
		currency = read_stdin_str("optional, currency[BTC/ETH/USDC/USDT/EURR/any]: ");
		kind = read_stdin_str("optional, kind[future/option/spot/future_combo/option_combo]: ");
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
		instrument = read_stdin_str("required, instrument name: ");
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
		channel = read_stdin_str("required, channel: ");
		action = read_stdin_str("required, action[add/remove]: ");
	}
	catch (std::exception &e) {
		std::cout << "invalid params" << std::endl;
		return;
	}

	std::vector<std::string>::iterator it;
	if (action == "add") {
		ws_server.channels.push_back(channel);
		std::cout << GRN << "--> channel added to list successfully" << RST << std::endl;
	}
	else if (action == "remove") {
		try {
			std::vector<std::string>::iterator it = ws_server.channels.begin();
			for (; it != ws_server.channels.end(); ++it) {
				if (channel == *it) {
					ws_server.channels.erase(it);
					std::cout << GRN << "--> channel removed from list successfully" << RST << std::endl;
					return;
				}
			}
			std::cout << RED << "error: " << RST << "channel is not in list" << std::endl;
		}
		catch (std::exception &e) {
			std::cout << RED << "error: " << RST << "can't remove channel" << std::endl;
		}
	}
	else std::cout << RED << "error: " << RST << "invalid action, ignoring.." << std::endl;
}

void	collect_streaming_params(WebSocketServer &ws_server) {
	std::string file_name;

	try {
		file_name = read_stdin_str("required, output file path: ");
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
		std::cout << RED << "initiation failed" << RST << std::endl;
		return 1;
	}
	std::cout << GRN << "--> initiation success" << RST << std::endl;

	/* using those data to identify user */
	if (!api_client.authenticate()) {
		std::cout << RED << "authentication failed" << RST << std::endl;
		return 1;
	}
	std::cout << GRN << "--> authentication success" << RST << std::endl;

	/* first time getting current time from last auth */
	api_client.snap_time();

	show_menu();
	while (true) {
		std::cout << "your choice > ";
		std::getline(std::cin, cin_input);

		if (!std::cin) break;

		if (cin_input.empty())
			continue;

		try {
			choice = std::stoi(cin_input);
			switch (choice) {
				case 1: collect_order_params(api_client); break;
				case 2: collect_cancelation_params(api_client); break;
				case 3: collect_edit_params(api_client); break;
				case 4: collect_book_params(api_client); break;
				case 5: collect_position_params(api_client); break;
				case 6: collect_subscription_params(ws_server); break;
				case 7: collect_streaming_params(ws_server); break;
				case 8: ws_server.stop(); break;
				case 9: show_menu(); break;
				case 0:
					std::cout << "Exiting.." << std::endl;
					return (0);
				default:	throw std::runtime_error("");
			}
		}
		catch (std::exception &e) {
			std::cout << RED << "Invalid Option\"" << RST << cin_input << RED "\"" << RST << std::endl;
			continue;
		}
	}
}


