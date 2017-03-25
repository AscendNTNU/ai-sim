class ActionNode{
private:
	action_t action;
	Drone drone;
	ActionNode* parent;
	std::list<ActionNode*> children;

public:
	ActionNode(ActionNode *parent): parent(parent){}
	ActionNode(ActionNode *parent, action_t action, Drone drone): parent(parent){

	}
	void addChild(ActionNode *child){
		children.push_back(child);
	}

	action_t getAction(){
		return this.action;
	}

	std::list<ActionNode*> getChildren(){
		return children
	}
}


//     root.parent = NIL
//     add root to S
//     Q.enqueue(root)                      

//     while Q is not empty:
//         current = Q.dequeue()
//         if current is the goal:
//             return current
//         for each node n that is adjacent to current:
//             if n is not in S:
//                 add n to S
//                 n.parent = current
//                 Q.enqueue(n)


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