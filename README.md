# NS-3 IoT Simulation with DDoS Attack Demonstration

This repository contains a simulation of an IoT network using NS-3, designed to demonstrate a DDoS attack and an Intrusion Detection System (IDS). The simulation allows manual triggering of a DDoS attack via a command line interface.

## Table of Contents

- [Prerequisites](#prerequisites)
- [Installation](#installation)
- [Usage](#usage)
    - [Running the Simulation](#running-the-simulation)
    - [Triggering the DDoS Attack](#triggering-the-ddos-attack)
- [Simulation Overview](#simulation-overview)
- [Code Explanation](#code-explanation)

## Prerequisites

- Docker
- Docker Compose

## Installation

1. Clone the repository:
   ```sh
   git clone https://github.com/mmmshakeel/ns3-IoT-simulation.git
   cd ns3-IoT-simulation

2. Clone the repository:
    ```sh
   docker-compose build

## Usage
### Running the Simulation

1. Start the simulation:
    ```sh
   docker-compose up

2. The simulation initializes an IoT network with two nodes. Node 1 runs a UDP echo server, and Node 0 runs a UDP echo client that sends packets to Node 1.

### Triggering the DDoS Attack
1. To manually trigger the DDoS attack, run the following command in a separate terminal:
    ```sh
   echo "start_ddos" | nc localhost 6000


## Simulation Overview
* Nodes: The simulation includes two nodes connected via a point-to-point link.
* Server: Node 1 runs a UDP echo server on port 5008.
* Client: Node 0 runs a UDP echo client that sends packets to the server.
* DDoS Attack: The attack can be triggered manually via a TCP command to port 6000.

## Code Explanation
* Main Simulation Script (iot_simulation.cc):
    * Sets up the IoT network with two nodes.
    * Configures a point-to-point link between the nodes.
    * Installs the UDP echo server on Node 1.
    * Installs the UDP echo client on Node 0.
    * Sets up a TCP server to listen for the "start_ddos" command.
    * Triggers a simulated DDoS attack by installing multiple UDP clients on Node 0 when the command is received.