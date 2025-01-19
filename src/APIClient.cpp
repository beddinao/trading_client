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

		std::cout << "raw response: " << response << std::endl;
		/* converting json string response to actual json */
		JsonResponse json_response(response);

		std::cout << std::endl;
		std::map<std::string, std::string>::iterator it = json_response.fields.begin();
		for (; it != json_response.fields.end(); ++it)
			std::cout << "[" << it->first << "]-->[" << it->second << "]" << std::endl;
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
		this->access_token = json_response.fields["token_type"] + " " + json_response.fields["access_token"];
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

void	APIClient::place_order( void ) {
	printf("placing an order\n");
}
void	APIClient::cancel_order( void ) {
	printf("canceling an order\n");
}
void	APIClient::modify_order( void ) {
	printf("modifying an order\n");
}

void	APIClient::get_order_book( void ) {
	printf("getting order book\n");
}

void	APIClient::get_position( void ) {
	printf("getting position\n");
}

