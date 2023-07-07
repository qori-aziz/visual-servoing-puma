#define _USE_MATH_DEFINES

#include <mqtt/client.h> // Mosquitto client
#include <ostream>       // std::cout.
#include <stdio.h>
#include <windows.h>
#include "picfunc.h"
#include <math.h>
#include <Eigen/Dense>
#include <iostream>
#include <string>
#include <chrono>
#include <fstream>
#include <iostream>
#include <cstdlib>
#include <signal.h>


using namespace std;
using namespace std::chrono;


const double u1_ref = 83;
const double v1_ref = 25;
const double u2_ref = 83;
const double v2_ref = 193;
const double u3_ref = 257;
const double v3_ref = 25;
const double u4_ref = 257;
const double v4_ref = 193;
const double q1_ref = 0;
const double q2_ref = -M_PI/2;
const double q3_ref = M_PI/2;
const double q4_ref = 0;
const double q5_ref = -M_PI / 2;
const double q6_ref = 0;
const double decoder1 = 50000;
const double decoder2 = 60000;
const double decoder3 = 180000;
const double decoder4 = 30000;
const double decoder5 = 42000;
const double decoder6 = 25000;
const double servoticktime = 0.000512;

void signal_callback_handler(int signum) {
    cout << "Caught signal " << signum << endl;
    // Terminate program
    exit(0);
}

/*
void atexit_handler_1(HINSTANCE hModule, Func_ServoLoadTraj ServoLoadTraj, Func_NmcShutdown NmcShutdown, ofstream AllDataCSV)
{

    AllDataCSV.close();
    ServoLoadTraj(1, // vertical
        LOAD_POS | VEL_MODE | LOAD_VEL | LOAD_ACC | ENABLE_SERVO | START_NOW,
        5000, // pos = 2000
        0,    // vel = 100,000
        1000, // acc = 100
        0     // pwm = 0
    );
    ServoLoadTraj(2, // vertical
        LOAD_POS | VEL_MODE | LOAD_VEL | LOAD_ACC | ENABLE_SERVO | START_NOW,
        5000, // pos = 2000
        0,     // vel = -100,000
        1000,  // acc = 100
        0      // pwm = 0
    );

    ServoLoadTraj(3, // vertical
        LOAD_POS | VEL_MODE | LOAD_VEL | LOAD_ACC | ENABLE_SERVO | START_NOW,
        5000, // pos = 2000
        0,     // vel = -100,000
        1000,  // acc = 100
        0      // pwm = 0
    );
    ServoLoadTraj(4, // vertical
        LOAD_POS | VEL_MODE | LOAD_VEL | LOAD_ACC | ENABLE_SERVO | START_NOW,
        5000, // pos = 2000
        0,    // vel = 100,000
        1000, // acc = 100
        0     // pwm = 0
    );
    ServoLoadTraj(5, // vertical
        LOAD_POS | VEL_MODE | LOAD_VEL | LOAD_ACC | ENABLE_SERVO | START_NOW,
        5000, // pos = 2000
        0,     // vel = -100,000
        1000,  // acc = 100
        0      // pwm = 0
    );

    ServoLoadTraj(6, // vertical
        LOAD_POS | VEL_MODE | LOAD_VEL | LOAD_ACC | ENABLE_SERVO | START_NOW,
        5000, // pos = 2000
        0,     // vel = -100,000
        1000,  // acc = 100
        0      // pwm = 0
    );

    NmcShutdown();
    FreeLibrary(hModule);

    std::cout << "at exit #1\n";
}
*/

