/*
 * guardian_joystick_teleop
 * Copyright (c) 2011, Robotnik Automation, SLL
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of the Robotnik Automation, SLL. nor the names of its
 *       contributors may be used to endorse or promote products derived from
 *       this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 * \author Marc Benetó (mbeneto@robotnik.es)
 * \brief Allows to use a joystick with the guardian_controller, sending the messages received through the joystick input, correctly adapted, to the "guardian_controller/command" topic.
 */


#include <ros/ros.h>
#include <sensor_msgs/Joy.h>
#include <geometry_msgs/Twist.h>

////////////////////////////////////////////////////////////////////////
//                               NOTE:                                //
// This configuration is made for a Logitech Freedom 2.4 Cordless Joy //
//   please feel free to modify to adapt for your own joystick.       //   
// 								      //
//  X: Move left/right actuating over only the wheels of one side     //
//  Y: Move forward/backwards                                         //
//  Z: Rotates left/right over itself                                 //
// B3: Decreasses max. velocity                                       //
// B4: Increment max. velocity                                 	      //
////////////////////////////////////////////////////////////////////////


class GuardianJoy
{
public:
  GuardianJoy();

private:
  void joyCallback(const sensor_msgs::Joy::ConstPtr& joy);
  
  ros::NodeHandle nh_;

  int linear_, angular_;
  double l_scale_, a_scale_;
  ros::Publisher vel_pub_;
  ros::Subscriber joy_sub_;
  std::string cmd_topic_;
  double current_vel;
  bool sim_mode_;
  
};


GuardianJoy::GuardianJoy():
  linear_(1),
  angular_(2)
{

  current_vel = 0.1;

  nh_.param("axis_linear", linear_, linear_);
  nh_.param("axis_angular", angular_, angular_);
  nh_.param("scale_angular", a_scale_, a_scale_);
  nh_.param("scale_linear", l_scale_, l_scale_);
  nh_.param("cmd_topic", cmd_topic_, cmd_topic_);
  nh_.param("sim_mode", sim_mode_, sim_mode_);

  // Publish through the node handle Twist type messages to the guardian_controller/command topic
  vel_pub_ = nh_.advertise<geometry_msgs::Twist>(cmd_topic_, 1);

  // Listen through the node handle Joy type messages from joystick (these are the orders that we will send to guardian_controller/command)
  joy_sub_ = nh_.subscribe<sensor_msgs::Joy>("joy", 10, &GuardianJoy::joyCallback, this);

}

void GuardianJoy::joyCallback(const sensor_msgs::Joy::ConstPtr& joy)
{
  geometry_msgs::Twist vel;

  // Set the current velocity
  if (joy->buttons[2] == 1 && current_vel > 0.1){
	current_vel = current_vel - 0.1;
	ROS_ERROR("Velocity: %f%%", current_vel*100.0);
  }else if (joy->buttons[3] == 1 && current_vel < 0.9){
	current_vel = current_vel + 0.1;
	ROS_ERROR("Velocity: %f%%", current_vel*100.0);
  }

  // If we detect a significant Y movement (move left/right), we will actuate in consequence.

  if (joy->axes[0]>0.95 && joy->axes[1]<0.2 && joy->axes[2]<0.2){  		// turning left
	vel.linear.x=0.0;
	vel.linear.y=l_scale_*joy->axes[0];
	vel.linear.z=0.0;
	vel.angular.x = 0.0;
        vel.angular.y = 0.0;
  	vel.angular.z = 0.0;
	vel_pub_.publish(vel);
  }else if (joy->axes[0]<-0.95 && joy->axes[1]>-0.2 && joy->axes[2]>-0.2 ){ 	// turning right
	vel.linear.x=0.0;
	vel.linear.y=l_scale_*joy->axes[0];
	vel.linear.z=0.0;
	vel.angular.x = 0.0;
        vel.angular.y = 0.0;
  	vel.angular.z = 0.0;
	vel_pub_.publish(vel);
  }else{
	if (sim_mode_){
  	   vel.angular.x = current_vel*(a_scale_*joy->axes[angular_])/2;
  	   vel.angular.y = current_vel*(a_scale_*joy->axes[angular_])/2; // Divided by 2 to adapt the velocities (without this, was too fast)
  	   vel.angular.z = current_vel*(a_scale_*joy->axes[angular_])/2;
  	   vel.linear.x = current_vel*1.5*l_scale_*joy->axes[linear_];
  	   vel.linear.y = current_vel*1.5*l_scale_*joy->axes[linear_]; 	// Multiplied by 1.5 to adapt the velocities (without this was too slow)
  	   vel.linear.z = current_vel*1.5*l_scale_*joy->axes[linear_];
  	   vel_pub_.publish(vel);	
	}else{
	   vel.angular.x = current_vel*2*(a_scale_*joy->axes[angular_])/2;
  	   vel.angular.y = current_vel*2*(a_scale_*joy->axes[angular_])/2;
  	   vel.angular.z = current_vel*2*(a_scale_*joy->axes[angular_])/2;
	   vel.linear.x = current_vel*1.25*l_scale_*joy->axes[linear_];
  	   vel.linear.y = current_vel*1.25*l_scale_*joy->axes[linear_];
  	   vel.linear.z = current_vel*1.25*l_scale_*joy->axes[linear_];
  	   vel_pub_.publish(vel);
	}
  }

}


int main(int argc, char** argv)
{
  ros::init(argc, argv, "guardian_joy");
  GuardianJoy guardian_joy;
  ros::spin();
}

