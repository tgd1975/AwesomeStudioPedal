#!/usr/bin/env python3
"""
Generate IDEA-014 test rig circuit schematics.

This script is purpose-specific to IDEA-014 and lives next to its SVG outputs.
It draws two representative sub-circuits (the rig has 4 relay channels and 9
opto channels — one of each is drawn, labelled so the reader knows it repeats):

  - relay channel: MCU GPIO -> ULN2803 -> reed coil -> SPST-NO contacts -> DUT
  - opto channel : DUT LED in series with quad-opto input LED;
                   transistor side -> 10 kOhm pull-up to 3V3 -> MCU GPIO input

Outputs (written next to this script, one .svg + one .net per schematic):
  idea-014-relay-channel.svg / .net
  idea-014-opto-channel.svg  / .net
  idea-014-system.svg        / .net
  idea-014-prototype.svg     / .net

The .net files are KiCad-compatible netlists (legacy "OrCAD/PCBNEW" S-expr
flavour) importable via eeschema/pcbnew "File → Import → Netlist". They are
emitted from the same component+net data the script already tracks for the
SVG, so the two never drift.

Usage:
    python docs/developers/ideas/open/assets/idea-014-schematic.py
"""

import argparse
import datetime
import pathlib
from collections import defaultdict
from typing import Dict, List, Tuple

import matplotlib
import schemdraw
import schemdraw.elements as elm

matplotlib.use("Agg")

ASSETS_DIR = pathlib.Path(__file__).resolve().parent


# ─────────────────────────────────────────────────────────────────────────────
# Minimal KiCad-netlist emitter
# ─────────────────────────────────────────────────────────────────────────────


class Netlist:
    """Collect components and net connections, emit a KiCad-compatible .net.

    Output format is the legacy OrCAD/PCBNEW S-expression netlist that
    eeschema and pcbnew accept via "File → Import → Netlist". Footprints are
    optional but recommended; values give pcbnew its BOM-side hint.
    """

    def __init__(self, source: str) -> None:
        self.source = source
        # ref -> (value, footprint)
        self._components: Dict[str, Tuple[str, str]] = {}
        # net_name -> list of (ref, pin)
        self._nets: Dict[str, List[Tuple[str, str]]] = defaultdict(list)

    def add_component(self, ref: str, value: str, footprint: str = "") -> None:
        if ref in self._components:
            # Same ref reused — must be identical or it's a bug
            assert self._components[ref] == (value, footprint), (
                f"Component {ref!r} re-declared with different value/footprint"
            )
            return
        self._components[ref] = (value, footprint)

    def connect(self, net: str, ref: str, pin: str) -> None:
        """Attach (ref, pin) to a named net. Idempotent."""
        node = (ref, str(pin))
        if node not in self._nets[net]:
            self._nets[net].append(node)

    def write(self, path: pathlib.Path) -> None:
        date = datetime.datetime.now().strftime("%Y-%m-%d %H:%M:%S")
        lines = [
            "(export (version \"E\")",
            f"  (design",
            f"    (source \"{self.source}\")",
            f"    (date \"{date}\")",
            f"    (tool \"idea-014-schematic.py\"))",
            "  (components",
        ]
        for ref in sorted(self._components):
            value, footprint = self._components[ref]
            fp = f' (footprint "{footprint}")' if footprint else ""
            lines.append(f'    (comp (ref "{ref}") (value "{value}"){fp})')
        lines.append("  )")
        lines.append("  (nets")
        for code, net_name in enumerate(sorted(self._nets), start=1):
            lines.append(f'    (net (code "{code}") (name "{net_name}")')
            for ref, pin in self._nets[net_name]:
                lines.append(f'      (node (ref "{ref}") (pin "{pin}"))')
            lines.append("    )")
        lines.append("  )")
        lines.append(")")
        path.write_text("\n".join(lines) + "\n")
        print(f"Saved: {path}")


def _whitebg(svg_path: pathlib.Path) -> None:
    """Inject a white background so the SVG renders cleanly on dark themes."""
    if svg_path.suffix.lower() != ".svg":
        return
    svg = svg_path.read_text()
    svg = svg.replace(
        '<g id="figure_1">',
        '<rect width="100%" height="100%" fill="white"/>\n <g id="figure_1">',
    )
    svg_path.write_text(svg)


