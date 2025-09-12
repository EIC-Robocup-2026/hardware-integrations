#include <Arduino.h>
#include <ESP32Encoder.h>
#include <Wire.h>
#include "motor.h"
#include "wheel_feedback.h"
#include "motor_controller.h"

// micro-ROS includes
#include <micro_ros_platformio.h>
#include <rcl/rcl.h>
#include <rclc/rclc.h>
#include <rclc/executor.h>
#include <rosidl_runtime_c/string_functions.h>
#include <sensor_msgs/msg/joint_state.h>
#include <std_msgs/msg/float32_multi_array.h>
#include <micro_ros_utilities/string_utilities.h>

// ROS2 handles
rclc_executor_t executor;
rclc_executor_t executor_cmd_vel_sub;
rclc_executor_t executor_joint_state_pub;

rclc_support_t support;
rcl_allocator_t allocator;
rcl_node_t node;
rcl_init_options_t init_options;

// Publishers and subscribers
rcl_publisher_t joint_state_publisher;
rcl_subscription_t velocity_command_subscriber;

// Messages
sensor_msgs__msg__JointState joint_state_msg;
sensor_msgs__msg__JointState velocity_command_msg;

// Message memory
#define JOINT_COUNT 4
double joint_positions[JOINT_COUNT];  // Change to double
double joint_velocities[JOINT_COUNT]; // Change to double
const char* joint_names[JOINT_COUNT] = {  // Change to const char*
    "fl_wheel_joint",
    "fr_wheel_joint",
    "br_wheel_joint",
    "bl_wheel_joint"
};

// ********* Encoder ***********
ESP32Encoder encoder_1;
ESP32Encoder encoder_2;
ESP32Encoder encoder_3;
ESP32Encoder encoder_4;

// Encoder pins
#define ENCODER_1_PIN1 12
#define ENCODER_1_PIN2 26

#define ENCODER_2_PIN1 27
#define ENCODER_2_PIN2 14

#define ENCODER_3_PIN1 17
#define ENCODER_3_PIN2 16

#define ENCODER_4_PIN1 19
#define ENCODER_4_PIN2 18

// Create wheel feedback objects
WheelFeedback wheel_1(encoder_1, 1228.8);
WheelFeedback wheel_2(encoder_2, 1228.8);
WheelFeedback wheel_3(encoder_3, 1228.8);
WheelFeedback wheel_4(encoder_4, 1228.8);

// ********* Smile Drive communication *************
#define MOTOR_SDA 21
#define MOTOR_SCL 22

// Smile Drive M1 M2 Address
#define FIRST_I2C_ADDRESS 0x85
#define SECOND_I2C_ADDRESS 0x86

#define FRONT_MDrive_I2C_ADDRESS 0x50
#define BACK_MDrive_I2C_ADDRESS 0x51
MotorI2C Motor_1(FRONT_MDrive_I2C_ADDRESS, SECOND_I2C_ADDRESS);
MotorI2C Motor_2(BACK_MDrive_I2C_ADDRESS, FIRST_I2C_ADDRESS); 
MotorI2C Motor_3(BACK_MDrive_I2C_ADDRESS, SECOND_I2C_ADDRESS);
MotorI2C Motor_4(FRONT_MDrive_I2C_ADDRESS, FIRST_I2C_ADDRESS);

// Create motor controllers
MotorSpeedController controller_fl(Motor_1, wheel_1);
MotorSpeedController controller_fr(Motor_2, wheel_2);
MotorSpeedController controller_br(Motor_3, wheel_3);
MotorSpeedController controller_bl(Motor_4, wheel_4);

