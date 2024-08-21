#ifndef LOCATION_HPP
# define LOCATION_HPP

#include "ServerConf.hpp"
#include "sstream"

class Location {
	private:
		std::vector<std::string>	_allowMethods;
		std::string 				_path;
		std::string 				_index;
		std::string					_root;
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
		std::string					getIndex(void) const;
		std::string					getRoot(void) const;
		std::string					getReturnURL(void) const;
		std::string					getUploadStore(void) const;
		std::string					getFastcgiPass(void) const;
		std::string					getFastcgiIndex(void) const;
		bool						getAutoIndex(void) const;
		bool						getUpload(void) const;

		/* SETTERS */
		void						setAllowMethods(const std::string &methods);
		void						setPath(const std::string &path);
		void						setIndex(const std::string &file);
		void						setRoot(const std::string &root);
		void						setReturnURL(const std::string &url);
		void						setUploadStore(const std::string &store);
		void						setFastcgiPass(const std::string &pass);
		void						setFastcgiIndex(const std::string &index);
		void						setAutoIndex(bool index);
		void						setUpload(const std::string &upload);
		
		/* MEMBER FUNCTIONS */
		void						print(void) const;

};

#endif