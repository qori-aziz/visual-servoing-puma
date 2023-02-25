// Start by `#include`-ing the Mosquitto MQTT Library and other standard libraries.
// g++ SimpleMqttConsumer.cpp -lpaho-mqttpp3 -lpaho-mqtt3as -o simpleConsumer
#include <mqtt/client.h>  // Mosquitto client.
#include <ostream>  // std::cout.

// With the library header files included, continue by defining a main function.
int main()
{
    // In order to connect the mqtt client to a broker, 
    // Define an Ip address pointing to a broker. In this case, the localhost on port 1883.
    std::string ip = "localhost:1883";
    // Then, define an ID to be used by the client when communicating with the broker.
    std::string id = "consumer";

    // Construct a client using the Ip and Id, specifying usage of MQTT V5.
    mqtt::client client(ip, id, mqtt::create_options(MQTTVERSION_5));
    // Use the connect method of the client to establish a connection to the broker.
    client.connect();
    // In order to receive messages from the broker, specify a topic to subscribe to.
    client.subscribe("errver");
    client.subscribe("errhor");
    // Begin the client's message processing loop, filling a queue with messages.
    client.start_consuming();

    bool running = true;
    std::string errVer = "0";
    std::string errHor = "0";
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
            std::string topicString = messagePointer -> get_topic();
             std::string messageString = messagePointer -> get_payload_str();
            if (topicString == "errver"){
                errVer = messageString;
            }
            if (topicString == "errhor"){
                errHor = messageString;
            }
            // Print payload string to console (debugging).
            
            // Perform processing on the string.
            // This is where message processing can be passed onto different variable
        }

        std::cout << "errver:" + errVer << std::endl;
        std::cout << "errHor:" + errHor << std::endl;
    }

    return 0;
}