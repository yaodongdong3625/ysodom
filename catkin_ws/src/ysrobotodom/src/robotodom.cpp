#include <SerialPort.h>
#include <cstring>
#include <sensor_msgs/Imu.h>

#include "ros/ros.h"
#include "std_msgs/Bool.h"
#include "std_msgs/Int32.h"
#include "std_msgs/String.h"
#include "nav_msgs/Odometry.h"
#include "ysrobotodom/robotencoder.h"
#include "base_controller.h"

#include "tf/transform_broadcaster.h"
#include <pthread.h>

#include "imu_data_decode.h"
#include "packet.h"


#define DEGREES_TO_RADIANS(angle) (angle/180.0 * M_PI) 
#define GRAVITY 9.7964

// 下位机可以扩大成以 u32的左右轮编码器数值 变量 上传，上传一个大的数值，靠脉冲数 实现 加减，这样能解决 下位机 u16 的最大值限制问题，经常到最大变0，或者0到最大
// #define ENCODER_MAX 2147483647
#define ENCODER_MAX 65535 

#define PI 3.141592653


typedef unsigned char uint8;
typedef char int8;
typedef unsigned short uint16;
typedef short int16;
typedef unsigned int uint32;
typedef signed int int32;

using namespace LibSerial;


ros::Publisher  pub_odom;
ros::Publisher  pub_imu;
ros::Subscriber sub_encoder;


SerialPort imu_serial_port;

std::string name_port_imu;

bool use_base;
bool use_imu;  
bool use_imu_compensate;
bool publish_tf;


int encoder_resolution=4096; // need modify 
double publish_rate=50.0;   
double controller_rate=20.0;
double cmd_time_out=100.0;

double yaw_initial=0;
double yaw=0;
double yaw_old=0;

bool flag_imu_data_init=false;


namespace base_controller
{
    /* code 底盘 */ 
    BaseController base_control;  

    void convert_utctime_to_rostime(ros::Time &ros_time)
    {
        ros_time=ros::Time::now();
    }

    double NormalizeAngle(const double&angle_radius)
    {
        double two_pi(2.0* M_PI);
        return angle_radius- two_pi * std::floor((angle_radius+ M_PI)/two_pi);
    }
    
    // convert encoder data to rad pose accumulater
    // ubuntu int32 max is [-2147483648 : 2147483647], firmware need modify ,now max is 65535, robot speed if big ,probobaly exist question
    void convert_encoder_to_pose(const int32& left_encoderr,const int32& right_encoderr,double *pose)
    {
    //    int32 delta_encoder[2]={0,0};
    //    int32 current_encoder[2]={left_encoderr,right_encoderr};
    
    //    static double cycle_pose[2]={0.0, 0.0}; 
    //    static int32 last_encoder[2]={0, 0};

       int32 delta_encoder[2]={-left_encoderr,right_encoderr};

       for (int i = 0; i < 2; i++)
       {
           /* code for loop body */

        //    delta_encoder[i]= current_encoder[i]-last_encoder[i];
        //    // 最开始是中间值 ENCODER_MAX/2 ，from max to 0，go header 
        //    if(delta_encoder[i]<0 && abs(delta_encoder[i])> (ENCODER_MAX/2.0))
        //    {
        //          cycle_pose[i]+= (double)(ENCODER_MAX)/ encoder_resolution *2*PI;
        //    }
        //    else if(delta_encoder[i]>0 && abs(delta_encoder[i])>(ENCODER_MAX/2) ) // from min to max, go back
        //    {
        //        cycle_pose[i]+=(double)(-ENCODER_MAX) / encoder_resolution *2*PI;
        //    }
            
            pose[i]=pose[i]+((double)delta_encoder[i])/ encoder_resolution *2*PI;

        //  pose[i]=cycle_pose[i]+((double)current_encoder[i])/ encoder_resolution *2*PI;
        //  last_encoder[i]=current_encoder[i];

       }
       
         
    }


    // convert encoder data to rad pose accumulater
    // void convert_encoder_to_pose(const int32& left_encoderr,const int32& right_encoderr,double *pose)
    // {
    //    int32 delta_encoder[2]={0,0};
    //    int32 current_encoder[2]={left_encoderr,right_encoderr};
    
