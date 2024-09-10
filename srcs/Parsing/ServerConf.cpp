#include "Parsing/ServerConf.hpp"
#include "Parsing/Location.hpp"
#include "webserv.hpp"

/* ------------------- CONSTRUCTORS ----------------------*/

ServerConf::ServerConf() : _ipAddr("127.0.0.1") {}

ServerConf::ServerConf(const std::string& configString) : _ipAddr("127.0.0.1"){
	std::stringstream	configStream(configString);
	std::string			line, locationString, locationPath;
	//long				bigPortCheck;
	short				port;
	char				*ptr;

	while(std::getline(configStream, line))
	{
		line.erase(0, line.find_first_not_of(WHITESPACES));
		line.erase(line.find_last_not_of(WHITESPACES) + 1);
		if (!line.find("server_name")) {
			line.erase(0, line.find_first_not_of("server_name"));
			line.erase(0, line.find_first_not_of(WHITESPACES));
			line.erase(line.find_last_not_of(WHITESPACES) + 1);
			this->_serverName = line;
		}
		else if (!line.find("root")) {
			line.erase(0, line.find_first_not_of("root"));
			line.erase(0, line.find_first_not_of(WHITESPACES));
			line.erase(line.find_last_not_of(WHITESPACES) + 1);
			this->_root = line;
		}
		else if (!line.find("index")) {
			line.erase(0, line.find_first_not_of("index"));
			line.erase(0, line.find_first_not_of(WHITESPACES));
			line.erase(line.find_last_not_of(WHITESPACES) + 1);
			this->_index = line;
		}
		else if (!line.find("error_page")) {
			line.erase(0, line.find_first_not_of("error_page"));
			line.erase(0, line.find_first_not_of(WHITESPACES));
			line.erase(line.find_last_not_of(WHITESPACES) + 1);
			this->_errorPage = line;
		}
		else if (!line.find("listen")) {
			line.erase(0, line.find_first_not_of("listen"));
			line.erase(0, line.find_first_not_of(WHITESPACES));
			line.erase(line.find_last_not_of(WHITESPACES) + 1);
			port = strtol(line.c_str(), &ptr, 10);
			this->_port.push_back(port);
		}
		else if (!line.find("max_body_size")){
			line.erase(0, line.find_first_not_of("max_body_size"));
			line.erase(0, line.find_first_not_of(WHITESPACES));
			line.erase(line.find_last_not_of(WHITESPACES) + 1);
			this->_maxBodySize = line;
		}
		else if (!line.find("location ")) {
			line.erase(0, line.find_first_not_of("location"));
			line.erase(0, line.find_first_not_of(WHITESPACES));
			locationPath = line.substr(0, line.find_first_of(WHITESPACES));
			while (line.find('}') == std::string::npos) {
				std::getline(configStream, line);
				locationString += line + '\n';
			}
			this->_location[locationPath] = Location(locationPath, locationString.substr(0, locationString.find_last_of('}')));
			locationString.clear();
		}
		else if (line.empty())
			continue ;
		else
			throw InvalidServerConfException();
	}
	checkAttribut();
}

/* ------------------- COPY CONSTRUCTOR ----------------------*/
ServerConf::ServerConf(ServerConf const &cpy){
    *this = cpy;
}

/* ------------------- DESTRUCTOR ----------------------*/
ServerConf::~ServerConf(){}

/* ------------------- SURCHARGED OPERATOR ----------------------*/
ServerConf  &ServerConf::operator=(ServerConf const &rhs){
    this->_serverName = rhs._serverName;
    this->_port = rhs._port;
    this->_root = rhs._root;
    this->_index = rhs._index;
	this->_maxBodySize = rhs._maxBodySize;
    this->_errorPage = rhs._errorPage;
	this->_ipAddr = rhs._ipAddr;
    this->_location = rhs._location;
    return (*this);
}

/* ------------------- GETTERS ----------------------*/
std::string ServerConf::getServerName(void) const{
    return (this->_serverName);
}

std::vector<unsigned short> ServerConf::getPort(void) const{
    return (this->_port);
}

std::string ServerConf::getRoot(void) const{
    return (this->_root);
}

std::string ServerConf::getIndex(void) const{
    return (this->_index);
}

std::string ServerConf::getMaxBodySize(void) const{
	return (this->_maxBodySize);
}

std::string	ServerConf::getErrorPage(void) const{
	return (this->_errorPage);
}
std::map<std::string, Location> ServerConf::getLocation(void) const{
    return (this->_location);
}

std::string ServerConf::getIpAddr(void)	const{
    return (this->_ipAddr);
}

/* ------------------- MEMBERS FUNCTIONS ----------------------*/
void     ServerConf::checkAttribut(void) const{
    std::vector<unsigned short>::const_iterator it;

    if (_serverName.empty() || _port.empty() || _errorPage.empty() || _maxBodySize.empty() || _location.empty()){
		std::cerr << "Error: " << RED << "Missing args in server configuration. " << RED << "\n" << RESET;
        throw InvalidServerConfException();
	}
	for (it = _port.begin(); it != _port.end(); it++){
		if (*it < 1 || *it > 65534){
			std::cerr << "Error: " << RED << "Port number is out of range." << RED << std::endl << RESET;
			throw InvalidServerConfException();
		}
	}
    if (_root[0] != '/'){
		std::cerr << "Error: " << RED << "Invalid root path. " << RED << "\n" << RESET;
        throw InvalidServerConfException();
	}
	else if (_errorPage.size() < 15 || _errorPage.substr(0, 15) != "/public/errors/" || _errorPage.substr(18, 5) != ".html"){
		std::cerr << "Error: " << RED << "Invalid error page path. " << RED << std::endl << RESET;
		throw InvalidServerConfException();
	}
    return ;
}

void    ServerConf::print(void) const {
        std::cout << "  Server: " << this->_serverName << std::endl;
        std::vector<unsigned short>::const_iterator it = _port.begin();
        for (; it != _port.end(); ++it){
           std::cout << "  Port: " << *it << std::endl;
        }
        std::cout << "  Root: " << this->_root << std::endl;
        std::cout << "  Index: " << this->_index << std::endl;
		std::cout << "  MaxBodySize: " << this->_maxBodySize << std::endl;
        std::cout << "  Error Page: " << this->_errorPage << std::endl;
		std::cout << "  IP Address: " << this->_ipAddr << std::endl;
        std::map<std::string, Location>::const_iterator it2 = _location.begin();
        for (; it2 != _location.end(); ++it2) {
            it2->second.print();
        }
}

/* ------------------- EXCEPTIONS ----------------------*/
char const	*ServerConf::InvalidServerConfException::what(void) const throw(){
    return ("Invalid <ServerConf> configurtation format");
}