#pragma once

#include <istream>
#include <string>
#include <optional>
#include <unordered_map>
#include <regex>
#include <iostream>
#include <cassert>
#include <sstream>
#include <fstream>
#include <sys/stat.h>
#include <ctime>
//#include <serverConfig.hpp>


typedef struct Uri {
	std::string	full;
	std::string	authority;
	std::string	host;
	std::string	port;
	std::string	path;
	std::string	query;
} uri;

typedef struct Request {
	std::optional<std::string>						method = std::nullopt;
	std::optional<Uri>								uri = std::nullopt;
	std::optional<std::string>						version = std::nullopt;
	std::unordered_map<std::string, std::string>	headers;
	std::optional<std::string>						body = std::nullopt;
	std::optional<int>								status_code = std::nullopt;
} Request;

class RequestParser {
public:
				RequestParser(void) = delete;
				RequestParser(std::string &input);
				~RequestParser();
	bool		parse_request_line(void);
	bool		parse_uri(void);
	bool		parse_headers(void);
	bool		parse_body(int max_body_len);
	Request		&&getRequest(void);

	Request		request;
	std::string &input;
private:

	void	setStatus(int status);
	bool	finished_request_line = false;
	bool	finished_headers = false;
	bool	finished = false;

	long					pos;
	const static std::regex	request_line_pat;
	const static std::regex	uri_pat;
	const static std::regex	header_name_pat;
	const static std::regex	header_value_pat;
	/*only called by a macro */

	bool	parse_not_encoded_body(int max_body_len);
	bool	parse_assertion_exec(bool cond, const char *str_cond,
						const char *file, const int line, const char *fn_str);
};


std::ostream& operator<<(std::ostream& output, Uri uri);
std::ostream& operator<<(std::ostream &output, const Request &request);

