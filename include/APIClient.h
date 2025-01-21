#pragma once

#include	<curl/curl.h>
#include	<sys/time.h>
#include	<utils.h>

class	APIClient {
	/* client info */
	std::string	client_id;
	std::string	client_secret;
	/* access token */
	std::string	access_token;
	unsigned		access_token_expire;
	struct timeval	last_auth;
	/* flexible endpoints */
	std::map<std::string, std::string>	endpoints;

	public:
		APIClient();
		APIClient( const APIClient& );
		APIClient &operator = ( const APIClient& );
		~APIClient();

		/* authentication */
		bool authenticate( );
		bool refresh_token( );
		void setup_client( std::string&, std::string& );
		void setup_endpoints( std::map<std::string, std::string>& );

		/* trade functions */
		void place_order( std::string&, std::string&, std::string&, int, double );
		void modify_order( std::string&, int, double );
		void cancel_order( std::string& );
		void get_position( std::string&, std::string& );
		void get_order_book( std::string& );

		/* helper functions */
		bool send_request(bool, const char*, nlohmann::json& );
		std::map<std::string, std::string> get_endpoints();
		void snap_time();
};

