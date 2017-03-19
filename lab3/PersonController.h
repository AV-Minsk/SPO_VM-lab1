﻿#ifndef PERSON_CONTROLLER
#define PERSON_CONTROLLER

#include <iostream>
#include "Person.h"
#include "AllExceptions.h"

class PersonController
{
#ifdef _WIN32
	HANDLE EVENT[4];
#elif (defined(__linux__) || defined(__unix__))
	pid_t serverPID;
	int shmPersonNameID;

	pid_t getServerPID();
#endif

	Person person;

public:
	PersonController(std::string name);
	~PersonController();

	void run();
};

#endif
