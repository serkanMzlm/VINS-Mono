#ifndef __PARAMETERS_HPP__
#define __PARAMETERS_HPP__

#include "rclcpp/rclcpp.hpp"
#include "opencv2/highgui/highgui.hpp"

extern int row;
extern int column;
extern double focal_length ;
const int NUM_OF_CAM = 1;


extern std::string IMAGE_TOPIC;
extern std::string IMU_TOPIC;
extern std::string FISHEYE_MASK;
extern std::vector<std::string> CAM_NAMES;
extern int MAX_CNT;
extern int MIN_DIST;
extern int WINDOW_SIZE;
extern int FREQ;
extern double F_THRESHOLD;
extern int SHOW_TRACK;
extern int STEREO_TRACK;
extern int EQUALIZE;
extern int FISHEYE;
extern bool PUB_THIS_FRAME;

void readParameters(rclcpp::Node &n);

#endif