void velocity_command_callback(const void * msgin) {
    const sensor_msgs__msg__JointState * msg = (const sensor_msgs__msg__JointState *)msgin;
    
    // Only check for velocity data presence, ignore timestamp and other fields
    if (msg->velocity.data != NULL && msg->velocity.size >= JOINT_COUNT) {
        // Map velocities regardless of timestamp
        float fl_vel = msg->velocity.data[0];
        float fr_vel = msg->velocity.data[1];
        float br_vel = msg->velocity.data[2];
        float bl_vel = msg->velocity.data[3];

        // Set motor speeds directly
        controller_fl.setTargetSpeed(fl_vel);
        controller_fr.setTargetSpeed(fr_vel);
        controller_br.setTargetSpeed(br_vel);
        controller_bl.setTargetSpeed(bl_vel);
    }
}

// Function to initialize micro-ROS
bool init_microros() {
    allocator = rcl_get_default_allocator();

    // Create init_options and set domain ID
    init_options = rcl_get_zero_initialized_init_options();
    rcl_init_options_init(&init_options, allocator);
    rcl_init_options_set_domain_id(&init_options, 23);
    
    // Initialize support with custom options
    rclc_support_init_with_options(&support, 0, NULL, &init_options, &allocator);
    
    // Create node
    rclc_node_init_default(&node, "drivebase_node", "", &support);

    // Initialize and set frame_id
    rosidl_runtime_c__String__init(&joint_state_msg.header.frame_id);
    rosidl_runtime_c__String__assign(&joint_state_msg.header.frame_id, "base_link");

    // Initialize position array
    joint_state_msg.position.capacity = JOINT_COUNT;
    joint_state_msg.position.size = JOINT_COUNT;
    joint_state_msg.position.data = joint_positions;

    // Initialize velocity array
    joint_state_msg.velocity.capacity = JOINT_COUNT;
    joint_state_msg.velocity.size = JOINT_COUNT;
    joint_state_msg.velocity.data = joint_velocities;

    // Initialize effort array (empty but initialize it)
    joint_state_msg.effort.capacity = 0;
    joint_state_msg.effort.size = 0;
    joint_state_msg.effort.data = NULL;

    // Initialize name array
    joint_state_msg.name.capacity = JOINT_COUNT;
    joint_state_msg.name.size = JOINT_COUNT;
    joint_state_msg.name.data = (rosidl_runtime_c__String*)malloc(JOINT_COUNT * sizeof(rosidl_runtime_c__String));
    
    for (size_t i = 0; i < JOINT_COUNT; i++) {
        rosidl_runtime_c__String__init(&joint_state_msg.name.data[i]);
        rosidl_runtime_c__String__assign(&joint_state_msg.name.data[i], joint_names[i]);
    }

    // Initialize velocity command message with JointState format
    rosidl_runtime_c__String__init(&velocity_command_msg.header.frame_id);
    rosidl_runtime_c__String__assign(&velocity_command_msg.header.frame_id, "");

    // Initialize velocity array for commands
    velocity_command_msg.velocity.capacity = JOINT_COUNT;
    velocity_command_msg.velocity.size = JOINT_COUNT;
    velocity_command_msg.velocity.data = (double*)malloc(JOINT_COUNT * sizeof(double));

    // Initialize other arrays (we won't use these but they need to be initialized)
    velocity_command_msg.position.capacity = 0;
    velocity_command_msg.position.size = 0;
    velocity_command_msg.position.data = NULL;

    velocity_command_msg.effort.capacity = 0;
    velocity_command_msg.effort.size = 0;
    velocity_command_msg.effort.data = NULL;

    velocity_command_msg.name.capacity = JOINT_COUNT;
    velocity_command_msg.name.size = JOINT_COUNT;
    velocity_command_msg.name.data = (rosidl_runtime_c__String*)malloc(JOINT_COUNT * sizeof(rosidl_runtime_c__String));

    for (size_t i = 0; i < JOINT_COUNT; i++) {
        rosidl_runtime_c__String__init(&velocity_command_msg.name.data[i]);
        rosidl_runtime_c__String__assign(&velocity_command_msg.name.data[i], joint_names[i]);
    }

    // Create publisher
    rclc_publisher_init_default(
        &joint_state_publisher,
        &node,
        ROSIDL_GET_MSG_TYPE_SUPPORT(sensor_msgs, msg, JointState),
        "drivebase_joint_states");

    // Update subscriber initialization to use JointState message type
    rclc_subscription_init_default(
        &velocity_command_subscriber,
        &node,
        ROSIDL_GET_MSG_TYPE_SUPPORT(sensor_msgs, msg, JointState),
        "drivebase_joint_cmd");

    // Create executors
    rclc_executor_init(&executor_cmd_vel_sub, &support.context, 1, &allocator);
    rclc_executor_init(&executor_joint_state_pub, &support.context, 1, &allocator);
    rclc_executor_add_subscription(&executor_cmd_vel_sub, &velocity_command_subscriber, 
                                 &velocity_command_msg, &velocity_command_callback,
                                 ON_NEW_DATA);

    return true;
}

