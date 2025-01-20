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

	/* getting current time from last auth */
	api_client.snap_time();

	while (true) {
		show_menu();
		std::cout << "your choice > ";
		std::getline(std::cin, cin_input);

		if (!std::cin) break;
		//clear_terminal();
		
		std::string instrument = "ETH-PERPETUAL";
		std::string action = "buy";
		std::string type = "limit";

		std::string order_id = "dkaodka";

		try {
			choice = std::stoi(cin_input);
			switch (choice) {
				case 1:
					api_client.place_order(action, instrument, type, 1, 1000);
					continue;
				case 2:
					api_client.cancel_order(order_id);
					continue;
				case 3:
					api_client.modify_order(order_id, 1, 1.2);
					continue;
				case 4:
					api_client.get_order_book();
					continue;
				case 5:
					api_client.get_position();
					continue;
				case 6:
					ws_server.start(ws_server);
					continue;
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


