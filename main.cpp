#include <iostream>

#include <mqtt/client.h> // Mosquitto client
#include <ostream>       // std::cout.
#include <stdio.h>
#include <windows.h>
#include "picfunc.h"

// With the library header files included, continue by defining a main function.
int main()
{

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

    for (int i = 1; i <= 2; i++)
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
    // In order to connect the mqtt client to a broker,
    // Define an Ip address pointing to a broker. In this case, the localhost on port 1883.
    std::string ip = "localhost:1883";
    // Then, define an ID to be used by the client when communicating with the broker.
    std::string id = "consumer";
    // Construct a client using the Ip and Id, specifying usage of MQTT V5.
    mqtt::client client(ip, id, mqtt::create_options(MQTTVERSION_5));
    // Use the connect method of the client to establish a connection to the broker.
    client.connect();
    // std::printf("tes4");
    // In order to receive messages from the broker, specify a topic to subscribe to.
    client.subscribe("errver");
    client.subscribe("errhor");
    std::printf("tes5");
    // Begin the client's message processing loop, filling a queue with messages.
    client.start_consuming();

    bool running = true;
    int errVer = 0;
    int errHor = 0;
    while (running)
    {
        // Construct a message pointer to hold an incoming message.
        mqtt::const_message_ptr messagePointer;

        // Try to consume a message, passing messagePointer by reference.
        // If a message is consumed, the function will return `true`,
        // allowing control to enter the if-statement body.
        if (client.try_consume_message(&messagePointer))
        {
            // Construct a string from the message payload.
            std::string topicString = messagePointer->get_topic();
            std::string messageString = messagePointer->get_payload_str();
            if (topicString == "errver")
            {
                // printf("%s", messageString);
                errVer = std::stoi(messageString);
            }
            if (topicString == "errhor")
            {
                // printf("%s", messageString);
                errHor = std::stoi(messageString);
            }
            // Print payload string to console (debugging).

            // Perform processing on the string.
            // This is where message processing can be passed onto different variable
        }
        if (errVer > 5)
        {
            BOOL success = ServoLoadTraj(1, // vertical
                                         LOAD_POS | VEL_MODE | LOAD_VEL | LOAD_ACC | ENABLE_SERVO | START_NOW,
                                         5000, // pos = 2000
                                         -100000,    // vel = 100,000
                                         1000, // acc = 100
                                         0     // pwm = 0
            );
        }
        else if (errVer < -5)
        {
            BOOL success = ServoLoadTraj(1, // vertical
                                         LOAD_POS | VEL_MODE | LOAD_VEL | LOAD_ACC | ENABLE_SERVO | START_NOW,
                                         5000, // pos = 2000
                                         100000,     // vel = -100,000
                                         1000,  // acc = 100
                                         0      // pwm = 0
            );
        }
        else
        {
            BOOL success = ServoLoadTraj(1, // vertical
                                         LOAD_POS | VEL_MODE | LOAD_VEL | LOAD_ACC | ENABLE_SERVO | START_NOW,
                                         5000,    // pos = 2000
                                         0,    // vel = 0
                                         1000, // acc = 100
                                         0     // pwm = 0
            );
        }

        if (errHor > 5)
        {
            BOOL success = ServoLoadTraj(2, // Horizontal
                                         LOAD_POS | VEL_MODE | LOAD_VEL | LOAD_ACC | ENABLE_SERVO | START_NOW,
                                         5000, // pos = 2000
                                         -100000,    // vel = 100,000
                                         1000, // acc = 100
                                         0     // pwm = 0
            );
        }
        else if (errHor < -5)
        {
            BOOL success = ServoLoadTraj(2, // Horizontal
                                         LOAD_POS | VEL_MODE | LOAD_VEL | LOAD_ACC | ENABLE_SERVO | START_NOW,
                                         -5000, // pos = 2000
                                         100000,     // vel = -100,000
                                         1000,  // acc = 100
                                         0      // pwm = 0
            );
        }
        else
        {
            BOOL success = ServoLoadTraj(2, // Horizontal
                                         LOAD_POS | VEL_MODE | LOAD_VEL | LOAD_ACC | ENABLE_SERVO | START_NOW,
                                         0,    // pos = 2000
                                         0,    // vel = 0
                                         1000, // acc = 100
                                         0     // pwm = 0
            );
        }

        printf("errver:%d\n", errVer);
        printf("errHor:%d\n", errHor);

        // std::cout << "errver:" + errVer << std::endl;
        // std::cout << "errHor:" + errHor << std::endl;
    }

    std::printf("done");

    return 0;
}
