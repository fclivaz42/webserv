#include "Parsing/Location.hpp"
#include "webserv.hpp"

/* ------------------- CONSTRUCTOR ----------------------*/
Location::Location() : _root(""), _autoIndex(false), _upload(false), _default(false)
{}

Location::Location(const std::string &path, const std::string& locationString) : _path(path), _autoIndex(false), _upload(false), _default(false)
{

	std::stringstream	locationStream(locationString);
	std::string			line;
	size_t				pos = 0;

	while(std::getline(locationStream, line))
	{
		ptrim(line);
		if (!line.find("root")) {
			line.erase(0, line.find_first_not_of("root"));
			ptrim(line);
			this->_root = line;
		}
		else if (!line.find("index")) {
			line.erase(0, line.find_first_not_of("index"));
			ptrim(line);
			this->_index = line;
		}
		else if (!line.find("default_location")) {
			line.erase(0, line.find_first_not_of("default_location"));
			ptrim(line);
			if (line.compare("1") || line.compare("yes") || line.compare("true"))
				this->_default = true;
			else if (line.compare("0") || line.compare("no") || line.compare("false"))
				this->_default = false;
			else {
				std::cerr << "Error: " << RED << " Invalid accept_upload config." << std::endl << RESET;
				throw InvalidLocationException();
			}
		}
		else if (!line.find("accept_uploads")) {
			line.erase(0, line.find_first_not_of("accept_uploads"));
			ptrim(line);
			if (line.compare("1") || line.compare("yes") || line.compare("true"))
				this->_upload = true;
			else if (line.compare("0") || line.compare("no") || line.compare("false"))
				this->_upload = false;
			else {
				std::cerr << "Error: " << RED << " Invalid accept_upload config." << std::endl << RESET;
				throw InvalidLocationException();
			}
		}
		else if (!line.find("auto_index")) {
			line.erase(0, line.find_first_not_of("auto_index"));
			ptrim(line);
			if (line.compare("1") || line.compare("yes") || line.compare("true"))
				this->_upload = true;
			else {
				std::cerr << "Error: " << RED << " Invalid auto_index config." << std::endl << RESET;
				throw InvalidLocationException();
			}
		}
		else if (!line.find("methods")) {
			line.erase(0, line.find_first_not_of("methods"));
			while (pos != std::string::npos)
			{
				ptrim(line);
				this->_allowMethods.push_back(line.substr(0, line.find_first_of(WHITESPACES)));
				pos = line.find_first_of(WHITESPACES);
				if (pos == std::string::npos)
					break ;
				line = line.substr(pos);
			}
		}
		else if (!line.find("return_url")) {
			line.erase(0, line.find_first_not_of("return_url"));
			ptrim(line);
			this->_returnURL = line;
		}
		else if (!line.find("fastcgiPass")){
			line.erase(0, line.find_first_not_of("fastcgiPass"));
			ptrim(line);
			this->_fastcgiPass = line;
		}
		else if (!line.find("fastcgiIndex")){
			line.erase(0, line.find_first_not_of("fastcgiIndex"));
			ptrim(line);
			this->_fastcgiIndex = line;
		}
		else if (line.empty())
			continue ;
		else 
			throw InvalidLocationException();
	}
	checkAttribut();
}

/* ------------------- COPY CONSTRUCTOR ----------------------*/
Location::Location(Location const &cpy) : 	_allowMethods(cpy._allowMethods),
											_path(cpy._path),
											_index(cpy._index),
											_root(cpy._root),
											_returnURL(cpy._returnURL),
											_fastcgiPass(cpy._fastcgiPass),
											_fastcgiIndex(cpy._fastcgiIndex),
											_autoIndex(cpy._autoIndex),
											_upload(cpy._upload),
											_default(cpy._default)
{}

/* ------------------- DESTRUCTOR ----------------------*/
Location::~Location(){}

