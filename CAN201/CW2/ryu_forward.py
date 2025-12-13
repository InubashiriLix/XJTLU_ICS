import sys
import time

from ryu.base import app_manager
from ryu.controller import ofp_event
from ryu.controller.handler import CONFIG_DISPATCHER, MAIN_DISPATCHER, set_ev_cls
from ryu.ofproto import ofproto_v1_3
from ryu.lib.packet import packet
from ryu.lib.packet import ethernet
from ryu.lib.packet import ipv4
from ryu.lib.packet import tcp


class ForwardingController(app_manager.RyuApp):
    """Ryu app skeleton for the forwarding case."""

    OFP_VERSIONS = [ofproto_v1_3.OFP_VERSION]

    def __init__(self, *args, **kwargs):
        super(ForwardingController, self).__init__(*args, **kwargs)
        # - mapping from datapath_id -> { MAC -> port }
        # - or specific ports for Client, Server1, Server2
        self.mac_to_port = {}
        self.client_ip = "10.0.1.5"
        self.server1_ip = "10.0.1.2"
        self.server2_ip = "10.0.1.3"
        self.server_port = 5000

        # Port numbers depend on link-add order in networkTopo.py.
        self.client_port = 1
        self.server1_port = 2
        self.server2_port = 3

        # Print Ryu version (can help debug environment on WSL)
        try:
            import ryu

            print(
                "[ryu_forward] Loaded with Ryu version:",
                getattr(ryu, "__version__", "unknown"),
            )
        except Exception:
            print("[ryu_forward] Unable to detect Ryu version.")

    def add_flow(
        self, datapath, priority, match, actions, idle_timeout=5, buffer_id=None
    ):
        """
        Helper for installing a flow entry on the switch.

        NOTE:
        - Default idle_timeout is set to 5 seconds to comply with coursework.
        - For the table-miss entry, you can explicitly pass idle_timeout=0.
        """
        ofproto = datapath.ofproto
        parser = datapath.ofproto_parser

        inst = [parser.OFPInstructionActions(ofproto.OFPIT_APPLY_ACTIONS, actions)]

        if buffer_id is not None:
            mod = parser.OFPFlowMod(
                datapath=datapath,
                buffer_id=buffer_id,
                priority=priority,
                match=match,
                idle_timeout=idle_timeout,
                instructions=inst,
            )
        else:
            mod = parser.OFPFlowMod(
                datapath=datapath,
                priority=priority,
                match=match,
                idle_timeout=idle_timeout,
                instructions=inst,
            )
        datapath.send_msg(mod)

    @set_ev_cls(ofp_event.EventOFPSwitchFeatures, CONFIG_DISPATCHER)
    def switch_features_handler(self, ev):
        """Install the table-miss flow entry when the switch connects."""
        datapath = ev.msg.datapath
        ofproto = datapath.ofproto
        parser = datapath.ofproto_parser

        # Table-miss flow: send unmatched packets to the controller.
        match = parser.OFPMatch()
        actions = [
            parser.OFPActionOutput(ofproto.OFPP_CONTROLLER, ofproto.OFPCML_NO_BUFFER)
        ]
        # Table-miss uses idle_timeout = 0 (never expires).
        self.add_flow(
            datapath, priority=0, match=match, actions=actions, idle_timeout=0
        )

        print(
            "[ryu_forward] Table-miss flow installed for datapath_id = %s" % datapath.id
        )

    def build_match_for_forward(self, datapath, in_port, eth, ip_pkt, tcp_pkt):
        """
        Build the OpenFlow match for the forwarding case.

        TODO (you):
        - Match only TCP packets from Client -> Server1.
        - Consider fields like:
            * in_port
            * eth_type (0x0800 for IPv4)
            * ipv4_src, ipv4_dst
            * ip_proto (6 for TCP)
            * tcp_dst (server port used by server.py)
        - The current implementation matches "everything", which is NOT enough.
        """
        parser = datapath.ofproto_parser
        match = parser.OFPMatch(
            in_port=in_port,
            eth_type=0x0800,
            ip_proto=6,
            ipv4_src=self.client_ip,
            ipv4_dst=self.server1_ip,
            tcp_dst=self.server_port,
        )
        return match

    def build_actions_for_forward(self, datapath, in_port, eth, ip_pkt, tcp_pkt):
        """
        Build OpenFlow actions for forwarding Client->Server1 traffic to Server1.

        TODO (you):
        - Decide the output port that leads to Server1.
        - Optionally, you may do ARP-learning / MAC-learning to figure out which
          port leads to which MAC.
        - The current implementation simply sends packets back to the input port,
          which is WRONG for the real task.
        """
        parser = datapath.ofproto_parser
        ofproto = datapath.ofproto
        # Direct traffic to Server1's port on the switch.
        actions = [parser.OFPActionOutput(self.server1_port, ofproto.OFPCML_NO_BUFFER)]
        return actions

    @set_ev_cls(ofp_event.EventOFPPacketIn, MAIN_DISPATCHER)
    def packet_in_handler(self, ev):
        """Handle Packet_In messages from the switch."""
        msg = ev.msg
        datapath = msg.datapath
        ofproto = datapath.ofproto
        parser = datapath.ofproto_parser

        in_port = msg.match.get("in_port")

        # Parse the received packet
        pkt = packet.Packet(msg.data)
        eth = pkt.get_protocol(ethernet.ethernet)
        ip_pkt = pkt.get_protocol(ipv4.ipv4)
        tcp_pkt = pkt.get_protocol(tcp.tcp)

        # Basic sanity check
        if eth is None:
            return

        # Ignore LLDP to avoid messing with discovery frames
        if eth.ethertype == 0x88CC:
            return

        # Optionally, you can maintain MAC-learning here
        dpid = datapath.id
        self.mac_to_port.setdefault(dpid, {})
        self.mac_to_port[dpid][eth.src] = in_port

        # For this coursework, we care mainly about TCP packets from Client to Server1.
        # Only when the first TCP SYN triggers Packet_In do we install a flow.
        is_syn = (tcp_pkt is not None) and (tcp_pkt.bits & 0x02 != 0)

        if ip_pkt is not None and tcp_pkt is not None and is_syn:
            # TODO (you):
            #   - Check ip_pkt.src == self.client_ip and ip_pkt.dst == self.server1_ip
            #   - Maybe also check TCP dst port matches server.py's listening port.
            #   - Only then install a flow entry and send Packet_Out.
            if (
                ip_pkt.src == self.client_ip
                and ip_pkt.dst == self.server1_ip
                and tcp_pkt.dst_port == self.server_port
            ):
                match = self.build_match_for_forward(
                    datapath, in_port, eth, ip_pkt, tcp_pkt
                )
                actions = self.build_actions_for_forward(
                    datapath, in_port, eth, ip_pkt, tcp_pkt
                )

                # Also pre-install reverse path (Server1 -> Client) to keep handshake fast.
                rev_match = parser.OFPMatch(
                    in_port=self.server1_port,
                    eth_type=0x0800,
                    ip_proto=6,
                    ipv4_src=self.server1_ip,
                    ipv4_dst=self.client_ip,
                    tcp_src=self.server_port,
                )
                rev_actions = [
                    parser.OFPActionOutput(self.client_port, ofproto.OFPCML_NO_BUFFER)
                ]

                # Install forward flow
                if msg.buffer_id != ofproto.OFP_NO_BUFFER:
                    self.add_flow(
                        datapath,
                        priority=10,
                        match=match,
                        actions=actions,
                        idle_timeout=5,
                        buffer_id=msg.buffer_id,
                    )
                else:
                    self.add_flow(
                        datapath,
                        priority=10,
                        match=match,
                        actions=actions,
                        idle_timeout=5,
                    )

                # Install reverse flow
                self.add_flow(
                    datapath,
                    priority=10,
                    match=rev_match,
                    actions=rev_actions,
                    idle_timeout=5,
                )

                # Send the original packet out using Packet_Out
                out = parser.OFPPacketOut(
                    datapath=datapath,
                    buffer_id=msg.buffer_id,
                    in_port=in_port,
                    actions=actions,
                    data=msg.data if msg.buffer_id == ofproto.OFP_NO_BUFFER else None,
                )
                datapath.send_msg(out)
                print("[ryu_forward] Installed forwarding flows for TCP SYN.")
                return

        # Fallback behaviour for other packets (optional):
        # Here we simply flood unknown traffic like a hub.
        # You may refine this part or remove it if not desired.
        actions = [parser.OFPActionOutput(ofproto.OFPP_FLOOD, ofproto.OFPCML_NO_BUFFER)]
        out = parser.OFPPacketOut(
            datapath=datapath,
            buffer_id=msg.buffer_id,
            in_port=in_port,
            actions=actions,
            data=msg.data if msg.buffer_id == ofproto.OFP_NO_BUFFER else None,
        )
        datapath.send_msg(out)
