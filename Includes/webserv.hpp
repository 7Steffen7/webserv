// #pragma once
#ifndef WEBSERV_HPP
#define WEBSERV_HPP

//Includes -- BEGIN
// #include "requestParsing.hpp"
#include "serverConfig.hpp"
#include "token.hpp"
#include "parser.hpp"
#include <iostream>
#include <string>
#include <string.h>
#include <cstring>
#include <sstream>
#include <fstream>
#include "Extra/printer.hpp"
//Beej's Buch
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <poll.h>
#include <errno.h>
#include <sys/wait.h>
#include <signal.h>
#include <memory>
#include <vector>
#include <unordered_map>
#include "printer.hpp"
#include <fcntl.h>
#include "Connection.hpp"
#include "serverConfig.hpp"
#include <chrono> // std::chrono
#include <charconv>
//Includes -- END

//Defines -- BEGIN
# define MYPORT "3490" // the port users will be connecting to
# define BACKLOG 10 // how many pending connections queue will hold
#define PORT "9034"   // Port we're listening on
//Defines -- END

//Headers -- BEGIN
#include "playground.hpp"
#include "Server.hpp"
//Headers -- END

//FNC-Prototypes -- BEGIN
void	src_function(void);
//FNC-Prototypes -- END

#endif