void setup() {
    Serial.begin(115200);
    Wire.begin(MOTOR_SDA, MOTOR_SCL);

    ESP32Encoder::useInternalWeakPullResistors = puType::up;

    // Setup encoders using the defined pins
    encoder_1.attachHalfQuad(ENCODER_1_PIN1, ENCODER_1_PIN2);
    encoder_2.attachHalfQuad(ENCODER_2_PIN1, ENCODER_2_PIN2);
    encoder_3.attachHalfQuad(ENCODER_3_PIN1, ENCODER_3_PIN2);
    encoder_4.attachHalfQuad(ENCODER_4_PIN1, ENCODER_4_PIN2);

    // Configure initial PID gains
    controller_fl.setPIDGains(0.05, 70.0, 0.07);
    controller_fl.setFeedForward(12.8311, 16.6545); 
    controller_fr.setPIDGains(0.05, 70.0, 0.07);
    controller_fr.setFeedForward(12.8311, 16.6545); 
    controller_br.setPIDGains(0.05, 70.0, 0.07);
    controller_br.setFeedForward(12.8311, 16.6545); 
    controller_bl.setPIDGains(0.05, 70.0, 0.07);
    controller_bl.setFeedForward(12.8311, 16.6545); 

    // Set output limits
    controller_fl.setOutputLimits(-255, 255);
    controller_fr.setOutputLimits(-255, 255);
    controller_br.setOutputLimits(-255, 255);
    controller_bl.setOutputLimits(-255, 255);

    // Initialize micro-ROS
    set_microros_serial_transports(Serial);
    init_microros();
}

void loop() {
    rclc_executor_spin_some(&executor_cmd_vel_sub, RCL_MS_TO_NS(10));

    // Update all controllers
    controller_fl.update();
    controller_fr.update();
    controller_br.update();
    controller_bl.update();

    // Update timestamp
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    joint_state_msg.header.stamp.sec = ts.tv_sec;
    joint_state_msg.header.stamp.nanosec = ts.tv_nsec;

    // Update joint states with proper casting
    joint_positions[0] = (double)wheel_1.getAngularPosition();
    joint_positions[1] = (double)wheel_2.getAngularPosition();
    joint_positions[2] = (double)wheel_3.getAngularPosition();
    joint_positions[3] = (double)wheel_4.getAngularPosition();

    joint_velocities[0] = (double)wheel_1.getAngularVelocity();
    joint_velocities[1] = (double)wheel_2.getAngularVelocity();
    joint_velocities[2] = (double)wheel_3.getAngularVelocity();
    joint_velocities[3] = (double)wheel_4.getAngularVelocity();

    // Publish joint states
    rcl_publish(&joint_state_publisher, &joint_state_msg, NULL);

    // Handle ROS communications
    rclc_executor_spin_some(&executor_joint_state_pub, RCL_MS_TO_NS(10));
    
    // delay(1);  // 100Hz update rate
}