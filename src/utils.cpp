#include <utils.h>

bool	get_client_info(std::string &client_id, std::string &client_secret) {
	/* retrieving client id and falling back */
	client_id = get_env("CLIENT_ID");
	if (client_id.empty()) {
		std::cout << "CLIENT_ID "<< WHT << "is not set in env, using the" << RST << " DEFAULT_CLIENT_ID" << std::endl;
		client_id = get_env("DEFAULT_CLIENT_ID");
	}
	/* retrieving client secret and failing back */
	client_secret = get_env("CLIENT_SECRET");
	if (client_secret.empty()) {
		std::cout << "CLIENT_SECRET" << WHT << " is not set, using the " << RST << "DEFAULT_CLIENT_SECRET" << std::endl;
		client_secret = get_env("DEFAULT_CLIENT_SECRET");
	}
	if (client_id.empty() || client_secret.empty()) {
		std::cout << RED << "error: " << RST << "failed to retrieve deribit account info" << std::endl;
		return false;
	}
	return true;
}

void	get_endpoints_info(std::map<std::string, std::string> &endpoints) {
	endpoints["url"] = get_env("SITE_URL");
	if (endpoints["url"].empty()) {
		endpoints["url"] = "https://test.deribit.com";
		std::cout << WHT << "custom "<< RST << "SITE_URL "<< WHT << "is not set, using: " << RST << endpoints["url"] << std::endl;
	}
	/* endpoint used for authentication: public */
	endpoints["auth"] = get_env("ENDPOINT_AUTH");
	if (endpoints["auth"].empty()) {
		endpoints["auth"] = "/public/auth";
		std::cout << WHT << "custom "<< RST << "ENDPOINT_AUTH" << WHT << " is not set, using: " << RST << endpoints["auth"] << std::endl;
	}
	/* endpoint used for buying: private */
	endpoints["buy"] = get_env("ENDPOINT_BUY");
	if (endpoints["buy"].empty()) {
		endpoints["buy"] = "/private/buy";
		std::cout << WHT << "custom " << RST << "ENDPOINT_BUY" << WHT << " is not set, using: " << RST << endpoints["buy"] << std::endl;
	}
	/* endpoinr used for selling: private */
	endpoints["sell"] = get_env("ENDPOINT_SELL");
	if (endpoints["sell"].empty()) {
		endpoints["sell"] = "/private/sell";
		std::cout << WHT << "custom " << RST << "ENDPOINT_SELL" << WHT << " is not set, using: " << RST << endpoints["sell"] << std::endl;
	}
	/* endpoint used for editing books: private */
	endpoints["edit"] = get_env("ENDPOINT_EDIT");
	if (endpoints["edit"].empty()) {
		endpoints["edit"] = "/private/edit";
		std::cout << WHT << "custom" << RST << " ENDPOINT_EDIT" << WHT << " is not set, using: " << RST << endpoints["edit"] << std::endl;
	}
	/* endpoint used of canceling orders: private */
	endpoints["cancel"] = get_env("ENDPOINT_CANCEL");
	if (endpoints["cancel"].empty()) {
		endpoints["cancel"] = "/private/cancel";
		std::cout << WHT << "custom" << RST << " ENDPOINT_CANCEL " << WHT << " is not set, using: " << RST << endpoints["cancel"] << std::endl;
	}
	/* endpoint used for getting position */
	endpoints["position"] = get_env("ENDPOINT_POSITION");
	if (endpoints["position"].empty()) {
		endpoints["position"] = "/private/get_positions";
		std::cout << WHT << "custom " << RST << "ENDPOINT_POSITION"<< WHT << " is not set, using: " << RST << endpoints["position"] << std::endl;
	}
	/* endpoint used for getting order book */
	endpoints["order_book"] = get_env("ENDPOINT_BOOK");
	if (endpoints["order_book"].empty()) {
		endpoints["order_book"] = "/public/get_order_book";
		std::cout << WHT << "custom " << RST << "ENDPOINT_BOOK " << WHT << "is not set, using: " << RST << endpoints["order_book"] << std::endl;
	}
}

void	show_menu() {
	std::cout << "--> Commands Menu:" << std::endl;
	std::cout << UND << "code "<< RST << ".Command                     | parameters" << std::endl;
	std::cout << "-----------------------------------------------------------------------" << std::endl;
	std::cout << UND << "   1 "<< RST << ".Place an Order              | action,instrument,type,amount,price" << std::endl;
	std::cout << UND << "   2 "<< RST << ".Cancel an Order             | order_id" << std::endl;
	std::cout << UND << "   3 "<< RST << ".Modify an Order             | order_id,new_amount,new_price" << std::endl;
	std::cout << UND << "   4 "<< RST << ".Request Orderbook           | instrument" << std::endl;
	std::cout << UND << "   5 "<< RST << ".Request your Positions      | currency,kind" << std::endl;
	std::cout << UND << "   6 "<< RST << ".Subscribe/remove channels   | action,channel" << std::endl;
	std::cout << UND << "   7 "<< RST << ".Start streaming market data | output_file_name" << std::endl;
	std::cout << UND << "   8 "<< RST << ".Stops streaming market data | -" << std::endl;
	std::cout << UND << "   9 "<< RST << ".Show this Menu              | -" << std::endl;
	std::cout << UND << "   0 "<< RST << ".Exit program                | -" << std::endl;
}

void	clear_terminal( void ) {
	std::cout << CLR_TERM;
}

std::string get_env(const char *var_name) {
	char *var = std::getenv(var_name);
	if (var)
		return std::string(var);
	else	return "";
}