/* ------------------- SURCHARGED OPERATOR ----------------------*/
Location	&Location::operator=(Location const &rhs)
{
	if (this != &rhs) {
		this->_allowMethods = rhs._allowMethods;
		this->_path = rhs._path;
		this->_index = rhs._index;
		this->_root = rhs._root;
		this->_returnURL = rhs._returnURL;
		this->_fastcgiPass = rhs._fastcgiPass;
		this->_fastcgiIndex = rhs._fastcgiIndex;
		this->_autoIndex = rhs._autoIndex;
		this->_upload = rhs._upload;
		this->_default = rhs._default;
	}
	return (*this);
}

/* ------------------- GETTERS ----------------------*/
const std::vector<std::string>&	Location::getAllowMethods(void) const{
	return (this->_allowMethods);
}

const std::string&	Location::getPath(void) const{
	return (this->_path);
}

const std::string&	Location::getIndex(void) const{
	return (this->_index);
}

const std::string&	Location::getRoot(void) const{
	return (this->_root);
}

const std::string&	Location::getReturnURL(void) const{
	return (this->_returnURL);
}

const std::string&	Location::getFastcgiPass(void) const{
	return (this->_fastcgiPass);
}

const std::string& Location::getFastcgiIndex(void) const{
	return (this->_fastcgiIndex);
}

bool	Location::hasAutoIndex(void) const{
	return (this->_autoIndex);
}

bool	Location::acceptsUploads(void) const{
	return (this->_upload);
}

bool	Location::isDefault(void) const{
	return (this->_default);
}

/* ------------------- MEMBERS FUNCTIONS ----------------------*/
void	Location::checkAttribut(void) const{
	std::vector<std::string>::const_iterator it;

	if (this->_root.empty() && (this->_fastcgiPass.empty() || this->_fastcgiIndex.empty()))
		throw MissingArgsException();
	if (!_allowMethods.empty()){
		for (it = _allowMethods.begin(); it != _allowMethods.end(); ++it){
			if (*it != "GET" && *it != "POST" && *it != "DELETE")
				throw InvalidMethodsException();
		}
	}
	return ;
}

void Location::print() const {
		std::cout << BGREEN << "\n│ " << CYAN << "┌────────── " << (_default ? "DEFAULT" : "") << " LOCATION " << _path << " ───\n";
		std::cout << BGREEN << "│ " << CYAN "│ " << MAGENTA << "Methods: " << RESET;
		std::vector<std::string>::const_iterator it;
		for (it = _allowMethods.begin(); it != _allowMethods.end(); ++it) {
			if (it + 1 == _allowMethods.end())
				std::cout << *it << std::endl;
			else
				std::cout << *it << ", ";
		}
		std::cout << BGREEN << "│ " << CYAN "│ " << MAGENTA << "Root: " << RESET << _root << std::endl;
		std::cout << BGREEN << "│ " << CYAN "│ " << MAGENTA << "Index: " << RESET << _index << std::endl;
		std::cout << BGREEN << "│ " << CYAN "│ " << MAGENTA << "Return URL: " << RESET << _returnURL << std::endl;
		std::cout << BGREEN << "│ " << CYAN "│ " << MAGENTA << "FastCgiPass: " << RESET << _fastcgiPass << std::endl;
		std::cout << BGREEN << "│ " << CYAN "│ " << MAGENTA << "FastCgiIndex: " << RESET << _fastcgiIndex << std::endl;
		std::cout << BGREEN << "│ " << CYAN "│ " << MAGENTA << "Accept Uploads: " << RESET << (_upload ? "Yes" : "No") << std::endl;
		std::cout << BGREEN << "│ " << CYAN "│ " << MAGENTA << "Auto Index: " << RESET << (_autoIndex ? "Yes" : "No") << std::endl;
		std::cout << BGREEN << "│ " << CYAN <<"└──────────────────────────";
}

void	ptrim(std::string& str)
{
	str.erase(0, str.find_first_not_of(WHITESPACES));
	str.erase(str.find_last_not_of(WHITESPACES) + 1);
}
