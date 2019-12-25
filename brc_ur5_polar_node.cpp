#include <iostream>
#include <sstream>
#include <cmath>
#include "ros/ros.h"
#include "std_msgs/String.h"
#include <string>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>

//Funktionen
struct Position new_position(int start_pos);

// Globale Variablen
float radius = 0.13;

struct Position
{
	float Xcoord;
	float Ycoord;
	int Angle;
};

int main(int argc, char **argv)
{

	int client, server;
	int portNum = 1700;
	char buffer[1];
	struct sockaddr_in server_addr;
	socklen_t size;
	if((client = socket(AF_INET, SOCK_STREAM, 0)) < 0)
        std::cout<<"Client Socket konnte nicht erstellt werden"<<std::endl;

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htons(INADDR_ANY);
    server_addr.sin_port = htons(portNum);
    int ErrorBinding;
	if(( ErrorBinding = bind(client, (struct sockaddr*)&server_addr, sizeof(server_addr))) < 0)
        std::cout<<"Binding failed"<<std::endl;
    size = sizeof(server_addr);

    listen(client, 1);
    std::cout << "\nAccepting clients..." <<std::endl;

    if((server = accept(client, (struct sockaddr *)&server_addr, &size)) < 0){
        std::cout<<"Couldn't accept"<<std::endl;
        return 1 ;
    }
    else
        std::cout<<"Client connected"<<std::endl;
        checkpoint1 :
    	strcpy(buffer,"T");
        
	for(int i = 0 ; i<2 ;i++ ){
    	send(server, buffer, 1, 0);
	}

	ros::init(argc, argv, "brc_ur5_polar_node");
	ros::NodeHandle n;
	std_msgs::String msg;

	// Topic Initialisieren
	ros::Publisher brc_topic_position = n.advertise<std_msgs::String>("/ur_driver/URScript", 1000);

	// Zufällige Anfangsposition zwischen 0° und 360° generieren.
	srand(time(0));

	struct Position init_pos;
	init_pos.Angle = rand() % 360;
	init_pos.Xcoord = 0.3 + radius * cos(init_pos.Angle * 2 * 3.141592 / 360);
	init_pos.Ycoord = 0.3 + radius * sin(init_pos.Angle * 2 * 3.141592 / 360);


	// Zufällige Zielposition zwischen 0° und 360° generieren.
	struct Position target_pos;
	target_pos.Angle = rand() % 360;
	target_pos.Xcoord = 0.3 + radius * cos(target_pos.Angle * 2 * 3.141592 / 360);
	target_pos.Ycoord = 0.3 + radius * sin(target_pos.Angle * 2 * 3.141592 / 360);
	ROS_INFO("Zufaellige Zielposition: %d, X: %f, Y: %f", target_pos.Angle, target_pos.Xcoord, target_pos.Ycoord);

	// Zum Mittelpunkt fahren höhe z = 0.2
	ROS_INFO("Fahre über Mittelpunkt ...");

	std::stringstream ss_postion;
	ss_postion.str("");
	ss_postion << "movej(p[0.3, 0.3, 0.2, 2.0, -2.4, 0.0], a=1, v=1, t=2)";

	msg.data = ss_postion.str(); // Nachricht publishen
	for (int i = 0; i < 3; i++)
	{
		brc_topic_position.publish(msg);
		ros::Duration(3.0).sleep();
	}

	// Zum Mittelpunkt fahren höhe z = 0.11
	ROS_INFO("Fahre zum Mittelpunkt...");

	ss_postion.str("");
	ss_postion << "movej(p[0.3, 0.3, 0.11, 2.0, -2.4, 0.0], a=1, v=1, t=2)";

	msg.data = ss_postion.str(); // Nachricht publishen
	for (int i = 0; i < 3; i++)
	{
		brc_topic_position.publish(msg);
		ros::Duration(2.0).sleep();
	}

	ROS_INFO("Enter drücken um fortzufahren...");
	for(;;)
	{
		if(getchar() == '\n')
			break;
	};

	// Zur Zielposition fahren
	ROS_INFO("Fahre zur Zielposition...");
	ss_postion.str("");
	ss_postion << "movej(p["<< target_pos.Xcoord <<"  , "<< target_pos.Ycoord <<", 0.11, 2.0, -2.4, 0.0], a=1, v=1, t=2)";

	msg.data = ss_postion.str(); // Nachricht publishen
	for (int i = 0; i < 3; i++)
	{
		brc_topic_position.publish(msg);
		ros::Duration(3.0).sleep();
	}

	ROS_INFO("Enter drücken um fortzufahren...");
	for(;;)
	{
		if(getchar() == '\n')
			break;
	};
        

	// Zur ersten zufaelligen Position fahren
	ss_postion.str("");
	ss_postion << "movej(p[" << init_pos.Xcoord << ", " << init_pos.Ycoord << ", 0.11, 2.0, -2.4, 0.0], a=1, v=1, t=2)";

	// Nachricht publishen
	ROS_INFO("Erste zufaellige Position: %d, X: %f, Y: %f", init_pos.Angle, init_pos.Xcoord, init_pos.Ycoord);
	msg.data = ss_postion.str();
        int i ;
	for (int i = 0; i < 3; i++)
	{
		brc_topic_position.publish(msg);
		ros::Duration(2.0).sleep();
	}

	///Zufällige Bewegung generieren
	struct Position new_pos;

	int diff_target_init;
	int diff_target_new;


	for(int i=0; i<20; i++)
	{	
		if((new_pos.Angle > target_pos.Angle - 15) && (new_pos.Angle < target_pos.Angle + 15))
		{
			strcpy(buffer,"Z");
            		send(server, buffer, 1, 0);
			ROS_INFO("Ziel wurde erreicht");
			break;
		}

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
			if(target_pos.Angle < new_pos.Angle && target_pos.Angle > init_pos.Angle){
				//Sende von + zu -
				ROS_INFO("Überspringt den Zielpunkt oder die Spiegelung des Zielpunkts");
				strcpy(buffer,"X");
				send(server, buffer, 1, 0);
				goto checkpoint;
                        }
		}

		//Sende -/+
		else if(new_pos.Angle < init_pos.Angle)
		{
			if(target_pos.Angle > new_pos.Angle && target_pos.Angle < init_pos.Angle){
				//Sende von - zu +
				ROS_INFO("Überspringt den Zielpunkt oder die Spiegelung des Zielpunkts");
				strcpy(buffer,"X");
				send(server, buffer, 1, 0);
				goto checkpoint ;
			}		 
		}




		//Sende negativen Marker
		if(diff_target_new > diff_target_init)
		{
			//Sende negativen Marker
			ROS_INFO("Bewegt sich vom Ziel weg.");
			strcpy(buffer,"0");
            		send(server, buffer, 1, 0);
		}

		//Sende positiven Marker
	        else if(diff_target_new < diff_target_init)
		{
			//Sende positiven Marker
			ROS_INFO("Bewegt sich zum Ziel hin.");
			strcpy(buffer,"1");
            		send(server, buffer, 1, 0);
  			
		}
		checkpoint :

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
			ros::Duration(2.0).sleep();
		}

		/*ROS_INFO("Enter drücken um fortzufahren...");
		for(;;)
		{
			if(getchar() == '\n')
				break;
		};
		*/
		

		
	}
    goto checkpoint1;
    close(client);
    close(server);
	return 0;
}


struct Position new_position(int start_pos)
{
	float rand_move;
	struct Position new_pos;

	srand(time(0));
	rand_move = rand() % 200;

	if (rand_move < 100)
		rand_move = rand_move - 100;
	else
		rand_move = 100 - rand_move;

	new_pos.Angle = start_pos + rand_move;
 	new_pos.Angle = new_pos.Angle % 360 ;

	new_pos.Xcoord = 0.3 + radius * cos(new_pos.Angle * 2 * 3.141592 / 360);
	new_pos.Ycoord = 0.3 + radius * sin(new_pos.Angle * 2 * 3.141592 / 360);

	return new_pos;
}



















