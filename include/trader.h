#pragma	once

#include	<iostream>
#include	<stdio.h>

#define CLR_TERM "\e[1;1H\e[2J"

/*######### APIClient #########*/

#include <APIClient.h>

/*######### WebSocketServer #########*/

#include <WebSocketServer.h>

/*######## utils ########*/

void	show_menu( void );
void	clear_terminal( void );