    //    static double cycle_pose[2]={0.0, 0.0}; 
    //    static int32 last_encoder[2]={0, 0};

    //    for (int i = 0; i < 2; i++)
    //    {
    //        /* code for loop body */

    //        delta_encoder[i]= current_encoder[i]-last_encoder[i];
    //        // 最开始是中间值 ENCODER_MAX/2 ，from max to 0，go header 
    //        if(delta_encoder[i]<0 && abs(delta_encoder[i])> (ENCODER_MAX/2.0))
    //        {
    //              cycle_pose[i]+= (double)(ENCODER_MAX)/ encoder_resolution *2*PI;
    //        }
    //        else if(delta_encoder[i]>0 && abs(delta_encoder[i])>(ENCODER_MAX/2) ) // from min to max, go back
    //        {
    //            cycle_pose[i]+=(double)(-ENCODER_MAX) / encoder_resolution *2*PI;
    //        }
            
    //         pose[i]=cycle_pose[i]+((double)current_encoder[i])/ encoder_resolution *2*PI;
    //         last_encoder[i]=current_encoder[i];

    //    }
       
         
    // }

    void init_serial(const char *fd, SerialPort& serial_port)
    {
        try {
            serial_port.Open(fd);
        }
        catch(OpenFailed)
        {
            ROS_ERROR("Open %s Failed! Please Check the serial port number and authority!", fd);
            return;
        }
        serial_port.SetBaudRate(BaudRate::BAUD_115200);
        serial_port.SetCharacterSize(CharacterSize::CHAR_SIZE_8);
        serial_port.SetFlowControl(FlowControl::FLOW_CONTROL_NONE);
        serial_port.SetParity(Parity::PARITY_NONE);
        serial_port.SetStopBits(StopBits::STOP_BITS_1);
        serial_port.FlushIOBuffers();
    }

    double GetCurrentTime()
    {
        return ros::Time::now().toSec();
    }

    // wxyz 的 排列 是根据 IMU 数据来对应的
    bool imu_data_publish(const double& acc_x,const double& acc_y,const double& acc_z,
                          const double& gyro_x,const double& gyro_y,const double& gyro_z,
                          const double& w,const double& x,const double& y,const double& z
                           )
    {
        // 
        const double orientation_covariance[9] = {DEGREES_TO_RADIANS(0.01), 0, 0,
                                       0, DEGREES_TO_RADIANS(0.01), 0,
                                       0, 0., DEGREES_TO_RADIANS(0.09)};
        const double angular_velocity_covariance[9] = { DEGREES_TO_RADIANS(0.0025), 0, 0,
                                                0, DEGREES_TO_RADIANS(0.0025), 0,
                                                0, 0, DEGREES_TO_RADIANS(0.0025)};
        const double linear_acceleration_covariance[9] = {0.0004, 0, 0,
                                                0, 0.0004, 0,
                                                0, 0, 0.0004};

         sensor_msgs::Imu imu_msg;
         imu_msg.header.stamp=ros::Time::now();
         imu_msg.header.frame_id="imu_link";
         imu_msg.orientation.x=x;
         imu_msg.orientation.y=y;
         imu_msg.orientation.z=z;
         imu_msg.orientation.w=w;

        imu_msg.linear_acceleration.x = acc_x;
        imu_msg.linear_acceleration.y = acc_y;
        imu_msg.linear_acceleration.z = acc_z;

        imu_msg.angular_velocity.x= gyro_x;
        imu_msg.angular_velocity.y= gyro_y;
        imu_msg.angular_velocity.z= gyro_z;


        for(int i=0; i< sizeof(orientation_covariance)/sizeof(double); i++)
        {
            imu_msg.orientation_covariance[i] = orientation_covariance[i];
        }

        for(int i=0; i< sizeof(angular_velocity_covariance)/sizeof(double); i++)
        {
            imu_msg.angular_velocity_covariance[i] = angular_velocity_covariance[i];
        }
        for(int i=0; i< sizeof(linear_acceleration_covariance)/sizeof(double); i++)
        {
            imu_msg.linear_acceleration_covariance[i] = linear_acceleration_covariance[i];
        }

        pub_imu.publish(imu_msg);

        static double previous_timestamp;
        double timestamp = GetCurrentTime();
       
        const double delta_T = (timestamp - previous_timestamp);
        if (delta_T > 2 )
        {
            ROS_INFO("imu status is abnormal ......");
        }
        else
        {
            // ROS_INFO("imu status is normal ......");
        }
        
        previous_timestamp = timestamp;
        return true;

    }


