#include "ros/ros.h"
#include "std_msgs/Float32.h"

class FlowValueSubscriber {
public:
    FlowValueSubscriber() : cumulative_volume(0.0), flow_value_positive(false) {
       
        ros::NodeHandle nh;

        
        flow_value_subscriber = nh.subscribe("/flow_value", 10, &FlowValueSubscriber::flowValueCallback, this);

        
        cumulative_volume_publisher = nh.advertise<std_msgs::Float32>("/cumulative_volume", 10);
    }

    void flowValueCallback(const std_msgs::Float32::ConstPtr& msg) {
        
        flow_value = msg->data;

        
        if (flow_value > 0) {
            if (!flow_value_positive) {
                
                start_time = ros::Time::now();
                flow_value_positive = true; 
            }
            time_increment = (ros::Time::now() - start_time).toSec();
        } else {
            
            flow_value_positive = false;
            time_increment = 1.0;
        }

        
        cumulative_volume += (flow_value);
        std_msgs::Float32 cumulative_volume_msg;
        cumulative_volume_msg.data = cumulative_volume;
        cumulative_volume_publisher.publish(cumulative_volume_msg);
    }

    void run() {
        ros::Rate rate(1.0 / time_increment);  

        while (ros::ok()) {
            
            ROS_INFO("Current Flow Value: %.2f mL/s | Time Increment: %.2f s | Cumulative Volume: %.2f mL",
                     flow_value, time_increment, cumulative_volume);

            

            rate.sleep();
            ros::spinOnce();
        }
    }

private:
    ros::Subscriber flow_value_subscriber;
    ros::Publisher cumulative_volume_publisher;
    double flow_value;
    double time_increment;
    double cumulative_volume;
    ros::Time start_time;
    bool flow_value_positive;
};

int main(int argc, char** argv) {
    
    ros::init(argc, argv, "flow_value_subscriber");

    FlowValueSubscriber flow_value_subscriber;
    flow_value_subscriber.run();

    return 0;
}
