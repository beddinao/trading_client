#include <JsonResponse.h>

std::string &polish_trim(std::string &input, char c) {
	input.erase( input.find_last_not_of( c ) + 1 );
	input.erase( 0, input.find_first_not_of( c ) );
	return input;
}

JsonResponse::JsonResponse( std::string &input ) {
	std::string name;
	std::string val;
	size_t input_pos;
	size_t buff_pos;

	for (; (input_pos = input.find(",")) != std::string::npos;) {
		std::string buffer(input.substr(0, input_pos));
		input = input.substr(input_pos + 1);

		buff_pos = buffer.find(":");
		name = buffer.substr(0, buff_pos);
		val = buffer.substr(buff_pos + 1);

		name = polish_trim(name, '{');
		val = polish_trim(val, '}');
		name = polish_trim(name, '\"');
		val = polish_trim(val, '\"');

		this->fields[name] = val;
	}
}

JsonResponse::~JsonResponse() { };

JsonResponse::JsonResponse( const JsonResponse &J ) {
	*this = J;
}

JsonResponse &JsonResponse::operator = ( const JsonResponse &J ) {
	this->fields = J.fields;
	return *this;
}
