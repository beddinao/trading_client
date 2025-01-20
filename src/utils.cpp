#include <trader.h>


bool	get_client_info(std::string &client_id, std::string &client_secret) {
	/* retrieving client id and falling back */
	client_id = get_env("CLIENT_ID");
	if (client_id.empty()) {
		std::cout << "CLIENT_ID is not set in env, using the DEFAULT_CLIENT_ID" << std::endl;
		client_id = get_env("DEFAULT_CLIENT_ID");
	}
	/* retrieving client secret and failing back */
	client_secret = get_env("CLIENT_SECRET");
	if (client_secret.empty()) {
		std::cout << "CLIENT_SECRET is not set, using the DEFAULT_CLIENT_SECRET" << std::endl;
		client_secret = get_env("DEFAULT_CLIENT_SECRET");
	}
	std::cout << std::endl;
	if (client_id.empty() || client_secret.empty()) {
		std::cout << "failed to retrieve deribit account info" << std::endl;
		return false;
	}
	return true;
}

void	get_endpoints_info(std::map<std::string, std::string> &endpoints) {
	endpoints["url"] = get_env("SITE_URL");
	if (endpoints["url"].empty()) {
		endpoints["url"] = "https://test.deribit.com";
		std::cout << "custom SITE_URL is not set, using: "
			<< endpoints["url"] << std::endl;
	}
	/* endpoint used for authentication: public */
	endpoints["auth"] = get_env("ENDPOINT_AUTH");
	if (endpoints["auth"].empty()) {
		endpoints["auth"] = "/public/auth";
		std::cout << "custom ENDPOINT_AUTH is not set, using: "
			<< endpoints["auth"] << std::endl;
	}
	/* endpoint used for buying: private */
	endpoints["buy"] = get_env("ENDPOINT_BUY");
	if (endpoints["buy"].empty()) {
		endpoints["buy"] = "/private/buy";
		std::cout << "custom ENDPOINT_BUY is not set, using: "
			<< endpoints["buy"] << std::endl;
	}
	/* endpoinr used for selling: private */
	endpoints["sell"] = get_env("ENDPOINT_SELL");
	if (endpoints["sell"].empty()) {
		endpoints["sell"] = "/private/sell";
		std::cout << "custom ENDPOINT_SELL is not set, using: "
			<< endpoints["sell"] << std::endl;
	}
	/* endpoint used for editing books: private */
	endpoints["edit"] = get_env("ENDPOINT_EDIT");
	if (endpoints["edit"].empty()) {
		endpoints["edit"] = "/private/edit";
		std::cout << "custom ENDPOINT_EDIT is not set, using: "
			<< endpoints["edit"] << std::endl;
	}
	/* endpoint used of getting position */
	endpoints["cancel"] = get_env("ENDPOINT_CANCEL");
	if (endpoints["cancel"].empty()) {
		endpoints["cancel"] = "/private/cancel";
		std::cout << "custom ENDPOINT_CANCEL is not set, using: "
			<< endpoints["cancel"] << std::endl;
	}
	std::cout << std::endl;
}

void	show_menu() {
	std::cout << "\nTrading System Menu:\n" << std::endl;
	std::cout << "1. Place Order" << std::endl;
	std::cout << "2. Cancel Order" << std::endl;
	std::cout << "3. Modify Order" << std::endl;
	std::cout << "4. View Orderbook" << std::endl;
	std::cout << "5. View Open Positions" << std::endl;
	std::cout << "6. Start WebSocket Market Data" << std::endl;
	std::cout << "0. Exit" << std::endl << std::endl;
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

