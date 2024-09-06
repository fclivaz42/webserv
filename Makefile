# Pas de header pour eviter les conflicts :)

export MAKEFLAGS += --silent

NAME	= webserv

SHELL	= /bin/bash

CXX	= c++

DEPS	=	${INCL}/webserv.hpp\
			${INCL}/Sockets/SocketManager.hpp\
			${INCL}/Parsing/Location.hpp\
			${INCL}/Parsing/ServerConf.hpp\
			${INCL}/Parsing/Servers.hpp\
			${INCL}/Requests/Get.hpp\
			${INCL}/Requests/Post.hpp\
			${INCL}/Requests/Delete.hpp\
			${INCL}/Requests/HttpRequest.hpp\
			${SRCS}

SRCS	=	${SUBDIR}/webserv.cpp\
			${SUBDIR}/Sockets/SocketManager.cpp\
			${SUBDIR}/Parsing/Servers.cpp\
			${SUBDIR}/Parsing/ServerConf.cpp\
			${SUBDIR}/Parsing/Location.cpp\
			${SUBDIR}/Requests/Get.cpp\
			${SUBDIR}/Requests/Post.cpp\
			${SUBDIR}/Requests/Delete.cpp\
			${SUBDIR}/Requests/HttpRequest.cpp\


SUBDIR	= srcs

SRCTREE	= $(shell find srcs -type d)

OBJ		= $(SRCS:${SUBDIR}/%.cpp=${OBJDIR}/%.o)

OBJDIR	= obj

OBJTREE	= $(SRCTREE:srcs%=${OBJDIR}%)

CXXFLAGS= -Wall -Werror -Wextra -O2 -std=c++98 -g3

DBFLAGS	= DEBUG=1

LFLAGS = -I${INCL} -g3

LDFLAGS	= -g3 -I${INCL}

INCL	= include

CRED = \033[1;31m
CGRN = \033[1;32m
CYEL = \033[1;33m
RSET = \033[0m

all:	${NAME}

${NAME}:	${DEPS}
		@if [[ $(if $(filter r,${MAKECMDGOALS}),1,0) == "1" ]]; then \
			printf "├──────────\n"; \
		else \
			printf "┌──────────\n"; \
		fi
		@${MAKE} ${OBJ}
		@printf "│\tSources for ${CGRN}${NAME}${RSET} done.\n"
		@${CXX} ${OBJ} -o ${NAME} ${LFLAGS}
		@printf "├──────────\n├─>>> ${CGRN}${NAME}${RSET} compiled!\n└──────────\n"

${OBJDIR}/%.o:	${SUBDIR}/%.cpp | ${OBJDIR}
				@if [[ $(if $(filter dbg,${MAKECMDGOALS}),1,0) == "1" ]]; then \
					printf "│\t > ${CYEL}DEBUG${RSET} Compiling ${CYEL}$<${RSET} with ${CYEL}${DBFLAGS}${RSET} and ${CYEL}${LDFLAGS}${RSET}...\r"; \
					${CXX} -D ${DBFLAGS} -c $< -o $@ ${LDFLAGS}; \
				else \
					printf "│\t > Compiling ${CYEL}$<${RSET} for ${CGRN}${NAME}${RSET}...\r"; \
					${CXX} ${CXXFLAGS} -c $< -o $@ ${LFLAGS}; \
				fi
				@printf "\33[2K"

${OBJDIR}:
		@printf "│ Compiling ${CGRN}${NAME}${RSET}...\n"
		@printf "│\tCreating ${CYEL}${OBJTREE}${RSET} for ${CGRN}${NAME}${RSET}\n"
		@mkdir -p ${OBJTREE}

debug:
		@printf "┌──────────\n"
		@rm -rf obj
		@rm -rf ${NAME}
		@${MAKE} ${OBJ} dbg
		@printf "│\t${CYEL}DEBUG${RSET} Sources for ${CGRN}${NAME}${RSET} done.\n"
		@${CXX} ${OBJ} -D ${DBFLAGS} -o ${NAME} ${LDFLAGS}
		@printf "├──────────\n├─>>> ${CYEL}DEBUG${RSET} ${CGRN}${NAME}${RSET} compiled!\n└──────────\n"

clean:
		@if [[ $(if $(filter r,${MAKECMDGOALS}),1,0) == "1" ]]; then \
			printf "├──────────\n"; \
		else \
			printf "┌──────────\n"; \
		fi
		@if [ -d "./${OBJDIR}" ]; then \
			printf "│\tRemoving ${CRED}${OBJDIR}${RSET} for ${CYEL}${NAME}${RSET}\n"; \
			rm -rf ${OBJDIR}; \
		else \
			if [[ $(if $(filter fclean,${MAKECMDGOALS}),1,0) == "1" ]]; then \
				printf "│\t${CYEL}${OBJDIR}${RSET} already removed!\n├──────────\n"; \
			else \
				printf "│\t${CYEL}${OBJDIR}${RSET} already removed!\n└──────────\n"; \
			fi; \
		fi
		@if [ -d "./${NAME}.dSYM" ]; then \
			printf "│\tRemoving ${CYEL}${NAME}.dSYM${RSET}\n"; \
			rm -rf ${NAME}.dSYM; \
		fi

fclean: clean
		@if [ -e "./${NAME}" ]; then \
			if [[ $(if $(filter r,${MAKECMDGOALS}),1,0) == "1" ]]; then \
				printf "│\tRemoving ${CYEL}${NAME}${RSET}\n"; \
			else \
				printf "│\tRemoving ${CYEL}${NAME}${RSET}\n└──────────\n"; \
			fi; \
			rm -f ${NAME}; \
		else \
			if [[ $(if $(filter r,${MAKECMDGOALS}),1,0) == "1" ]]; then \
				printf "│\t${CYEL}${NAME}${RSET} already removed!\n├──────────\n"; \
			else \
				printf "│\t${CYEL}${NAME}${RSET} already removed!\n└──────────\n"; \
			fi; \
		fi

re:
		@printf "\n┌──────────\n│ Cleaning and ${CGRN}recompiling${RSET}...\n"
		@${MAKE} fclean r
		@${MAKE} all r

test: debug
	@printf "Testing ${CYEL}a non-existent file...${RSET}\n"
	-./webserv configs/doesntexist.conf
	@printf "Testing ${CYEL}a non-existent file with an invalid extension...${RSET}\n"
	-./webserv configs/doesntexist.conffff
	@printf "Testing ${CYEL}a jpg...${RSET}\n"
	-./webserv public/uploads/400.jpg
	@printf "Testing ${CYEL}an empty file...${RSET}\n"
	-./webserv configs/empty.conf
	@printf "Testing ${CYEL}an file with only a whitespace...${RSET}\n"
	-./webserv configs/whitespace.conf
	@printf "Testing ${CYEL}a broken config...${RSET}\n"
	-./webserv configs/broken.conf
	@printf "Testing ${CYEL}another broken config...${RSET}\n"
	-./webserv configs/broken2.conf
	@printf "Testing ${CYEL}YET another broken config...${RSET}\n"
	-./webserv configs/broken3.conf
	@printf "Testing ${CYEL}the last broken config...${RSET}\n"
	-./webserv configs/broken4.conf
	@printf "Testing ${CYEL}an incomplete config...${RSET}\n"
	-./webserv configs/incomplete.conf
	@printf "Testing a ${CGRN}working config...${RSET}\n"
	-./webserv configs/file.conf

.PHONY: all fclean clean re

$(eval r:;@:)
$(eval dbg:;@:)
