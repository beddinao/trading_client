#pragma once

#include	<curl/curl.h>
#include	<sys/time.h>
#include	<JsonResponse.h>
#include	<string>
#include	<map>

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
		void place_order( void );
		void cancel_order( void );
		void modify_order( void );
		void get_order_book( void );
		void get_position( void );

		/* helper functions */
		void snap_time();
};

