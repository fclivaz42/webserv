#include "Parsing/ServerConf.hpp"
#include "Parsing/Location.hpp"
#include "webserv.hpp"

ServerConf::ServerConf() : _ipAddr("127.0.0.1") {}

ServerConf::ServerConf(const std::string& configString) : _ipAddr("127.0.0.1")
{
	std::stringstream	configStream(configString);
	const std::string	whitespaces(" \t\f\v\n\r");
	std::string			line;
	long				bigPortCheck;
	short				port;
	char				*ptr;

	while(std::getline(configStream, line))
	{
		line.erase(0, line.find_first_not_of(whitespaces));
		line.erase(line.find_last_not_of(whitespaces) + 1);
		if (!line.find("server_name")) {
			line.erase(0, line.find_first_not_of("server_name"));
			line.erase(0, line.find_first_not_of(whitespaces));
			line.erase(line.find_last_not_of(whitespaces) + 1);
			this->_serverName = line;
			std::cout << this->_serverName << std::endl;
		}
		else if (!line.find("root")) {
			line.erase(0, line.find_first_not_of("root"));
			line.erase(0, line.find_first_not_of(whitespaces));
			line.erase(line.find_last_not_of(whitespaces) + 1);
			this->_root = line;
			std::cout << this->_root << std::endl;
		}
		else if (!line.find("index")) {
			line.erase(0, line.find_first_not_of("index"));
			line.erase(0, line.find_first_not_of(whitespaces));
			line.erase(line.find_last_not_of(whitespaces) + 1);
			this->_index = line;
			std::cout << this->_index << std::endl;
		}
		else if (!line.find("error_page")) {
			line.erase(0, line.find_first_not_of("error_page"));
			line.erase(0, line.find_first_not_of(whitespaces));
			line.erase(line.find_last_not_of(whitespaces) + 1);
			this->_errorPage = line;
			std::cout << this->_errorPage << std::endl;
		}
		else if (!line.find("listen")) {
			line.erase(0, line.find_first_not_of("listen"));
			line.erase(0, line.find_first_not_of(whitespaces));
			line.erase(line.find_last_not_of(whitespaces) + 1);
			bigPortCheck = strtol(line.c_str(), &ptr, 10);
			port = strtol(line.c_str(), &ptr, 10);
			if (ptr[0] != 0 || port != bigPortCheck || port == 0)
				std::cerr << "Error: " << RED << "Invalid port.\n" << RESET;
			this->_port.push_back(port);
		}
		else if (!line.find("location ")) {
			// HOLY FUCK TODO
		}
	}
}

ServerConf::ServerConf(ServerConf const &cpy){
    *this = cpy;
}

ServerConf::~ServerConf(){}

ServerConf  &ServerConf::operator=(ServerConf const &rhs){
    this->_serverName = rhs._serverName;
    this->_port = rhs._port;
    this->_root = rhs._root;
    this->_index = rhs._index;
    this->_errorPage = rhs._errorPage;
    this->_location = rhs._location;
    return (*this);
}

                /* GETTERS */
std::string ServerConf::getServerName(void) const{
    return (this->_serverName);
}

std::vector<unsigned short>         ServerConf::getPort(void) const{
    return (this->_port);
}

std::string ServerConf::getRoot(void) const{
    return (this->_root);
}

std::string ServerConf::getIndex(void) const{
    return (this->_index);
}

std::map<std::string, Location> ServerConf::getLocation(void) const{
    return (this->_location);
}

std::string ServerConf::getIpAddr(void)	const{
    return (this->_ipAddr);
}

                /* SETTERS */
void    ServerConf::setServerName(const std::string &serverName){
    this->_serverName = serverName;
}

void    ServerConf::setPort(const std::string &port){
    /*for (int i = 0; port[i]; i++){
        if (!isdigit(port[i]))
            return ;
    }
    */
   
    const char *tmp = port.c_str();
    _port.push_back(atoi(tmp));
}

void    ServerConf::setRoot(const std::string &root){
    this->_root = root;
}

void    ServerConf::setIndex(const std::string &index){
    this->_index = index;
}

void    ServerConf::setLocation(const std::string &path, Location &locations){
    this->_location[path] = locations;
}

void    ServerConf::setErrorPage(const std::string &error){
    this->_errorPage = error;
}

int     ServerConf::checkAttribut(void) const{
    std::map<std::string, Location>::const_iterator it;

    if (_serverName.empty() || _port.empty() || _root.empty() || _index.empty() || _errorPage.empty() || _location.empty())
        return (1);
    /*for (int i = 0; _port[i]; i++){
        if (!isdigit(_port[i]))
            return (1);
    }*/
    if (_root[0] != '/')
        return (1);
    for (it = _location.begin(); it != _location.end(); ++it){
        if (it->second.checkAttribut() != 0)
            return (1);
    }
    
    return (0);
}

/* Fonction permettant de print les attributs de la class ServerConf. */
void    ServerConf::print(void) const {
        std::cout << "  Server: " << this->_serverName << std::endl;
        std::vector<unsigned short>::const_iterator it = _port.begin();
        for (; it != _port.end(); ++it){
           std::cout << "  Port: " << *it << std::endl;
        }
        std::cout << "  Root: " << this->_root << std::endl;
        std::cout << "  Index: " << this->_index << std::endl;
        std::cout << "  Error Page: " << this->_errorPage << std::endl;
        std::map<std::string, Location>::const_iterator it2 = _location.begin();
        for (; it2 != _location.end(); ++it2) {
            it2->second.print();
        }
}
