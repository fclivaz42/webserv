#include "Parsing/Servers.hpp"
#include "Parsing/ServerConf.hpp"
#include "webserv.hpp"

/* ------------------- CONSTRUCTORS ----------------------*/
Servers::Servers() {}

Servers::Servers(const std::string &conf_file){
	std::ifstream		confFile;
	std::string 		line, configString;

	confFile.open(conf_file.c_str(), std::ios::in);
	if (!confFile.is_open()) {
		std::cerr << RED << "Error: " << RESET << "could not open file \"" << ORANGE << conf_file << RESET << "\"\n";
		throw AlreadyPrintedException();
	}
	while (std::getline(confFile, line)){
		line.erase(0, line.find_first_not_of(WHITESPACES));
		line.erase(line.find_last_not_of(WHITESPACES) + 1);
		if (line.empty() || line[0] == '#')
			continue;
		else if (line.find("server {") != std::string::npos) {
			while (line.find("}") || confFile.peek() != EOF) {
				std::getline(confFile, line);
				configString += line + '\n';
			if (confFile.peek() == EOF && line[0] != '}')
				throw UnexpectedEOFException();
			}
			this->_servConf.push_back(ServerConf(configString.substr(0, configString.find_last_of('}'))));
			configString.clear();
		}
		else {
			std::cerr << RED << "Error: " << RESET << "Invalid config line \"" << ORANGE << line << RESET << "\"\n";
			confFile.close();
			this->_servConf.clear();
			throw AlreadyPrintedException();
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
