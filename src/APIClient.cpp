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

bool	APIClient::authenticate( ) {

	CURL *curl = curl_easy_init();

	if (curl) {
		std::string response;
		struct curl_slist *headers = nullptr;
		
		/* request url with query string */
		std::string url = this->endpoints["url"] + "/api/v2" + this->endpoints["auth"] + "?client_id="
			+ this->client_id + "&client_secret=" + this->client_secret + "&grant_type=client_credentials";

		std::cout << std::endl << "requestURL: " << BLU << url << RST << std::endl;
		
		/* http headers */
		headers = curl_slist_append(headers, "Content-Type: application/json");

		/* curl options */
		curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
		curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_call_back);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

		/* curl to action */
		CURLcode res = curl_easy_perform(curl);
		curl_easy_cleanup(curl);
		if (res != CURLE_OK || response.empty())
			return false;

		JsonResponse json_response;
		try {
			/* converting json string response to an actual json */
			json_response.init(response);
		}
		catch (std::exception &e) {
			std::cout << "response parser failed" << std::endl;
			return false;
		}

		std::cout << std::endl;
		std::map<std::string, std::string>::iterator it = json_response.fields.begin();
		for (; it != json_response.fields.end(); ++it)
			std::cout << "[" << WHT << it->first << RST << "]-->[" << it->second << "]" << std::endl;
		std::cout << std::endl;

		/* response error test */
		if (json_response.fields.find("error") != json_response.fields.end()) {
			std::cout << "auth endpoint responded with error: " << json_response.fields["error"] << std::endl;
			return false;
		}

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
	return false;
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
	/* updating access_token if expired */
	if (!this->refresh_token()) {
		std::cout << "placing order failed due to an expired access token" << std::endl;
		return;
	}
	/* performing the transfer */
	CURL *curl = curl_easy_init();
	if (curl) {
		struct curl_slist *headers = nullptr;
		std::string response;
		std::string url = this->endpoints["url"] + "/api/v2" + this->endpoints[action] + "?"
			+ "amount=" + std::to_string(amount) + "&instrument_name=" + instrument
			+ "&price=" + std::to_string(price) + "&type=" + type;

		std::cout << std::endl << "requestURL: " << BLU << url << RST << std::endl;

		headers = curl_slist_append(headers, "Content-Type: application/json");
		headers = curl_slist_append(headers, ("Authorization: Bearer " + this->access_token).c_str());

		curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
		curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_call_back);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

		CURLcode res = curl_easy_perform(curl);
		curl_easy_cleanup(curl);

		if (res != CURLE_OK || response.empty()) {
			std::cout << "placing order failed due to connection(curl) failure" << std::endl;
			return;
		}

		JsonResponse json_response;
		try {
			json_response.init(response);
		}
		catch (std::exception &e) {
			std::cout << "response parsing failure" << std::endl;
			return;
		}

		std::cout << std::endl;
		std::map<std::string, std::string>::iterator it = json_response.fields.begin();
		for (; it != json_response.fields.end(); ++it)
			std::cout << "[" << WHT << it->first << RST << "]-->[" << it->second << "]" << std::endl;
		std::cout << std::endl;


		if (json_response.fields.find("error") != json_response.fields.end()) {
			std::cout << "buy endpoint responded with error: " << RED << json_response.fields["error"] << RST << std::endl;
			return;
		}
		if (json_response.fields.find("order") == json_response.fields.end()
			|| json_response.fields.find("order_id") == json_response.fields.end()
			|| json_response.fields["order_id"].empty()) {
			std::cout << "buy endpoint resopnse doesn't have the order fields" << std::endl;
			return;
		}
		std::cout << GRN << "Order done successfully" << RST << std::endl;
		this->orders[ json_response.fields["order_id"] ] = json_response.fields;
	}
	/* curl failed */
	else std::cout << "placing order failed due to connection(curl) failure" << std::endl;
}

