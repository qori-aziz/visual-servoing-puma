#include <iostream>
#define _USE_MATH_DEFINES

#include <mqtt/client.h> // Mosquitto client
#include <ostream>       // std::cout.
#include <stdio.h>
#include <windows.h>
#include "picfunc.h"
#include <math.h>
#include <Eigen/Dense>

#include <chrono>
using namespace std::chrono;

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
    double u1 = 80;
    double v1 = 116;
    double u2 = 169;
    double v2 = 116;
    double u3 = 80;
    double v3 = 206;
    double u4 = 169;
    double v4 = 206;
    double Z = 50;

    HINSTANCE hModule = LoadLibraryA("NMCLIB04v64.dll");
    Func_NmcInit NmcInit = LoadNmcInit(hModule);
    Func_NmcGetModType NmcGetModType = LoadNmcGetModType(hModule);
    Func_NmcNoOp NmcNoOp = LoadNmcNoOp(hModule);
    Func_NmcGetStat NmcGetStat = LoadNmcGetStat(hModule);
    Func_NmcShutdown NmcShutdown = LoadNmcShutdown(hModule);
    Func_ServoResetPos ServoResetPos = LoadServoResetPos(hModule);
    Func_ServoLoadTraj ServoLoadTraj = LoadServoLoadTraj(hModule);
    Func_ServoStopMotor ServoStopMotor = LoadServoStopMotor(hModule);
    Func_ServoSetGain ServoSetGain = LoadServoSetGain(hModule);

    //int numberOfModules = NmcInit("COM1:", 19200);
    //printf("Number of Modules detected: %d\n", numberOfModules);
    //if (numberOfModules == 0)
    //{
    //    // Connect to COM1 for linux
    //    //  int numberOfModules = NmcInit("COM1:", 19200);
    //    // printf("Number of Modules detected: %d\n", numberOfModules);
    //    // if (numberOfModules == 0) 
    //    {
    //    printf("failed to initialize. Shutdown.");
    //    NmcShutdown();
    //    FreeLibrary(hModule);
    //    printf("Done.\n");
    //    exit(0);
    //    return 0;
    //    }
    //}

    //if (NmcGetModType(2) == SERVOMODTYPE)
    //{
    //    printf("OK\n");
    //}

    //for (int i = 1; i <= 2; i++)
    //{
    //    ServoStopMotor(i, AMP_ENABLE | MOTOR_OFF);   // enable amp
    //    ServoStopMotor(i, AMP_ENABLE | STOP_ABRUPT); // stop at current pos.
    //    ServoResetPos(i);                            // reset the posiiton counter to 0

    //    ServoSetGain(i,    // axis = 1
    //                 100,  // Kp = 100
    //                 1000, // Kd = 1000
    //                 0,    // Ki = 0
    //                 0,    // IL = 0
    //                 255,  // OL = 255
    //                 0,    // CL = 0
    //                 4000, // EL = 4000
    //                 1,    // SR = 1
    //                 0     // DC = 0
    //    );
    //}

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
                printf("%s", messagestring);
            }
            //    errver = std::stoi(messagestring);
            //}
            //if (topicstring == "errhor")
            //{
            //    // printf("%s", messagestring);
            //    errhor = std::stoi(messagestring);
            //}
            // print payload string to console (debugging).

            // perform processing on the string.
            // this is where message processing can be passed onto different variable
        }

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



        //  
        //if (errVer > 20)
        //{
        //    BOOL success = ServoLoadTraj(1, // vertical
        //                                 LOAD_POS | VEL_MODE | LOAD_VEL | LOAD_ACC | ENABLE_SERVO | START_NOW,
        //                                 5000, // pos = 2000
        //                                 -50000,    // vel = 100,000
        //                                 1000, // acc = 100
        //                                 0     // pwm = 0
        //    );
        //}
        //else if (errVer < -20)
        //{
        //    BOOL success = ServoLoadTraj(1, // vertical
        //                                 LOAD_POS | VEL_MODE | LOAD_VEL | LOAD_ACC | ENABLE_SERVO | START_NOW,
        //                                 5000, // pos = 2000
        //                                 50000,     // vel = -100,000
        //                                 1000,  // acc = 100
        //                                 0      // pwm = 0
        //    );
        //}
        //else
        //{
        //    BOOL success = ServoLoadTraj(1, // vertical
        //                                 LOAD_POS | VEL_MODE | LOAD_VEL | LOAD_ACC | ENABLE_SERVO | START_NOW,
        //                                 5000,    // pos = 2000
        //                                 0,    // vel = 0
        //                                 1000, // acc = 100
        //                                 0     // pwm = 0
        //    );
        //}

        //if (errHor > 20)
        //{
        //    BOOL success = ServoLoadTraj(2, // Horizontal
        //                                 LOAD_POS | VEL_MODE | LOAD_VEL | LOAD_ACC | ENABLE_SERVO | START_NOW,
        //                                 5000, // pos = 2000
        //                                 50000,    // vel = 100,000
        //                                 1000, // acc = 100
        //                                 0     // pwm = 0
        //    );
        //}
        //else if (errHor < -20)
        //{
        //    BOOL success = ServoLoadTraj(2, // Horizontal
        //                                 LOAD_POS | VEL_MODE | LOAD_VEL | LOAD_ACC | ENABLE_SERVO | START_NOW,
        //                                 -5000, // pos = 2000
        //                                 -50000,     // vel = -100,000
        //                                 1000,  // acc = 100
        //                                 0      // pwm = 0
        //    );
        //}
        //else
        //{
        //    BOOL success = ServoLoadTraj(2, // Horizontal
        //                                 LOAD_POS | VEL_MODE | LOAD_VEL | LOAD_ACC | ENABLE_SERVO | START_NOW,
        //                                 0,    // pos = 2000
        //                                 0,    // vel = 0
        //                                 1000, // acc = 100
        //                                 0     // pwm = 0
        //    );
        //}

        std::cout << jacobiRobot << std::endl;
        std::cout << jacobiRobot.determinant() << std::endl;
        std::cout << jacobiRobot.inverse()<< std::endl;


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
