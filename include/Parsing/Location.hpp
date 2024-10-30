#ifndef LOCATION_HPP
# define LOCATION_HPP

#include <sstream>
#include <vector>
#include <iostream>

class Location {
	private:
		std::vector<std::string>	_allowMethods;
		std::string					_path;
		std::string					_index;
		std::string					_root;
		std::string					_returnURL;
		std::string					_fastcgiPass;
		std::string					_fastcgiIndex;
		bool						_autoIndex;
		bool						_upload;
		bool						_default;

	public:
		/* CONSTRUCTORS */
		Location();
		Location(const std::string &path);
		Location(const std::string &path, const std::string &locationString);

		/* COPY CONSTRUCTOR */
		Location(Location const &cpy);

		/* DESTRUCTOR */
		~Location();

		/* SURCHARGED OPERATOR */
		Location					&operator=(Location const &rhs);

		/* GETTERS */
		const std::vector<std::string>&	getAllowMethods(void) const;
		const std::string&				getPath(void) const;
		const std::string&				getIndex(void) const;
		const std::string&				getRoot(void) const;
		const std::string&				getReturnURL(void) const;
		const std::string&				getUploadStore(void) const;
		const std::string&				getFastcgiPass(void) const;
		const std::string&				getFastcgiIndex(void) const;
		bool							hasAutoIndex(void) const;
		bool							acceptsUploads(void) const;
		bool							isDefault(void) const;

		/* MEMBER FUNCTIONS */
		void	 					checkAttribut(void) const;
		void						print(void) const;

		/* EXCEPTIONS */
		class InvalidLocationException : public std::exception{
		public:
			virtual char const		*what(void) const throw() {
				return ("Invalid <Location> configuration format.\n");
			}
		};
		class UnexpectedEOFException : public std::exception{
			public:
				virtual char const	*what(void) const throw() {
					return "Unexpected End Of File.\n";
				}
		};
		class MissingArgsException : public std::exception{
			public:
				virtual char const	*what(void) const throw() {
					return "Missing args in location configuration.\n";
				}
		};
		class InvalidMethodsException : public std::exception{
			public:
				virtual char const	*what(void) const throw() {
					return "Invalid methods.\n";
				}
		};
		class InvalidRootException : public std::exception{
			public:
				virtual char const	*what(void) const throw() {
					return "Invalid location root.\n";
				}
		};
};

void	ptrim(std::string& str);

#endif
