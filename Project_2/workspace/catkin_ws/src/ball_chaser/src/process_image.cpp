#include "ros/ros.h"
#include "ball_chaser/DriveToTarget.h"
#include <sensor_msgs/Image.h>

ros::ServiceClient client;

void drive_robot(float lin_x, float ang_z)
{
	ROS_INFO_STREAM("Moving the robot");

	ball_chaser::DriveToTarget srv;
	srv.request.linear_x = lin_x;
	srv.request.angular_z = ang_z;

	if(!client.call(srv))
		ROS_ERROR("Failed to call service DriveToTarget");
}

void process_image_callback(const sensor_msgs::Image img)
{
	int white_pixel = 255;
	bool is_white = false;

	int index = -1;
	float pos = -1.0;

	// loop through pixels
	for (int i = 0; i < img.height * img.width; i++)
	{
		// loop through rbg values		
		for (int j = 0; j < 3; j++)
		{ 	
			// stop the loop if a value is not white
			if(img.data[i*3 + j] == white_pixel)
				is_white = true;
			else if(img.data[i*3 + j] != white_pixel){
				is_white = false;
				break;
			}
		}
		
		// stop seaching if a white pixel is found
		if (is_white == true){
			index = i % img.width;
			pos = (float)index / img.width;	
			break;
		}
	}
	
	if (index == -1)
		drive_robot(0.0, 0.0);
	else if (pos < 0.33)
		drive_robot(0.0, 0.1);
	else if (pos < 0.67)
		drive_robot(0.2, 0.0);
	else 
		drive_robot(0.0, -0.1);

}

int main(int argc, char** argv)
{
	ros::init(argc, argv, "process_image");
	ros::NodeHandle n;

	client = n.serviceClient<ball_chaser::DriveToTarget>("/ball_chaser/command_robot");
	ros::Subscriber sub1 = n.subscribe("/camera/rgb/image_raw", 10, process_image_callback);
	ROS_INFO("Ready to send process images");
	ros::spin();

	return 0;
}