    void *imu_serial_imu_thread(void *arg)
    {
        uint8 ms_timeout=1;

        // buffer init
        const int size_buffer=200;
        uint8 buffer_temp[size_buffer];
        memset(buffer_temp,0,size_buffer);

        uint8 *ptr, *PtrHead, *PtrTail;
        ptr=&buffer_temp[0];
        PtrHead=&buffer_temp[0];
        PtrTail=&buffer_temp[0];

        // value from uart imu
        int16 acc[3];
        double acc_data[3];  // use publish imu
        int16 gyo[3];
        double gyo_data[3]; // use publish imu
        int16 mag[3];
        float eular[3];
        float quat[4];
        uint8 id;
        
        imu_data_decode_init(); // init imu
        double time_start=GetCurrentTime();  // begin read time from IMU 
        double time_now=GetCurrentTime();  
        while(1)
        {
            time_now=GetCurrentTime();  
            // Read a single byte of data from the serial port.
            try
            {
                imu_serial_port.ReadByte(*ptr, ms_timeout);
                //printf("%x ",ch );
            }
            catch (ReadTimeout)
            {
                continue;
            }

            if(*ptr==0xA5 && (*(ptr-1)== 0x5A)) // catch head
            {
                PtrHead=ptr-1;
            }
            if(*ptr==0xD1 && (ptr-PtrHead)==29) // catch tail
            {
                PtrTail=ptr;
            }
            if(PtrHead[1]==0xA5 && (ptr-PtrTail)==16 && ((PtrTail-PtrHead)==29)) // a frame data 
            {
                ROS_INFO("get a frame data ...... ");
                int len= (ptr-PtrHead); // length of valid data
                for(int i=0;i<=len;i++) // need test: i<len
                {
                    uint8 ch=PtrHead[i];
                    Packet_Decode(ch);
                    // std::cout<<" ptr[i]= "<< PtrHead[i] <<std::endl;
                }

                get_raw_acc(acc);
                get_raw_gyo(gyo);
                get_raw_mag(mag);
                get_eular(eular);
                get_quat(quat);
                get_id(&id);

                for(int i=0;i<3;i++)
                {
                    acc_data[i]=acc[i] * 0.001f; // to g (m/s^2)
                    gyo_data[i]=gyo[i]; // jiaodu/s transfrom rad/s
                }


                std::cout<<"acc is  "<< acc_data[0]<<"  "<<acc_data[1]<<"  "<<acc_data[2]<<std::endl;
                std::cout<<"gyo is  "<<gyo_data[0]<<"  "<<gyo_data[1]<<"  "<<gyo_data[2]<<std::endl;

                 std::cout<<"eular is  "<< eular[0]<<"  "<<eular[1]<<"  "<<eular[2]<<std::endl;

                // get yaw from imu  
                yaw=DEGREES_TO_RADIANS(eular[2]); // 需要除于10 按照 厂家给的驱动？？？？ 
                imu_data_publish(acc_data[0],acc_data[1],acc_data[2],gyo_data[0],gyo_data[1],gyo_data[2],quat[0],quat[1],quat[2],quat[3]);
                time_start=GetCurrentTime();
                memset(&buffer_temp[0],0,size_buffer);
                ptr=&buffer_temp[0];
                continue;
            }

            ptr++;
            // to avoid overflow
            if((ptr-buffer_temp) >= (size_buffer-1))
            {
                memset(&buffer_temp[0],0,size_buffer);
                ptr=&buffer_temp[0];
            }

            usleep(10);
            if(time_now-time_start > 2.0)
            {
                ROS_INFO("IMU data error, can not read imu data for two second", name_port_imu.c_str());
            }


        }


    }