def draw_relay_channel(output: pathlib.Path) -> None:
    """One representative relay channel: MCU -> ULN2803 -> relay -> DUT pin."""
    # Netlist for this single-channel illustration.
    nl = Netlist(source=output.name)
    nl.add_component("U1", "ESP32-S3", "Module:ESP32-S3-DevKitC-1")
    nl.add_component("U2", "ULN2803A", "Package_DIP:DIP-18_W7.62mm")
    nl.add_component("K1", "Reed-SPST-NO-5V", "Relay_THT:Relay_DPDT_Omron_G5V-2")
    nl.add_component("C1", "100uF", "CP_Radial_D5.0mm_P2.00mm")
    nl.add_component("C2", "100nF", "Capacitor_THT:C_Disc_D5.0mm_W2.5mm_P5.00mm")
    nl.add_component("J_DUT", "Conn_BTN_A", "Connector:Conn_01x01_Pin")
    # Connections (one channel — see system netlist for the full 4×).
    nl.connect("NET_5V",         "U2", "10")  # COM
    nl.connect("NET_5V",         "K1", "1")   # coil high
    nl.connect("NET_5V",         "C1", "1")
    nl.connect("NET_5V",         "C2", "1")
    nl.connect("NET_GND",        "U1", "GND")
    nl.connect("NET_GND",        "U2", "9")
    nl.connect("NET_GND",        "K1", "5")   # COM contact -> rig GND
    nl.connect("NET_GND",        "C1", "2")
    nl.connect("NET_GND",        "C2", "2")
    nl.connect("NET_GPIO_BTN_A", "U1", "BTN_A")
    nl.connect("NET_GPIO_BTN_A", "U2", "1")   # IN1
    nl.connect("NET_ULN_OUT_1",  "U2", "18")  # OUT1
    nl.connect("NET_ULN_OUT_1",  "K1", "2")   # coil low
    nl.connect("NET_DUT_BTN_A",  "K1", "6")   # NO contact
    nl.connect("NET_DUT_BTN_A",  "J_DUT", "1")
    nl.write(output.with_suffix(".net"))

    with schemdraw.Drawing(show=False) as d:
        d.config(fontsize=10, unit=2.5)

        # ── MCU on the left ───────────────────────────────────────────────
        mcu = d.add(elm.Ic(
            pins=[
                elm.IcPin(side="right", pin="GPIO\nBTN_A"),
                elm.IcPin(side="bottom", pin="GND"),
            ],
            size=(3.0, 2.5),
            label="ESP32-S3\ntest MCU",
        ))
        # Anchors auto-named pin1 (right) / pin2 (bottom); use explicit anchors instead
        d.add(elm.Ground().at(mcu.inB1))

        # ── ULN2803 in the middle ─────────────────────────────────────────
        d.add(elm.Line().at(mcu.inR1).right().length(2.5))

        uln = d.add(elm.Ic(
            pins=[
                elm.IcPin(side="left",   pin="IN1 (1)"),
                elm.IcPin(side="top",    pin="COM (10)"),
                elm.IcPin(side="bottom", pin="GND (9)"),
                elm.IcPin(side="right",  pin="OUT1 (18)"),
            ],
            size=(3.5, 3.0),
            label="ULN2803A\n(1 ch shown,\n3 more identical)",
        ))
        d.add(elm.Ground().at(uln.inB1))

        # ── Relay anchored explicitly at ULN.OUT1 + horizontal offset ────
        # Place relay so its in2 (coil low side) sits to the right of OUT1
        relay_x = uln.inR1[0] + 4.5
        relay_y = uln.inR1[1]
        relay = d.add(elm.Relay(switch="spst")
                      .at((relay_x, relay_y))
                      .anchor("in2"))

        # Wire ULN.OUT1 -> relay.in2 (coil low side)
        d.add(elm.Line().at(uln.inR1).to(relay.in2))

        # Coil high side (in1) up to +5V rail
        coil_top = d.add(elm.Line().at(relay.in1).up().length(2.5))
        # Tie ULN COM to same +5V net via a horizontal rail across the top
        com_up = d.add(elm.Line().at(uln.inT1).up().toy(coil_top.end))
        d.add(elm.Line().at(com_up.end).right().tox(coil_top.end))
        d.add(elm.Vdd().at(com_up.end).label("+5 V"))

        # Bulk cap on +5V rail between ULN.COM and the relay coil
        cap_x = (com_up.end[0] + coil_top.end[0]) / 2
        cap_top = (cap_x, com_up.end[1])
        d.add(elm.Dot().at(cap_top))
        d.add(elm.Capacitor().at(cap_top).down().length(2.5)
              .label("100 µF\n+ 100 nF", loc="right"))
        d.add(elm.Ground())

        # ── Relay output contacts ─────────────────────────────────────────
        # a = COM contact -> rig GND ; b = NO contact -> DUT pin
        d.add(elm.Line().at(relay.a).left().length(1.2))
        d.add(elm.Ground().label("rig GND", loc="left"))

        d.add(elm.Line().at(relay.b).right().length(1.5))
        d.add(elm.Dot(open=True).label("DUT BTN_A\n(via 30-pin socket\nor screw terminal)", loc="right"))

        # Relay label
        d.add(elm.Label().at((relay_x + 1.2, relay_y - 3.5))
              .label("RY1 — 5 V SPST-NO reed\n(~10–20 mA coil)"))

        # ── Title ─────────────────────────────────────────────────────────
        d.add(elm.Label().at((mcu.center[0] + 8, mcu.center[1] + 5.2)).label(
            "Relay channel — one of 4 (BTN_A shown; BTN_B, BTN_SWITCH, "
            "RESET wire identically; RESET routes to DUT EN, not a button line)",
            loc="top",
        ))

        d.save(str(output))

    _whitebg(output)
    print(f"Saved: {output}")


