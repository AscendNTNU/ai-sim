p1 : main.o AI.o World.o State.o Robot.o Drone.o Plank.o
	g++ -Wall main.o World.o AI.o State.o Robot.o Drone.o Plank.o -o p1

main.o : main.cpp AI.cpp AI.h State.cpp State.h Robot.cpp Robot.h Drone.cpp Drone.h Plank.cpp Plank.h structs.h
	g++ -Wall -c -std=c++11 main.cpp

World.o : World.h World.cpp structs.h
	g++ -Wall -c -std=c++11 World.cpp

AI.o : AI.cpp AI.h State.cpp State.h Robot.cpp Robot.h Drone.cpp Drone.h Plank.cpp Plank.h structs.h
	g++ -Wall -c -std=c++11 AI.cpp

State.o : State.cpp State.h Robot.cpp Robot.h Drone.cpp Drone.h Plank.cpp Plank.h structs.h
	g++ -Wall -c -std=c++11 State.cpp

Robot.o : Robot.cpp Robot.h Plank.cpp Plank.h structs.h
	g++ -Wall -c -std=c++11 Robot.cpp

Plank.o : Plank.cpp Plank.h structs.h
	g++ -Wall -c -std=c++11 Plank.cpp

Drone.o : Drone.cpp Drone.h Robot.cpp Robot.h structs.h
	g++ -Wall -c -std=c++11 Drone.cpp

SimIn.o : SimIn.cpp SimIn.h
	g++ -Wall -c -std=c++11 -Wswitch SimIn.cpp
clean:
	\rm *.o *~ p1