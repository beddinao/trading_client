#pragma once

#include	<string>

class	APIClient {
	std::string	api_key;
	public:
		APIClient();
		~APIClient();
		APIClient( const APIClient& );
		APIClient &operator = ( const APIClient& );

		std::string const& get_key( void ) const;
		void set_key( std::string const& );

		void place_order( void );
		void cancel_order( void );
		void modify_order( void );
		void get_order_book( void );
		void get_position( void );
};

