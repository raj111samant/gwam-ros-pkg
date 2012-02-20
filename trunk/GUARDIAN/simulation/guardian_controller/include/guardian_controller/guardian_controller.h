/*
 * guardian_controller
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
 * \brief Relate the 4-wheels joints with each motor, apply the control correction in closed loop for the motor and set the command received by the        
joystick/keyboard teleop node.
 */


#include <pr2_controller_interface/controller.h>
#include <pr2_mechanism_model/joint.h>
#include <geometry_msgs/Twist.h>
#include <tf/transform_broadcaster.h>
#include <nav_msgs/Odometry.h>

#define    GUARDIAN_D_TRACKS_M    1.0       // check !
#define    GUARDIAN_WHEEL_DIAMETER	0.25

namespace guardian_controller_ns
{
/*! \class
\brief This class inherits from Controller and implements the actual controls.
*/
class GuardianControllerClass: public pr2_controller_interface::Controller
{
 private:
  // Joint states
  pr2_mechanism_model::JointState* joint_state_blw_;
  pr2_mechanism_model::JointState* joint_state_flw_;
  pr2_mechanism_model::JointState* joint_state_brw_;
  pr2_mechanism_model::JointState* joint_state_frw_; 
  // Joint Positions
  double init_pos_blw_;
  double init_pos_flw_;
  double init_pos_brw_;
  double init_pos_frw_;
  // Joint Speeds
  double init_vel_blw_;
  double init_vel_flw_;
  double init_vel_brw_;
  double init_vel_frw_;
   
  // Robot Speeds
  double linearSpeedMps_;
  double angularSpeedRads_;

  // Robot Positions
  double robot_pose_px_;
  double robot_pose_py_;
  double robot_pose_pa_;

  // External speed references
  double v_ref_;
  double w_ref_;

  // Left-right reference
  int left_right;

  // Frequency
  double frequency;

  // Speed references for motor control
  double v_left_mps, v_right_mps;

  /*!
   * \brief callback message, used to remember where the base is commanded to go
  */
  geometry_msgs::Twist base_vel_msg_;

  ros::NodeHandle node_;
  
  ros::Subscriber cmd_sub_;

public:
  virtual bool init(pr2_mechanism_model::RobotState *robot,
                   ros::NodeHandle &n);
  virtual void starting();
  virtual void update();
  virtual void stopping();
  
  //set control action (base velocity command)
  void setCommand(const geometry_msgs::Twist &cmd_vel);

  //deal with Twist commands 
  void commandCallback (const geometry_msgs::TwistConstPtr &msg);

private:
  double saturation(double u, double min, double max);

};
} 

