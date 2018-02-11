//#include <ArduinoHardware.h>
//#include <ArduinoTcpHardware.h>
#include <ros.h>
#include <nav_msgs/Odometry.h>

extern int clock_pin; // output to clock
extern int CSn_pin; // output to chip select
extern int input_pin; // read AS5045
extern int input_stream; // one bit read from pin
extern long packed_data; // two bytes concatenated from inputstream
extern long angle; // holds processed angle value
extern float angle_Float;
extern float true_angle;
extern float calib_angle;
extern float offset_angle; //17.02
extern float resting_angle;
extern float rotary_angle;
extern float starting_angle;
extern long printing_angle;
extern long angle_mask; // 0x111111111111000000: mask to obtain first 12 digits with position info
extern long status_mask; // 0x000000000000111111; mask to obtain last 6 digits containing status info
extern long status_bits; // holds status/error information
extern int DECn; // bit holding decreasing magnet field error data
extern int INCn; // bit holding increasing magnet field error data
extern int OCF; // bit holding startup-valid bit
extern int COF; // bit holding cordic DSP processing error data
extern int LIN; // bit holding magnet field displacement error data
extern int debug; // SET THIS TO 0 TO DISABLE PRINTING OF ERROR CODES

int parse = 0;


ros::NodeHandle nh;
nav_msgs::Odometry odom_msg;
ros::Publisher odomPub("scorpion/odom", &odom_msg);
char frame_id[] = "odom";
char child_frame_id[] = "base_link";
int seqVal = 0;
int ODOM_DELAY_MS = 20;

void setup() {
  Serial.begin(9600);
  setup_rotary_encoder();
  calibrate_rotary_encoder();
}

void loop(){
  delay(200);
//  if(Serial.available()>0)
//  {  
//    parse = Serial.parseInt();
//    if(parse == 0){calibrate_rotary_encoder();}
//    if(parse == 1){rotary_data();}
//  }
  float answer = rotary_data();
  Serial.println(answer);

  readyOdomMsg();
  odomPub.publish(&odom_msg);
  nh.spinOnce();
  delay(ODOM_DELAY_MS);
}

void readyOdomMsg(){
  odom_msg.header.seq               = seqVal;
  odom_msg.header.stamp             = nh.now();
  odom_msg.header.frame_id          = frame_id;

  odom_msg.child_frame_id           = child_frame_id;

  odom_msg.pose.pose.position.x     = 0.0;
  odom_msg.pose.pose.position.y     = 0.0;
  odom_msg.pose.pose.position.z     = 0.0;
  odom_msg.pose.pose.orientation.x  = 0.0;
  odom_msg.pose.pose.orientation.y  = 0.0;
  odom_msg.pose.pose.orientation.z  = 0.0;
  odom_msg.pose.pose.orientation.w  = 0.0;
  odom_msg.pose.covariance[0]       = 0.001;  //0.001,  0.0,    0.0,    0.0,    0.0,    0.0,
  odom_msg.pose.covariance[7]       = 0.001;  //0.0,    0.001,  0.0,    0.0,    0.0,    0.0,
  odom_msg.pose.covariance[14]      = 0.001;  //0.0,    0.0,    0.001,  0.0,    0.0,    0.0,
  odom_msg.pose.covariance[21]      = 0.001;  //0.0,    0.0,    0.0,    0.001,  0.0,    0.0,
  odom_msg.pose.covariance[28]      = 0.001;  //0.0,    0.0,    0.0,    0.0,    0.001,  0.0,
  odom_msg.pose.covariance[35]      = 0.03;   //0.0,    0.0,    0.0,    0.0,    0.0,    0.03

  odom_msg.twist.twist.linear.x     = 0.0;  //change this
  odom_msg.twist.twist.linear.y     = 0.0;
  odom_msg.twist.twist.linear.z     = 0.0;       
  odom_msg.twist.twist.angular.x    = 0.0;   
  odom_msg.twist.twist.angular.y    = 0.0;  
  odom_msg.twist.twist.angular.z    = 0.0;
  odom_msg.twist.covariance[0]      = 0.001;  //0.001,  0.0,    0.0,    0.0,    0.0,    0.0,
  odom_msg.twist.covariance[7]      = 0.001;  //0.0,    0.001,  0.0,    0.0,    0.0,    0.0,
  odom_msg.twist.covariance[14]     = 0.001;  //0.0,    0.0,    0.001,  0.0,    0.0,    0.0,
  odom_msg.twist.covariance[21]     = 0.001;  //0.0,    0.0,    0.0,    0.001,  0.0,    0.0,
  odom_msg.twist.covariance[28]     = 0.001;  //0.0,    0.0,    0.0,    0.0,    0.001,  0.0,
  odom_msg.twist.covariance[35]     = 0.03;   //0.0,    0.0,    0.0,    0.0,    0.0,    0.03
}

