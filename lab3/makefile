objects = main.o CoffeeMachine.o Person.o Drink.o CoffeeMachineController.o PersonController.o Controller.o AJIOBlib.o
CFLAGS = -std=c++11
CXXFLAGS = $(CFLAGS)
linkFlags = -lrt -lpthread
sourceWay =
deps = $(sourceWay)info.db
destinationWay = bin/unix/
destinationName = lab3

objWay = $(destinationWay)obj/
objBackHome = ../../../

goObject = cd $(objWay) &&
compilerPrefix = $(goObject) $(CXX)
prefix = $(compilerPrefix) $(CXXFLAGS) -c $(objBackHome)$(sourceWay)

MKDIR = mkdir -p
RMDIR = -rm -r
COPY = cp


.PHONY: $(destinationName) clean max_clean baseInit copyDeps run runPerson runMachine


$(destinationName) : baseInit $(objects) copyDeps
	$(compilerPrefix) -o $(objBackHome)$(destinationWay)$(destinationName) $(objects) $(linkFlags)

main.o : $(sourceWay)main.cpp $(sourceWay)Controller.h
	$(prefix)main.cpp
CoffeeMachine.o : $(sourceWay)CoffeeMachine.cpp $(sourceWay)CoffeeMachine.h
	$(prefix)CoffeeMachine.cpp
Person.o : $(sourceWay)Person.cpp $(sourceWay)Person.h
	$(prefix)Person.cpp
Drink.o : $(sourceWay)Drink.cpp $(sourceWay)Drink.h
	$(prefix)Drink.cpp
CoffeeMachineController.o : $(sourceWay)CoffeeMachineController.cpp $(sourceWay)CoffeeMachineController.h
	$(prefix)CoffeeMachineController.cpp
PersonController.o : $(sourceWay)PersonController.cpp $(sourceWay)PersonController.h
	$(prefix)PersonController.cpp
Controller.o : $(sourceWay)Controller.cpp $(sourceWay)Controller.h
	$(prefix)Controller.cpp
AJIOBlib.o : $(sourceWay)AJIOBlib.cpp $(sourceWay)AJIOBlib.h
	$(prefix)AJIOBlib.cpp

max_clean :
	$(MAKE) -i clean
	$(RMDIR) $(destinationWay)

clean :
	$(RMDIR) $(objWay)

baseInit :
	$(MKDIR) $(destinationWay)
	$(MKDIR) $(objWay)

copyDeps : $(deps)
	$(COPY) $(deps) $(destinationWay)

run :
	cd $(destinationWay) && ./$(destinationName)

runPerson :
	cd $(destinationWay) && ./$(destinationName) -p

runMachine :
	cd $(destinationWay) && ./$(destinationName) -m