﻿#ifdef _WIN32

#include <windows.h>

#elif (defined(__linux__) || defined(__unix__))

#include <queue>
#include <cstring>
#include <signal.h>
#include <sys/types.h>
#include <unistd.h>

#else
#error Bad operation system. Please, recompile me to Linux, Unix or Windows
#endif

#include "CoffeeMachineController.h"

#ifdef _WIN32

CoffeeMachineController::CoffeeMachineController()
{
	using namespace VA::constants;

	//check existing
	EVENT[0] = OpenEvent(EVENT_ALL_ACCESS, NULL, isMachineFree);
	if (EVENT[0] != NULL)
	{
		std::cout << "Ошибка! Автомат уже запущен. Для выхода введите любой символ"; //error. Machine already started
		CloseHandle(EVENT[0]);
		std::cin.get();
		return;
	}

	EVENT[1] = OpenEvent(EVENT_ALL_ACCESS, NULL, fromUser);
	if (EVENT[1] != NULL)
	{
		std::cout << "Ошибка! Автомат уже запущен. Для выхода введите любой символ"; //error. Machine already started
		CloseHandle(EVENT[1]);
		std::cin.get();
		return;
	}

	EVENT[2] = OpenEvent(EVENT_ALL_ACCESS, NULL, fromMachine);
	if (EVENT[2] != NULL)
	{
		std::cout << "Ошибка! Автомат уже запущен. Для выхода введите любой символ"; //error. Machine already started
		CloseHandle(EVENT[2]);
		std::cin.get();
		return;
	}
	EVENT[3] = OpenEvent(EVENT_ALL_ACCESS, NULL, disconnectUser);
	if (EVENT[3] != NULL)
	{
		std::cout << "Ошибка! Автомат уже запущен. Для выхода введите любой символ"; //error. Machine already started
		CloseHandle(EVENT[3]);
		std::cin.get();
		return;
	}

	//create events
	EVENT[0] = CreateEvent(NULL, false, false, isMachineFree);
	EVENT[1] = CreateEvent(NULL, false, false, fromUser);
	EVENT[2] = CreateEvent(NULL, false, false, fromMachine);
	EVENT[3] = CreateEvent(NULL, false, false, disconnectUser);
}

CoffeeMachineController::~CoffeeMachineController()
{
	for (int i = 0; i < 3; i ++)
	{
		CloseHandle(EVENT[i]);
	}
}

void CoffeeMachineController::run()
{
	//raise flag1
	if (!SetEvent(EVENT[0]))
	{
		std::cout << "Ошибка! Не уделось провзаимодействовать с пользователем."; //error. Event is not pulsed
		return;
	}

	HANDLE E1[] = {EVENT[1], EVENT[3]};

	do
	{
		//wait flag2
		DWORD dw = WaitForMultipleObjects(2, E1, false, INFINITE);
		switch (dw)
		{
		case WAIT_FAILED:
			std::cout << "Ошибка! Не уделось провзаимодействовать с пользователем";
			break;
		case WAIT_OBJECT_0 + 0:
			machine.proceed();
			machine.writeToFile();

			//raise flag3
			if (!SetEvent(EVENT[2]))
			{
				std::cout << "Ошибка! Не уделось провзаимодействовать с пользователем"; //error. Event is not pulsed
				return;
			}

			WaitForSingleObject(EVENT[1],INFINITE);

			break;
		case WAIT_OBJECT_0 + 1:
			//raise flag1
			if (!SetEvent(EVENT[0]))
			{
				std::cout << "Ошибка! Не уделось провзаимодействовать с пользователем."; //error. Event is not pulsed
				return;
			}
			break;
		default: 
			std::cout << "Ошибка! Что-то пошло не так. Вы не должны видеть это.";
		}
	}
	while (true);
}

#elif (defined(__linux__) || defined(__unix__))

namespace
{
	std::queue<pid_t> PIDq;
	bool signalIsHere[] = {false, false, false};
}

pid_t StartWorkingWithNewUser()
{
	if (PIDq.empty())
	{
		return 0;
	}

	pid_t currPID = PIDq.front();
	PIDq.pop();
	if (currPID == 0)
	{
		return 0;
	}

	kill(currPID, SIGF0);

	return currPID;
}

//save pid to queue
void hdlF0Machine(int sig, siginfo_t* sigptr, void*)
{
	if (!sigptr)
	{
		return;
	}

	PIDq.push(sigptr -> si_pid);

	signalIsHere[0] = true;
}

//нужно обработать результаты
void hdlF1Machine(int sig, siginfo_t* sigptr, void*)
{
	signalIsHere[1] = true;
}

//предыдущий пользователь закончил работу
void hdlF2Machine(int sig, siginfo_t* sigptr, void*)
{
	signalIsHere[2] = true;
}

int setSigAction(int sig, void (*handleFun) (int, siginfo_t*, void*))
{
	struct sigaction act;
	memset(&act, NULL, sizeof(act));	//clear all struct
	act.sa_sigaction = handleFun;
	act.sa_flags = SA_SIGINFO;
	sigset_t set;
	sigemptyset(&set);
	sigaddset(&set, sig);
	act.sa_mask = set;
	return sigaction(sig, &act, NULL);
}

void CoffeeMachineController::writePID()
{
	std::fstream f;
	f.open(ServerPIDfilename, std::ios::out | std::ios::trunc);

	if (!f)
	{
		std::cout << ("Ошибка открытия файла") << std::endl;
		return;
	}

	f << (int)getpid();
	f.close();
}

CoffeeMachineController::CoffeeMachineController()
{
	setSigAction(SIGF0, hdlF0Machine);
	setSigAction(SIGF1, hdlF1Machine);
	setSigAction(SIGF2, hdlF2Machine);

	writePID();

	currPID = 0;
}

CoffeeMachineController::~CoffeeMachineController()
{
	
}

void CoffeeMachineController::run()
{
	bool isWorkWithUserNow = false;

	while (true)
	{
		if (!isWorkWithUserNow)
		{
			currPID = StartWorkingWithNewUser();

			if (currPID != 0)
			{
				isWorkWithUserNow = true;
			}
		}

		if (signalIsHere[2])
		{
			signalIsHere[2] = false;
			isWorkWithUserNow = false;
			continue;
		}

		if (signalIsHere[1])
		{
			signalIsHere[1] = false;
			machine.proceed();
			machine.writeToFile();

			kill(currPID, SIGF1);

			continue;
		}
	}
}

#endif