def draw_opto_channel(output: pathlib.Path) -> None:
    """One representative opto channel: DUT LED in series with quad-opto input,
    transistor side -> 10 kΩ pull-up -> MCU GPIO."""
    # Netlist for this single-channel illustration. Uses generic pin numbers
    # for the quad opto (1=anode, 2=cathode, 3=collector, 4=emitter — channel 1).
    nl = Netlist(source=output.name)
    nl.add_component("U1",  "ESP32-S3",  "Module:ESP32-S3-DevKitC-1")
    nl.add_component("J_DUT_GPIO", "DUT_GPIO_LED", "Connector:Conn_01x01_Pin")
    nl.add_component("J_DUT_GND",  "DUT_GND",      "Connector:Conn_01x01_Pin")
    nl.add_component("R_DUT", "470R", "Resistor_THT:R_Axial_DIN0207_L6.3mm_D2.5mm_P2.54mm_Horizontal")
    nl.add_component("D_DUT", "LED",  "LED_THT:LED_D3.0mm")
    nl.add_component("U_OPT", "LTV-846", "Package_DIP:DIP-16_W7.62mm")
    nl.add_component("R_PU",  "10k",   "Resistor_THT:R_Axial_DIN0207_L6.3mm_D2.5mm_P2.54mm_Horizontal")

    nl.connect("NET_DUT_LED_DRIVE",   "J_DUT_GPIO", "1")
    nl.connect("NET_DUT_LED_DRIVE",   "R_DUT", "1")
    nl.connect("NET_DUT_LED_AFTER_R", "R_DUT", "2")
    nl.connect("NET_DUT_LED_AFTER_R", "D_DUT", "1")  # anode
    nl.connect("NET_OPTO_IN_ANODE",   "D_DUT", "2")  # cathode
    nl.connect("NET_OPTO_IN_ANODE",   "U_OPT", "1")  # opto LED anode (ch1)
    nl.connect("NET_DUT_GND",         "U_OPT", "2")  # opto LED cathode
    nl.connect("NET_DUT_GND",         "J_DUT_GND", "1")
    nl.connect("NET_3V3",             "R_PU", "1")
    nl.connect("NET_GPIO_OPTO_OUT",   "R_PU", "2")
    nl.connect("NET_GPIO_OPTO_OUT",   "U_OPT", "3")  # collector (ch1)
    nl.connect("NET_GPIO_OPTO_OUT",   "U1", "GPIO_OPTO")
    nl.connect("NET_GND",             "U_OPT", "4")  # emitter (ch1)
    nl.write(output.with_suffix(".net"))

    with schemdraw.Drawing(show=False) as d:
        d.config(fontsize=10, unit=2.5)

        # ── DUT GPIO source (left) ────────────────────────────────────────
        d.add(elm.Dot(open=True).label("DUT GPIO\n(LED drive)", loc="left"))
        d.add(elm.Line().right().length(1.0))

        # ── DUT R → DUT LED → opto input LED → DUT GND (single series chain) ─
        d.add(elm.Resistor().right().length(2.0)
              .label("R_DUT\n(existing)", loc="top"))
        d.add(elm.LED().right().length(1.8)
              .label("DUT LED\n(visible)", loc="top"))
        d.add(elm.LED().right().length(1.8)
              .label("opto input LED\n(quad-opto, 1 of 4)", loc="bottom"))
        chain_end = d.add(elm.Dot())
        d.add(elm.Line().right().length(1.5))
        d.add(elm.Ground().label("DUT GND"))

        # ── Opto transistor side (right of the input chain) ───────────────
        opto_centre_x = chain_end.center[0] + 6.0
        opto_centre_y = chain_end.center[1]
        opto = d.add(elm.Optocoupler(base=False).at(
            (opto_centre_x, opto_centre_y)).label(
            "Quad opto\n(LTV-846 /\nPS2501-4 /\nTLP281-4)", loc="bottom"))

        # Pull-up from collector to +3V3
        d.add(elm.Line().at(opto.collector).right().length(1.0))
        pu_node = d.add(elm.Dot())
        d.add(elm.Resistor().up().length(2.0).label("10 kΩ", loc="right"))
        d.add(elm.Vdd().label("+3V3"))

        # MCU GPIO tap from the pull-up node
        d.add(elm.Line().at(pu_node.center).right().length(2.5))
        d.add(elm.Dot(open=True).label("ESP32-S3 GPIO in\n(active-low)", loc="right"))

        # Emitter -> rig GND
        d.add(elm.Line().at(opto.emitter).right().length(1.0))
        d.add(elm.Ground().label("rig GND"))

        # Dashed line between the input chain and the transistor symbol —
        # the optocoupler IS that disconnect optically.
        bridge_y = chain_end.center[1] - 1.2
        bridge_left = (chain_end.center[0] + 0.2, bridge_y)
        bridge_right = (opto.anode[0] - 0.2, bridge_y)
        d.add(elm.Line().at(bridge_left).to(bridge_right).linestyle(":")
              .label("optical coupling\n(same package)", loc="top"))

        # ── Title ─────────────────────────────────────────────────────────
        d.add(elm.Label().label(
            "Opto channel — one of 9 (default: opto LED in SERIES with "
            "DUT LED, sharing R_DUT; parallel-with-its-own-R is the "
            "fallback for high-Vf visible LEDs)",
            loc="top",
        ).at((opto_centre_x - 4, chain_end.center[1] + 4.5)))

        d.save(str(output))

    _whitebg(output)
    print(f"Saved: {output}")


