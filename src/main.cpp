#include <trader.h>

int	main() {
	APIClient		api_client;
	WebSocketServer	ws_server;
	std::string	cin_input;
	int		choice;

	clear_terminal();
	while (true) {

		show_menu();
		std::getline(std::cin, cin_input);

		try {
			choice = std::stoi(cin_input);
		}
		catch (std::exception &e) {
			std::cout << "invalid input! try again." << std::endl;
			getchar();
			continue;
		}


		clear_terminal();
		switch (choice) {
			case 1:
				api_client.place_order();
				break;
			case 2:
				api_client.cancel_order();
				break;
			case 3:
				api_client.modify_order();
				break;
			case 4:
				api_client.get_order_book();
				break;
			case 5:
				api_client.get_position();
				break;
			case 6:
				std::cout << "starting WebSocket server.." << std::endl;
				ws_server.start(ws_server);
				break;
			case 0:
				std::cout << "Exiting.." << std::endl;
				return 0;
			default:	std::cout << "Invalid Option\"" << choice << "\"" << std::endl;
		}
	}
}


