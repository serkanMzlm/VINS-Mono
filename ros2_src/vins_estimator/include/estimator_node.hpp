#ifndef __ESTIMATOR_NODE_HPP__
#define __ESTIMATOR_NODE_HPP__

#include <map>
#include <mutex>
#include <queue>
#include <thread>
#include <stdio.h>
#include <condition_variable>

#include <opencv2/opencv.hpp>

#include "rclcpp/rclcpp.hpp"
#include "cv_bridge/cv_bridge.h"

#include "estimator.h"
#include "parameters_.h"
#include "utility/visualization.h"

using imuMsg = sensor_msgs::msg::Imu;
using pointCloudMsg = sensor_msgs::msg::PointCloud;
using boolMsg = std_msgs::msg::Bool;

class EstimatorNode: public rclcpp::Node{
public:
    EstimatorNode();
    void predict(const imuMsg::SharedPtr imu_msg);
    void update();
    std::vector<std::pair<std::vector<imuMsg::SharedPtr>, 
        pointCloudMsg::SharedPtr>> getMeasurements();
    void imuCallback(const imuMsg::SharedPtr imu_msg);
    void feature_callback(const pointCloudMsg::SharedPtr feature_msg);
    void restart_callback(const boolMsg::SharedPtr restart_msg);
    void relocalization_callback(const pointCloudMsg::SharedPtr points_msg);
    void process();
    void initTopics();
private:
    std::condition_variable con;
    double current_time = -1;
    queue<imuMsg::SharedPtr> imu_buf;
    queue<pointCloudMsg::SharedPtr> feature_buf;
    queue<pointCloudMsg::SharedPtr> relo_buf;
    int sum_of_wait = 0;
    std::mutex m_buf;
    std::mutex m_state;
    std::mutex i_buf;
    std::mutex m_estimator;
    double latest_time;
    Eigen::Vector3d tmp_P;
    Eigen::Quaterniond tmp_Q;
    Eigen::Vector3d tmp_V;
    Eigen::Vector3d tmp_Ba;
    Eigen::Vector3d tmp_Bg;
    Eigen::Vector3d acc_0;
    Eigen::Vector3d gyr_0;
    bool init_feature = 0;
    bool init_imu = 1;
    double last_imu_t = 0;
    rclcpp::Subscription<imuMsg>::SharedPtr sub_imu ;
    rclcpp::Subscription<pointCloudMsg>::SharedPtr sub_image;
    rclcpp::Subscription<boolMsg>::SharedPtr sub_restart;
    rclcpp::Subscription<pointCloudMsg>::SharedPtr sub_relo_points;
    std::thread measurement_process;
    rclcpp::Node::SharedPtr node; 
};

#endif