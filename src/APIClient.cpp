#include <APIClient.h>

APIClient::~APIClient() { };

APIClient::APIClient() { };

APIClient::APIClient( const APIClient &A ) {
	*this = A;
}

APIClient	&APIClient::operator = ( const APIClient &A ) {
	this->access_token = A.access_token;
	return (*this);
}

/* updating current client info */
void	APIClient::setup_client( std::string &client_id, std::string &client_secret ) {
	this->client_id = client_id;
	this->client_secret = client_secret;
}

/* updating endpoints map */
void	APIClient::setup_endpoints( std::map<std::string, std::string> &endpoints ) {
	this->endpoints = endpoints;
}

std::map<std::string, std::string> APIClient::get_endpoints() {
	return this->endpoints;
}

/* taking a snapshot of current time */
void	APIClient::snap_time() {
	memset(&last_auth, 0, sizeof(struct timeval));
	gettimeofday(&last_auth, NULL);
}

/* callback for curl */
static	size_t write_call_back(void *content, size_t size, size_t nmemb, std::string *output) {
	size_t total_size = size*nmemb;
	output->append((char*)content, total_size);
	return total_size;
}

bool	APIClient::send_request(bool private_endpoint, const char *url, nlohmann::json &json_response) {
	/* updating access_token if expired */
	if (private_endpoint && !this->refresh_token()) {
		std::cout << "refreshing access_token after expiration failed" << std::endl;
		return false;
	}
	/* printing url for clearence */
	std::string Url(url);
	std::cout << "Requesting URL:[" << CYN << Url.substr(0, Url.find("?")) << RST << "]" << std::endl;
	std::string response;
	CURL *curl = curl_easy_init();
	if (curl) {
		/* http headers */
		struct curl_slist *headers = nullptr;
		headers = curl_slist_append(headers, "Content-Type: application/json");
		if (private_endpoint)
			/* private endpoints require access_token */
			headers = curl_slist_append(headers, ("Authorization: Bearer " + this->access_token).c_str());
		/* curl options */
		curl_easy_setopt(curl, CURLOPT_URL, url);
		curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_call_back);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
		/* curl to action */
		CURLcode res = curl_easy_perform(curl);
		curl_easy_cleanup(curl);
		if (res != CURLE_OK || response.empty()) {
			std::cout << "connection(curl) failure" << std::endl;
			return false;
		}
		try {
			/* converting json string response to an actual json */
			json_response = nlohmann::json::parse(response);
		}
		catch (std::exception &e) {
			std::cout << "json parser failure" << std::endl;
			return false;
		}
		/* response global error test */
		if (json_response.contains("error")) {
			std::cout << "endpoint responded with error: " << RED << json_response["error"] << RST << std::endl;
			return false;
		}
		return true;
	}
	return false;
}

bool	APIClient::authenticate( ) {
	nlohmann::json json_response;
	bool status = this->send_request(false,
			(this->endpoints["url"] + "/api/v2" + this->endpoints["auth"] + "?"
				+ "client_id=" + this->client_id + "&client_secret=" + this->client_secret
				+ "&grant_type=client_credentials").c_str(),
			json_response);
	if (!status)
		return status;
	/* basic access token retrieval tests */
	if (json_response["result"]["access_token"] == NULL || json_response["result"]["access_token"].empty()) {
		std::cout << "auth endpoint response doesn't have an access token" << std::endl;
		return false;
	}

	/* access token just landed along it's expire time in seconds */
	this->access_token_expire = json_response["result"]["expires_in"];
	this->access_token = json_response["result"]["access_token"];
	return true;
}

bool	APIClient::refresh_token( ) {
	double time_diff;
	struct timeval time_now;

	/* getting the time pased from the last access token retrieval */
	memset(&time_now, 0, sizeof(time_now));
	gettimeofday(&time_now, NULL);
	time_diff = (time_now.tv_sec + time_now.tv_usec / 1000000)
		- (this->last_auth.tv_sec + this->last_auth.tv_usec / 1000000);

	/* checking if the time pased exceds the current token expire time */
	if (time_diff >= this->access_token_expire) {
		if (!this->authenticate()) {
			std::cout << "failed to refresh expired token" << std::endl;
			return false;
		}
		/* updating the time for next time refresh */
		this->last_auth.tv_sec = time_now.tv_sec;
		this->last_auth.tv_usec = time_now.tv_usec;
	}
	return true;	
}

