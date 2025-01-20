#pragma once

#include	<curl/curl.h>
#include	<sys/time.h>
#include	<JsonResponse.h>
#include	<string>
#include	<map>

# define RST     "\x1B[0m"
# define RED     "\x1B[31m"
# define GRN     "\x1B[32m"
# define YEL     "\x1B[33m"
# define BLU     "\x1B[34m"
# define AG      "\x1B[35m"
# define CYN     "\x1B[36m"
# define WHT     "\x1B[37m"
# define UND     "\033[4m"
# define BGR     "\033[7m"

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
	/*
		orders book
		: order_id -> order_json
	*/
	std::map<std::string, std::map<std::string, std::string> > orders;

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
		void place_order( std::string&, std::string&, int, double );
		void cancel_order( void );
		void modify_order( void );
		void get_order_book( void );
		void get_position( void );

		/* helper functions */
		void snap_time();
};