    /*
        # This represents an estimate of a position and velocity in free space.  
        # The pose in this message should be specified in the coordinate frame given by header.frame_id.
        # The twist in this message should be specified in the coordinate frame given by the child_frame_id
        Header header
        string child_frame_id
        geometry_msgs/PoseWithCovariance pose
        geometry_msgs/TwistWithCovariance twist
    */
    void odometry_data_process(const ysrobotodom::robotencoder& data_encoder)
    {
        static int i=0;
        static ros::Time previous_timestamp;
        nav_msgs::Odometry odom_msg;

        static double pose[2]={0,0};
        ros::Time timestamp;

            if(i%18==0)
            {
                std::cout<<" the data of left encoder is: "<< data_encoder.leftencoder<<std::endl;
                std::cout<<" the data of right encoder is: "<< data_encoder.rightencoder<<std::endl;

                std::cout<<" begin  L pose[0]= "<<pose[0]<<"     R pose[1]="<<pose[1]<<std::endl;
            }
            

      

        // calculate left right wheel distance in rad
        convert_encoder_to_pose((const int)(data_encoder.leftencoder),(const int)(data_encoder.rightencoder),pose);

      
            if(i%18==0)
            {
                std::cout<<" after  L pose[0]= "<<pose[0]<<"     R pose[1]="<<pose[1]<<std::endl;
            }
          
        // get current time
        convert_utctime_to_rostime(timestamp);

        if(use_imu_compensate)
        {
            // use imu 
            
        }

        // update robot current linear angular
        base_control.update(timestamp,pose[0],pose[1]);
        // get odometry and fill 
        base_control.getOdometryMsg(odom_msg);

        odom_msg.header.seq=data_encoder.seq; // 上传的 数据帧数，需要下位机上传 
        odom_msg.header.frame_id="odom";
        odom_msg.child_frame_id="base_link";
        odom_msg.pose.pose.orientation.w=1.0; 

        if(i%18==0)
        {
            pub_odom.publish(odom_msg);  // “/base_control/odom” 话题 发布 nav_msgs/Odometry 数据
        }
        i++;

        const double delta_T=(timestamp-previous_timestamp).toSec();
        if(delta_T> 10)
        {
            ROS_ERROR("odometry data is abnormal ....... ");
        }

        previous_timestamp=timestamp;

        // send tf transform
        if(publish_tf)
        {
            static tf::TransformBroadcaster br;
            tf::Transform transform;
            transform.setOrigin(tf::Vector3(odom_msg.pose.pose.position.x,odom_msg.pose.pose.position.y,0.0));
            tf::Quaternion q(odom_msg.pose.pose.orientation.x,
                            odom_msg.pose.pose.orientation.y,
                            odom_msg.pose.pose.orientation.z,
                            odom_msg.pose.pose.orientation.w);
            transform.setRotation(q);

            br.sendTransform(tf::StampedTransform(transform,timestamp,"odom","base_link"));
        }



    }

}





int main(int argc, char *argv[])
{
    /* code for main function */
    ros::init(argc, argv, "robotodom");
    ros::NodeHandle nh("base_control");

    pub_odom = nh.advertise<nav_msgs::Odometry>("odom", 1000);
    pub_imu  = nh.advertise<sensor_msgs::Imu>("/imu_raw",50);
    sub_encoder = nh.subscribe("/rencoder", 1000, base_controller::odometry_data_process);

    base_controller::base_control.starting(ros::Time::now());

    nh.param<std::string>("/imu_port_name",name_port_imu,"/dev/ttyUSB0");
    nh.param<int>("/encoder_resolution",encoder_resolution,4096);
    nh.param<double>("/publish_rate",publish_rate,50);

    nh.param<bool>("/publish_tf",publish_tf,true);
    nh.param<bool>("/use_imu_compensate",use_imu_compensate,true);

    ROS_INFO("encoder_resolution =%d ",encoder_resolution);
    ROS_INFO("publish_rate =%f ",publish_rate);
  

    base_controller::base_control.init(nh);

    if(use_imu_compensate)
    {
        base_controller::init_serial(name_port_imu.c_str(),imu_serial_port);
        pthread_t imu_serial_thread;
        int ret_imu= pthread_create(&imu_serial_thread,NULL,base_controller::imu_serial_imu_thread,NULL);
        if(ret_imu)
        {
            std::cout<< "ERROR ,return code from pthread_create() is %d\n"<< ret_imu <<std::endl;
        }
        
        
    }
    
    
    ros::Rate hz_pub(100);


    while (ros::ok())
    {
        /* code for loop body */

        ros::spinOnce();
        
        hz_pub.sleep();


    }
    

    return 0;
}