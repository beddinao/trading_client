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

bool	APIClient::send_request(bool private_endpoint, const char *url, JsonResponse *json_response) {
	/* updating access_token if expired */
	if (private_endpoint && !this->refresh_token()) {
		std::cout << "refreshing access_token after expiration failed" << std::endl;
		return false;
	}
	std::cout << std::endl << CYN << "requestURL:" << url << RST << std::endl;
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
		/* for string manipulation safety */
		try {
			/* converting json string response to an actual json */
			json_response->init(response);
		}
		catch (std::exception &e) {
			std::cout << "json parser failure" << std::endl;
			return false;
		}
		//
		std::cout << std::endl;
		std::map<std::string, std::string>::iterator it = json_response->fields.begin();
		for (; it != json_response->fields.end(); ++it)
			std::cout << "[" << WHT << it->first << RST << "]-->[" << it->second << "]" << std::endl;
		std::cout << std::endl;
		//
		/* response global error test */
		if (json_response->fields.find("error") != json_response->fields.end()) {
			std::cout << "got the response error: " << json_response->fields["error"] << std::endl;
			return false;
		}
		return true;
	}
	return false;
}

bool	APIClient::authenticate( ) {
	JsonResponse json_response;
	bool status = this->send_request(false,
			(this->endpoints["url"] + "/api/v2" + this->endpoints["auth"] + "?"
				+ "client_id=" + this->client_id + "&client_secret=" + this->client_secret
				+ "&grant_type=client_credentials").c_str(),
			&json_response);
	if (!status)
		return status;
	/* basic access token retrieval tests */
	if (json_response.fields.find("access_token") == json_response.fields.end()
			|| json_response.fields["access_token"].empty()) {
		std::cout << "auth endpoint response doesn't have an access token" << std::endl;
		return false;
	}

	/* access token just landed */
	this->access_token = json_response.fields["access_token"];
	this->access_token_expire = std::atoi(json_response.fields["expires_in"].c_str());
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
	JsonResponse json_response;
	bool status = this->send_request(true,
			(this->endpoints["url"] + "/api/v2" + this->endpoints[action] + "?"
				+ "amount=" + std::to_string(amount) + "&instrument_name=" + instrument
				+ "&price=" + std::to_string(price) + "&type=" + type).c_str(),
			&json_response);
	if (!status) {
		std::cout << "placing order failed" << std::endl;
		return;
	}

	if (json_response.fields.find("order") == json_response.fields.end()
			|| json_response.fields.find("order_id") == json_response.fields.end()
			|| json_response.fields["order_id"].empty()) {
		std::cout << "buy endpoint resopnse doesn't have the order fields" << std::endl;
		return;
	}

	std::cout << GRN << "Order done successfully" << RST << std::endl;
}

void	APIClient::cancel_order( std::string &order_id ) {
	JsonResponse json_response;
	bool status = this->send_request(true,
			(this->endpoints["url"] + "/api/v2" + this->endpoints["cancel"] + "?"
				+ "order_id" + order_id).c_str(),
			&json_response);

	if (!status) {
		std::cout << "canceling order failed" << std::endl;
		return;
	}
	if (json_response.fields.find("result") == json_response.fields.end()) {
		std::cout << "cancel endpoint response doesn't have enough fields" << std::endl;
		return;
	}

	std::cout << GRN << "Order canceled successfully" << RST << std::endl;
}
void	APIClient::modify_order(std::string &order_id, int amount, double price) {
	JsonResponse json_response;
	bool status = this->send_request(true,
			(this->endpoints["url"] + "/api/v2" + this->endpoints["edit"] + "?"
				+ "order_id=" + order_id + "&amount=" + std::to_string(amount) 
				+ "&price=" + std::to_string(price)).c_str(),
			&json_response);

	if (!status) {
		std::cout << "editing order failed" << std::endl;
		return;
	}

	if (json_response.fields.find("result") == json_response.fields.end()) {
		std::cout << "cancel endpoint response doesn't have enough fields" << std::endl;
		return;
	}
	std::cout << GRN << "Order edited successfully" << RST << std::endl;
}

void	APIClient::get_order_book(std::string &instrument) {
	JsonResponse json_response;
	bool status = this->send_request(false,
			(this->endpoints["url"] + "/api/v2" + this->endpoints["order_book"] + "?"
			 "instrument_name=" + instrument).c_str(),
			&json_response);
	if (!status) {
		std::cout << "getting order book failed" << std::endl;
		return;
	}

	if (json_response.fields.find("result") == json_response.fields.end()
		|| json_response.fields["result"].empty()) {
		std::cout << this->endpoints["order_book"]
			<< " endpoint response doesn't have enough fields" << std::endl;
		return;
	}
	std::cout << GRN"order book retrived successfully"RST << std::endl;
}

void	APIClient::get_position(std::string &currency, std::string &kind) {
	JsonResponse json_response;
	bool status = this->send_request(true,
			(this->endpoints["url"] + "/api/v2" + this->endpoints["position"] + "?"
			 + "currency=" + currency + "&kind=" + kind).c_str(),
			&json_response);

	if (!status) {
		std::cout << "getting position failed" << std::endl;
		return;
	}
	
	if (json_response.fields.find("result") == json_response.fields.end()
		|| json_response.fields.find("size") == json_response.fields.end()
		|| json_response.fields["size"].empty()) {
		std::cout << "position endpoint reponse doesn't have enough fields" << std::endl;
		return;
	}
	std::cout << GRN << "position retrieved successfully" << RST << std::endl;
}

