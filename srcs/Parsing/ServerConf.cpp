#include "Parsing/ServerConf.hpp"
#include "Parsing/Location.hpp"
#include "webserv.hpp"

/* ------------------- CONSTRUCTORS ----------------------*/

ServerConf::ServerConf() : _ipAddr("127.0.0.1") {}

ServerConf::ServerConf(const std::string& configString) : _maxBodySize(0), _ipAddr("127.0.0.1"), _flag(0) {
	std::stringstream	configStream(configString);
	std::string			line, locationString, locationPath;
	std::size_t			maxBodySize;
	long				bigPortCheck;
	short				port;
	char				*ptr;

	while(!configStream.eof())
	{
		std::getline(configStream, line);
		ptrim(line);
		if (!line.find("server_name")) {
			line.erase(0, line.find_first_not_of("server_name"));
			ptrim(line);
			this->_serverName = line;
		}
		else if (!line.find("root")) {
			line.erase(0, line.find_first_not_of("root"));
			ptrim(line);
			this->_root = line;
		}
		else if (!line.find("index")) {
			line.erase(0, line.find_first_not_of("index"));
			ptrim(line);
			this->_index = line;
		}
		else if (!line.find("error_path")) {
			line.erase(0, line.find_first_not_of("error_path"));
			ptrim(line);
			this->_errorPath = line;
		}
		else if (!line.find("listen")) {
			line.erase(0, line.find_first_not_of("listen"));
			ptrim(line);
			port = strtol(line.c_str(), &ptr, 10);
			bigPortCheck = strtol(line.c_str(), &ptr, 10);
			if (ptr[0] != 0 || port != bigPortCheck || port == 0)
				throw InvalidPortException();
			this->_port.push_back(port);
		}
		else if (!line.find("max_body_size")){
			line.erase(0, line.find_first_not_of("max_body_size"));
			ptrim(line);
			maxBodySize = strtoul(line.c_str(), &ptr, 10);
			bigPortCheck = strtol(line.c_str(), &ptr, 10);
			if (ptr[0] != 0 || bigPortCheck < 1)
				throw InvalidBodySizeException();
			this->_maxBodySize = maxBodySize;
		}
		else if (!line.find("location ")) {
			line.erase(0, line.find_first_not_of("location"));
			ptrim(line);
			locationPath = line.substr(0, line.find_first_of(WHITESPACES));
			while (!(line.find('}') == 4 && configStream.peek() != EOF)) {
				if (std::getline(configStream, line).eof())
					throw UnexpectedEOFException();
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
	if (this != &rhs) {
		this->_serverName = rhs._serverName;
		this->_port = rhs._port;
		this->_root = rhs._root;
		this->_index = rhs._index;
		this->_maxBodySize = rhs._maxBodySize;
		this->_errorPath = rhs._errorPath;
		this->_ipAddr = rhs._ipAddr;
		this->_location = rhs._location;
	}
	return (*this);
}

/* ------------------- GETTERS ----------------------*/
const std::string&	ServerConf::getServerName(void) const{
	return (this->_serverName);
}

const std::vector<unsigned short>&	ServerConf::getPort(void) const{
	return (this->_port);
}

const std::string&	ServerConf::getRoot(void) const{
	return (this->_root);
}

const std::string&	ServerConf::getIndex(void) const{
	return (this->_index);
}

size_t	ServerConf::getMaxBodySize(void) const{
	return (this->_maxBodySize);
}

const std::string&	ServerConf::getErrorPath(void) const{
	return (this->_errorPath);
}
const std::map<std::string, Location>&	ServerConf::getLocation(void) const{
	return (this->_location);
}

const std::string&	ServerConf::getIpAddr(void) const{
	return (this->_ipAddr);
}

void	ServerConf::setFlag(int nb){
	_flag = nb;
}
/* ------------------- MEMBERS FUNCTIONS ----------------------*/
void	 ServerConf::checkAttribut(void) const
{
	if (_serverName.empty() || _port.empty() || _errorPath.empty() || _maxBodySize == 0 || _location.empty())
		throw MissingArgsException();

	for (std::map<std::string, Location>::const_iterator iter = _location.begin(); iter != _location.end(); iter++) {
		if (iter->second.isDefault()) {
			for (std::map<std::string, Location>::const_iterator iter2 = _location.begin(); iter2 != _location.end(); iter2++) {
				if (iter2 == iter)
					continue;
				else if (iter2->second.isDefault())
					throw MoreThanOneDefaultException();
			}
			return;
		}
	}
	throw NoDefaultException();
}

void	ServerConf::print(void) const {
		std::cout << BGREEN << "\n┌────────── SERVER ──────────\n";
		std::cout << BGREEN << "│ " << YELLOW << "Server: " << RESET << this->_serverName << std::endl;
		std::cout << BGREEN << "│ " << YELLOW << "Ports: " << RESET;
		std::vector<unsigned short>::const_iterator it = _port.begin();
		for (; it != _port.end(); ++it){
			if (it + 1 == _port.end())
				std::cout << *it << std::endl;
			else
				std::cout << *it << ", ";
		}
		std::cout << BGREEN << "│ " << YELLOW << "Root: " << RESET << this->_root << std::endl;
		std::cout << BGREEN << "│ " << YELLOW << "Index: " << RESET << this->_index << std::endl;
		std::cout << BGREEN << "│ " << YELLOW << "MaxBodySize: " << RESET << this->_maxBodySize << std::endl;
		std::cout << BGREEN << "│ " << YELLOW << "Error Path: " << RESET << this->_errorPath << std::endl;
		std::cout << BGREEN << "│ " << YELLOW << "IP Address: " << RESET << this->_ipAddr;
		std::map<std::string, Location>::const_iterator it2 = _location.begin();
		for (; it2 != _location.end(); ++it2) {
			it2->second.print();
		}
		std::cout << BGREEN << "\n└────────────────────────────\n\n" << RESET;
}
