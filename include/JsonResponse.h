#pragma once

#include <string>
#include <iostream>
#include <map>

class	JsonResponse {
	public:
		JsonResponse();
		~JsonResponse();
		JsonResponse( const JsonResponse& );
		JsonResponse &operator = ( const JsonResponse& );
		std::map<std::string, std::string> fields;
		void init( std::string& );
};
