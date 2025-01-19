#pragma	once

#include	<iostream>
#include	<cstdlib>
#include	<string>

#define CLR_TERM "\e[1;1H\e[2J"

/*######### APIClient #########*/

#include <APIClient.h>

/*######### WebSocketServer #########*/

#include <WebSocketServer.h>

/*######## JsonResponse ########*/

//#include <JsonResponse.h>

/*######## utils ########*/

void		show_menu( void );
void		clear_terminal( void );
std::string	get_env( const char* );
bool		get_client_info( std::string&, std::string& );
void		get_endpoints_info( std::map<std::string, std::string>& );
