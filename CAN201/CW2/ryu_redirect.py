#!/usr/bin/env python3

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


class RedirectController(app_manager.RyuApp):
    """Ryu app skeleton for the redirection case."""

    OFP_VERSIONS = [ofproto_v1_3.OFP_VERSION]

    def __init__(self, *args, **kwargs):
        super(RedirectController, self).__init__(*args, **kwargs)
        self.mac_to_port = {}
        self.client_ip = "10.0.1.5"
        self.server1_ip = "10.0.1.2"
        self.server2_ip = "10.0.1.3"
        self.server_port = 5000

        # Deterministic ports based on networkTopo.py link order.
        self.client_port = 1
        self.server1_port = 2
        self.server2_port = 3

        # MAC addresses must match those assigned in networkTopo.py.
        self.client_mac = "00:00:00:00:01:05"
        self.server1_mac = "00:00:00:00:01:02"
        self.server2_mac = "00:00:00:00:01:03"

        try:
            import ryu

            print(
                "[ryu_redirect] Loaded with Ryu version:",
                getattr(ryu, "__version__", "unknown"),
            )
        except Exception:
            print("[ryu_redirect] Unable to detect Ryu version.")

    def add_flow(
        self, datapath, priority, match, actions, idle_timeout=5, hard_timeout=0, buffer_id=None
    ):
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
                hard_timeout=hard_timeout,
                instructions=inst,
                flags=ofproto.OFPFF_SEND_FLOW_REM,
            )
        else:
            mod = parser.OFPFlowMod(
                datapath=datapath,
                priority=priority,
                match=match,
                idle_timeout=idle_timeout,
                hard_timeout=hard_timeout,
                instructions=inst,
                flags=ofproto.OFPFF_SEND_FLOW_REM,
            )
        datapath.send_msg(mod)

    @set_ev_cls(ofp_event.EventOFPSwitchFeatures, CONFIG_DISPATCHER)
    def switch_features_handler(self, ev):
        datapath = ev.msg.datapath
        ofproto = datapath.ofproto
        parser = datapath.ofproto_parser

        # Table-miss flow
        match = parser.OFPMatch()
        actions = [
            parser.OFPActionOutput(ofproto.OFPP_CONTROLLER, ofproto.OFPCML_NO_BUFFER)
        ]
        self.add_flow(
            datapath, priority=0, match=match, actions=actions, idle_timeout=0
        )

        # Proactive forward path: Client -> Server1 redirected to Server2
        forward_match = parser.OFPMatch(
            in_port=self.client_port,
            eth_type=0x0800,
            ip_proto=6,
            ipv4_src=self.client_ip,
            ipv4_dst=self.server1_ip,
            tcp_dst=self.server_port,
        )
        forward_actions = [
            parser.OFPActionSetField(eth_dst=self.server2_mac),
            parser.OFPActionSetField(ipv4_dst=self.server2_ip),
            parser.OFPActionOutput(self.server2_port),
        ]
        self.add_flow(
            datapath,
            priority=100,
            match=forward_match,
            actions=forward_actions,
            idle_timeout=0,
            hard_timeout=0,
        )

        # Proactive reverse path: Server2 -> Client (rewrite as Server1)
        reverse_match = parser.OFPMatch(
            in_port=self.server2_port,
            eth_type=0x0800,
            ip_proto=6,
            ipv4_src=self.server2_ip,
            ipv4_dst=self.client_ip,
            tcp_src=self.server_port,
        )
        reverse_actions = [
            parser.OFPActionSetField(eth_src=self.server1_mac),
            parser.OFPActionSetField(eth_dst=self.client_mac),
            parser.OFPActionSetField(ipv4_src=self.server1_ip),
            parser.OFPActionOutput(self.client_port),
        ]
        self.add_flow(
            datapath,
            priority=100,
            match=reverse_match,
            actions=reverse_actions,
            idle_timeout=0,
            hard_timeout=0,
        )

        print(
            "[ryu_redirect] Proactive redirect flows installed for datapath_id = %s"
            % datapath.id
        )

    def build_match_for_redirect(self, datapath, in_port, eth, ip_pkt, tcp_pkt):
        """
        Build the OpenFlow match for the redirection case.

        TODO (you):
        - Match TCP packets from Client -> (what client thinks is) Server1.
        - Similar fields as in the forwarding case:
            * in_port, eth_type, ipv4_src/dst, ip_proto, tcp_dst.
        - The current implementation matches everything and is NOT correct.
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

    def build_actions_for_redirect(self, datapath, in_port, eth, ip_pkt, tcp_pkt):
        """
        Build actions that redirect Client->Server1 traffic to Server2.

        High-level idea (you must implement it yourself):
        - Rewrite destination IP and MAC from Server1 to Server2 on the way forward.
        - Rewrite source IP and MAC from Server2 back to Server1 on the way back, so the
          client believes it is talking to Server1.
        - Output to the port that leads to Server2.

        TODO (you):
        - Decide the correct output port for Server2.
        - Add appropriate OFPActionSetField() actions to rewrite IP/MAC as needed.
        - The current implementation simply sends packets back to the input port and
          does NOT satisfy the coursework requirements.
        """
        parser = datapath.ofproto_parser
        ofproto = datapath.ofproto
        # Forward path: rewrite destination to Server2, then output to Server2's port.
        actions = [
            parser.OFPActionSetField(eth_dst=self.server2_mac),
            parser.OFPActionSetField(ipv4_dst=self.server2_ip),
            parser.OFPActionOutput(self.server2_port, ofproto.OFPCML_NO_BUFFER),
        ]
        return actions

    @set_ev_cls(ofp_event.EventOFPPacketIn, MAIN_DISPATCHER)
    def packet_in_handler(self, ev):
        msg = ev.msg
        datapath = msg.datapath
        ofproto = datapath.ofproto
        parser = datapath.ofproto_parser

        in_port = msg.match.get("in_port")

        pkt = packet.Packet(msg.data)
        eth = pkt.get_protocol(ethernet.ethernet)
        ip_pkt = pkt.get_protocol(ipv4.ipv4)

        if eth is None:
            return

        # Ignore LLDP packets
        if eth.ethertype == 0x88CC:
            return

        # Learn MAC addresses
        dpid = datapath.id
        self.mac_to_port.setdefault(dpid, {})
        self.mac_to_port[dpid][eth.src] = in_port

        # Handle ARP and other non-TCP traffic with flooding
        # Proactive flows already handle TCP redirect traffic
        if eth.ethertype == 0x0806:  # ARP
            actions = [parser.OFPActionOutput(ofproto.OFPP_FLOOD)]
            out = parser.OFPPacketOut(
                datapath=datapath,
                buffer_id=msg.buffer_id,
                in_port=in_port,
                actions=actions,
                data=msg.data if msg.buffer_id == ofproto.OFP_NO_BUFFER else None,
            )
            datapath.send_msg(out)
            return

        # For any other packet_in (shouldn't happen with proactive flows)
        # Learn and forward based on MAC table
        dst = eth.dst
        if dst in self.mac_to_port[dpid]:
            out_port = self.mac_to_port[dpid][dst]
        else:
            out_port = ofproto.OFPP_FLOOD

        actions = [parser.OFPActionOutput(out_port)]
        out = parser.OFPPacketOut(
            datapath=datapath,
            buffer_id=msg.buffer_id,
            in_port=in_port,
            actions=actions,
            data=msg.data if msg.buffer_id == ofproto.OFP_NO_BUFFER else None,
        )
        datapath.send_msg(out)
