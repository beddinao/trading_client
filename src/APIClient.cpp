#include <APIClient.h>

APIClient::APIClient() { };
APIClient::~APIClient() { };

APIClient::APIClient( const APIClient &A ) {
	*this = A;
}

APIClient	&APIClient::operator = ( const APIClient &A ) {
	set_key( A.get_key() );
	return (*this);
}

std::string const& APIClient::get_key( void ) const {
	return (this->api_key);
}

void	APIClient::set_key( std::string const& new_key ) {
	this->api_key = new_key;
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

