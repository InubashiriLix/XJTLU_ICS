#!/usr/bin/env python3

import sys
import time
import os

from mininet.net import Mininet
from mininet.topo import Topo
from mininet.node import RemoteController, OVSSwitch
from mininet.link import TCLink
from mininet.cli import CLI
from mininet.log import setLogLevel


class SimpleSDNTopo(Topo):
    """
    Simple SDN topology
    """

    def build(self):
        """
        Build the simple SDN topology.
        build hosts, switch, and links.
        Mac and IP addresses are statically assigned, and do not modify it.
        """
        h_client = self.addHost(
            "client",
            ip="10.0.1.5/24",
            mac="00:00:00:00:01:05",
        )
        h_server1 = self.addHost(
            "server1",
            ip="10.0.1.2/24",
            mac="00:00:00:00:01:02",
        )
        h_server2 = self.addHost(
            "server2",
            ip="10.0.1.3/24",
            mac="00:00:00:00:01:03",
        )

        s1 = self.addSwitch("s1", protocols="OpenFlow13")

        # NOTE: link order
        #  - s1-eth1 <-> client
        #  - s1-eth2 <-> server1
        #  - s1-eth3 <-> server2
        self.addLink(h_client, s1)
        self.addLink(h_server1, s1)
        self.addLink(h_server2, s1)


def build_network():
    """
    Create the Mininet instance with the SimpleSDNTopo and a remote Ryu controller.

    TODO (you):
    - Adjust controller IP/port to match your ryu-manager instance.
    - Consider whether you want static ARP entries or let the controller handle ARP.
    """
    topo = SimpleSDNTopo()

    controller_ip = os.environ.get("RYU_IP", "127.0.0.1")
    controller_port = int(os.environ.get("RYU_PORT", "6633"))

    net = Mininet(
        topo=topo,
        controller=lambda name: RemoteController(
            name,
            ip=controller_ip,
            port=controller_port,
        ),
        switch=OVSSwitch,
        link=TCLink,
        autoSetMacs=False,  # we want to control MAC addresses manually
        autoStaticArp=False,  # ARP will be handled via controller / normal ARP
    )
    return net


def main():
    """Entry point used by the markers to create and test the topology."""
    setLogLevel("info")

    # NOTE: we intentionally keep the framework minimal here.
    # You probably want to add:
    #   - net.start()
    #   - connectivity tests (e.g., pingAll() or custom ping)
    #   - CLI(net)
    #   - net.stop()
    #
    # so that the topology is fully usable.
    net = build_network()
    net.start()
    print("*** Testing connectivity with pingAll() ...")
    net.pingAll()

    # Drop into CLI only when explicitly requested to avoid blocking automation.
    if "--cli" in sys.argv:
        CLI(net)

    net.stop()
    print("*** Mininet topology stopped cleanly.")


if __name__ == "__main__":
    main()
