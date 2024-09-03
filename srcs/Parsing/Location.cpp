#include "Parsing/Location.hpp"
#include "webserv.hpp"

Location::Location(){}

Location::Location(const std::string &path, const std::string& locationString) : _path(path)
{
	std::stringstream	locationStream(locationString);
	std::string			line;
	size_t				pos = 0;

	while(std::getline(locationStream, line))
	{
		line.erase(0, line.find_first_not_of(WHITESPACES));
		line.erase(line.find_last_not_of(WHITESPACES) + 1);
		if (!line.find("root")) {
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
		else if (!line.find("accept_uploads")) {
			line.erase(0, line.find_first_not_of("accept_uploads"));
			line.erase(0, line.find_first_not_of(WHITESPACES));
			line.erase(line.find_last_not_of(WHITESPACES) + 1);
			if (line.compare("1") || line.compare("yes") || line.compare("true"))
				this->_upload = true;
			else if (line.compare("0") || line.compare("no") || line.compare("false"))
				this->_upload = false;
			else {
				// TODO: Throw either a warning or an error and exit.
			}
		}
		else if (!line.find("methods")) {
			line.erase(0, line.find_first_not_of("methods"));
			line.erase(line.find_last_not_of(WHITESPACES) + 1);
			while (pos != std::string::npos)
			{
				line.erase(0, line.find_first_not_of(WHITESPACES));
				this->_allowMethods.push_back(line.substr(0, line.find_first_of(WHITESPACES)));
				pos = line.find_first_of(WHITESPACES);
				if (pos == std::string::npos)
					break ;
				line = line.substr(pos);
			}
		}
	}
}

Location::Location(Location const &cpy){
	*this = cpy;
}

Location::~Location(){}

Location    &Location::operator=(Location const &rhs)
{
	if (this != &rhs) {
		this->_allowMethods = rhs._allowMethods;
		this->_path = rhs._path;
		this->_index = rhs._index;
		this->_root = rhs._root;
		this->_returnURL = rhs._returnURL;
		this->_uploadStore = rhs._uploadStore;
		this->_fastcgiPass = rhs._fastcgiPass;
		this->_fastcgiIndex = rhs._fastcgiIndex;
		this->_autoIndex = rhs._autoIndex;
		this->_upload = rhs._upload;
	}
	return (*this);
}

				/* GETTERS */
std::vector<std::string>    Location::getAllowMethods(void) const{
	return (this->_allowMethods);
}

std::string	Location::getPath(void) const{
	return (this->_path);
}

std::string	Location::getIndex(void) const{
	return (this->_index);
}

std::string	Location::getRoot(void) const{
	return (this->_root);
}

std::string	Location::getReturnURL(void) const{
	return (this->_returnURL);
}

std::string	Location::getUploadStore(void) const{
	return (this->_uploadStore);
}

std::string	Location::getFastcgiPass(void) const{
	return (this->_fastcgiPass);
}

std::string Location::getFastcgiIndex(void) const{
	return (this->_fastcgiIndex);
}

bool	Location::getAutoIndex(void) const{
	return (this->_autoIndex);
}

bool	Location::getUpload(void) const{
	return (this->_upload);
}

int     Location::checkAttribut(void) const{
	std::vector<std::string>::const_iterator it;

	if (_allowMethods.empty() || _root.empty())
		return (1);
	for (it = _allowMethods.begin(); it != _allowMethods.end(); ++it){
		if (*it != "GET" && *it != "POST" && *it != "DELETE")
			return (1);
	}
	if (_root[0] != '/')
		return (1);

	return (0);
}

/* Fonction permettant de print les attributs de la class Location. */
void Location::print() const {
		std::cout << "  Location: " << _path << std::endl;
		std::vector<std::string>::const_iterator it;
		for (it = _allowMethods.begin(); it != _allowMethods.end(); ++it) {
			std::cout << "    Methods: " << *it << std::endl;
		}
		std::cout << "    Root: " << _root << std::endl;
		std::cout << "    Index: " << _index << std::endl;
		std::cout << "    Accept Uploads: " << (_upload ? "Yes" : "No") << std::endl;
}
