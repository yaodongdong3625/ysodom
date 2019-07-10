#include <sstream>
#include "ros/ros.h"
#include "std_msgs/String.h"
#include "encoder/robotencoder.h"



int main(int argc, char *argv[])
{
    /* code for main function */
    ros::init(argc, argv, "robotencoder");
    ros::NodeHandle nh;

    ros::Publisher pub_encoder=nh.advertise<encoder::robotencoder>("/rencoder",100);
    
    ros::Rate pub_hz(50);

    int count=0;

    encoder::robotencoder data_encoder;
    data_encoder.leftencoder=65530/2;
    data_encoder.rightencoder=65530/2;

    while (ros::ok())
    {
        /* code for loop body */

         pub_encoder.publish(data_encoder);
         
         ros::spinOnce();

         pub_hz.sleep();

         ++count;

         data_encoder.rightencoder=data_encoder.rightencoder+40;
         data_encoder.leftencoder=data_encoder.leftencoder+40;



    }
    

    return 0;
}