def draw_system_schematic(output: pathlib.Path) -> None:
    """Full system schematic — every component and every net.

    Layout uses **labelled nets** (KiCad-style) rather than running every wire
    physically across the page. A pin terminating in `[NET_NAME]` is
    electrically the same node as every other `[NET_NAME]` stub on the page.
    This keeps the diagram readable while remaining complete; it also maps
    1-to-1 onto KiCad's net-label workflow when you redraw it in eeschema.
    """
    GPIO_OUTS = ["GPIO_BTN_A", "GPIO_BTN_B", "GPIO_BTN_SWITCH", "GPIO_RESET"]
    GPIO_INS = ["GPIO_btn_a_1", "GPIO_btn_a_2", "GPIO_btn_b_1", "GPIO_btn_b_2",
                "GPIO_sw_1", "GPIO_sw_2", "GPIO_sw_3", "GPIO_ble", "GPIO_power"]
    DUT_BTN_NETS = ["DUT_BTN_A", "DUT_BTN_B", "DUT_BTN_SWITCH", "DUT_EN"]
    DUT_LED_NETS = ["DUT_LED_btn_a_1", "DUT_LED_btn_a_2", "DUT_LED_btn_b_1",
                    "DUT_LED_btn_b_2", "DUT_LED_sw_1", "DUT_LED_sw_2",
                    "DUT_LED_sw_3", "DUT_LED_ble", "DUT_LED_power"]
    # Pin map for the rig MCU outputs (matches the doc's "Rig MCU pin map" table).
    ESP_OUT_PINS = {"GPIO_BTN_A": "4", "GPIO_BTN_B": "5",
                    "GPIO_BTN_SWITCH": "6", "GPIO_RESET": "7"}
    ESP_IN_PINS = {"GPIO_btn_a_1": "15", "GPIO_btn_a_2": "16",
                   "GPIO_btn_b_1": "17", "GPIO_btn_b_2": "18",
                   "GPIO_sw_1":   "38", "GPIO_sw_2":   "39",
                   "GPIO_sw_3":   "40", "GPIO_ble":    "41",
                   "GPIO_power":  "42"}

    # ── Build the netlist in parallel with the SVG ───────────────────────
    nl = Netlist(source=output.name)
    nl.add_component("U1", "ESP32-S3-DevKitC-1", "Module:ESP32-S3-DevKitC-1")
    nl.add_component("U2", "ULN2803A",           "Package_DIP:DIP-18_W7.62mm")
    # 3 quad optos (U3/U4/U5), 9 channels used, 3 unused (left disconnected).
    for u in ("U3", "U4", "U5"):
        nl.add_component(u, "LTV-846", "Package_DIP:DIP-16_W7.62mm")
    nl.add_component("C1", "100uF", "CP_Radial_D5.0mm_P2.00mm")
    nl.add_component("C2", "100nF", "Capacitor_THT:C_Disc_D5.0mm_W2.5mm_P5.00mm")
    nl.add_component("J1", "DUT_NodeMCU-30",
                     "Connector_PinSocket_2.54mm:PinSocket_1x15_P2.54mm_Vertical_x2_0.9in")

    # ESP32 power
    nl.connect("NET_5V",  "U1", "VBUS")
    nl.connect("NET_3V3", "U1", "3V3")
    nl.connect("NET_GND", "U1", "GND")
    # ULN power
    nl.connect("NET_5V",  "U2", "10")  # COM
    nl.connect("NET_GND", "U2", "9")
    # Bulk decoupling
    nl.connect("NET_5V",  "C1", "1")
    nl.connect("NET_GND", "C1", "2")
    nl.connect("NET_5V",  "C2", "1")
    nl.connect("NET_GND", "C2", "2")
    # ULN unused inputs (IN5..IN8) tied to GND
    for in_pin in ("5", "6", "7", "8"):
        nl.connect("NET_GND", "U2", in_pin)

    # 4 relays (K1..K4) — coil pins 1/2, contacts 5 (COM) / 6 (NO) per channel.
    for i in range(4):
        ref = f"K{i+1}"
        nl.add_component(ref, "Reed-SPST-NO-5V",
                         "Relay_THT:Relay_DPDT_Omron_G5V-2")
        # Coil
        nl.connect("NET_5V",                        ref, "1")
        nl.connect(f"NET_ULN_OUT_{i+1}",            ref, "2")
        # Contacts
        nl.connect("NET_GND",                       ref, "5")  # COM -> rig GND
        nl.connect(f"NET_{DUT_BTN_NETS[i]}",        ref, "6")  # NO  -> DUT pin
        # ESP -> ULN input -> ULN output -> coil
        gpio_net = f"NET_{GPIO_OUTS[i]}"
        nl.connect(gpio_net, "U1", ESP_OUT_PINS[GPIO_OUTS[i]])
        nl.connect(gpio_net, "U2", str(i + 1))               # ULN IN1..IN4
        nl.connect(f"NET_ULN_OUT_{i+1}", "U2", str(18 - i))  # ULN OUT1..OUT4 = pins 18,17,16,15

    # 9 opto channels distributed over U3/U4/U5 (4 ch each, 1 unused on U5).
    # Per quad-opto pinout (LTV-846 / PS2501-4 / TLP281-4):
    #   ch1: A=1  K=2  C=16 E=15
    #   ch2: A=3  K=4  C=14 E=13
    #   ch3: A=5  K=6  C=12 E=11
    #   ch4: A=7  K=8  C=10 E=9
    QUAD_PINS = {
        1: ("1", "2", "16", "15"),
        2: ("3", "4", "14", "13"),
        3: ("5", "6", "12", "11"),
        4: ("7", "8", "10", "9"),
    }
    for i in range(9):
        pkg = (i // 4) + 1                      # 1..3
        ch  = (i % 4) + 1                       # 1..4
        ref = {1: "U3", 2: "U4", 3: "U5"}[pkg]
        a, k, c, e = QUAD_PINS[ch]
        # Add a per-channel pull-up resistor (R1..R9)
        rref = f"R{i+1}"
        nl.add_component(rref, "10k",
                         "Resistor_THT:R_Axial_DIN0207_L6.3mm_D2.5mm_P2.54mm_Horizontal")
        # DUT LED -> opto input anode  (cathode -> DUT GND)
        nl.connect(f"NET_{DUT_LED_NETS[i]}", ref, a)
        nl.connect("NET_DUT_GND",            ref, k)
        # Collector -> pull-up + ESP GPIO input ; emitter -> rig GND
        gpio_in = f"NET_{GPIO_INS[i]}"
        nl.connect(gpio_in, ref, c)
        nl.connect(gpio_in, rref, "2")
        nl.connect(gpio_in, "U1", ESP_IN_PINS[GPIO_INS[i]])
        nl.connect("NET_3V3", rref, "1")
        nl.connect("NET_GND", ref, e)

    # Three unused opto channels on U5 (channels 2, 3, 4 — channel 1 is OK9).
    # Leave them unconnected at the input (anode floats); cathode tied to GND
    # so the LED can never forward-bias.
    for unused_ch in (2, 3, 4):
        _, k_pin, _, _ = QUAD_PINS[unused_ch]
        nl.connect("NET_GND", "U5", k_pin)

    # DUT socket pins — use the net name as the "pin name" since the socket
    # mapping is documented separately in the doc table.
    for n in DUT_BTN_NETS + DUT_LED_NETS:
        nl.connect(f"NET_{n}", "J1", n)
    nl.connect("NET_DUT_GND", "J1", "DUT_GND")

    nl.write(output.with_suffix(".net"))

    def net(name: str, loc: str = "right") -> elm.Element:
        """A labelled net stub. Multiple stubs with the same name are the
        same electrical node (KiCad convention)."""
        return elm.Dot(open=True).label(f"[{name}]", loc=loc, fontsize=7)

    with schemdraw.Drawing(show=False) as d:
        d.config(unit=1.5, fontsize=8)

        # ──────────────────── Power section (header) ─────────────────────
        d.add(elm.Label().at((0, 4)).label(
            "POWER (from host PC USB via ESP32-S3 dev board)", loc="top"))
        # +5V rail
        d.add(elm.Vdd().at((0, 2.5)).label("+5V"))
        d.add(elm.Line().at((0, 2.5)).right().length(2.0))
        d.add(net("NET_5V", "right"))
        # +3V3 rail
        d.add(elm.Vdd().at((6, 2.5)).label("+3V3"))
        d.add(elm.Line().at((6, 2.5)).right().length(2.0))
        d.add(net("NET_3V3", "right"))
        # GND
        d.add(elm.Ground().at((12, 2.0)).label("rig GND"))
        d.add(elm.Line().at((12, 2.0)).up().length(0.5))
        d.add(net("NET_GND", "right"))
        # Bulk cap on +5V
        d.add(elm.Capacitor().at((16, 2.5)).down().length(1.5)
              .label("C1\n100 µF\n+ 100 nF", loc="right"))
        d.add(elm.Ground())
        d.add(elm.Line().at((16, 2.5)).left().length(2.0))
        d.add(elm.Dot())
        d.add(elm.Label().at((16, 2.5)).label("[NET_5V]", loc="right", fontsize=7))

        # ──────────────────── ESP32-S3 (left) ────────────────────────────
        # Use anchor names only — labels carried by the net stubs, not the IC pins
        esp_pins = []
        for n in GPIO_OUTS + GPIO_INS:
            esp_pins.append(elm.IcPin(side="right"))
        esp_pins.append(elm.IcPin(side="top"))     # +5V (VBUS)
        esp_pins.append(elm.IcPin(side="top"))     # +3V3
        esp_pins.append(elm.IcPin(side="bottom"))  # GND

        esp = d.add(elm.Ic(
            pins=esp_pins,
            size=(5, 22),
            label="U1\nESP32-S3\ntest MCU\n\n(13 GPIO pins;\nanchor by side+index)",
        ).at((0, -22)))

        # Wire each ESP pin to a labelled net stub
        for i, pin_name in enumerate(GPIO_OUTS + GPIO_INS):
            anchor = getattr(esp, f"inR{i+1}")
            d.add(elm.Line().at(anchor).right().length(1.5))
            d.add(net(f"NET_{pin_name}", "right"))
        # Power pins
        d.add(elm.Line().at(esp.inT1).up().length(0.8))
        d.add(net("NET_5V", "top"))
        d.add(elm.Line().at(esp.inT2).up().length(0.8))
        d.add(net("NET_3V3", "top"))
        d.add(elm.Line().at(esp.inB1).down().length(0.8))
        d.add(elm.Ground())

        # ──────────────────── ULN2803A ───────────────────────────────────
        # Anchor-only pins; labels come from net stubs
        uln_pins = ([elm.IcPin(side="left")  for _ in range(8)]
                    + [elm.IcPin(side="right") for _ in range(8)]
                    + [elm.IcPin(side="top"),
                       elm.IcPin(side="bottom")])
        uln = d.add(elm.Ic(
            pins=uln_pins,
            size=(5, 18),
            label="U2\nULN2803A\n\nIN1-IN8 left\nOUT1-OUT8 right\nCOM=10 top\nGND=9 bottom",
        ).at((16, -22)))

        # ULN inputs IN1-IN4 from ESP32 GPIO outputs (named net stubs)
        for i in range(4):
            anchor = getattr(uln, f"inL{i+1}")
            d.add(elm.Line().at(anchor).left().length(1.0))
            d.add(net(f"NET_{GPIO_OUTS[i]}", "left"))
        # ULN inputs IN5-IN8: tied to GND (unused channels held off)
        for i in range(4, 8):
            anchor = getattr(uln, f"inL{i+1}")
            d.add(elm.Line().at(anchor).left().length(0.8))
            d.add(elm.Ground().label("GND", loc="left"))
        # ULN outputs OUT1-OUT4: to relay coil low side (named nets)
        for i in range(4):
            anchor = getattr(uln, f"inR{i+1}")
            d.add(elm.Line().at(anchor).right().length(1.0))
            d.add(net(f"NET_ULN_OUT_{i+1}", "right"))
        # ULN outputs OUT5-OUT8: NC (not used)
        for i in range(4, 8):
            anchor = getattr(uln, f"inR{i+1}")
            d.add(elm.NoConnect().at(anchor))
        # ULN COM to +5V
        d.add(elm.Line().at(uln.inT1).up().length(0.8))
        d.add(net("NET_5V", "top"))
        # ULN GND
        d.add(elm.Line().at(uln.inB1).down().length(0.8))
        d.add(elm.Ground())

        # ──────────────────── 4 relays (right of ULN) ────────────────────
        for i in range(4):
            ry_y = -10 - i * 5
            relay = d.add(elm.Relay(switch="spst")
                          .at((26, ry_y))
                          .label(f"K{i+1}\n5V reed\nSPST-NO", loc="right"))
            # Coil high side (in1) -> +5V
            d.add(elm.Line().at(relay.in1).up().length(0.8))
            d.add(net("NET_5V", "top"))
            # Coil low side (in2) -> ULN OUT_n net
            d.add(elm.Line().at(relay.in2).left().length(1.5))
            d.add(net(f"NET_ULN_OUT_{i+1}", "left"))
            # Contact a (COM) -> rig GND
            d.add(elm.Line().at(relay.a).left().length(1.0))
            d.add(net("NET_GND", "left"))
            # Contact b (NO) -> DUT pin
            d.add(elm.Line().at(relay.b).right().length(1.5))
            d.add(net(f"NET_{DUT_BTN_NETS[i]}", "right"))

        # ──────────────────── 9 optocouplers (below) ─────────────────────
        # Stack 9 PC817s in a 3×3 grid below the ICs
        for i in range(9):
            col, row = i % 3, i // 3
            opto_x = col * 9.0
            opto_y = -42 - row * 7
            pkg = (i // 4) + 1  # 3 quad packages: OK1 = pkg1ch1, OK5 = pkg2ch1...
            ch = (i % 4) + 1
            opto = d.add(elm.Optocoupler(base=False)
                         .at((opto_x, opto_y))
                         .label(f"OK{i+1}\nU{2+pkg} ch{ch}\n(LTV-846)", loc="bottom"))
            # Anode -> DUT_LED net (series default: shares R_DUT in DUT,
            # no R_opto on rig side)
            d.add(elm.Line().at(opto.anode).left().length(2.5))
            d.add(net(f"NET_{DUT_LED_NETS[i]}", "left"))
            # Cathode -> DUT_GND
            d.add(elm.Line().at(opto.cathode).left().length(0.8))
            d.add(net("NET_DUT_GND", "left"))
            # Collector -> 10 kΩ pull-up to +3V3 AND to ESP GPIO input
            d.add(elm.Line().at(opto.collector).right().length(0.8))
            pu_node = d.add(elm.Dot())
            d.add(elm.Resistor().up().length(1.5)
                  .label(f"R{i+1}\n10 kΩ", loc="right", fontsize=7))
            d.add(net("NET_3V3", "top"))
            # MCU input net
            d.add(elm.Line().at(pu_node.center).right().length(1.0))
            d.add(net(f"NET_{GPIO_INS[i]}", "right"))
            # Emitter -> rig GND
            d.add(elm.Line().at(opto.emitter).right().length(0.8))
            d.add(net("NET_GND", "right"))

        # ──────────────────── DUT socket (far right) ─────────────────────
        # 30-pin NodeMCU-ESP32 socket — show only the 14 used pins for clarity
        dut_pins = []
        for n in DUT_BTN_NETS:
            dut_pins.append(elm.IcPin(side="left", pin=n))
        for n in DUT_LED_NETS:
            dut_pins.append(elm.IcPin(side="left", pin=n))
        dut_pins.append(elm.IcPin(side="bottom", pin="DUT GND"))

        dut = d.add(elm.Ic(
            pins=dut_pins,
            size=(4.5, 16),
            label="J1\nDUT socket\n(30-pin NodeMCU-ESP32;\nonly 14 used pins shown)",
        ).at((40, -16)))

        # Wire DUT pin labels to their respective nets
        for i, n in enumerate(DUT_BTN_NETS + DUT_LED_NETS):
            anchor = getattr(dut, f"inL{i+1}")
            d.add(elm.Line().at(anchor).left().length(1.5))
            net_name = (f"NET_{n}" if n in DUT_BTN_NETS
                        else f"NET_DUT_LED_{n.replace('DUT_LED_', '')}")
            # The DUT_LED_NETS entries already have correct format
            net_name = f"NET_{n}"
            d.add(net(net_name, "left"))
        # DUT GND
        d.add(elm.Line().at(dut.inB1).down().length(1.0))
        d.add(net("NET_DUT_GND", "right"))

        # ──────────────────── Title + footer ─────────────────────────────
        d.add(elm.Label().at((20, 6)).label(
            "Test rig — full system schematic (all 4 relay + all 9 opto channels). "
            "Net labels in [BRACKETS] are nominal — dots with the same label are "
            "the same node.",
            loc="top",
        ))
        d.add(elm.Label().at((20, -68)).label(
            "Note: rig GND and DUT GND share host-PC ground via the two USB "
            "cables; the rig-to-DUT connector also carries a direct GND wire "
            "for short signal returns.",
            loc="bottom",
        ))

        d.save(str(output))

    _whitebg(output)
    print(f"Saved: {output}")


def draw_prototype_schematic(output: pathlib.Path) -> None:
    """Single-channel breadboard prototype: combines one reed channel and one
    opto channel between two ESP32 dev boards. Throwaway proof-of-concept for
    IDEA-014 — not the production design."""
    # Netlist for the breadboard prototype (1 reed + 1 PC817 + 2 ESP boards).
    nl = Netlist(source=output.name)
    nl.add_component("U1_RIG", "ESP32-S3", "Module:ESP32-S3-DevKitC-1")
    nl.add_component("U2_DUT", "ESP32-S3", "Module:ESP32-S3-DevKitC-1")
    nl.add_component("U_ULN",  "ULN2803A", "Package_DIP:DIP-18_W7.62mm")
    nl.add_component("K1",     "Reed-SPST-NO-5V",
                     "Relay_THT:Relay_DPDT_Omron_G5V-2")
    nl.add_component("U_OPT",  "PC817",    "Package_DIP:DIP-4_W7.62mm")
    nl.add_component("R_DUT",  "470R",
                     "Resistor_THT:R_Axial_DIN0207_L6.3mm_D2.5mm_P2.54mm_Horizontal")
    nl.add_component("R_PU",   "10k",
                     "Resistor_THT:R_Axial_DIN0207_L6.3mm_D2.5mm_P2.54mm_Horizontal")
    nl.add_component("D_DUT",  "LED",      "LED_THT:LED_D3.0mm")

    # Stimulus path
    nl.connect("NET_RIG_5V",         "U1_RIG", "VBUS")
    nl.connect("NET_RIG_3V3",        "U1_RIG", "3V3")
    nl.connect("NET_GND",            "U1_RIG", "GND")
    nl.connect("NET_GND",            "U2_DUT", "GND")
    nl.connect("NET_RIG_BTN_A_OUT",  "U1_RIG", "BTN_A_OUT")
    nl.connect("NET_RIG_BTN_A_OUT",  "U_ULN",  "1")   # IN1
    nl.connect("NET_RIG_5V",         "U_ULN",  "10")  # COM
    nl.connect("NET_GND",            "U_ULN",  "9")
    nl.connect("NET_ULN_OUT_1",      "U_ULN",  "18")  # OUT1
    nl.connect("NET_ULN_OUT_1",      "K1",     "2")   # coil low
    nl.connect("NET_RIG_5V",         "K1",     "1")   # coil high
    nl.connect("NET_GND",            "K1",     "5")   # COM contact
    nl.connect("NET_DUT_BTN_A_IN",   "K1",     "6")   # NO contact
    nl.connect("NET_DUT_BTN_A_IN",   "U2_DUT", "BTN_A_IN")

    # Sense path (PC817 pin map: 1=A, 2=K, 3=E, 4=C)
    nl.connect("NET_DUT_LED_OUT",    "U2_DUT", "LED_OUT")
    nl.connect("NET_DUT_LED_OUT",    "R_DUT",  "1")
    nl.connect("NET_DUT_LED_AFTER_R","R_DUT",  "2")
    nl.connect("NET_DUT_LED_AFTER_R","D_DUT",  "1")
    nl.connect("NET_OPTO_IN_ANODE",  "D_DUT",  "2")
    nl.connect("NET_OPTO_IN_ANODE",  "U_OPT",  "1")  # opto LED A
    nl.connect("NET_GND",            "U_OPT",  "2")  # opto LED K -> shared GND
    nl.connect("NET_RIG_3V3",        "R_PU",   "1")
    nl.connect("NET_RIG_LED_SENSE",  "R_PU",   "2")
    nl.connect("NET_RIG_LED_SENSE",  "U_OPT",  "4")  # collector
    nl.connect("NET_RIG_LED_SENSE",  "U1_RIG", "LED_SENSE")
    nl.connect("NET_GND",            "U_OPT",  "3")  # emitter
    nl.write(output.with_suffix(".net"))

    with schemdraw.Drawing(show=False) as d:
        d.config(fontsize=9, unit=2.0)

        # ─── Title ────────────────────────────────────────────────────────
        d.add(elm.Label().at((10, 9)).label(
            "IDEA-014 prototype — single reed channel + single opto channel "
            "between two ESP32 dev boards (throwaway breadboard build)",
            loc="top",
        ))

        # ════════ STIMULUS PATH (top half) ════════════════════════════════
        d.add(elm.Label().at((10, 7)).label(
            "── STIMULUS: rig MCU asserts a button press on the fake DUT ──",
            loc="top",
        ))

        # Rig MCU output stub
        rig_out = d.add(elm.Dot(open=True).at((0, 5))
                        .label("Rig ESP32-S3\nGPIO BTN_A_OUT", loc="left"))
        d.add(elm.Line().right().length(2.0))

        # ULN2803
        uln = d.add(elm.Ic(
            pins=[
                elm.IcPin(side="left", pin="IN1"),
                elm.IcPin(side="right", pin="OUT1"),
                elm.IcPin(side="top", pin="COM"),
                elm.IcPin(side="bottom", pin="GND"),
            ],
            size=(2.5, 2.0),
            label="ULN2803A\n(1 ch used)",
        ))
        d.add(elm.Vdd().at(uln.inT1).label("+5 V"))
        d.add(elm.Ground().at(uln.inB1))

        # Reed coil + contact (use Relay element)
        relay_x = uln.inR1[0] + 4.0
        relay_y = uln.inR1[1]
        relay = d.add(elm.Relay(switch="spst")
                      .at((relay_x, relay_y)).anchor("in2"))
        d.add(elm.Line().at(uln.inR1).to(relay.in2))
        coil_top = d.add(elm.Line().at(relay.in1).up().length(2.0))
        d.add(elm.Vdd().at(coil_top.end).label("+5 V"))
        d.add(elm.Label().at((relay_x + 1.0, relay_y - 3.2))
              .label("5 V reed\nSPST-NO"))

        # Reed contacts → fake DUT
        d.add(elm.Line().at(relay.a).left().length(1.0))
        d.add(elm.Ground().label("rig GND", loc="left"))
        d.add(elm.Line().at(relay.b).right().length(2.0))
        d.add(elm.Dot(open=True).label(
            "Fake DUT ESP32-S3\nGPIO BTN_A_IN\n(internal pull-up enabled)",
            loc="right"))

        # ════════ SENSE PATH (bottom half) ════════════════════════════════
        d.add(elm.Label().at((10, -1.5)).label(
            "── SENSE: fake DUT lights its LED, opto reports state to rig ──",
            loc="top",
        ))

        # Fake DUT LED_OUT → R → DUT LED → opto input → DUT GND
        d.add(elm.Dot(open=True).at((0, -3))
              .label("Fake DUT ESP32-S3\nGPIO LED_OUT", loc="left"))
        d.add(elm.Line().right().length(1.0))
        d.add(elm.Resistor().right().length(1.8)
              .label("R_DUT\n470 Ω", loc="top"))
        d.add(elm.LED().right().length(1.8)
              .label("DUT LED\n(visible)", loc="top"))
        d.add(elm.LED().right().length(1.8)
              .label("PC817\ninput LED", loc="bottom"))
        chain_end = d.add(elm.Dot())
        d.add(elm.Line().right().length(1.5))
        d.add(elm.Ground().label("DUT GND"))

        # Opto transistor side
        opto_x = chain_end.center[0] + 5.0
        opto_y = chain_end.center[1]
        opto = d.add(elm.Optocoupler(base=False).at((opto_x, opto_y))
                     .label("PC817\n(transistor)", loc="bottom"))

        # Pull-up + rig MCU input
        d.add(elm.Line().at(opto.collector).right().length(1.0))
        pu_node = d.add(elm.Dot())
        d.add(elm.Resistor().up().length(1.8).label("10 kΩ", loc="right"))
        d.add(elm.Vdd().label("+3.3 V (rig)"))
        d.add(elm.Line().at(pu_node.center).right().length(2.0))
        d.add(elm.Dot(open=True).label(
            "Rig ESP32-S3\nGPIO LED_SENSE\n(active-low)", loc="right"))

        # Emitter
        d.add(elm.Line().at(opto.emitter).right().length(1.0))
        d.add(elm.Ground().label("rig GND"))

        # Dashed line for optical coupling
        bridge_y = chain_end.center[1] - 1.2
        d.add(elm.Line().at((chain_end.center[0] + 0.2, bridge_y))
              .to((opto.anode[0] - 0.2, bridge_y)).linestyle(":")
              .label("optical coupling\n(same PC817)", loc="top"))

        # Footer
        d.add(elm.Label().at((10, -8)).label(
            "Rig +5 V / +3.3 V come from the rig dev board. "
            "Rig GND ↔ DUT GND tied via one jumper. "
            "Fake-DUT LED is the only LED on the breadboard — both ESP32s "
            "have their own USB cable to the host PC.",
            loc="bottom"))

        d.save(str(output))

    _whitebg(output)
    print(f"Saved: {output}")


def main() -> None:
    parser = argparse.ArgumentParser(description="Generate IDEA-014 rig schematics.")
    parser.add_argument("--out-dir", type=pathlib.Path, default=ASSETS_DIR,
                        help="Output directory (default: alongside this script)")
    args = parser.parse_args()
    args.out_dir.mkdir(parents=True, exist_ok=True)

    draw_relay_channel(args.out_dir / "idea-014-relay-channel.svg")
    draw_opto_channel(args.out_dir / "idea-014-opto-channel.svg")
    draw_system_schematic(args.out_dir / "idea-014-system.svg")
    draw_prototype_schematic(args.out_dir / "idea-014-prototype.svg")


if __name__ == "__main__":
    main()
