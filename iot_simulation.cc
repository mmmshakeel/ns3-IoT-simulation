#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"
#include "ns3/pcap-file.h"
#include <thread>
#include <atomic>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

using namespace ns3;

std::atomic<bool> attackTriggered(false);

void ListenForCommands() {
    int server_fd, new_socket;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);

    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
        perror("setsockopt");
        close(server_fd);
        exit(EXIT_FAILURE);
    }
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(6000);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("bind failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }
    if (listen(server_fd, 3) < 0) {
        perror("listen");
        close(server_fd);
        exit(EXIT_FAILURE);
    }
    while (true) {
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen)) < 0) {
            perror("accept");
            close(server_fd);
            exit(EXIT_FAILURE);
        }
        char buffer[1024] = {0};
        read(new_socket, buffer, 1024);
        std::string command(buffer);
        if (command.find("start_ddos") != std::string::npos) {
            attackTriggered.store(true);
            std::cout << "DDoS attack triggered!" << std::endl;
        }
        close(new_socket);
    }
}

void CheckAttackTrigger(Ptr<Node> attackNode, Ipv4Address targetAddr, uint16_t port) {
    if (attackTriggered.load()) {
        std::cout << "Launching DDoS attack..." << std::endl;
        // Install multiple UDP clients to simulate DDoS attack
        for (int i = 0; i < 50; ++i) {
            UdpEchoClientHelper attackClient(targetAddr, port);
            attackClient.SetAttribute("MaxPackets", UintegerValue(1000000));
            attackClient.SetAttribute("Interval", TimeValue(Seconds(0.001))); // More frequent packets
            attackClient.SetAttribute("PacketSize", UintegerValue(1024));

            ApplicationContainer clientApps = attackClient.Install(attackNode);
            clientApps.Start(Seconds(2.0 + i * 0.1)); // Stagger start times slightly
            clientApps.Stop(Seconds(100.0));
        }
    } else {
        Simulator::Schedule(Seconds(1.0), &CheckAttackTrigger, attackNode, targetAddr, port);
    }
}

int main(int argc, char *argv[]) {
    CommandLine cmd;
    cmd.Parse(argc, argv);

    NodeContainer nodes;
    nodes.Create(2);

    PointToPointHelper pointToPoint;
    pointToPoint.SetDeviceAttribute("DataRate", StringValue("5Mbps"));
    pointToPoint.SetChannelAttribute("Delay", StringValue("2ms"));

    NetDeviceContainer devices;
    devices = pointToPoint.Install(nodes);

    InternetStackHelper stack;
    stack.Install(nodes);

    Ipv4AddressHelper address;
    address.SetBase("10.1.1.0", "255.255.255.0");

    Ipv4InterfaceContainer interfaces = address.Assign(devices);

    // Ensure the index is within bounds
    if (interfaces.GetN() > 1) {
        Ipv4Address addr = interfaces.GetAddress(1);
        std::cout << "Node 1 IP Address: " << addr << std::endl;
    } else {
        std::cerr << "Error: Not enough interfaces in the container" << std::endl;
    }

    uint16_t port = 5008;

    // Install a UDP Echo Server on Node 1
    UdpEchoServerHelper echoServer(port);
    ApplicationContainer serverApps = echoServer.Install(nodes.Get(1));
    serverApps.Start(Seconds(1.0));
    serverApps.Stop(Seconds(100.0));

    // Install a UDP Echo Client on Node 0
    UdpEchoClientHelper echoClient(interfaces.GetAddress(1), port);
    echoClient.SetAttribute("MaxPackets", UintegerValue(1000000));
    echoClient.SetAttribute("Interval", TimeValue(Seconds(1.0)));
    echoClient.SetAttribute("PacketSize", UintegerValue(1024));

    ApplicationContainer clientApps = echoClient.Install(nodes.Get(0));
    clientApps.Start(Seconds(2.0));
    clientApps.Stop(Seconds(100.0));

    // Enable pcap tracing
    pointToPoint.EnablePcapAll("/var/traffic_data/capture");

    // Start the command listener in a separate thread
    std::thread commandListener(ListenForCommands);
    commandListener.detach();

    // Check for attack trigger every second
    Simulator::Schedule(Seconds(1.0), &CheckAttackTrigger, nodes.Get(0), interfaces.GetAddress(1), port);

    Simulator::Run();
    Simulator::Destroy();
    return 0;
}
