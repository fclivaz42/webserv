#include "Parsing/ServerConf.hpp"
#include "Parsing/Location.hpp"

ServerConf::ServerConf() : _ipAddr("127.0.0.1"){}

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

std::vector<int>         ServerConf::getPort(void) const{
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
        std::vector<int>::const_iterator it = _port.begin();
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
