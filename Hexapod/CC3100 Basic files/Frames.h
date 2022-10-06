#ifndef FRAMES_H_
#define FRAMES_H_

#define RADIO_ID 0x01, 0x02

//#define STREAM_FRAME_FREQ         333     // clock ticks == 30Hz
//#define MOTION_FRAME_FREQ         1000    // clock ticks == 10Hz
//#define BODY_CONFIG_FRAME_FREQ    5000    // clock ticks == 2Hz
//#define SENSOR_DATA_FRAME_FREQ    10000   // clock ticks == 1Hz

#define STREAM_FRAME_FREQ         820     // clock ticks == 50Hz
#define MOTION_FRAME_FREQ         1000    // clock ticks == 10Hz
#define BODY_CONFIG_FRAME_FREQ    5000    // clock ticks == 2Hz
#define SENSOR_DATA_FRAME_FREQ    10000   // clock ticks == 1Hz
#define LISTEN_TIME               20000
#define FIND_ROBOT_FREQ           1000      // 100ms

#define STREAM_DATA_FRAME_SIZE          1400
#define STREAM_DATA_REQUEST_FRAME_SIZE  14
#define SENSOR_DATA_FRAME_SIZE          14
#define SENSOR_DATA_REQUEST_FRAME_SIZE  14
#define BODY_CONFIG_FRAME_SIZE          14
#define MOTION_FRAME_SIZE               14
#define FIND_ROBOT_FRAME_SIZE           1400
#define FIND_ROBOT_RESPONSE_FRAME_SIZE  14
#define ACK_RESPONSE_FRAME_SIZE         14
#define ACK_REQUEST_FRAME_SIZE          14

#define StDF    1   // Stream Data Frame
#define StDRF   2   // Stream Data Request Frame
#define SDF     3   // Sensor Data Frame
#define SDRF    4   // Sensor Data Request Frame
#define CF      5   // Config Frame
#define MF      6   // Motion Frame
#define FRF     7   // Find Robot Frame
#define FRRF    8   // Find Robot Response
#define AF      9   // ACK Response
#define ARF    10   // ACK Request

extern _u8 Frame_Stream_Data_Response[STREAM_DATA_FRAME_SIZE];
extern _u8 Frame_Stream_Data_Request[STREAM_DATA_FRAME_SIZE];
extern _u8 Frame_Sensor_Data_Response[SENSOR_DATA_FRAME_SIZE];
extern _u8 Frame_Sensor_Data_Request[SENSOR_DATA_REQUEST_FRAME_SIZE];
extern _u8 Frame_Body_Config[BODY_CONFIG_FRAME_SIZE];
extern _u8 Frame_Motion[MOTION_FRAME_SIZE];
extern _u8 Frame_Find_Robot[FIND_ROBOT_FRAME_SIZE];
extern _u8 Frame_Find_Robot_Response[FIND_ROBOT_RESPONSE_FRAME_SIZE];
extern _u8 Frame_ACK_Response[ACK_RESPONSE_FRAME_SIZE];
extern _u8 Frame_ACK_Request[ACK_REQUEST_FRAME_SIZE];

#endif /* VAR_H_ */