void	APIClient::cancel_order( std::string &order_id ) {
	/* checking if order is present */
	if (this->orders.find(order_id) == this->orders.end()) {
		std::cout << "Order is not in OrderBook" << std::endl;
		return;
	}
	/* updating access_token if expired */
	if (!this->refresh_token()) {
		std::cout << "canceling order failed due to an expired access token" << std::endl;
		return;
	}
	CURL *curl = curl_easy_init();
	if (curl) {
		struct curl_slist *headers = nullptr;
		std::string response;
		std::string url = this->endpoints["url"] + "/api/v2" + this->endpoints["cancel"] + "?"
			+ "order_id" + order_id;
		std::cout << std::endl << "requestURL: " << BLU << url << RST << std::endl;

		headers = curl_slist_append(headers, "Content-Type: application/json");
		headers = curl_slist_append(headers, ("Authorization: Bearer " + this->access_token).c_str());

		curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
		curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_call_back);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

		CURLcode res = curl_easy_perform(curl);
		curl_easy_cleanup(curl);

		if (res != CURLE_OK || response.empty()) {
			std::cout << "placing order failed due to connection(curl) failure" << std::endl;
			return;
		}

		JsonResponse json_response;
		try {
			json_response.init(response);
		}
		catch (std::exception &e) {
			std::cout << "response parser failure" << std::endl;
			return;
		}

		std::cout << std::endl;
		std::map<std::string, std::string>::iterator it = json_response.fields.begin();
		for (; it != json_response.fields.end(); ++it)
			std::cout << "[" << WHT << it->first << RST << "]-->[" << it->second << "]" << std::endl;
		std::cout << std::endl;

		if (json_response.fields.find("error") != json_response.fields.end()) {
			std::cout << "cancel endpoint responded with error: " << RED << json_response.fields["error"] << RST << std::endl;
			return;
		}

		if (json_response.fields.find("result") == json_response.fields.end()) {
			std::cout << "cancel endpoint response doesn't have enough fields" << std::endl;
			return;
		}
		std::cout << GRN << "Order canceled successfully" << RST << std::endl;
		this->orders.erase(order_id);
	}
	else std::cout << "canceling order failed due to connection(curl) failure" << std::endl;
}
void	APIClient::modify_order(std::string &order_id, int amount, double price) {
	/* checking if order is present */
	if (this->orders.find(order_id) == this->orders.end()) {
		std::cout << "Order is not in OrderBook" << std::endl;
		return;
	}
	/* updating token if expired */
	if (!this->refresh_token()) {
		std::cout << "modifying order failed due to an expired access token" << std::endl;
		return;
	}
	CURL *curl = curl_easy_init();
	if (curl) {
		struct curl_slist *headers = nullptr;
		std::string response;
		std::string url = this->endpoints["url"] + "/api/v2" + this->endpoints["edit"] + "?"
			+ "order_id=" + order_id + "&amount=" + std::to_string(amount) 
			+ "&price=" + std::to_string(price);
		std::cout << std::endl << BLU << "requestURL: " << url << RST << std::endl;

		headers = curl_slist_append(headers, "Content-Type: application/json");
		headers = curl_slist_append(headers, ("Authorization: Bearer " + this->access_token).c_str());

		curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
		curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_call_back);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

		CURLcode res = curl_easy_perform(curl);
		curl_easy_cleanup(curl);

		if (res != CURLE_OK || response.empty()) {
			std::cout << "editing order failed due to connection(curl) failure" << std::endl;
			return;
		}

		JsonResponse json_response;
		try {
			json_response.init(response);
		}
		catch (std::exception &e) {
			std::cout << "response parser failure" << std::endl;
			return;
		}

		std::cout << std::endl;
		std::map<std::string, std::string>::iterator it = json_response.fields.begin();
		for (; it != json_response.fields.end(); ++it)
			std::cout << "[" << WHT << it->first << RST << "]-->[" << it->second << "]" << std::endl;
		std::cout << std::endl;

		if (json_response.fields.find("error") != json_response.fields.end()) {
			std::cout << "edit endpoint responded with error: " << RED << json_response.fields["error"] << RST << std::endl;
			return;
		}

		if (json_response.fields.find("result") == json_response.fields.end()) {
			std::cout << "cancel endpoint response doesn't have enough fields" << std::endl;
			return;
		}
		std::cout << GRN << "Order edited successfully" << RST << std::endl;
		this->orders.erase(order_id);
		this->orders[order_id] = json_response.fields;
		
	}
}

void	APIClient::get_order_book( ) {
	if (this->orders.empty()) {
		std::cout << "there is no pending orders" << std::endl;
		return;
	}
	std::cout << "Active Orders: " << std::endl;
	std::cout << "order_id | order_data" << std::endl;
	std::map<std::string, std::map<std::string, std::string> >::iterator it = this->orders.begin();
	for (; it != this->orders.end(); ++it) {
		std::cout << CYN"------------------------------------------------------"RST << std::endl;
		std::cout << GRN << it->first << ": "RST << std::endl;
		for (std::map<std::string, std::string>::iterator it_2 = it->second.begin(); it_2 != it->second.end(); ++it_2) {
			std::cout << BLU"------------------------------"RST << std::endl;
			std::cout << WHT << it_2->first << ": "RST << it_2->second << std::endl;
		}
		std::cout << CYN"------------------------------------------------------"RST << std::endl;
	}
}

void	APIClient::get_position( void ) {
	printf("getting position\n");
}