// With the library header files included, continue by defining a main function.
int main()
{   
    // Initialize start angle position
    double q1 = 0;
    double q2 = 0;
    double q3 = 0;
    double q4 = 0;
    double q5 = 0;
    double q6 = 0;
    double u1 = 0;
    double v1 = 0;
    double u2 = 0;
    double v2 = 0;
    double u3 = 0;
    double v3 = 0;
    double u4 = 0;
    double v4 = 0;
    double Z = 50;

    HINSTANCE hModule = LoadLibraryA("NMCLIB04v64.dll");
    Func_NmcInit NmcInit = LoadNmcInit(hModule);
    Func_NmcGetModType NmcGetModType = LoadNmcGetModType(hModule);
    Func_NmcNoOp NmcNoOp = LoadNmcNoOp(hModule);
    Func_NmcGetStat NmcGetStat = LoadNmcGetStat(hModule);
    Func_NmcShutdown NmcShutdown = LoadNmcShutdown(hModule);
    Func_NmcReadStatus NmcReadStatus = LoadNmcReadStatus(hModule);
    Func_ServoResetPos ServoResetPos = LoadServoResetPos(hModule);
    Func_ServoLoadTraj ServoLoadTraj = LoadServoLoadTraj(hModule);
    Func_ServoStopMotor ServoStopMotor = LoadServoStopMotor(hModule);
    Func_ServoSetGain ServoSetGain = LoadServoSetGain(hModule);
    Func_ServoGetPos ServoGetPos = LoadServoGetPos(hModule);
    Func_ServoGetVel ServoGetVel = LoadServoGetVel(hModule);


    // Depth, u_lefttop, v_lefttop, u_rightdown, v_rightdown; 
    string mockData = "0,0,0,0,0";

    int numberOfModules = NmcInit("COM1:", 19200);
    //int numberOfModules = NmcInit("COM3:", 19200);
    printf("Number of Modules detected: %d\n", numberOfModules);
    if (numberOfModules == 0)
    {
        // Connect to COM1 for linux
        //  int numberOfModules = NmcInit("COM1:", 19200);
        // printf("Number of Modules detected: %d\n", numberOfModules);
        // if (numberOfModules == 0) 
        {
        printf("failed to initialize. Shutdown.");
        NmcShutdown();
        FreeLibrary(hModule);
        printf("Done.\n");
        exit(0);
        return 0;
        }
    }

    if (NmcGetModType(2) == SERVOMODTYPE)
    {
        printf("OK\n");
    }

    for (int i = 1; i <= 6; i++) //try 6 motor
    {
        ServoStopMotor(i, AMP_ENABLE | MOTOR_OFF);   // enable amp
        ServoStopMotor(i, AMP_ENABLE | STOP_ABRUPT); // stop at current pos.
        ServoResetPos(i);                            // reset the posiiton counter to 0

        ServoSetGain(i,    // axis = 1
                     100,  // Kp = 100
                     1000, // Kd = 1000
                     0,    // Ki = 0
                     0,    // IL = 0
                     255,  // OL = 255
                     0,    // CL = 0
                     4000, // EL = 4000
                     1,    // SR = 1
                     0     // DC = 0
        );
    }

    std::printf("tes1");
    // in order to connect the mqtt client to a broker,
    // define an ip address pointing to a broker. in this case, the localhost on port 1883.
    std::string ip = "localhost:1883";
    // then, define an id to be used by the client when communicating with the broker.
    std::string id = "consumer";
    // construct a client using the ip and id, specifying usage of mqtt v5.
    mqtt::client client(ip, id, mqtt::create_options(MQTTVERSION_5));
    auto connOpts = mqtt::connect_options_builder()
        .mqtt_version(MQTTVERSION_5)
        .automatic_reconnect(seconds(2), seconds(30))
        .clean_session(true)
        .finalize();
     // use the connect method of the client to establish a connection to the broker.
    client.connect();
    // std::printf("tes4");
    // in order to receive messages from the broker, specify a topic to subscribe to.
    client.subscribe("data", 0);
    //client.subscribe("errhor");
    std::printf("tes5");                      
    // begin the client's message processing loop, filling a queue with messages.
    //client.start_consuming();

    bool running = true;
    ofstream AllDataCSV;
    AllDataCSV.open("alldata.csv");
    AllDataCSV << "u1,v1,u2,v2,u3,v3,u4,v4,pos1,pos2,pos3,pos4,pos5,pos6,vel1,vel2,vel3,vel4,vel5,vel6,err1,err2,err3,err4,err5,err6,err7,err8,speed1,speed2,speed3,speed4,speed5,speed6,cmdspeed1,cmdspeed2,cmdspeed3,cmdspeed4,cmdspeed5,cmdspeed6,time";
    AllDataCSV << ",\n";

    signal(SIGINT, signal_callback_handler);
    while (running)
    {
        auto start = high_resolution_clock::now();
        //// Construct a message pointer to hold an incoming message.
        mqtt::const_message_ptr messagePointer;
        ServoLoadTraj(1, // vertical
            LOAD_POS | VEL_MODE | LOAD_VEL | LOAD_ACC | ENABLE_SERVO | START_NOW,
            5000, // pos = 2000
            0,    // vel = 100,000
            1000, // acc = 100
            0     // pwm = 0
        );
        ServoLoadTraj(2, // vertical
            LOAD_POS | VEL_MODE | LOAD_VEL | LOAD_ACC | ENABLE_SERVO | START_NOW,
            5000, // pos = 2000
            0,     // vel = -100,000
            1000,  // acc = 100
            0      // pwm = 0
        );

        ServoLoadTraj(3, // vertical
            LOAD_POS | VEL_MODE | LOAD_VEL | LOAD_ACC | ENABLE_SERVO | START_NOW,
            5000, // pos = 2000
            0,     // vel = -100,000
            1000,  // acc = 100
            0      // pwm = 0
        );
        ServoLoadTraj(4, // vertical
            LOAD_POS | VEL_MODE | LOAD_VEL | LOAD_ACC | ENABLE_SERVO | START_NOW,
            5000, // pos = 2000
            0,    // vel = 100,000
            1000, // acc = 100
            0     // pwm = 0
        );
        ServoLoadTraj(5, // vertical
            LOAD_POS | VEL_MODE | LOAD_VEL | LOAD_ACC | ENABLE_SERVO | START_NOW,
            5000, // pos = 2000
            0,     // vel = -100,000
            1000,  // acc = 100
            0      // pwm = 0
        );

        ServoLoadTraj(6, // vertical
            LOAD_POS | VEL_MODE | LOAD_VEL | LOAD_ACC | ENABLE_SERVO | START_NOW,
            5000, // pos = 2000
            0,     // vel = -100,000
            1000,  // acc = 100
            0      // pwm = 0
        );
        auto msg = client.consume_message();
        if (msg) {
            if (msg->get_topic() == "data") {
                mockData = msg->to_string();
            }
        }
        else {
            ServoLoadTraj(1, // vertical
                LOAD_POS | VEL_MODE | LOAD_VEL | LOAD_ACC | ENABLE_SERVO | START_NOW,
                5000, // pos = 2000
                0,    // vel = 100,000
                1000, // acc = 100
                0     // pwm = 0
            );
            ServoLoadTraj(2, // vertical
                LOAD_POS | VEL_MODE | LOAD_VEL | LOAD_ACC | ENABLE_SERVO | START_NOW,
                5000, // pos = 2000
                0,     // vel = -100,000
                1000,  // acc = 100
                0      // pwm = 0
            );

            ServoLoadTraj(3, // vertical
                LOAD_POS | VEL_MODE | LOAD_VEL | LOAD_ACC | ENABLE_SERVO | START_NOW,
                5000, // pos = 2000
                0,     // vel = -100,000
                1000,  // acc = 100
                0      // pwm = 0
            );
            ServoLoadTraj(4, // vertical
                LOAD_POS | VEL_MODE | LOAD_VEL | LOAD_ACC | ENABLE_SERVO | START_NOW,
                5000, // pos = 2000
                0,    // vel = 100,000
                1000, // acc = 100
                0     // pwm = 0
            );
            ServoLoadTraj(5, // vertical
                LOAD_POS | VEL_MODE | LOAD_VEL | LOAD_ACC | ENABLE_SERVO | START_NOW,
                5000, // pos = 2000
                0,     // vel = -100,000
                1000,  // acc = 100
                0      // pwm = 0
            );

            ServoLoadTraj(6, // vertical
                LOAD_POS | VEL_MODE | LOAD_VEL | LOAD_ACC | ENABLE_SERVO | START_NOW,
                5000, // pos = 2000
                0,     // vel = -100,000
                1000,  // acc = 100
                0      // pwm = 0
            );
            //cout << "depth zero" << endl;
            continue;
        }

        //// Try to consume a message, passing messagePointer by reference.
        //// If a message is consumed, the function will return `true`,
        //// allowing control to enter the if-statement body.        
        //if (client.try_consume_message(&messagePointer))
        //{
        //    // construct a string from the message payload.
        //    std::string topicstring = messagePointer->get_topic();
        //    std::string messagestring = messagePointer->get_payload_str();
        //    if (topicstring == "data")
        //    {
        //        //std::cout << messagestring << std::endl;
        //        mockData = messagestring;
        //    }
        //}
  

        vector<string> strings;

        string s;
        istringstream ss{ mockData };

        // Depth, u_lefttop, v_lefttop, u_rightdown, v_rightdown; 
        int i = 0;
        while (getline(ss, s, ',')) {
            double temp = stod(s);
            switch (i) {
            case 0:
                i++;
                Z = temp;
                break;
            case 1:
                i++;
                u1 = temp;
                u2 = temp;
                break;
            case 2:
                i++;
                v1 = temp;
                v3 = temp;
                break;
            case 3:
                i++;
                u3 = temp;
                u4 = temp;
                break;
            case 4:
                i = 0;
                v2 = temp;
                v4 = temp;
                break;
            default:
                i = 0;
                break;
            }
            strings.push_back(s);
        }
    
        if (Z == 0) {
            // Add zero set here
            ServoLoadTraj(1, // vertical
                LOAD_POS | VEL_MODE | LOAD_VEL | LOAD_ACC | ENABLE_SERVO | START_NOW,
                5000, // pos = 2000
                0,    // vel = 100,000
                1000, // acc = 100
                0     // pwm = 0
            );
            ServoLoadTraj(2, // vertical
                LOAD_POS | VEL_MODE | LOAD_VEL | LOAD_ACC | ENABLE_SERVO | START_NOW,
                5000, // pos = 2000
                0,     // vel = -100,000
                1000,  // acc = 100
                0      // pwm = 0
            );

            ServoLoadTraj(3, // vertical
                LOAD_POS | VEL_MODE | LOAD_VEL | LOAD_ACC | ENABLE_SERVO | START_NOW,
                5000, // pos = 2000
                0,     // vel = -100,000
                1000,  // acc = 100
                0      // pwm = 0
            );
            ServoLoadTraj(4, // vertical
                LOAD_POS | VEL_MODE | LOAD_VEL | LOAD_ACC | ENABLE_SERVO | START_NOW,
                5000, // pos = 2000
                0,    // vel = 100,000
                1000, // acc = 100
                0     // pwm = 0
            );
            ServoLoadTraj(5, // vertical
                LOAD_POS | VEL_MODE | LOAD_VEL | LOAD_ACC | ENABLE_SERVO | START_NOW,
                5000, // pos = 2000
                0,     // vel = -100,000
                1000,  // acc = 100
                0      // pwm = 0
            );

            ServoLoadTraj(6, // vertical
                LOAD_POS | VEL_MODE | LOAD_VEL | LOAD_ACC | ENABLE_SERVO | START_NOW,
                5000, // pos = 2000
                0,     // vel = -100,000
                1000,  // acc = 100
                0      // pwm = 0
            );
            //cout << "depth zero" << endl;
            continue;
        }

        // get current position
        unsigned char addrMotor1 = 6; //module address
        unsigned char addrMotor2 = 5; //module address
        unsigned char addrMotor3 = 4; //module address
        unsigned char addrMotor4 = 3; //module address
        unsigned char addrMotor5 = 2; //module address
        unsigned char addrMotor6 = 1; //module address
        long int pos1, pos2, pos3, pos4, pos5, pos6; //motor position in encoder counts
        short int vel1, vel2, vel3, vel4, vel5, vel6; //motor vel. In encoder counts/servo tick
        unsigned char stat_items; //specify which data should be returned

        stat_items = SEND_POS | SEND_VEL; //specify both position and velocity
         // should be read from contorller
        // be careful when addressing. Motor 1 is farthest
        NmcReadStatus(addrMotor1, stat_items); //Read data from controllers
        pos1 = ServoGetPos(addrMotor1); //retrieve the position and velocity data
        vel1 = ServoGetVel(addrMotor1); //from the local internal data structure

        NmcReadStatus(addrMotor2, stat_items); //Read data from controllers
        pos2 = ServoGetPos(addrMotor2); //retrieve the position and velocity data
        vel2 = ServoGetVel(addrMotor2); //from the local internal data structure

        NmcReadStatus(addrMotor3, stat_items); //Read data from controllers
        pos3 = ServoGetPos(addrMotor3); //retrieve the position and velocity data
        vel3 = ServoGetVel(addrMotor3); //from the local internal data structure

        NmcReadStatus(addrMotor4, stat_items); //Read data from controllers
        pos4 = ServoGetPos(addrMotor4); //retrieve the position and velocity data
        vel4 = ServoGetVel(addrMotor4); //from the local internal data structure


        NmcReadStatus(addrMotor5, stat_items); //Read data from controllers
        pos5 = ServoGetPos(addrMotor5); //retrieve the position and velocity data
        vel5 = ServoGetVel(addrMotor5); //from the local internal data structure

        NmcReadStatus(addrMotor6, stat_items); //Read data from controllers
        pos6 = ServoGetPos(addrMotor6); //retrieve the position and velocity data
        vel6 = ServoGetVel(addrMotor6); //from the local internal data structure

     

        // Calulate new q1 to q6
        q1 = (q1_ref + (pos1 / decoder1 * 6.2832) * -1);
        q2 = (q2_ref + (pos2 / decoder2 * 6.2832) * -1);
        q3 = (q3_ref + (pos3 / decoder3 * 6.2832) * -1);
        q4 = (q4_ref + (pos4 / decoder3 * 6.2832));
        q5 = (q5_ref + (pos5 / decoder3 * 6.2832)*-1 );
        q6 = q6_ref + (pos6 / decoder3 * 6.2832);

        //// write to csv
        //ofstream position;
        //position.open("realPos.csv", std::ios::out | std::ios::app);
        //position << pos1;
        //position << ",";
        //position << pos2;
        //position << ",";
        //position << pos3;
        //position << ",";
        //position << pos4;
        //position << ",";
        //position << pos5;
        //position << ",";
        //position << pos6;
        //position << ",\n";

        //position.close();

        //ofstream realSpeed;
        //realSpeed.open("realSpeed.csv", std::ios::out | std::ios::app);
        //realSpeed << vel1;
        //realSpeed << ",";
        //realSpeed << vel2;
        //realSpeed << ",";
        //realSpeed << vel3;
        //realSpeed << ",";
        //realSpeed << vel4;
        //realSpeed << ",";
        //realSpeed << vel5;
        //realSpeed << ",";
        //realSpeed << vel6;
        //realSpeed << ",\n";

        //realSpeed.close();

        // Calculate Jacobian Robot matrix
        Eigen::Matrix<double, 6, 6> jacobiRobot{
            {(79 * cos(q2) * cos(q3) * sin(q1)) / 10 - (1016 * cos(q2) * sin(q1)) / 5 - (127 * sin(q5) * (cos(q1) * sin(q4) - cos(q4) * (sin(q1) * sin(q2) * sin(q3) - cos(q2) * cos(q3) * sin(q1)))) / 2 - (127 * cos(q5) * (cos(q2) * sin(q1) * sin(q3) + cos(q3) * sin(q1) * sin(q2))) / 2 - (79 * sin(q1) * sin(q2) * sin(q3)) / 10 - (627 * cos(q1)) / 5 - (1016 * cos(q2) * sin(q1) * sin(q3)) / 5 - (1016 * cos(q3) * sin(q1) * sin(q2)) / 5, cos(q1) * ((1016 * cos(q2) * cos(q3)) / 5 - (1016 * sin(q2)) / 5 + (79 * cos(q2) * sin(q3)) / 10 + (79 * cos(q3) * sin(q2)) / 10 - (1016 * sin(q2) * sin(q3)) / 5 + (127 * cos(q5) * (cos(q2) * cos(q3) - sin(q2) * sin(q3))) / 2 - (127 * cos(q4) * sin(q5) * (cos(q2) * sin(q3) + cos(q3) * sin(q2))) / 2), cos(q1)* ((1016 * cos(q2) * cos(q3)) / 5 + (79 * cos(q2) * sin(q3)) / 10 + (79 * cos(q3) * sin(q2)) / 10 - (1016 * sin(q2) * sin(q3)) / 5 + (127 * cos(q5) * (cos(q2) * cos(q3) - sin(q2) * sin(q3))) / 2 - (127 * cos(q4) * sin(q5) * (cos(q2) * sin(q3) + cos(q3) * sin(q2))) / 2) ,(cos(q2) * sin(q1) * sin(q3) + cos(q3) * sin(q1) * sin(q2)) * ((1016 * cos(q2) * cos(q3)) / 5 - (1016 * sin(q2) * sin(q3)) / 5 + (127 * cos(q5) * (cos(q2) * cos(q3) - sin(q2) * sin(q3))) / 2 - (127 * cos(q4) * sin(q5) * (cos(q2) * sin(q3) + cos(q3) * sin(q2))) / 2) - (cos(q2) * cos(q3) - sin(q2) * sin(q3)) * ((127 * sin(q5) * (cos(q1) * sin(q4) - cos(q4) * (sin(q1) * sin(q2) * sin(q3) - cos(q2) * cos(q3) * sin(q1)))) / 2 + (127 * cos(q5) * (cos(q2) * sin(q1) * sin(q3) + cos(q3) * sin(q1) * sin(q2))) / 2 + (1016 * cos(q2) * sin(q1) * sin(q3)) / 5 + (1016 * cos(q3) * sin(q1) * sin(q2)) / 5) , (cos(q1) * cos(q4) + sin(q4) * (sin(q1) * sin(q2) * sin(q3) - cos(q2) * cos(q3) * sin(q1))) * ((127 * cos(q5) * (cos(q2) * cos(q3) - sin(q2) * sin(q3))) / 2 - (127 * cos(q4) * sin(q5) * (cos(q2) * sin(q3) + cos(q3) * sin(q2))) / 2) - sin(q4) * ((127 * sin(q5) * (cos(q1) * sin(q4) - cos(q4) * (sin(q1) * sin(q2) * sin(q3) - cos(q2) * cos(q3) * sin(q1)))) / 2 + (127 * cos(q5) * (cos(q2) * sin(q1) * sin(q3) + cos(q3) * sin(q1) * sin(q2))) / 2) * (cos(q2) * sin(q3) + cos(q3) * sin(q2)) ,(sin(q5) * (cos(q1) * sin(q4) - cos(q4) * (sin(q1) * sin(q2) * sin(q3) - cos(q2) * cos(q3) * sin(q1))) + cos(q5) * (cos(q2) * sin(q1) * sin(q3) + cos(q3) * sin(q1) * sin(q2))) * ((127 * cos(q5) * (cos(q2) * cos(q3) - sin(q2) * sin(q3))) / 2 - (127 * cos(q4) * sin(q5) * (cos(q2) * sin(q3) + cos(q3) * sin(q2))) / 2) - ((127 * sin(q5) * (cos(q1) * sin(q4) - cos(q4) * (sin(q1) * sin(q2) * sin(q3) - cos(q2) * cos(q3) * sin(q1)))) / 2 + (127 * cos(q5) * (cos(q2) * sin(q1) * sin(q3) + cos(q3) * sin(q1) * sin(q2))) / 2) * (cos(q5) * (cos(q2) * cos(q3) - sin(q2) * sin(q3)) - cos(q4) * sin(q5) * (cos(q2) * sin(q3) + cos(q3) * sin(q2))) },
            {(1016 * cos(q1) * cos(q2)) / 5 - (627 * sin(q1)) / 5 - (127 * sin(q5) * (sin(q1) * sin(q4) - cos(q4) * (cos(q1) * cos(q2) * cos(q3) - cos(q1) * sin(q2) * sin(q3)))) / 2 + (127 * cos(q5) * (cos(q1) * cos(q2) * sin(q3) + cos(q1) * cos(q3) * sin(q2))) / 2 - (79 * cos(q1) * cos(q2) * cos(q3)) / 10 + (1016 * cos(q1) * cos(q2) * sin(q3)) / 5 + (1016 * cos(q1) * cos(q3) * sin(q2)) / 5 + (79 * cos(q1) * sin(q2) * sin(q3)) / 10, sin(q1) * ((1016 * cos(q2) * cos(q3)) / 5 - (1016 * sin(q2)) / 5 + (79 * cos(q2) * sin(q3)) / 10 + (79 * cos(q3) * sin(q2)) / 10 - (1016 * sin(q2) * sin(q3)) / 5 + (127 * cos(q5) * (cos(q2) * cos(q3) - sin(q2) * sin(q3))) / 2 - (127 * cos(q4) * sin(q5) * (cos(q2) * sin(q3) + cos(q3) * sin(q2))) / 2), sin(q1) * ((1016 * cos(q2) * cos(q3)) / 5 + (79 * cos(q2) * sin(q3)) / 10 + (79 * cos(q3) * sin(q2)) / 10 - (1016 * sin(q2) * sin(q3)) / 5 + (127 * cos(q5) * (cos(q2) * cos(q3) - sin(q2) * sin(q3))) / 2 - (127 * cos(q4) * sin(q5) * (cos(q2) * sin(q3) + cos(q3) * sin(q2))) / 2), (cos(q2) * cos(q3) - sin(q2) * sin(q3)) * ((127 * cos(q5) * (cos(q1) * cos(q2) * sin(q3) + cos(q1) * cos(q3) * sin(q2))) / 2 - (127 * sin(q5) * (sin(q1) * sin(q4) - cos(q4) * (cos(q1) * cos(q2) * cos(q3) - cos(q1) * sin(q2) * sin(q3)))) / 2 + (1016 * cos(q1) * cos(q2) * sin(q3)) / 5 + (1016 * cos(q1) * cos(q3) * sin(q2)) / 5) - (cos(q1) * cos(q2) * sin(q3) + cos(q1) * cos(q3) * sin(q2)) * ((1016 * cos(q2) * cos(q3)) / 5 - (1016 * sin(q2) * sin(q3)) / 5 + (127 * cos(q5) * (cos(q2) * cos(q3) - sin(q2) * sin(q3))) / 2 - (127 * cos(q4) * sin(q5) * (cos(q2) * sin(q3) + cos(q3) * sin(q2))) / 2), ((127 * cos(q5) * (cos(q2) * cos(q3) - sin(q2) * sin(q3))) / 2 - (127 * cos(q4) * sin(q5) * (cos(q2) * sin(q3) + cos(q3) * sin(q2))) / 2) * (cos(q4) * sin(q1) + sin(q4) * (cos(q1) * cos(q2) * cos(q3) - cos(q1) * sin(q2) * sin(q3))) - sin(q4) * ((127 * sin(q5) * (sin(q1) * sin(q4) - cos(q4) * (cos(q1) * cos(q2) * cos(q3) - cos(q1) * sin(q2) * sin(q3)))) / 2 - (127 * cos(q5) * (cos(q1) * cos(q2) * sin(q3) + cos(q1) * cos(q3) * sin(q2))) / 2) * (cos(q2) * sin(q3) + cos(q3) * sin(q2)) ,(sin(q5) * (sin(q1) * sin(q4) - cos(q4) * (cos(q1) * cos(q2) * cos(q3) - cos(q1) * sin(q2) * sin(q3))) - cos(q5) * (cos(q1) * cos(q2) * sin(q3) + cos(q1) * cos(q3) * sin(q2))) * ((127 * cos(q5) * (cos(q2) * cos(q3) - sin(q2) * sin(q3))) / 2 - (127 * cos(q4) * sin(q5) * (cos(q2) * sin(q3) + cos(q3) * sin(q2))) / 2) - ((127 * sin(q5) * (sin(q1) * sin(q4) - cos(q4) * (cos(q1) * cos(q2) * cos(q3) - cos(q1) * sin(q2) * sin(q3)))) / 2 - (127 * cos(q5) * (cos(q1) * cos(q2) * sin(q3) + cos(q1) * cos(q3) * sin(q2))) / 2) * (cos(q5) * (cos(q2) * cos(q3) - sin(q2) * sin(q3)) - cos(q4) * sin(q5) * (cos(q2) * sin(q3) + cos(q3) * sin(q2))) },
            {0, -cos(q1) * ((1016 * cos(q1) * cos(q2)) / 5 - (627 * sin(q1)) / 5 - (127 * sin(q5) * (sin(q1) * sin(q4) - cos(q4) * (cos(q1) * cos(q2) * cos(q3) - cos(q1) * sin(q2) * sin(q3)))) / 2 + (127 * cos(q5) * (cos(q1) * cos(q2) * sin(q3) + cos(q1) * cos(q3) * sin(q2))) / 2 - (79 * cos(q1) * cos(q2) * cos(q3)) / 10 + (1016 * cos(q1) * cos(q2) * sin(q3)) / 5 + (1016 * cos(q1) * cos(q3) * sin(q2)) / 5 + (79 * cos(q1) * sin(q2) * sin(q3)) / 10) - sin(q1) * ((627 * cos(q1)) / 5 + (1016 * cos(q2) * sin(q1)) / 5 + (127 * sin(q5) * (cos(q1) * sin(q4) - cos(q4) * (sin(q1) * sin(q2) * sin(q3) - cos(q2) * cos(q3) * sin(q1)))) / 2 + (127 * cos(q5) * (cos(q2) * sin(q1) * sin(q3) + cos(q3) * sin(q1) * sin(q2))) / 2 + (79 * sin(q1) * sin(q2) * sin(q3)) / 10 - (79 * cos(q2) * cos(q3) * sin(q1)) / 10 + (1016 * cos(q2) * sin(q1) * sin(q3)) / 5 + (1016 * cos(q3) * sin(q1) * sin(q2)) / 5),-sin(q1) * ((127 * sin(q5) * (cos(q1) * sin(q4) - cos(q4) * (sin(q1) * sin(q2) * sin(q3) - cos(q2) * cos(q3) * sin(q1)))) / 2 + (127 * cos(q5) * (cos(q2) * sin(q1) * sin(q3) + cos(q3) * sin(q1) * sin(q2))) / 2 + (79 * sin(q1) * sin(q2) * sin(q3)) / 10 - (79 * cos(q2) * cos(q3) * sin(q1)) / 10 + (1016 * cos(q2) * sin(q1) * sin(q3)) / 5 + (1016 * cos(q3) * sin(q1) * sin(q2)) / 5) - cos(q1) * ((127 * cos(q5) * (cos(q1) * cos(q2) * sin(q3) + cos(q1) * cos(q3) * sin(q2))) / 2 - (127 * sin(q5) * (sin(q1) * sin(q4) - cos(q4) * (cos(q1) * cos(q2) * cos(q3) - cos(q1) * sin(q2) * sin(q3)))) / 2 - (79 * cos(q1) * cos(q2) * cos(q3)) / 10 + (1016 * cos(q1) * cos(q2) * sin(q3)) / 5 + (1016 * cos(q1) * cos(q3) * sin(q2)) / 5 + (79 * cos(q1) * sin(q2) * sin(q3)) / 10) , (cos(q1) * cos(q2) * sin(q3) + cos(q1) * cos(q3) * sin(q2)) * ((127 * sin(q5) * (cos(q1) * sin(q4) - cos(q4) * (sin(q1) * sin(q2) * sin(q3) - cos(q2) * cos(q3) * sin(q1)))) / 2 + (127 * cos(q5) * (cos(q2) * sin(q1) * sin(q3) + cos(q3) * sin(q1) * sin(q2))) / 2 + (1016 * cos(q2) * sin(q1) * sin(q3)) / 5 + (1016 * cos(q3) * sin(q1) * sin(q2)) / 5) - (cos(q2) * sin(q1) * sin(q3) + cos(q3) * sin(q1) * sin(q2)) * ((127 * cos(q5) * (cos(q1) * cos(q2) * sin(q3) + cos(q1) * cos(q3) * sin(q2))) / 2 - (127 * sin(q5) * (sin(q1) * sin(q4) - cos(q4) * (cos(q1) * cos(q2) * cos(q3) - cos(q1) * sin(q2) * sin(q3)))) / 2 + (1016 * cos(q1) * cos(q2) * sin(q3)) / 5 + (1016 * cos(q1) * cos(q3) * sin(q2)) / 5), (cos(q1) * cos(q4) + sin(q4) * (sin(q1) * sin(q2) * sin(q3) - cos(q2) * cos(q3) * sin(q1))) * ((127 * sin(q5) * (sin(q1) * sin(q4) - cos(q4) * (cos(q1) * cos(q2) * cos(q3) - cos(q1) * sin(q2) * sin(q3)))) / 2 - (127 * cos(q5) * (cos(q1) * cos(q2) * sin(q3) + cos(q1) * cos(q3) * sin(q2))) / 2) - ((127 * sin(q5) * (cos(q1) * sin(q4) - cos(q4) * (sin(q1) * sin(q2) * sin(q3) - cos(q2) * cos(q3) * sin(q1)))) / 2 + (127 * cos(q5) * (cos(q2) * sin(q1) * sin(q3) + cos(q3) * sin(q1) * sin(q2))) / 2) * (cos(q4) * sin(q1) + sin(q4) * (cos(q1) * cos(q2) * cos(q3) - cos(q1) * sin(q2) * sin(q3))) ,((127 * sin(q5) * (sin(q1) * sin(q4) - cos(q4) * (cos(q1) * cos(q2) * cos(q3) - cos(q1) * sin(q2) * sin(q3)))) / 2 - (127 * cos(q5) * (cos(q1) * cos(q2) * sin(q3) + cos(q1) * cos(q3) * sin(q2))) / 2) * (sin(q5) * (cos(q1) * sin(q4) - cos(q4) * (sin(q1) * sin(q2) * sin(q3) - cos(q2) * cos(q3) * sin(q1))) + cos(q5) * (cos(q2) * sin(q1) * sin(q3) + cos(q3) * sin(q1) * sin(q2))) - (sin(q5) * (sin(q1) * sin(q4) - cos(q4) * (cos(q1) * cos(q2) * cos(q3) - cos(q1) * sin(q2) * sin(q3))) - cos(q5) * (cos(q1) * cos(q2) * sin(q3) + cos(q1) * cos(q3) * sin(q2))) * ((127 * sin(q5) * (cos(q1) * sin(q4) - cos(q4) * (sin(q1) * sin(q2) * sin(q3) - cos(q2) * cos(q3) * sin(q1)))) / 2 + (127 * cos(q5) * (cos(q2) * sin(q1) * sin(q3) + cos(q3) * sin(q1) * sin(q2))) / 2) },
            {0, -sin(q1),-sin(q1), cos(q1) * cos(q2) * sin(q3) + cos(q1) * cos(q3) * sin(q2), -cos(q4) * sin(q1) - sin(q4) * (cos(q1) * cos(q2) * cos(q3) - cos(q1) * sin(q2) * sin(q3)) ,cos(q5) * (cos(q1) * cos(q2) * sin(q3) + cos(q1) * cos(q3) * sin(q2)) - sin(q5) * (sin(q1) * sin(q4) - cos(q4) * (cos(q1) * cos(q2) * cos(q3) - cos(q1) * sin(q2) * sin(q3))) },
            {0, cos(q1), cos(q1), cos(q2) * sin(q1) * sin(q3) + cos(q3) * sin(q1) * sin(q2), cos(q1) * cos(q4) + sin(q4) * (sin(q1) * sin(q2) * sin(q3) - cos(q2) * cos(q3) * sin(q1)) ,sin(q5) * (cos(q1) * sin(q4) - cos(q4) * (sin(q1) * sin(q2) * sin(q3) - cos(q2) * cos(q3) * sin(q1))) + cos(q5) * (cos(q2) * sin(q1) * sin(q3) + cos(q3) * sin(q1) * sin(q2)) },
            {1, 0, 0, cos(q2) * cos(q3) - sin(q2) * sin(q3),  sin(q4) * (cos(q2) * sin(q3) + cos(q3) * sin(q2)), cos(q5) * (cos(q2) * cos(q3) - sin(q2) * sin(q3)) - cos(q4) * sin(q5) * (cos(q2) * sin(q3) + cos(q3) * sin(q2))},
        };

        // Calculate Jacobian Image matrix
        Eigen::Matrix<double, 8, 6> jacobiImage{
            {-947.0 / Z,0, (u1 - 160.0) / Z, 0.556 * (v1 - 100.0) * (0.0019 * u1 - 0.304), -0.00106 * pow((u1 - 160.0),2) - 947.0,     v1 - 100.0},
            {0, -947.0 / Z, (v1 - 100.0) / Z, 0.00106 * pow((v1 - 100.0),2) + 947.0, -0.556 * (v1 - 100.0) * (0.0019 * u1 - 0.304), 160.0 - 1.0 * u1},
            {-947.0 / Z,        0, (u2 - 160.0) / Z, 0.556 * (v2 - 100.0) * (0.0019 * u2 - 0.304), -0.00106 * pow((u2 - 160.0),2) - 947.0,     v2 - 100.0},
            {0, -947.0 / Z, (v2 - 100.0) / Z, 0.00106 * pow((v2 - 100.0),2) + 947.0, -0.556 * (v2 - 100.0) * (0.0019 * u2 - 0.304), 160.0 - 1.0 * u2},
            {-947.0 / Z,        0, (u3 - 160.0) / Z, 0.556 * (v3 - 100.0) * (0.0019 * u3 - 0.304),        -0.00106 * pow((u3 - 160.0),2) - 947.0,     v3 - 100.0},
            { 0, -947.0 / Z, (v3 - 100.0) / Z,         0.00106 * pow((v3 - 100.0),2) + 947.0, -0.556 * (v3 - 100.0) * (0.0019 * u3 - 0.304), 160.0 - 1.0 * u3},
            {-947.0 / Z,        0, (u4 - 160.0) / Z, 0.556 * (v4 - 100.0) * (0.0019 * u4 - 0.304),        -0.00106 * pow((u4 - 160.0),2) - 947.0,     v4 - 100.0},
            {0, -947.0 / Z, (v4 - 100.0) / Z,         0.00106 * pow((v4 - 100.0), 2) + 947.0, -0.556 * (v4 - 100.0) * (0.0019 * u4 - 0.304), 160.0 - 1.0 * u4},
        };

        if (jacobiRobot.determinant() == 0) {
            // Add zero set here
            ServoLoadTraj(1, // vertical
                LOAD_POS | VEL_MODE | LOAD_VEL | LOAD_ACC | ENABLE_SERVO | START_NOW,
                5000, // pos = 2000
                0,    // vel = 100,000
                1000, // acc = 100
                0     // pwm = 0
            );
            ServoLoadTraj(2, // vertical
                LOAD_POS | VEL_MODE | LOAD_VEL | LOAD_ACC | ENABLE_SERVO | START_NOW,
                5000, // pos = 2000
                0,     // vel = -100,000
                1000,  // acc = 100
                0      // pwm = 0
            );

            ServoLoadTraj(3, // vertical
                LOAD_POS | VEL_MODE | LOAD_VEL | LOAD_ACC | ENABLE_SERVO | START_NOW,
                5000, // pos = 2000
                0,     // vel = -100,000
                1000,  // acc = 100
                0      // pwm = 0
            );
            ServoLoadTraj(4, // vertical
                LOAD_POS | VEL_MODE | LOAD_VEL | LOAD_ACC | ENABLE_SERVO | START_NOW,
                5000, // pos = 2000
                0,    // vel = 100,000
                1000, // acc = 100
                0     // pwm = 0
            );
            ServoLoadTraj(5, // vertical
                LOAD_POS | VEL_MODE | LOAD_VEL | LOAD_ACC | ENABLE_SERVO | START_NOW,
                5000, // pos = 2000
                0,     // vel = -100,000
                1000,  // acc = 100
                0      // pwm = 0
            );

            ServoLoadTraj(6, // vertical
                LOAD_POS | VEL_MODE | LOAD_VEL | LOAD_ACC | ENABLE_SERVO | START_NOW,
                5000, // pos = 2000
                0,     // vel = -100,000
                1000,  // acc = 100
                0      // pwm = 0
            );
            cout << "jacobiRobot not invertible" << endl;
            continue;
        }

        // Calculate Jacobian image pseudoinverse
        Eigen::Matrix<double, 6, 8>jacobiImagePInv = (jacobiImage.transpose() * jacobiImage).inverse() * jacobiImage.transpose();

        // Calculate error vec
        Eigen::Matrix<double, 8, 1> errorVect{
            {u1_ref - u1},
            {v1_ref - v1},
            {u2_ref - u2},
            {v2_ref - v2},
            {u3_ref - u3},
            {v3_ref - v3},
            {u4_ref - u4},
            {v4_ref - v4},
        };

        double normErr = sqrt(pow(errorVect(0, 0), 2) + pow(errorVect(1, 0), 2) + pow(errorVect(2, 0), 2) + pow(errorVect(3, 0), 2) + pow(errorVect(4, 0), 2)+ pow(errorVect(5, 0), 2));
        //ofstream errorVisual;
        //errorVisual.open("errorvisual.csv", std::ios::out | std::ios::app);
        //errorVisual << u1;
        //errorVisual << ",";
        //errorVisual << v1;
        //errorVisual << ",";
        //errorVisual << u2;
        //errorVisual << ",";
        //errorVisual << v2;
        //errorVisual << ",";
        //errorVisual << u3;
        //errorVisual << ",";
        //errorVisual << v3;
        //errorVisual << ",";
        //errorVisual << u4;
        //errorVisual << ",";
        //errorVisual << v4;
        //errorVisual << ",";
        //errorVisual << errorVect(0,0);
        //errorVisual << ",";
        //errorVisual << errorVect(1,0);
        //errorVisual << ",";
        //errorVisual << errorVect(2,0);
        //errorVisual << ",";
        //errorVisual << errorVect(3,0);
        //errorVisual << ",";
        //errorVisual << errorVect(4,0);
        //errorVisual << ",";
        //errorVisual << errorVect(5,0);
        //errorVisual << ",";
        //errorVisual << errorVect(6, 0);
        //errorVisual << ",";
        //errorVisual << errorVect(7, 0);
        //errorVisual << ",";
        //errorVisual << normErr;
        //errorVisual << ",\n";

        //errorVisual.close();

        if (normErr <= 50) {
            // Add zero set here
            ServoLoadTraj(1, // vertical
                LOAD_POS | VEL_MODE | LOAD_VEL | LOAD_ACC | ENABLE_SERVO | START_NOW,
                5000, // pos = 2000
                0,    // vel = 100,000
                1000, // acc = 100
                0     // pwm = 0
            );
            ServoLoadTraj(2, // vertical
                LOAD_POS | VEL_MODE | LOAD_VEL | LOAD_ACC | ENABLE_SERVO | START_NOW,
                5000, // pos = 2000
                0,     // vel = -100,000
                1000,  // acc = 100
                0      // pwm = 0
            );

            ServoLoadTraj(3, // vertical
                LOAD_POS | VEL_MODE | LOAD_VEL | LOAD_ACC | ENABLE_SERVO | START_NOW,
                5000, // pos = 2000
                0,     // vel = -100,000
                1000,  // acc = 100
                0      // pwm = 0
            );
            ServoLoadTraj(4, // vertical
                LOAD_POS | VEL_MODE | LOAD_VEL | LOAD_ACC | ENABLE_SERVO | START_NOW,
                5000, // pos = 2000
                0,    // vel = 100,000
                1000, // acc = 100
                0     // pwm = 0
            );
            ServoLoadTraj(5, // vertical
                LOAD_POS | VEL_MODE | LOAD_VEL | LOAD_ACC | ENABLE_SERVO | START_NOW,
                5000, // pos = 2000
                0,     // vel = -100,000
                1000,  // acc = 100
                0      // pwm = 0
            );

            ServoLoadTraj(6, // vertical
                LOAD_POS | VEL_MODE | LOAD_VEL | LOAD_ACC | ENABLE_SERVO | START_NOW,
                5000, // pos = 2000
                0,     // vel = -100,000
                1000,  // acc = 100
                0      // pwm = 0
            );
            cout << "Tracking done" << endl;
            continue;
        }

        // Calculate joint speed
        Eigen::Matrix<double, 6, 6>proportionalGain{
            {1.1,0,0,0,0,0},
            {0,0.5,0,0,0,0},
            {0,0,0.5,0,0,0},
            {0,0,0,0.5,0,0},
            {0,0,0,0,0.5,0},
            {0,0,0,0,0,0.5},
        };
        Eigen::Matrix<double, 6, 1>jointSpeed = proportionalGain * jacobiRobot.inverse()* jacobiImagePInv*errorVect;
        cout << "Calculated joint speed" << endl;
        cout << jointSpeed << endl;

        // Convert from rad/s to decoder/tick time
        int speed1 = jointSpeed(0, 0) * 0.15915 * decoder1 ;
        int speed2 = jointSpeed(1, 0) * 0.15915 * decoder2 ;
        int speed3 = jointSpeed(2, 0) * 0.15915 * decoder3 ;
        int speed4 = jointSpeed(3, 0) * 0.15915 * decoder4 ;
        int speed5 = jointSpeed(4, 0) * 0.15915 * decoder5 ;
        int speed6 = jointSpeed(5, 0) * 0.15915 * decoder6 ;

        //speed1 = speed1 * -1;
        speed2 = speed2 * -1;
        speed3 = speed3 * -1;
        //speed4 = speed4 * -1;
        speed5 = speed5 * -1;
        //speed6 = speed6 * -1;

        //int speed1 = -50000;
        //int speed2 = 0;
        //int speed3 = 0;
        // write to csv
        //ofstream myfile;
        //myfile.open("speed.csv", std::ios::out | std::ios::app);
        //myfile << speed1;
        //myfile << ",";
        //myfile << speed2;
        //myfile << ",";
        //myfile << speed3;
        //myfile << ",";
        //myfile << speed4;
        //myfile << ",";
        //myfile << speed5;
        //myfile << ",";
        //myfile << speed6;
        //myfile << ",\n";
      
        //myfile.close();


  
        // Be careful when addressing. Address 1 is farthest from the PC
        // Seems that the desired speed vs real speed is reversed
            ServoLoadTraj(6, // vertical
                LOAD_POS | VEL_MODE | LOAD_VEL | LOAD_ACC | ENABLE_SERVO | START_NOW,
                0, // pos = 2000
                speed1,    // vel = 100,000
                1000, // acc = 100
                0     // pwm = 0
            );
            ServoLoadTraj(5, // vertical
                LOAD_POS | VEL_MODE | LOAD_VEL | LOAD_ACC | ENABLE_SERVO | START_NOW,
                0, // pos = 2000
                speed2,    // vel = 100,000
                1000, // acc = 100
                0     // pwm = 0
            );
       
            ServoLoadTraj(4, // vertical
                LOAD_POS | VEL_MODE | LOAD_VEL | LOAD_ACC | ENABLE_SERVO | START_NOW,
                0, // pos = 2000
                speed3,    // vel = 100,000
                1000, // acc = 100
                0     // pwm = 0
            );
            ServoLoadTraj(3, // vertical
                LOAD_POS | VEL_MODE | LOAD_VEL | LOAD_ACC | ENABLE_SERVO | START_NOW,
                0, // pos = 2000
                speed4,    // vel = 100,000
                1000, // acc = 100
                0     // pwm = 0
            );
            ServoLoadTraj(2, // vertical
                LOAD_POS | VEL_MODE | LOAD_VEL | LOAD_ACC | ENABLE_SERVO | START_NOW,
                0, // pos = 2000
                speed5,    // vel = 100,000
                1000, // acc = 100
                0     // pwm = 0
            );

            ServoLoadTraj(1, // vertical
                LOAD_POS | VEL_MODE | LOAD_VEL | LOAD_ACC | ENABLE_SERVO | START_NOW,
                0, // pos = 2000
                speed6,    // vel = 100,000
                1000, // acc = 100
                0     // pwm = 0
            );
  

        //printf("errver:%d\n", errVer);
        //printf("errHor:%d\n", errHor);

        // std::cout << "errver:" + errVer << std::endl;
        // std::cout << "errHor:" + errHor << std::endl;
        //ofstream AllDataCSV;
        //AllDataCSV.open("alldata.csv");
        AllDataCSV << u1;
        AllDataCSV << ",";
        AllDataCSV << v1;
        AllDataCSV << ",";
        AllDataCSV << u2;
        AllDataCSV << ",";
        AllDataCSV << v2;
        AllDataCSV << ",";
        AllDataCSV << u3;
        AllDataCSV << ",";
        AllDataCSV << v3;
        AllDataCSV << ",";
        AllDataCSV << u4;
        AllDataCSV << ",";
        AllDataCSV << v4;
        AllDataCSV << ",";
        AllDataCSV << q1;
        AllDataCSV << ",";
        AllDataCSV << q2;
        AllDataCSV << ",";
        AllDataCSV << q3;
        AllDataCSV << ",";
        AllDataCSV << q4;
        AllDataCSV << ",";
        AllDataCSV << q5;
        AllDataCSV << ",";
        AllDataCSV << q6;
        AllDataCSV << ",";
        AllDataCSV << vel1;
        AllDataCSV << ",";
        AllDataCSV << vel2;
        AllDataCSV << ",";
        AllDataCSV << vel3;
        AllDataCSV << ",";
        AllDataCSV << vel4;
        AllDataCSV << ",";
        AllDataCSV << vel5;
        AllDataCSV << ",";
        AllDataCSV << vel6;
        AllDataCSV << ",";
        AllDataCSV << errorVect(0, 0);
        AllDataCSV << ",";
        AllDataCSV << errorVect(1, 0);
        AllDataCSV << ",";
        AllDataCSV << errorVect(2, 0);
        AllDataCSV << ",";
        AllDataCSV << errorVect(3, 0);
        AllDataCSV << ",";
        AllDataCSV << errorVect(4, 0);
        AllDataCSV << ",";
        AllDataCSV << errorVect(5, 0);
        AllDataCSV << ",";
        AllDataCSV << errorVect(6, 0);
        AllDataCSV << ",";
        AllDataCSV << errorVect(7, 0);
        AllDataCSV << ",";
        AllDataCSV << jointSpeed(0, 0);
        AllDataCSV << ",";
        AllDataCSV << jointSpeed(1, 0);
        AllDataCSV << ",";
        AllDataCSV << jointSpeed(2, 0);
        AllDataCSV << ",";
        AllDataCSV << jointSpeed(3, 0);
        AllDataCSV << ",";
        AllDataCSV << jointSpeed(4, 0);
        AllDataCSV << ",";
        AllDataCSV << jointSpeed(5, 0);
        AllDataCSV << ",";
        AllDataCSV << speed1;
        AllDataCSV << ",";
        AllDataCSV << speed2;
        AllDataCSV << ",";
        AllDataCSV << speed3;
        AllDataCSV << ",";
        AllDataCSV << speed4;
        AllDataCSV << ",";
        AllDataCSV << speed5;
        AllDataCSV << ",";
        AllDataCSV << speed6;
        AllDataCSV << ",";
        auto stop = high_resolution_clock::now();
        auto duration = duration_cast<milliseconds>(stop - start);
        std::cout << "duration: " << duration.count() << std::endl;
        AllDataCSV << duration.count();
        AllDataCSV << ",\n";
    }
    std::printf("done");
    return EXIT_SUCCESS;
}