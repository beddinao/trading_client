#pragma once

#include	<curl/curl.h>
#include	<sys/time.h>
#include	<JsonResponse.h>
#include	<string>

class	APIClient {
	std::string	access_token;
	unsigned		access_token_expire;
	//struct timeval	last_auth;

	public:
		APIClient();
		~APIClient();
		APIClient( const APIClient& );
		APIClient &operator = ( const APIClient& );

		std::string const& get_token( void ) const;
		void set_token( std::string const& );

		bool authenticate( std::string&, std::string& );

		void place_order( void );
		void cancel_order( void );
		void modify_order( void );
		void get_order_book( void );
		void get_position( void );
};