void	APIClient::place_order(std::string &action, std::string &instrument, std::string &type, int amount, double price) {
	nlohmann::json json_response;
	/* forming the query string from user input */
	bool status = this->send_request(true,
			(this->endpoints["url"] + "/api/v2" + this->endpoints[action] + "?"
				+ "amount=" + std::to_string(amount) + "&instrument_name=" + instrument
				+ "&price=" + std::to_string(price) + "&type=" + type).c_str(),
			json_response);
	if (!status) {
		std::cout << RED << "error: " << RST << "placing order failed" << std::endl;
		return;
	}
	/*
		basic response validation,
		successfull responds from deribit api always have the result field
	*/
	if (json_response["result"]["order"] == NULL
			|| json_response["result"]["order"]["order_id"] == NULL
			|| json_response["result"]["order"]["order_id"].empty()) {
		std::cout << RED "error: " << RST << "buy endpoint resopnse doesn't have the order fields" << std::endl;
		return;
	}

	std::cout << GRN << "--> order placed successfully:" << RST << std::endl;
	std::cout << WHT << "order id == " << RST << "[" << json_response["result"]["order"]["order_id"] << "]" << std::endl;
	std::cout << WHT << "direction == " << RST << "[" << json_response["result"]["order"]["direction"] << "]" << std::endl;
	std::cout << WHT << "instrument == " << RST << "[" << json_response["result"]["order"]["instrument_name"] << "]" << std::endl;
	std::cout << WHT << "amount == " << RST << "[" << json_response["result"]["order"]["amount"] << "]" << std::endl;
	std::cout << WHT << "order state == " << RST << "[" << json_response["result"]["order"]["order_state"] << "]" << std::endl;
	std::cout << WHT << "order type == " << RST << "[" << json_response["result"]["order"]["order_type"] << "]" <<  std::endl;
}

void	APIClient::cancel_order( std::string &order_id ) {
	nlohmann::json json_response;
	bool status = this->send_request(true,
			(this->endpoints["url"] + "/api/v2" + this->endpoints["cancel"] + "?"
				+ "order_id=" + order_id).c_str(),
			json_response);
	if (!status) {
		std::cout << RED << "error: " << RST << "canceling order failed" << std::endl;
		return;
	}
	if (!json_response.contains("result")) {
		std::cout << RED << "error: " << RST << "cancel endpoint response doesn't have enough fields" << std::endl;
		return;
	}
	
	std::cout << GRN << "--> order canceled successfully:" << RST << std::endl;
	std::cout << WHT << "order id == " << RST << "[" << json_response["result"]["order_id"] << "]" << std::endl;
	std::cout << WHT << "order state == " << RST << "[" << json_response["result"]["order_state"] << "]" << std::endl;
	std::cout << WHT << "order type == " << RST << "[" << json_response["result"]["order_type"] << "]" <<  std::endl;
}
void	APIClient::modify_order(std::string &order_id, int amount, double price) {
	nlohmann::json json_response;
	bool status = this->send_request(true,
			(this->endpoints["url"] + "/api/v2" + this->endpoints["edit"] + "?"
				+ "order_id=" + order_id + "&amount=" + std::to_string(amount) 
				+ "&price=" + std::to_string(price)).c_str(),
			json_response);
	if (!status) {
		std::cout << RED << "error: " << RST << "editing order failed" << std::endl;
		return;
	}

	if (!json_response.contains("result")) {
		std::cout << RED << "error: " << RST << "cancel endpoint response doesn't have enough fields" << std::endl;
		return;
	}

	std::cout << GRN << "--> order edited successfully" << RST << std::endl;
	std::cout << WHT << "order id == " << RST << "[" << json_response["result"]["order"]["order_id"] << "]" << std::endl;
	std::cout << WHT << "direction == " << RST << "[" << json_response["result"]["order"]["direction"] << "]" << std::endl;
	std::cout << WHT << "instrument == " << RST << "[" << json_response["result"]["order"]["instrument_name"] << "]" << std::endl;
	std::cout << WHT << "amount == " << RST << "[" << json_response["result"]["order"]["amount"] << "]" << std::endl;
	std::cout << WHT << "order state == " << RST << "[" << json_response["result"]["order"]["order_state"] << "]" << std::endl;
	std::cout << WHT << "order type == " << RST << "[" << json_response["result"]["order"]["order_type"] << "]" <<  std::endl;
}

void	APIClient::get_order_book(std::string &instrument) {
	nlohmann::json json_response;
	bool status = this->send_request(false,
			(this->endpoints["url"] + "/api/v2" + this->endpoints["order_book"] + "?"
			 "instrument_name=" + instrument).c_str(),
			json_response);
	if (!status) {
		std::cout << RED << "error: " << RST << "getting order book failed" << std::endl;
		return;
	}

	if (!json_response.contains("result")
		|| json_response["result"].empty()) {
		std::cout << RED << "error: " << RST << this->endpoints["order_book"]
			<< " endpoint response doesn't have enough fields" << std::endl;
		return;
	}
	std::cout << GRN << "--> order book retrived successfully:" << RST << std::endl;
	std::cout << json_response.dump(2) << std::endl;
}

void	APIClient::get_position(std::string &currency, std::string &kind) {
	nlohmann::json json_response;
	bool status = this->send_request(true,
			(this->endpoints["url"] + "/api/v2" + this->endpoints["position"] + "?"
			 + "currency=" + currency + "&kind=" + kind).c_str(),
			json_response);

	if (!status) {
		std::cout << RED << "error: " << RST << "getting position failed" << std::endl;
		return;
	}
	
	if (!json_response.contains("result")) {
		std::cout << RED << "error: " << RST << "position endpoint reponse doesn't have enough fields" << std::endl;
		return;
	}
	std::cout << GRN << "--> position retrieved successfully:" << RST << std::endl;
	std::cout << json_response.dump(2) << std::endl;
}

