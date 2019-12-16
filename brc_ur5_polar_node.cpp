#include <iostream>
#include <sstream>
#include <cmath>
#include "ros/ros.h"
#include "std_msgs/String.h"

//Funktionen
struct Position new_position(int start_pos);

// Globale Variablen
float radius = 0.1;

struct Position
{
	float Xcoord;
	float Ycoord;
	int Angle;
};

int main(int argc, char **argv)
{
	ros::init(argc, argv, "brc_ur5_polar_node");
	ros::NodeHandle n;
	std_msgs::String msg;
		
	// Topic Initialisieren
	ros::Publisher brc_topic_position = n.advertise<std_msgs::String>("/ur_driver/URScript", 1000);
	
	//Zur Anfangsposition fahren
	std::stringstream ss_postion;
	ss_postion.str("");
	ss_postion << "movej(p[0.3, 0.3, 0.2, 2.0, -2.4, 0.0], a=1, v=1, t=2)";

	// Nachricht publishen
	ROS_INFO("Fahre zur Anfangsposition...");
	msg.data = ss_postion.str();

	for (int i = 0; i < 3; i++)
	{
		brc_topic_position.publish(msg);
		ros::Duration(3.0).sleep();
	}

	//Zufällige Anfangsposition und Zielposition zwischen 0° und 360° generieren.
	srand(time(0));
	
	struct Position init_pos;
	init_pos.Angle = rand() % 360;
	init_pos.Xcoord = 0.3 + radius * cos(init_pos.Angle * 2 * 3.141592 / 360);
	init_pos.Ycoord = 0.3 + radius * sin(init_pos.Angle * 2 * 3.141592 / 360);
	ROS_INFO("Zufaellige Anfangsposition: %d, X: %f, Y: %f", init_pos.Angle % 360, init_pos.Xcoord, init_pos.Ycoord);
	
	struct Position target_pos;
	target_pos.Angle = rand() % 360;
	target_pos.Xcoord = 0.3 + radius * cos(target_pos.Angle * 2 * 3.141592 / 360);
	target_pos.Ycoord = 0.3 + radius * sin(target_pos.Angle * 2 * 3.141592 / 360);
	ROS_INFO("Zielposition: %d", target_pos.Angle % 360);
	
	//Zufall Anfangsposition fahren
	
	ss_postion.str("");
	ss_postion << "movej(p[" << init_pos.Xcoord << ", " << init_pos.Ycoord << ", 0.1124, 2.0, -2.4, 0.0], a=1, v=1, t=2)";

	// Nachricht publishen
	ROS_INFO("Fahre zur zufaelligen Anfangsposition...");
	msg.data = ss_postion.str();

	for (int i = 0; i < 3; i++)
	{
		brc_topic_position.publish(msg);
		ros::Duration(3.0).sleep();
	}

	//Zufällige Bewegung generieren
	struct Position new_pos;
	
	int diff_target_init
	int diff_target_new
	
	for(int i=0; i<20; i++)
	{
		
		new_pos = new_position(init_pos.Angle);
		
		diff_target_init = std::abs(target_pos.Angle - init_pos.Angle);
		diff_target_new = std::abs(target_pos.Angle - new_pos.Angle);
		
		if(diff_target_new > 180)
		{
			new_pos.Angle += 360;
			diff_target_new = std::abs(target_pos.Angle - new_pos.Angle);
		}
		
		//Sende +/-
		if(new_pos.Angle > init_pos.Angle)
		{
			if(target_pos.Angle < new_pos.Angle && target_pos.Angle > init_pos.Angle)
				//Sende von + zu -
				ROS_INFO("Von (+) zu (-)");
		}
		
		//Sende -/+
		if(new_pos.Angle < init_pos.Angle)
		{
			if(target_pos.Angle > new_pos.Angle && target_pos.Angle < init_pos.Angle)
				//Sende von + zu -
				ROS_INFO("Von (+) zu (-)");
		}
			
		//Sende positiven Marker
		else if(diff_target_new < diff_target_init)
		{
			//Sende positiven Marker
			ROS_INFO("Bewegt sich zum Ziel hin.");
		}
		
		//Sende negativen Marker
		else if(diff_target_new > diff_target_init)
		{
			//Sende negativen Marker
			ROS_INFO("Bewegt sich vom Ziel weg.");
		}
		
		init_pos.Angle = new_pos.Angle;
		
		//Fahre zur neuen Position
		ROS_INFO("Neue zufallige Position: %d, X,Y: %f, %f", new_pos.Angle % 360, new_pos.Xcoord, new_pos.Ycoord);
		ros::Duration(2.0).sleep();

		ss_postion.str("");
		ss_postion << "movej(p[" << new_pos.Xcoord << ", " << new_pos.Ycoord << ", 0.1124, 2.0, -2.4, 0.0], a=1, v=1, t=2)";
		msg.data = ss_postion.str();

		for (int i = 0; i < 3; i++)
		{
			brc_topic_position.publish(msg);
			ros::Duration(3.0).sleep();
		}
	}

	return 0;
}


struct Position new_position(int start_pos)
{
	float rand_move;
	struct Position new_pos;

	srand(time(0));
	rand_move = rand() % 150;

	if (rand_move < 75)
		rand_move = rand_move - 75;
	else
		rand_move = 150 - rand_move;

	new_pos.Angle = (start_pos + rand_move) % 360;	
	
	new_pos.Xcoord = 0.3 + radius * cos(new_pos.Angle * 2 * 3.141592 / 360);
	new_pos.Ycoord = 0.3 + radius * sin(new_pos.Angle * 2 * 3.141592 / 360);
	
	return new_pos;
}



















