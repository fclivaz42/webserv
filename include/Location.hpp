#ifndef LOCATION_HPP
# define LOCATION_HPP

#include "ServerConf.hpp"

class Location {
	private:
		std::vector<std::string>	_allowMethods;
		std::string 				_path;
		std::string 				_defaultFile;
		std::string 				_returnURL;
		std::string					_uploadStore;
		std::string 				_fastcgiPass;
		std::string 				_fastcgiIndex;
		bool 						_autoIndex;
		bool 						_upload;

	public:
		/* CONSTRUCTORS */
		Location();
		Location(const std::string &path);

		/* COPY CONSTRUCTOR */
		Location(Location const &cpy);

		/* DESTRUCTOR */
		~Location();

		/* SURCHARGED OPERATOR */
		Location					&operator=(Location const &rhs);

		/* GETTERS */
		std::vector<std::string>	getAllowMethods(void) const;
		std::string					getPath(void) const;
		std::string					getDefaultFile(void) const;
		std::string					getReturnURL(void) const;
		std::string					getUploadStore(void) const;
		std::string					getFastcgiPass(void) const;
		std::string					getFastcgiIndex(void) const;
		bool						getAutoIndex(void) const;
		bool						getUpload(void) const;

		/* SETTERS */
		void						setAllowMethods(const std::vector<std::string> &methods);
		void						setPath(const std::string &path);
		void						setDefaultFile(const std::string &file);
		void						setReturnURL(const std::string &url);
		void						setUploadStore(const std::string &store);
		void						setFastcgiPass(const std::string &pass);
		void						setFastcgiIndex(const std::string &index);
		void						setAutoIndex(bool &index);
		void						setUpload(bool &upload);
		
		/* MEMBER FUNCTIONS */

};

#endif