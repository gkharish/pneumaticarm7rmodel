#include <string>
#include <ros/ros.h>
#include <sensor_msgs/JointState.h>
#include <control_msgs/JointControllerState.h>
#include <tf/transform_broadcaster.h>
#include </usr/users/localuser/catkin_workspace/src/pneumaticarm7rmodel/src/shared_memory.hh>
//#include <shared_memory.hh>

double * shmaddr_;
Semaphore shm_sem_;
void InitSharedMemory()
{
  
  // Attached the shared memory to a memory segment.
  shmaddr_ = CreateSharedMemoryForPneumaticArm(false);

}

void CloseSharedMemory()
{
  shmdt(shmaddr_);

  // Update the file.
  std::ofstream aof;
  aof.open(SHM_LOG_FILENAME,
           std::ofstream::out | std::ofstream::app);
  struct timeval current_time;
  gettimeofday(&current_time,0);
    
  aof << " Stop control at " 
      << current_time.tv_sec << "." 
      << current_time.tv_usec << std::endl;
  aof.close();

}



int main(int argc, char** argv) {
    ros::init(argc, argv, "state_publisher");
    ros::NodeHandle n;
    ros::Publisher joint_pub = n.advertise<sensor_msgs::JointState>("joint_states", 1);
    ros::Publisher reference_pub = n.advertise<sensor_msgs::JointState>("reference_point", 1); 
    //ros::Publisher control_pub = n.advertise<control_msgs::JointControllerState>("control_value", 1);
    tf::TransformBroadcaster broadcaster;
    ros::Rate loop_rate(100);
    InitSharedMemory();
    const double degree = M_PI/180;

    // robot state
    //double tilt = 0, tinc = degree, swivel=0, angle=0, height=0, hinc=0.005;
    double theta0=1, theta2=0, theta3=0, theta4=0, theta5=0, theta6=0, theta1=0;
    // message declarations
    geometry_msgs::TransformStamped odom_trans;
    sensor_msgs::JointState joint_state, reference_point;
    //control_msgs::set_point reference_point;
    //control_msgs::JointControllerState  controller_value;
    odom_trans.header.frame_id = "odom";
    odom_trans.child_frame_id = "axis";
    /*ros::Time tim_prev  = ros::Time::now();
    ros::Time   tim, duration;
    tim.sec =0.0;
    tim.nsec =0.0;
    duration.sec=0.0;
    duration.nsec =0.0;*/
    while (ros::ok()) {
        //update joint_state
        /*tim = ros::Time::now();
        duration.sec = tim.sec - tim_prev.sec;
        duration.nsec = tim.nsec - tim_prev.nsec;
        tim_prev = tim;*/
        joint_state.header.stamp = ros::Time::now();
        //controller_value.header.stamp = ros::Time::now(); 
        reference_point.header.stamp = ros::Time::now();

        joint_state.name.resize(7);
        joint_state.position.resize(7);
        joint_state.effort.resize(16);
        joint_state.velocity.resize(1);
        reference_point.position.resize(7);
        joint_state.name[0] ="SA";
        joint_state.position[0] = theta0;
        joint_state.name[1] ="SF";
        joint_state.position[1] = theta1;
        joint_state.name[2] ="AR";
        joint_state.position[2] = theta2;
        joint_state.name[3] ="EF";
        joint_state.position[3] = theta3;
        joint_state.name[4] ="WA";
        joint_state.position[4] = theta4;
        joint_state.name[5] ="WF";
        joint_state.position[5] = theta5;
        joint_state.name[6] ="PS";
        joint_state.position[6] = theta6;

        
        shm_sem_.Acquire();
        for (unsigned int i =0; i<7; i++) {
            joint_state.position[i] = shmaddr_[16+i]*3.14/180;
            reference_point.position[i] = 0.0; //shmaddr_[24]*3.14/180;
        }
        
       //joint_state.position[4] = shmaddr_[23]; 
        for (unsigned int i =0; i<16; i++)
            joint_state.effort[i] = 1.0*shmaddr_[i];
         //controller.set_point = 1.1; //shmaddr_[24];
        reference_point.position[3] = shmaddr_[23]*3.14/180;
        reference_point.position[0] = shmaddr_[24]*3.14/180;
        shm_sem_.Release();
        //std::cout << "Joint position5 =" << jointposition5 << "\n" << endl;

        // update transform
        // (moving in a circle with radius=2)
       /* odom_trans.header.stamp = ros::Time::now();
        odom_trans.transform.translation.x = cos(angle)*2;
        odom_trans.transform.translation.y = sin(angle)*2;
        odom_trans.transform.translation.z = .7;
        odom_trans.transform.rotation = tf::createQuaternionMsgFromYaw(angle+M_PI/2); */

        //send the joint state and transform
        joint_pub.publish(joint_state);
        reference_pub.publish(reference_point);
        broadcaster.sendTransform(odom_trans);

        // Create new robot state
       /* tilt += tinc;
        if (tilt<-.5 || tilt>0) tinc *= -1;
        height += hinc;
        if (height>.2 || height<0) hinc *= -1;
        swivel += degree;
        angle += degree/4; */

        // This will adjust as needed per iteration
        loop_rate.sleep();
    }


    return 0;
}
