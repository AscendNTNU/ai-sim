#include "structs.h"
#include "Robot.h"

//functions
//isOutsideOfPlank
//

tree_action_t DFS(Robot robot,tree_action_t currentLevel, int n){
	
	tree_action_t temp = currentLevel;
	tree_action_t max

	action_t action_On = {.type = actionType_LandOnTopOf, .when_To_Act = 0};
	action_t action_Front= {.type = actionType_LandInFrontOf, .when_To_Act = 0};
	
	point_t startPosition = robot.position;
	point_t tempPosition = robot.position;

	float startOrientation = robot.orientation;

	int i = 0;
	
	while (i > 0) {
		if (isOutsideOfPlank(x,y, target.plank)) {
            if (backwards) {
                return best_action;
            } else {
                i = n+1;
                backwards = true;
                target.angle += MATH_PI;
            }
		}

		robot.addToTimer(target.intersection.travel_time+time_after_intersection);


		//Try action land on top.
        temp.reward = findRobotValue(robot);
		temp = currentLevel;
		robot.setPositionOrientation(robot.getPosition(), robot.getOrientation()+0.785);
		action_On.when_To_Act = i;
        temp.actions.push(action_On);
        temp = DFS(robot,temp, n);

        if(temp.reward > max.reward){
        	max = temp;
        }

        //Try action land in front
        temp = currentLevel;
        robot.setPositionOrientation(robot.getPosition(), robot.getOrientation()-0.785 + 3.14);
        float rewardInFront = findRobotValue(robot);
        temp.reward = findRobotValue(robot);
        action_Front.when_To_Act = i;
        temp.actions.push(action_On);
        temp = DFS(robot, temp, n);
        if(temp.reward > max.reward){
        	max = temp;
        }


        if (backwards) {
            tempPosition.x -= step_x;
            tempPosition.y -= step_y;
            robot.setPositionOrientation(tempPosition, startOrientation + 3.14);
            i -= 1;

        } else {
			tempPosition.x += tempPosition.x;
            tempPosition.y += tempPosition.y;
            robot.setPositionOrientation(tempPosition, startOrientation);
            i += 1;
        }
        time_after_intersection = time_after_intersection + (step_size)/Robot_Speed;
	}
    return max;
}

// dfs(state)
// for each action from state
//		get action, get action reward, get new state
//		if reward better than previous max reward update reward and action array
//		dfs(new_state)
//		
//		

// procedure DFS(G,v):
// 2      label v as discovered
// 3      for all edges from v to w in G.adjacentEdges(v) do
// 4          if vertex w is not labeled as discovered then
// 5              recursively call DFS(G,w)

// ActionNode root = new ActionNode(NULL);
// for(int target = 0; target < number_Of_Targets; target){
// 	Robot target = new Robot(this.state.robot[i]);
// 	reward = 
// 	action_t action1 = {.type = sim_CommandType_LandOnTopOf, .target = target, .reward = 
//     action_t action2 = {.type = sim_CommandType_LandInFrontOf, 
// 	ActionNode child1 = new ActionNode(root*, );
// 	ActionNode child2 = new ActionNode(root*);
// 	root.addChild(child1*);
// 	root.addChild(child2*);
// }



// struct action_t{
// 	action_Type type;
// 	Robot target;
// 	point_t waypoints[10];
// 	float reward;
// 	float when_To_Act;
// }

//     int index = target.index;
//     float angle = wrap_angle(state.target_q[index]);
    
//     target.intersection = calculateInterceptionPoint(state, target);
//     float temp = target.intersection.travel_time;
//     //target.intersection.travel_time = 0;

//     float n = 10;
//     float step_size = target.plank.length/n;
//     float step_x = step_size*cos(angle);
//     float step_y = step_size*sin(angle);

//     float x = target.intersection.x;
//     float y = target.intersection.y;
//     float time_after_intersection = 0;

//     // Temporary max rewarded action
//     ActionReward best_action;
//     best_action.reward = -1000;
//     best_action.action = ai_waiting;
//     best_action.time_until_intersection = target.intersection.travel_time;
    
//     ActionReward action_to_check;
//     bool backwards = false;
//     int i = 1;
//     while (i > 0) {
//         printActionIteration(i, target, x, y, state, best_action.time_until_intersection, time_after_intersection);

//         if (isOutsideOfPlank(x,y, target.plank)) {
//             std::cout << "End of plank was reached " << std::endl;
//             if (backwards) {
//                 return best_action;
//             } else {
//                 i = n+1;
//                 backwards = true;
//                 target.angle += MATH_PI;
//             }
//         }
//         action_to_check = getBestActionAtPoint(target, x, y, state, time_after_intersection);

//         if (action_to_check.reward > best_action.reward) {
//             best_action = action_to_check;
//             best_action.x = x;
//             best_action.y = y;
//             best_action.time_after_intersection = time_after_intersection;
//             best_action.time_until_intersection = target.intersection.travel_time;
//         }

        

//         if (backwards) {
//             x = x-step_x;
//             y = y-step_y;
//             i -= 1;
//         } else {
//             x = x+step_x;
//             y = y+step_y;
//             i += 1;
//         }
//         time_after_intersection = time_after_intersection + (step_size)/Robot_Speed;

//     }
//     best_action.time_until_intersection = temp;
//     return best_action;
// }