#include "base_controller.h"
namespace base_controller
{
	BaseController::BaseController():
     wheel_separation_(1.0)
    , wheel_radius_(1.0)
    , wheel_reduction_ratio(1.0)
    , wheel_separation_multiplier_(1.0)
    , left_wheel_radius_multiplier_(1.0)
    , right_wheel_radius_multiplier_(1.0)
    , base_frame_id_("base_link")
    , odom_frame_id_("odom")
    , enable_odom_tf_(true)
  {
  }
    BaseController::~BaseController(){}



 /**
     * \brief Initialize controller
     * \param hw            Velocity joint interface for the wheels
     * \param root_nh       Node handle at root namespace
     * \param controller_nh Node handle inside the controller namespace
     */
    bool BaseController::init(ros::NodeHandle& root_nh)
    {
        double publish_rate = 50;
        root_nh.param("/wheel_radius", wheel_radius_, 0.12);
        root_nh.param("/wheel_separation", wheel_separation_, 0.3);
        root_nh.param("/wheel_reduction_radio", wheel_reduction_ratio, 1.0);
        root_nh.param("/publish_rate", publish_rate, 50.0);
        
        ROS_INFO("wheel_radius %f ", wheel_radius_);
        ROS_INFO("wheel_separation %f ", wheel_separation_);
        ROS_INFO("wheel_reduction_radio %f ", wheel_reduction_ratio);
        ROS_INFO("publish_rate %f ", publish_rate);

        //ROS_INFO("Controller state will be published at %d HZ", publish_rate);
        publish_period_ = ros::Duration(1.0 / publish_rate);

       
         // Apply (possibly new) multipliers:
        const double ws  = wheel_separation_multiplier_   * wheel_separation_;
        const double lwr = left_wheel_radius_multiplier_  * wheel_radius_;
        const double rwr = right_wheel_radius_multiplier_ * wheel_radius_;

        odometry_.setWheelParams(ws, lwr, rwr);  
        //odometry_.init();
        odometry_.setVelocityRollingWindowSize(10);

        // Odometry related:
        return true;
    }

    /**
     * \brief Updates controller, i.e. computes the odometry and sets the new velocity commands
     * \param time   Current time
     * \param period Time since the last called to update
     */
    void BaseController::update(const ros::Time& time, double& left_encoder, double& right_encoder)
    {
         // Apply (possibly new) multipliers:
          const double ws  = wheel_separation_multiplier_   * wheel_separation_;
          const double lwr = left_wheel_radius_multiplier_  * wheel_radius_;
          const double rwr = right_wheel_radius_multiplier_ * wheel_radius_;

          odometry_.setWheelParams(ws, lwr, rwr);
        
          // Estimate linear and angular velocity using joint information
          odometry_.update(left_encoder, right_encoder, time);
    }


 	  /* synchronizate time */
    void BaseController::starting(const ros::Time& time)
    {
        // Register starting time used to keep fixed rate
        last_state_publish_time_ = time;
        odometry_.init(time);

        ROS_INFO("enter BaseController starting ......");
      
    }

    /**/
    void BaseController::stopping(const ros::Time& /*time*/)
    {



    }


    void BaseController::getOdometryMsg(nav_msgs::Odometry& msg_)
    {
       const geometry_msgs::Quaternion orientation(tf::createQuaternionMsgFromYaw(odometry_.getHeading())); 
       
        msg_.header.stamp = odometry_.getTimestamp();
        msg_.pose.pose.position.x = odometry_.getX();
        msg_.pose.pose.position.y = odometry_.getY();
        msg_.pose.pose.orientation = orientation;
        msg_.twist.twist.linear.x  = odometry_.getLinear();
        msg_.twist.twist.angular.z = odometry_.getAngular(); 
        for(int i =0; i< 6; i++)
          for(int j =0; j<6; j++)
          {
            msg_.pose.covariance[i * 6 + j] = 0.0f;
            msg_.twist.covariance[i * 6 + j] = 0.0f;
            if(j == i)
            {
              msg_.pose.covariance[i*6 + j] = 0.05f;
              msg_.twist.covariance[i *6 + j] = 0.01f;
            }
          }


    }
    void BaseController::computeMotorCmd(const double& v, const double &w, double &left_motor, double &right_motor)
    {
        left_motor =(v - wheel_separation_* 0.5 * w) * wheel_reduction_ratio * 60/(wheel_radius_ * 2.0 * 3.141592653);
        right_motor = (v + wheel_separation_* 0.5 * w) * wheel_reduction_ratio * 60/(wheel_radius_ * 2.0 * 3.141592653);
    }


    void BaseController::setHeading(const double& heading)
    {
      odometry_.setHeading(heading);

    }
    void BaseController::setDeltaAngular(const double& delta_angluar)
    {
      odometry_.setDeltaAngular(delta_angluar);
    }
    void BaseController::useExtrinsicAngular()
    {
      odometry_.useExtrinsicAngular();
    }


}//end of namespace