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

void	APIClient::setup_client( std::string &client_id, std::string &client_secret ) {
	this->client_id = client_id;
	this->client_secret = client_secret;
	std::cout << "Client info: " << std::endl;
	std::cout << "client_id: " << client_id << std::endl;
	std::cout << "client_secret: " << client_secret << std::endl;
	std::cout << std::endl;
}

void	APIClient::setup_endpoints( std::map<std::string, std::string> &endpoints ) {
	this->endpoints = endpoints;
	std::cout << "Endpoints info: " << std::endl;
	std::map<std::string, std::string>::iterator it = endpoints.begin();
	for (; it != endpoints.end(); ++it)
		std::cout << it->first << ": " << it->second << std::endl;
	std::cout << std::endl;
}

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

		std::string url = this->endpoints["url"] + "/api/v2" + this->endpoints["auth"] + "?client_id="
			+ this->client_id + "&client_secret=" + this->client_secret + "&grant_type=client_credentials";

		headers = curl_slist_append(headers, "Content-Type: application/json");
		curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
		curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_call_back);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

		CURLcode res = curl_easy_perform(curl);
		curl_easy_cleanup(curl);
		if (res != CURLE_OK)
			return false;

		JsonResponse json_response(response);

		this->access_token = json_response.fields["token_type"] + " " + json_response.fields["access_token"];
		this->access_token_expire = std::atoi(json_response.fields["expires_in"].c_str());

		/*std::cout << "access_token: " << this->access_token << std::endl;
		std::cout << "access_token_expire: " << this->access_token_expire << std::endl;*/
		return true;
	}
	return false;
}

bool	APIClient::refresh_token( ) {
	double time_diff;
	struct timeval time_now;

	memset(&time_now, 0, sizeof(time_now));
	gettimeofday(&time_now, NULL);

	time_diff = (time_now.tv_sec + time_now.tv_usec / 1000000)
		- (this->last_auth.tv_sec + this->last_auth.tv_usec / 1000000);

	if (time_diff >= this->access_token_expire) {
		if (!this->authenticate()) {
			std::cout << "failed to refresh expired token" << std::endl;
			return false;
		}
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

