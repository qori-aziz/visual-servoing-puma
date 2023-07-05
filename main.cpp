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
const double decoder1 = 50000;
const double decoder2 = 60000;
const double decoder3 = 180000;
const double decoder4 = 30000;
const double decoder5 = 42000;
const double decoder6 = 25000;
const double servoticktime = 0.000512;


// With the library header files included, continue by defining a main function.
int main()
{
    // Initialize start angle position
    double q1 = 0;
    double q2 = 0;
    double q3 = 0;
    double q4 = M_PI;
    double q5 = -M_PI/2;
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

    // Depth, u_lefttop, v_lefttop, u_rightdown, v_rightdown; 
    string mockData = "0,0,0,0,0";

    HINSTANCE hModule = LoadLibraryA("NMCLIB04v64.dll");
    Func_NmcInit NmcInit = LoadNmcInit(hModule);
    Func_NmcGetModType NmcGetModType = LoadNmcGetModType(hModule);
    Func_NmcNoOp NmcNoOp = LoadNmcNoOp(hModule);
    Func_NmcGetStat NmcGetStat = LoadNmcGetStat(hModule);
    Func_NmcShutdown NmcShutdown = LoadNmcShutdown(hModule);
    Func_NmcReadStatus NmcReadStatus =  LoadNmcReadStatus(hModule);
    Func_ServoResetPos ServoResetPos = LoadServoResetPos(hModule);
    Func_ServoLoadTraj ServoLoadTraj = LoadServoLoadTraj(hModule);
    Func_ServoStopMotor ServoStopMotor = LoadServoStopMotor(hModule);
    Func_ServoSetGain ServoSetGain = LoadServoSetGain(hModule);
    Func_ServoGetPos ServoGetPos = LoadServoGetPos(hModule);
    Func_ServoGetVel ServoGetVel = LoadServoGetVel(hModule);

    int numberOfModules = NmcInit("COM1:", 19200);
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

    for (int i = 1; i <= 3; i++)
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
     // use the connect method of the client to establish a connection to the broker.
    client.connect();
    // std::printf("tes4");
    // in order to receive messages from the broker, specify a topic to subscribe to.
    client.subscribe("data");
    //client.subscribe("errhor");
    std::printf("tes5");
    // begin the client's message processing loop, filling a queue with messages.
    client.start_consuming();

    bool running = true;
    int errVer = 0;
    int errHor = 0;
    while (running)
    {
        auto start = high_resolution_clock::now();
        //// Construct a message pointer to hold an incoming message.
        mqtt::const_message_ptr messagePointer;

        //// Try to consume a message, passing messagePointer by reference.
        //// If a message is consumed, the function will return `true`,
        //// allowing control to enter the if-statement body.
        if (client.try_consume_message(&messagePointer))
        {
            // construct a string from the message payload.
            std::string topicstring = messagePointer->get_topic();
            std::string messagestring = messagePointer->get_payload_str();
            if (topicstring == "data")
            {
                //std::cout << messagestring << std::endl;
                mockData = messagestring;
            }
        }

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
                u3 = temp;
                break;
            case 2:
                i++;
                v1 = temp;
                v3 = temp;
                break;
            case 3:
                i++;
                u2 = temp;
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
            cout << "depth zero" << endl;
            continue;
        }

        // get current position
        unsigned char addrMotor1 = 3; //module address
        unsigned char addrMotor2 = 2; //module address
        unsigned char addrMotor3 = 1; //module address
        //unsigned char addr4 = 4; //module address
        //unsigned char addr5 = 5; //module address
        //unsigned char addr6 = 6; //module address
        long pos1, pos2, pos3, pos4, pos5, pos6; //motor position in encoder counts
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
     

        // Calulate new q1 to q6
        q1 = q1 + (pos1 / decoder1 * 6.2832);
        q2 = q2 + (pos2 / decoder2 * 6.2832);
        q3 = q3 + (pos3 / decoder3 * 6.2832);

        // write to csv
        ofstream position;
        position.open("realPos.csv", std::ios::out | std::ios::app);
        position << q1;
        position << ",";
        position << q2;
        position << ",";
        position << q3;
        position << ",\n";

        position.close();

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

  /*      cout << "matrix" << endl;
        cout << jacobiImage << endl;*/

        if (jacobiRobot.determinant() == 0) {
            // Add zero set here
            ServoLoadTraj(3, // vertical
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

            ServoLoadTraj(1, // vertical
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


        // Calculate joint speed
        Eigen::Matrix<double, 6, 1>jointSpeed = 0.3 * jacobiRobot.inverse()* jacobiImagePInv*errorVect;
        cout << "Calculated joint speed" << endl;
        cout << jointSpeed << endl;

        // Convert from rad/s to decoder/tick time
        double speed1 = jointSpeed(0, 0) * 0.15915 * decoder1 * servoticktime * 65536;
        double speed2 = jointSpeed(1, 0) * 0.15915 * decoder2 * servoticktime * 65536;
        double speed3 = jointSpeed(2, 0) * 0.15915 * decoder3 * servoticktime * 65536;
        double speed4 = jointSpeed(3, 0) * 0.15915 * decoder4 * servoticktime * 65536;
        double speed5 = jointSpeed(4, 0) * 0.15915 * decoder5 * servoticktime * 65536;
        double speed6 = jointSpeed(5, 0) * 0.15915 * decoder6 * servoticktime * 65536;

        // write to csv
        ofstream myfile;
        myfile.open("speed.csv", std::ios::out | std::ios::app);
        myfile << speed1;
        myfile << ",";
        myfile << speed2;
        myfile << ",";
        myfile << speed3;
        myfile << ",";
        myfile << speed4;
        myfile << ",";
        myfile << speed5;
        myfile << ",";
        myfile << speed6;
        myfile << ",\n";
      
        myfile.close();
  
        // Be careful when addressing. Address 1 is farthest from the PC
		ServoLoadTraj(3, // vertical
			LOAD_POS | VEL_MODE | LOAD_VEL | LOAD_ACC | ENABLE_SERVO | START_NOW,
			5000, // pos = 2000
            speed1,    // vel = 100,000
			1000, // acc = 100
			0     // pwm = 0
		);
		ServoLoadTraj(2, // vertical
			LOAD_POS | VEL_MODE | LOAD_VEL | LOAD_ACC | ENABLE_SERVO | START_NOW,
			5000, // pos = 2000
            speed2,     // vel = -100,000
			1000,  // acc = 100
			0      // pwm = 0
		);

		ServoLoadTraj(1, // vertical
			LOAD_POS | VEL_MODE | LOAD_VEL | LOAD_ACC | ENABLE_SERVO | START_NOW,
			5000, // pos = 2000
            speed3,     // vel = -100,000
			1000,  // acc = 100
			0      // pwm = 0
		);
  

        //printf("errver:%d\n", errVer);
        //printf("errHor:%d\n", errHor);

        // std::cout << "errver:" + errVer << std::endl;
        // std::cout << "errHor:" + errHor << std::endl;
        auto stop = high_resolution_clock::now();
        auto duration = duration_cast<milliseconds>(stop - start);
        std::cout << "duration: " << duration.count()<< std::endl;


    }

    std::printf("done");

    return 0;
}