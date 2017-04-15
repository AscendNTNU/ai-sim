p1 : main.o AI.o World.o State.o Robot.o Drone.o Plank.o SimSim.o
	g++ -Wall main.o World.o AI.o State.o Robot.o Drone.o Plank.o SimSim.o -o p1


SimSim.o : SimSim.cpp
	g++ -Wall -c -std=c++11 -Wswitch SimSim.cpp

main.o : main.cpp AI.cpp State.cpp Robot.cpp Drone.cpp Plank.cpp SimSim.cpp
	g++ -Wall -c -std=c++11 main.cpp

World.o :  World.cpp
	g++ -Wall -c -std=c++11 World.cpp

AI.o : AI.cpp State.cpp Robot.cpp Drone.cpp Plank.cpp
	g++ -Wall -c -std=c++11 AI.cpp

State.o : State.cpp Robot.cpp Drone.cpp Plank.cpp
	g++ -Wall -c -std=c++11 State.cpp

Robot.o : Robot.cpp Plank.cpp
	g++ -Wall -c -std=c++11 Robot.cpp

Plank.o : Plank.cpp
	g++ -Wall -c -std=c++11 Plank.cpp

Drone.o : Drone.cpp Robot.cpp	
	g++ -Wall -c -std=c++11 Drone.cpp


clean:
	\rm *.o *~ p1