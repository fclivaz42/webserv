#include "Location.hpp"

Location::Location(){}

Location::Location(const std::string &path) : _path(path){}

Location::Location(Location const &cpy){
	*this = cpy;
}

Location::~Location(){}

Location    &Location::operator=(Location const &rhs){
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
	
	return (*this);
}

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

void	Location::setAllowMethods(const std::string &methods){
	std::string	method;
	std::istringstream	iss(methods);

	this->_allowMethods.clear();
	while (iss >> method){
		_allowMethods.push_back(method);
	}
}

void	Location::setPath(const std::string &path){
	this->_path = path;
}

void	Location::setIndex(const std::string &index){
	this->_index = index;
}

void	Location::setRoot(const std::string &root){
	this->_root = root;
}

void	Location::setReturnURL(const std::string &url){
	this->_returnURL = url;
}

void	Location::setUploadStore(const std::string &store){
	this->_uploadStore = store;
}

void	Location::setFastcgiPass(const std::string &pass){
	this->_fastcgiPass = pass;
}

void	Location::setFastcgiIndex(const std::string &index){
	this->_fastcgiIndex = index;
}

void	Location::setAutoIndex(bool index){
	this->_autoIndex = index;
}

void	Location::setUpload(const std::string &upload){
	if (upload == "yes")
		this->_upload = true;
	else
		this->_upload = false;
}

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