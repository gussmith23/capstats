#ifndef CAPSTATS_EXCEPTIONS_H
#define CAPSTATS_EXCEPTIONS_H

#include <exception>

class PlayerNotFoundException : public std::exception {
public:
	inline const char* what() const throw() {
		return "Player not found";
	};
};

class GameNotFoundException : public std::exception {
public:
	inline const char* what() const throw() {
		return "Game not found";
	};
};

#endif