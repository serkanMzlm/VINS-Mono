#include "parameters_.h"
#define COLOR_YLW "\x1b[33m"
bool flag = true;

double INIT_DEPTH;
double MIN_PARALLAX;
double ACC_N, ACC_W;
double GYR_N, GYR_W;

std::vector<Eigen::Matrix3d> RIC;
std::vector<Eigen::Vector3d> TIC;

Eigen::Vector3d G{0.0, 0.0, 9.8};

double BIAS_ACC_THRESHOLD;
double BIAS_GYR_THRESHOLD;
double SOLVER_TIME;
int NUM_ITERATIONS;
int ESTIMATE_EXTRINSIC;
int ESTIMATE_TD;
int ROLLING_SHUTTER;
std::string EX_CALIB_RESULT_PATH;
std::string VINS_RESULT_PATH;
std::string IMU_TOPIC;
double ROW, COL;
double TD, TR;


void readParameters(std::string config_file)
{
    cv::FileStorage fsSettings(config_file, cv::FileStorage::READ);
    if(!fsSettings.isOpened())
    {
        RCLCPP_ERROR(rclcpp::get_logger("parameters"), 
                            "ERROR: Wrong path to settings");
    }

    fsSettings["imu_topic"] >> IMU_TOPIC;

    SOLVER_TIME = fsSettings["max_solver_time"];
    NUM_ITERATIONS = fsSettings["max_num_iterations"];
    MIN_PARALLAX = fsSettings["keyframe_parallax"];
    MIN_PARALLAX = MIN_PARALLAX / FOCAL_LENGTH;

    std::string OUTPUT_PATH;
    fsSettings["output_path"] >> OUTPUT_PATH;
    VINS_RESULT_PATH = OUTPUT_PATH + "/vins_result_no_loop.csv";
    // create folder if not exists
    FileSystemHelper::createDirectoryIfNotExists(OUTPUT_PATH.c_str());

    std::ofstream fout(VINS_RESULT_PATH, std::ios::out);
    fout.close();

    ACC_N = fsSettings["acc_n"];
    ACC_W = fsSettings["acc_w"];
    GYR_N = fsSettings["gyr_n"];
    GYR_W = fsSettings["gyr_w"];
    G.z() = fsSettings["g_norm"];
    ROW = fsSettings["image_height"];
    COL = fsSettings["image_width"];

    RCLCPP_INFO_STREAM(rclcpp::get_logger("parameters"), 
        COLOR_GRN 
        << "\n" << LINE
        << "\n- COL: " << COL
        << "\n- ROW: " << ROW
        << "\n- IMU_TOPIC: " << IMU_TOPIC
        << "\n- SOLVER_TIME: " << SOLVER_TIME 
        << "\n- MIN_PARALLAX: " << MIN_PARALLAX
        << "\n- FOCAL_LENGTH: " << FOCAL_LENGTH
        << "\n- NUM_ITERATIONS: " << NUM_ITERATIONS
        << "\n- VINS_RESULT_PATH: " << VINS_RESULT_PATH
        << "\n" << LINE
    );

    ESTIMATE_EXTRINSIC = fsSettings["estimate_extrinsic"];
    if (ESTIMATE_EXTRINSIC == 2)
    {
        RCLCPP_WARN(rclcpp::get_logger("parameters"), "have no prior about extrinsic param, calibrate extrinsic param");
        RIC.push_back(Eigen::Matrix3d::Identity());
        TIC.push_back(Eigen::Vector3d::Zero());
        EX_CALIB_RESULT_PATH = OUTPUT_PATH + "/extrinsic_parameter.csv";

    }
    else 
    {
        if ( ESTIMATE_EXTRINSIC == 1)
        {
            RCLCPP_WARN(rclcpp::get_logger("parameters"), " Optimize extrinsic param around initial guess!");
            EX_CALIB_RESULT_PATH = OUTPUT_PATH + "/extrinsic_parameter.csv";
        }
        if (ESTIMATE_EXTRINSIC == 0)
            RCLCPP_WARN(rclcpp::get_logger("parameters"), " fix extrinsic param ");

        cv::Mat cv_R, cv_T;
        fsSettings["extrinsicRotation"] >> cv_R;
        fsSettings["extrinsicTranslation"] >> cv_T;
        Eigen::Matrix3d eigen_R;
        Eigen::Vector3d eigen_T;
        cv::cv2eigen(cv_R, eigen_R);
        cv::cv2eigen(cv_T, eigen_T);
        Eigen::Quaterniond Q(eigen_R);
        eigen_R = Q.normalized();
        RIC.push_back(eigen_R);
        TIC.push_back(eigen_T);
        RCLCPP_INFO_STREAM(rclcpp::get_logger("parameters"), "\n\nExtrinsic_R : "
                                << std::endl << RIC[0] << "\nExtrinsic_T : "
                                << std::endl << TIC[0].transpose() << std::endl;
        );
    } 

    INIT_DEPTH = 5.0;
    BIAS_ACC_THRESHOLD = 0.1;
    BIAS_GYR_THRESHOLD = 0.1;

    TD = fsSettings["td"];
    ESTIMATE_TD = fsSettings["estimate_td"];
    if (ESTIMATE_TD)
        RCLCPP_INFO_STREAM(rclcpp::get_logger("parameters"), "Unsynchronized sensors, online estimate time offset, initial td: " << TD);
    else
        RCLCPP_INFO_STREAM(rclcpp::get_logger("parameters"), "Synchronized sensors, fix time offset: " << TD);

    ROLLING_SHUTTER = fsSettings["rolling_shutter"];
    if (ROLLING_SHUTTER)
    {
        TR = fsSettings["rolling_shutter_tr"];
        RCLCPP_INFO_STREAM(rclcpp::get_logger("parameters"), "rolling shutter camera, read out time per line: " << TR);
    }
    else
    {
        TR = 0;
    }
    
    fsSettings.release();
}


double toSec(headerMsg header_time){
    return static_cast<double>(header_time.stamp.sec + (header_time.stamp.nanosec /  1.0e9));
}