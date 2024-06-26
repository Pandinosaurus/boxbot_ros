// Gazebo simulated-servo class.
//
// Copyright 2022-2023 University of Cincinnati
// All rights reserved. See LICENSE file at:
// https://github.com/MatthewVerbryke/rse_dam
// Additional copyright may be held by others, as reflected in the
// commit history.

#include <string>
#include <math.h>

#include "boxbot_driver/sim_servo.h"

SimServo::SimServo(std::string name_in, std::string side_in, ros::NodeHandle n, ros::NodeHandle nh, std::string robot_in){
    
    // Setup names
    side = side_in;
    robot = robot_in;
    name = name_in;
    full_name = side_in + "_" + name_in;
    std::string param_name = "joints/" + name_in;
    
    // Set gripper flag
    std::string gripper_str = "gripper";
    if (name.find(gripper_str) != std::string::npos){
        is_gripper = true;
    }
    else {
        is_gripper = false;
    }
    
    // Build topic names
    command_topic = robot + "/" + side + "_" + name_in + "_controller/command";
    mimic_topic = robot + "/" + side + "_" + name_in + "_mimic_controller/command";
    
    // Initialize variables
    id = -1;
    position = 0.0;
    desired = 0.0;
    velocity = 0.0;
    
    // Setup publishers and subscribers
    ControlPub = n.advertise<std_msgs::Float64>(command_topic, 1);
    if (is_gripper){
        MimicPub = n.advertise<std_msgs::Float64>(mimic_topic, 1);
    }
}

void SimServo::setCommandOutput(){
    
    // Calculate desired gripper opening, if needed
    double desired_act;
    if (is_gripper){

        desired_act = 0.00707054*desired + 0.0198936; // overly simple linear model
    }
    else {
        desired_act = desired;
    }
    
    // Build command message
    std_msgs::Float64 commandMsg;
    commandMsg.data = desired_act;
    
    // Publish commands
    ControlPub.publish(commandMsg);
    if (is_gripper){
        MimicPub.publish(commandMsg);
    }
}

void SimServo::setPosition(double pose_in){
    
    // Convert opening to notional angle for simulation
    if (is_gripper){
        position = 141.432*pose_in - 2.81359; // overly simple linear model
    }
    else {
        position = pose_in;
    }
}
