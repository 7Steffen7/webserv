#include "../../Includes/cgi/cgi.hpp"
#include <vector>
#include <filesystem>

CGI::CGI() :    _is_finished(false),
                _state(INIT) {
    _method = "GET";
    _query_string = "name=joe&name=bill";
    _script = "/Users/sparth/Documents/Tests/webservTests/get.py";
    // _script = "/Users/sparth/Documents/Tests/webservTests/TestCgiPath/Test/getPath.py";

    // _env.emplace_back("REQUEST_METHOD=GET");
    // _env.emplace_back("SCRIPT_NAME=/Users/sparth/Documents/Tests/webservTests/hello.py");
    // _env.emplace_back("QUERY_STRING=name=joe&name=bill");
    // _env.emplace_back("CONTENT_TYPE=");
    // _env.emplace_back( "CONTENT_LENGTH=");
    // _env.emplace_back("PATH_INFO=");
    // todo add header !!!!
    _envp.emplace_back((char *)"REQUEST_METHOD=GET");
    _envp.emplace_back((char *)"SCRIPT_NAME=/Users/sparth/Documents/Tests/webservTests/get.py");
    // _envp.emplace_back((char *)"SCRIPT_NAME=/Users/sparth/Documents/Tests/webservTests/getWait.py"); // Test Timeout
    // _envp.emplace_back((char *)"SCRIPT_NAME=/Users/sparth/Documents/Tests/webservTests/TestCgiPath/Test/getPath.py");
    _envp.emplace_back((char *)"QUERY_STRING=name=joe&name=bill");
    _envp.emplace_back((char *)"CONTENT_TYPE=");
    _envp.emplace_back((char *) "CONTENT_LENGTH=");
    _envp.emplace_back((char *)"PATH_INFO=");
    _envp.emplace_back(nullptr);

    _argv.emplace_back((char *)"/usr/local/bin/python3.9");
    _argv.emplace_back((char *)"/Users/sparth/Documents/Tests/webservTests/get.py");
    // _argv.emplace_back((char *)"/Users/sparth/Documents/Tests/webservTests/getWait.py"); // Test Timeout
    // _argv.emplace_back((char *)"/Users/sparth/Documents/Tests/webservTests/TestCgiPath/Test/getPath.py");
    _argv.emplace_back(nullptr);

    // _method = "GET";
    // _query_string = "name=joe&name=bill";
    // _script = "/Users/sparth/Documents/Tests/webservTests/TestCgiPath/Test/getPath.py";
}

CGI::~CGI() = default;

std::string toUpperCase(std::string str) {
    std::transform(str.begin(), str.end(), str.begin(), ::toupper);
    return str;
}

// CGI::CGI(const Request& request) :  _is_finished(false),
//                                     _state(INIT) {
//     if (request.method.has_value()) {
//         _method = request.method.value();
//         _env.emplace_back("REQUEST_METHOD=" + toUpperCase(_method) );
//     }
//     if (_method == "POST" && request.body.has_value()) {
//         _env.emplace_back("CONTENT_LENGTH=" + std::to_string(request.body.value().size()));
//     }
//     if (request.uri.has_value()) {
//         if (!request.uri->query.empty()) {
//             _env.emplace_back("QUERY_STRING=" + request.uri->query);
//         }
//     }
//     if (_method == "POST") {
//         std::string temp = request.headers.find("Content-Length");
//     }
// }

bool CGI::getIsFinished() const {
    return _is_finished;
}

void CGI::cgiProcess() {
    if (close(_pipeIn[1]) == -1 || close(_pipeOut[0]) == -1
    || dup2(_pipeIn[0], STDIN_FILENO) == - 1 || dup2(_pipeOut[1], STDOUT_FILENO) == -1
    || close(_pipeIn[0]) == -1 || close(_pipeOut[1]) == -1) {
        std::cerr << "Internal CGI error" << std::endl;
        exit(1);
    }

    std::size_t pos = _script.rfind('/');
    if (pos != std::string::npos) {
        std::string temp = _script.substr(0, pos);
        std::filesystem::current_path(_script.substr(0, pos));
    }
    std::cerr << "script location: " << std::filesystem::current_path() << std::endl;
    execve(_argv[0], _argv.data(), _envp.data());
    std::cerr << "Internal CGI error" << std::endl;
    exit(1);
}

void CGI::readCgiOutput() {
    constexpr size_t BUFFER_SIZE = 96;
    char buffer[BUFFER_SIZE];

    ssize_t bytesRead = read(_pipeOut[0], buffer, BUFFER_SIZE);
    if (bytesRead < 0) {
        std::cerr << "Error reading from pipe." << std::endl;
        close(_pipeOut[0]);
        _is_finished = true;
    }
    if (bytesRead == 0) {
        std::cout << "Finish reading" << std::endl;
        close(_pipeOut[0]);
        _is_finished = true;
        std::cout << "read Data: " << _output << std::endl;
    }
    _output.append(buffer, static_cast<size_t>(bytesRead));
}


void CGI::runCgi() {
    if (_state == INIT) {
        // int pipeIn[2], _pipeOut[2];

        if (pipe(_pipeIn) == -1 || pipe(_pipeOut) == -1) {
            std::cout << "pipe failed" << std::endl;
            exit(1);
        }
        fcntl(_pipeIn[1], F_SETFD, O_NONBLOCK); // write end
        fcntl(_pipeOut[0], F_SETFD, O_NONBLOCK); // read end

        _pid = fork();
        if (_pid == -1) {
            std::cout << "fork failed" << std::endl;
            exit (1);
        }
        if (_pid == 0) {
            cgiProcess();
        }
        if (close(_pipeIn[0]) == -1 || close(_pipeOut[1])) {
            std::cout << "internal server error" << std::endl;
        }
        if (_method == "POST") {
            _state = WRITE;
        } else {
            _state = WAIT;
            _start = std::chrono::high_resolution_clock::now();
        }
    }
    if (_state == WRITE) {
            // Todo Should be non-blocking !!!
            write(_pipeIn[1], "whats up", 8);
            _state = WAIT;
            _start = std::chrono::high_resolution_clock::now();
    }
    if (_state == WAIT) {
        // auto start = std::chrono::high_resolution_clock::now();
        pid_t process = waitpid(_pid, &_wpidstatus, WNOHANG);
        auto current = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(current - _start);
        // std::cout << "duration: " << duration.count() << std::endl;
        if (duration.count() >= CGI_TIMEOUT) {
            std::cout << "CGI_TIMEOUT after " << duration.count() << std::endl;
            kill(_pid, SIGKILL);
           _is_finished = true;
        } else if (process == _pid) {
            std::cout << "Script finished in " << duration.count() << std::endl;
            _state = READ;
        }
    }
    if (_state == READ) {
        readCgiOutput();
    }
}