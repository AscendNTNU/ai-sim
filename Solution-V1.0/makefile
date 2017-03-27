p1 : main.o World.o SimIn.o AI.o State.o Robot.o Drone.o Plank.o structs.o
	g++ -Wall main.o World.o SimIn.o AI.o State.o Robot.o Drone.o Plank.o structs.o -o p1

World.o : World.h World.cpp structs.h
	g++ -Wall -c World.cpp

AI.o : AI.cpp AI.h State.cpp State.h Robot.cpp Robot.h Drone.cpp Drone.h Plank.cpp Plank.h structs.h
	g++ -Wall -c AI.cpp

State.o : State.cpp State.h Robot.cpp Robot.h Drone.cpp Drone.h Plank.cpp Plank.h structs.h
	g++ -Wall -c State.cpp

Robot.o : Robot.cpp Robot.h Plank.cpp Plank.h structs.h
	g++ -Wall -c Robot.cpp

Plank.o : Plank.cpp Plank.h structs.h
	g++ -Wall -c Plank.cpp

Drone.o : Drone.cpp Drone.h Robot.cpp Robot.h structs.h
	g++ -Wall -c Drone.cpp

SimIn.o : SimIn.cpp SimIn.h
	g++ -Wall -c SimIn.cpp SimIn.h
clean:
	\rm *.o *~ p1