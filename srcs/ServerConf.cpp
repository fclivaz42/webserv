#include "ServerConf.hpp"

ServerConf::ServerConf(){}

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

std::string ServerConf::getServerName(void) const{
    return (this->_serverName);
}

std::string ServerConf::getPort(void) const{
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

void    ServerConf::setServerName(const std::string &serverName){
    this->_serverName = serverName;
}

void    ServerConf::setPort(const std::string &port){
    this->_port = port;
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

void    ServerConf::print(void) const {
        std::cout << "  Server: " << this->_serverName << std::endl;
        std::cout << "  Port: " << this->_port << std::endl;
        std::cout << "  Root: " << this->_root << std::endl;
        std::cout << "  Index: " << this->_index << std::endl;
        std::cout << "  Error Page: " << this->_errorPage << std::endl;
        std::map<std::string, Location>::const_iterator it = _location.begin();
        for (; it != _location.end(); ++it) {
            it->second.print();
        }
}