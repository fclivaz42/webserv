#include "Parsing/Servers.hpp"
#include "Parsing/ServerConf.hpp"
#include "Parsing/Location.hpp"
#include "webserv.hpp"

/* ------------------- CONSTRUCTORS ----------------------*/
Servers::Servers() {}

Servers::Servers(const std::string &conf_file){
	std::ifstream		confFile;
	std::string 		line, configString;
	ServerConf			currentServer;
	Location			currentLocation;

	confFile.open(conf_file.c_str(), std::ios::in);
	if (!confFile.is_open()) {
		std::cerr << "Error: " << RED << "could not open file \"" << ORANGE << conf_file << RED << "\"" << RESET << std::endl;
		return ;
	}
	while (std::getline(confFile, line)){
		line.erase(0, line.find_first_not_of(WHITESPACES));
		line.erase(line.find_last_not_of(WHITESPACES) + 1);
		if (line.empty() || line[0] == '#')
			continue;
		else if (line.find("server {") != std::string::npos) {
			while (line.find("}")) {
				std::getline(confFile, line);
				configString += line + '\n';
			}
			this->_servConf.push_back(ServerConf(configString.substr(0, configString.find_last_of('}'))));
			configString.clear();
		}
		else {
			std::cerr << "Error: " << RED << "Invalid config line \"" << ORANGE << line << RED << "\"\n" << RESET;
			confFile.close();
			this->_servConf.clear();
			throw InvalidServersException();
		}
	}
	confFile.close();
	if (this->_servConf.empty())
		throw InvalidServersException();
}

/* ------------------- COPY CONSTRUCTOR ----------------------*/
Servers::Servers(Servers const &cpy){
	*this = cpy;
}

/* ------------------- DESTRUCTOR ----------------------*/
Servers::~Servers(){}

/* ------------------- SURCHARGED OPERATOR ----------------------*/
Servers &Servers::operator=(Servers const &rhs){
	this->_servConf = rhs._servConf;
	return (*this);
}

/* ------------------- GETTER ----------------------*/
ServerConf	Servers::getServConf(int lequel) const{
	std::vector<ServerConf>::const_iterator it;
	int i = 1;
	if (lequel <= 0){
		std::cerr << "Error: Invalid config number." << std::endl;
	}
	for (it = _servConf.begin(); it != _servConf.end(); it++){
		if (i == lequel)
			return (*it);
		else i++;
	}
	if (i > lequel){
		std::cerr << "Error: No config number found." << std::endl;
	}
	return (*it);
}

/* ------------------- MEMBERS FUNCTIONS ----------------------*/
void	Servers::printConfigs() const {
    std::vector<ServerConf>::const_iterator it = _servConf.begin();
    for (; it != _servConf.end(); ++it) {
        it->print();
    }
}

bool	Servers::isConfigured() const {
	return !this->_servConf.empty();
}

/* ------------------- EXCEPTION ----------------------*/
char const	*Servers::InvalidServersException::what(void) const throw(){
    return ("Invalid <Servers> configurtation format");
}
