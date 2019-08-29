#include "ros/ros.h"
#include "ball_chaser/DriveToTarget.h"
#include <sensor_msgs/Image.h>
#include <iostream>

using namespace std;

// Define a global client that can request services
ros::ServiceClient client;

// This function calls the command_robot service to drive the robot in the specified direction
void drive_robot(float lin_x, float ang_z)
{

    ROS_INFO_STREAM("Driving the robot");

    // Request a service and pass the velocities to it to drive the robot
    ball_chaser::DriveToTarget srv;
    srv.request.linear_x = lin_x;
    srv.request.angular_z = ang_z;

    // Throws error if fail to call drive service
    if (!client.call(srv))
        ROS_ERROR("Failed to call service command_robot");    
}

// This callback function continuously executes and reads the image data
void process_image_callback(const sensor_msgs::Image img)
{   
    // Loop through each pixel in the image and check if there's a bright white one
    // Then, identify if this pixel falls in the left, mid, or right side of the image
    // Depending on the white ball position, call the drive_bot function and pass velocities to it
    // Request a stop when there's no white ball seen by the camera
   
    // Comparing R,G,B values to find white pixel 
    for (int i = 0; i < img.height; ++i){
	for (int j = 0; j < img.width; ++j){
            if ((img.data[i + j*3 + 0] == 255) && (img.data[i + j*3 + 1] == 255) && (img.data[i + j*3 + 2] == 255)) {
                string position; 
                if (j < (img.width / 3)) {
	            position = "left";
		    ROS_INFO_STREAM("Left");
	    	    drive_robot(5.0, -1.0);
	        } 
	        else if (j < (2 * img.width / 3)) {
                    position = "center";
		    ROS_INFO_STREAM("Center");
		    drive_robot(5.0, 0.0);
                } 
	        else{
		    position = "right";
		    ROS_INFO_STREAM("Right");
		    drive_robot(5.0, 1.0);
	        }
            }
	    else {
	        drive_robot(0.0, 0.0);
		ROS_INFO_STREAM("Stopped");
	    }
        }
    }       
}

int main(int argc, char** argv)
{
    // Initialize the process_image node and create a handle to it
    ros::init(argc, argv, "process_image");
    ros::NodeHandle n;

    // Define a client service capable of requesting services from command_robot
    client = n.serviceClient<ball_chaser::DriveToTarget>("/ball_chaser/command_robot");

    // Subscribe to /camera/rgb/image_raw topic to read the image data inside the process_image_callback function
    ros::Subscriber sub1 = n.subscribe("/camera/rgb/image_raw", 10, process_image_callback);

    // Handle ROS communication events
    ros::spin();

    return 0;
}
