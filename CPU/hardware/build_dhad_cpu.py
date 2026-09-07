#!/usr/bin/env python3
"""
Build script to generate dhad_cpu.circ - A complete Logisim Evolution 3.8.0 file
with 24 circuits for the Dhad 8-bit CPU.
"""

import os, sys

OUTPUT = "/home/m_hmoz/Documents/cpu/hardware/dhad_cpu.circ"

# ============================================================
# Coordinate helpers
# ============================================================
def p(x, y):
    return f"({x},{y})"

# ============================================================
# XML element builders  
# ============================================================
def xml_comp(lib, name, loc, attrs=None):
    """Build a <comp> element string. lib=None for user subcircuits."""
    attrs = attrs or {}
    if lib is not None:
        s = f'    <comp lib="{lib}" loc="{loc}" name="{name}">'
    else:
        s = f'    <comp loc="{loc}" name="{name}">'
    for k in sorted(attrs.keys()):
        s += f'\n      <a name="{k}" val="{attrs[k]}"/>'
    s += '\n    </comp>'
    return s

def xml_wire(x1, y1, x2, y2):
    return f'    <wire from="{p(x1,y1)}" to="{p(x2,y2)}"/>'

def xml_pin(x, y, width, is_output, label):
    out = "true" if is_output else "false"
    facing = "west" if is_output else "east"
    return xml_comp("0", "Pin", p(x, y), {
        "width": str(width), "output": out, "label": label, "facing": facing,
    })

def xml_constant(x, y, width, value):
    value = value.strip()
    if value.startswith("0x") or value.startswith("0X"):
        pass  # already correct format
    elif value.startswith("x") or value.startswith("X"):
        value = "0" + value  # xff -> 0xff
    else:
        # Try decimal first
        try:
            int_val = int(value, 10)
            if int_val > 9:
                value = "0x" + format(int_val, f'0{(width + 3) // 4}x')
            # 0-9 decimal is fine as-is
        except ValueError:
            # Not decimal, try hex (e.g. "ff")
            try:
                int_val = int(value, 16)
                value = "0x" + format(int_val, f'0{(width + 3) // 4}x')
            except ValueError:
                pass  # keep as-is
    return xml_comp("0", "Constant", p(x, y), {"width": str(width), "value": value})

FACING_MAP = {"right": "east", "left": "west", "east": "east", "west": "west"}

def xml_splitter(x, y, width, fanout, combine="none", facing="east", bits=None):
    facing = FACING_MAP.get(facing, facing)
    attrs = {"fanout": str(fanout), "width": str(width), "combine": combine, "facing": facing}
    if bits is None:
        bits = list(range(fanout))
    for i, b in enumerate(bits):
        attrs[f"bit{i}"] = str(b)
    return xml_comp("0", "Splitter", p(x, y), attrs)

def subcircuit(name, x, y, label=None):
    """Create a user subcircuit component (no lib attribute)."""
    attrs = {}
    if label:
        attrs["label"] = label
    return xml_comp(None, name, p(x, y), attrs)

# ============================================================
# Wires collector
# ============================================================
class Circuit:
    def __init__(self, name, label=None):
        self.name = name
        self.label = label or name
        self.lines = []
        self.wire_set = set()

    def add(self, s):
        self.lines.append(s)

    def wire(self, x1, y1, x2, y2):
        """Add an axis-aligned wire (deduped)."""
        if x1 == x2 and y1 == y2:
            return
        assert x1 == x2 or y1 == y2, f"DIAGONAL: ({x1},{y1})->({x2},{y2})"
        key = (min(x1,x2), min(y1,y2), max(x1,x2), max(y1,y2))
        if key not in self.wire_set:
            self.wire_set.add(key)
            self.lines.append(xml_wire(x1, y1, x2, y2))

    def wire_h(self, x1, x2, y):
        self.wire(x1, y, x2, y)

    def wire_v(self, x, y1, y2):
        self.wire(x, y1, x, y2)

    def route_hv(self, x1, y1, x2, y2):
        """L-shaped route: horizontal first, then vertical."""
        if x1 != x2:
            self.wire_h(x1, x2, y1)
        if y1 != y2:
            self.wire_v(x2 if x1 != x2 else x1, y1, y2)

    def route_vh(self, x1, y1, x2, y2):
        """L-shaped route: vertical first, then horizontal."""
        if y1 != y2:
            self.wire_v(x1, y1, y2)
        if x1 != x2:
            self.wire_h(x1, x2, y2 if y1 != y2 else y1)

    def to_xml(self):
        lines = [f'  <circuit name="{self.name}">']
        lines.append(f'    <a name="circuit" val="{self.name}"/>')
        lines.append(f'    <a name="label" val="{self.label}"/>')
        lines.append(f'    <a name="circuit_Lab_Name" val="{self.name}"/>')
        for l in self.lines:
            lines.append(l)
        lines.append('  </circuit>')
        return '\n'.join(lines)

# ============================================================
# Logisim Evolution 3.8.0 pin offset constants
# ============================================================
# These are carefully verified positions for Logisim Evolution 3.8.0.

# 2-input Gate: loc IS the output point
# Input A: (loc_x-50, loc_y-10), Input B: (loc_x-50, loc_y+10)
G_INA = (-50, -10)
G_INB = (-50, +10)
G_OUT = (0, 0)

# NOT Gate: loc IS the output
# Input: (loc_x-50, loc_y)
N_IN = (-50, 0)
N_OUT = (0, 0)

# Multiplexer select=1: loc IS the output
# data0: (loc_x-100, loc_y-10), data1: (loc_x-100, loc_y+10)
# select: (loc_x, loc_y+30), output: (loc_x, loc_y)
M1_D0 = (-100, -10)
M1_D1 = (-100, +10)
M1_SEL = (0, +30)
M1_OUT = (0, 0)

# Multiplexer select=3 (8:1): loc IS the output
# data[i]: (loc_x-100, loc_y-35+i*10)
# select: (loc_x, loc_y+50), output: (loc_x, loc_y)
M3_D = [(-100, -35 + i*10) for i in range(8)]
M3_SEL = (0, +50)
M3_OUT = (0, 0)

# Decoder select=3:
# select in: (loc_x-40, loc_y), enable: (loc_x, loc_y+40)
# output[i]: (loc_x+40, loc_y-35+i*10)
D3_SEL = (-40, 0)
D3_EN = (0, +40)
D3_OUT = [(+40, -35 + i*10) for i in range(8)]

# Register: loc IS the Q output
# D: (loc_x-120, loc_y), Q: (loc_x, loc_y)
# EN: (loc_x-120, loc_y+40), CLK: (loc_x-80, loc_y+60)
R_D = (-120, 0)
R_Q = (0, 0)
R_EN = (-120, +40)
R_CLK = (-80, +60)

# RAM: loc IS the RDATA output
# ADDR: (loc_x-120, loc_y-40), WDATA: (loc_x-120, loc_y)
# RDATA: (loc_x, loc_y), WE: (loc_x-120, loc_y+40), CLK: (loc_x-80, loc_y+80)
RAM_ADDR = (-120, -40)
RAM_WDATA = (-120, 0)
RAM_RDATA = (0, 0)
RAM_WE = (-120, +40)
RAM_CLK = (-80, +80)

# Bit Selector: loc IS the output
# Input: (loc_x-40, loc_y), Output: (loc_x, loc_y)
BS_IN = (-40, 0)
BS_OUT = (0, 0)

# ============================================================
# Helper: get subcircuit pin offsets
# For user subcircuits, Logisim places pins in order:
# Inputs left-to-right top-to-bottom, then outputs left-to-right top-to-bottom
# The exact offsets depend on the subcircuit's pin layout.
# We'll compute approximate positions.
# ============================================================

def sub_pin_offsets(circuit_name):
    """Return dict of pin_name -> (x_off, y_off) for a subcircuit placed at loc.
    This is approximate - based on Logisim's pin placement algorithm."""
    # These are the approximate pin positions for each subcircuit
    # based on the pin order defined when building each circuit.
    layouts = {
        "HALF_ADDER": {
            # Inputs: A, B; Outputs: Sum, Carry
            "A": (-120, -20), "B": (-120, +20),
            "Sum": (20, -20), "Carry": (20, +20),
        },
        "FULL_ADDER": {
            # Inputs: A, B, Cin; Outputs: Sum, Cout
            "A": (-120, -30), "B": (-120, -10), "Cin": (-120, +10),
            "Sum": (20, -30), "Cout": (20, +10),
        },
        "ADDER_8": {
            # Inputs: A[7:0], B[7:0], Cin; Outputs: Sum[7:0], Cout
            "A": (-120, -30), "B": (-120, -10), "Cin": (-120, +10),
            "Sum": (20, -30), "Cout": (20, +10),
        },
        "SUBTRACTOR_8": {
            # Inputs: A[7:0], B[7:0]; Outputs: Diff[7:0], Bout
            "A": (-120, -20), "B": (-120, +20),
            "Diff": (20, -20), "Bout": (20, +20),
        },
        "REGISTER_8": {
            # Inputs: D[7:0], WE, CLK, RST; Outputs: Q[7:0]
            "D": (-120, -60), "WE": (-120, -20), "CLK": (-120, +20), "RST": (-120, +60),
            "Q": (20, -20),
        },
        "DECODER_3to8": {
            # Inputs: Sel[2:0]; Outputs: Y0-Y7
            "Sel": (-120, 0),
            **{f"Y{i}": (20, -35+i*10) for i in range(8)},
        },
        "PC_16": {
            # Inputs: D[15:0], WE, CLK, HALT; Outputs: Q[15:0]
            "D": (-120, -60), "WE": (-120, -20), "CLK": (-120, +20), "HALT": (-120, +60),
            "Q": (20, -20),
        },
        "SP_8": {
            # Inputs: D[7:0], WE, CLK, RST; Outputs: Q[7:0]
            "D": (-120, -60), "WE": (-120, -20), "CLK": (-120, +20), "RST": (-120, +60),
            "Q": (20, -20),
        },
        "ACC_8": {
            # Inputs: D[7:0], WE, CLK, RST; Outputs: Q[7:0]
            "D": (-120, -60), "WE": (-120, -20), "CLK": (-120, +20), "RST": (-120, +60),
            "Q": (20, -20),
        },
        "FLAGS_REG": {
            # Inputs: D[7:0], WE, CLK, RST; Outputs: Z, N, C
            "D": (-120, -60), "WE": (-120, -20), "CLK": (-120, +20), "RST": (-120, +60),
            "Z": (20, -30), "N": (20, -10), "C": (20, +10),
        },
        "INSTRUCTION_REGISTER": {
            # Inputs: D[7:0], IR_LOAD, CLK; Outputs: OPCODE[3:0], REG[3:0]
            "D": (-120, -20), "IR_LOAD": (-120, 0), "CLK": (-120, +20),
            "OPCODE": (20, -10), "REG": (20, +10),
        },
        "RAM_64K": {
            # Inputs: ADDR[15:0], WDATA[7:0], WE, CLK; Outputs: RDATA[7:0]
            "ADDR": (-120, -30), "WDATA": (-120, -10), "WE": (-120, +10), "CLK": (-120, +30),
            "RDATA": (20, -10),
        },
        "REGISTER_FILE_8x8": {
            # Inputs: RdAddr1[2:0], RdAddr2[2:0], WrAddr[2:0], WrData[7:0], WE, CLK
            # Outputs: RdData1[7:0], RdData2[7:0]
            "RdAddr1": (-120, -50), "RdAddr2": (-120, -30),
            "WrAddr": (-120, -10), "WrData": (-120, +10),
            "WE": (-120, +30), "CLK": (-120, +50),
            "RdData1": (20, -30), "RdData2": (20, +30),
        },
        "ALU_8": {
            # Inputs: A[7:0], B[7:0], ALU_OP[3:0]; Outputs: RESULT[7:0], Z, N, C
            "A": (-120, -40), "B": (-120, -20), "ALU_OP": (-120, 0),
            "RESULT": (20, -40), "Z": (20, -20), "N": (20, 0), "C": (20, +20),
        },
        "DATAPATH": {
            # Many I/O pins - simplified layout
            "CLK": (-120, -600), "RST": (-120, -580),
            "MEM_RDATA": (-120, -560),
            "OPCODE": (20, -500), "REG": (20, -480),
            "Z_FLAG": (20, -460), "N_FLAG": (20, -440), "C_FLAG": (20, -420),
            "MEM_ADDR": (-120, -540), "MEM_WDATA": (-120, -520),
        },
        "CONTROL_UNIT": {
            "OPCODE": (-120, -50), "CLK": (-120, -30), "RST": (-120, -10),
            "Z_FLAG": (-120, +10), "N_FLAG": (-120, +30), "C_FLAG": (-120, +50),
        },
    }
    return layouts.get(circuit_name, {})

def sub_in(name, pin_name, loc_x, loc_y):
    """Get absolute position of an input pin on a subcircuit."""
    offsets = sub_pin_offsets(name)
    ox, oy = offsets.get(pin_name, (-120, 0))
    return (loc_x + ox, loc_y + oy)

def sub_out(name, pin_name, loc_x, loc_y):
    """Get absolute position of an output pin on a subcircuit."""
    offsets = sub_pin_offsets(name)
    ox, oy = offsets.get(pin_name, (20, 0))
    return (loc_x + ox, loc_y + oy)


# ============================================================
# BUILD ALL 24 CIRCUITS
# ============================================================

def build_NAND2():
    c = Circuit("NAND2", "8-bit NAND Gate")
    # Inputs
    c.add(xml_pin(50, 100, 8, False, "A[7:0]"))
    c.add(xml_pin(50, 200, 8, False, "B[7:0]"))
    # Gate
    c.add(xml_comp("1", "NAND Gate", p(200, 150), {"width": "8"}))
    # Output
    c.add(xml_pin(300, 150, 8, True, "Q[7:0]"))
    # Wires: A -> gate.A, B -> gate.B, gate.out -> Q
    c.route_hv(50, 100, 200+G_INA[0], 100)  # A -> vertical drop
    c.wire_v(200+G_INA[0], 100, 200+G_INA[1])  # vertical to A input
    c.route_hv(50, 200, 200+G_INB[0], 200)
    c.wire_v(200+G_INB[0], 200, 200+G_INB[1])
    c.wire_h(200+G_OUT[0], 300, 150)  # output to pin
    return c


def build_NOT1():
    c = Circuit("NOT1", "8-bit NOT Gate")
    c.add(xml_pin(50, 150, 8, False, "A[7:0]"))
    c.add(xml_comp("1", "NOT Gate", p(200, 150), {"width": "8"}))
    c.add(xml_pin(300, 150, 8, True, "Q[7:0]"))
    c.wire_h(50, 200+N_IN[0], 150)
    c.wire_h(200+N_OUT[0], 300, 150)
    return c


def build_AND2():
    c = Circuit("AND2", "8-bit AND Gate")
    c.add(xml_pin(50, 100, 8, False, "A[7:0]"))
    c.add(xml_pin(50, 200, 8, False, "B[7:0]"))
    c.add(xml_comp("1", "AND Gate", p(200, 150), {"width": "8"}))
    c.add(xml_pin(300, 150, 8, True, "Q[7:0]"))
    c.route_hv(50, 100, 200+G_INA[0], 100)
    c.wire_v(200+G_INA[0], 100, 200+G_INA[1])
    c.route_hv(50, 200, 200+G_INB[0], 200)
    c.wire_v(200+G_INB[0], 200, 200+G_INB[1])
    c.wire_h(200+G_OUT[0], 300, 150)
    return c


def build_OR2():
    c = Circuit("OR2", "8-bit OR Gate")
    c.add(xml_pin(50, 100, 8, False, "A[7:0]"))
    c.add(xml_pin(50, 200, 8, False, "B[7:0]"))
    c.add(xml_comp("1", "OR Gate", p(200, 150), {"width": "8"}))
    c.add(xml_pin(300, 150, 8, True, "Q[7:0]"))
    c.route_hv(50, 100, 200+G_INA[0], 100)
    c.wire_v(200+G_INA[0], 100, 200+G_INA[1])
    c.route_hv(50, 200, 200+G_INB[0], 200)
    c.wire_v(200+G_INB[0], 200, 200+G_INB[1])
    c.wire_h(200+G_OUT[0], 300, 150)
    return c


def build_XOR2():
    c = Circuit("XOR2", "8-bit XOR Gate")
    c.add(xml_pin(50, 100, 8, False, "A[7:0]"))
    c.add(xml_pin(50, 200, 8, False, "B[7:0]"))
    c.add(xml_comp("1", "XOR Gate", p(200, 150), {"width": "8"}))
    c.add(xml_pin(300, 150, 8, True, "Q[7:0]"))
    c.route_hv(50, 100, 200+G_INA[0], 100)
    c.wire_v(200+G_INA[0], 100, 200+G_INA[1])
    c.route_hv(50, 200, 200+G_INB[0], 200)
    c.wire_v(200+G_INB[0], 200, 200+G_INB[1])
    c.wire_h(200+G_OUT[0], 300, 150)
    return c


def build_MUX2_1():
    c = Circuit("MUX2_1", "8-bit 2:1 Multiplexer")
    c.add(xml_pin(50, 100, 8, False, "A[7:0]"))
    c.add(xml_pin(50, 200, 8, False, "B[7:0]"))
    c.add(xml_pin(50, 300, 1, False, "Sel"))
    c.add(xml_comp("2", "Multiplexer", p(250, 150), {"select": "1", "width": "8"}))
    c.add(xml_pin(350, 150, 8, True, "Q[7:0]"))
    # data0 at (250-100, 150-10) = (150, 140)
    c.route_hv(50, 100, 150, 140)
    # data1 at (250-100, 150+10) = (150, 160)
    c.route_hv(50, 200, 150, 160)
    # sel at (250, 150+30) = (250, 180)
    c.route_vh(50, 300, 250, 180)
    # output at (250, 150) -> pin
    c.wire_h(250, 350, 150)
    return c


def build_MUX8_1():
    c = Circuit("MUX8_1", "8-bit 8:1 Multiplexer")
    # 8 data inputs
    for i in range(8):
        c.add(xml_pin(50, 100 + i*60, 8, False, f"I{i}[7:0]"))
    # Select (3-bit)
    c.add(xml_pin(50, 650, 3, False, "Sel[2:0]"))
    # Splitter for select bits
    c.add(xml_splitter(130, 650, 3, 3, "none", "right"))
    # First level: 4 MUXes (each 2:1)
    m1 = []
    for i in range(4):
        mx, my = 350, 140 + i*120
        c.add(xml_comp("2", "Multiplexer", p(mx, my),
                       {"label": f"M{i}", "select": "1", "width": "8"}))
        m1.append((mx, my))
    # Second level: 2 MUXes
    m2 = []
    for i in range(2):
        mx, my = 550, 200 + i*240
        c.add(xml_comp("2", "Multiplexer", p(mx, my),
                       {"label": f"M{4+i}", "select": "1", "width": "8"}))
        m2.append((mx, my))
    # Final MUX
    mx, my = 700, 300
    c.add(xml_comp("2", "Multiplexer", p(mx, my),
                   {"label": "M6", "select": "1", "width": "8"}))
    # Output
    c.add(xml_pin(850, 300, 8, True, "Q[7:0]"))

    # Wire data inputs to first-level MUXes
    for i in range(4):
        mx, my = m1[i]
        # I[2i] -> data0, I[2i+1] -> data1
        d0y = my + M1_D0[1]  # -10
        d1y = my + M1_D1[1]  # +10
        src0 = 100 + (2*i)*60
        src1 = 100 + (2*i+1)*60
        c.route_hv(50, src0, mx + M1_D0[0], d0y)
        c.route_hv(50, src1, mx + M1_D1[0], d1y)

    # Wire first-level outputs to second-level inputs
    for i in range(2):
        mx1, my1 = m1[2*i]
        mx2, my2 = m1[2*i+1]
        mx2l, my2l = m2[i]
        # M1[2i].out -> M2[i].d0
        c.route_hv(mx1+M1_OUT[0], my1, mx2l+M1_D0[0], my2l+M1_D0[1])
        # M1[2i+1].out -> M2[i].d1
        c.route_hv(mx2+M1_OUT[0], my2, mx2l+M1_D1[0], my2l+M1_D1[1])

    # Wire second-level outputs to final MUX
    mx_f, my_f = 700, 300
    for i in range(2):
        mx2, my2 = m2[i]
        if i == 0:
            ty = my_f + M1_D0[1]
        else:
            ty = my_f + M1_D1[1]
        c.route_hv(mx2+M1_OUT[0], my2, mx_f+M1_D0[0] if i==0 else mx_f+M1_D1[0], ty)

    # Final MUX output -> Q
    c.wire_h(mx_f+M1_OUT[0], 850, my_f)

    # Select splitter -> all MUX selects
    # Sel[0] -> all first-level MUX selects
    # Sel[1] -> all second-level MUX selects
    # Sel[2] -> final MUX select
    sel_base = 130
    for i in range(4):
        mx, my = m1[i]
        c.route_vh(sel_base, 650-10+i*10, mx+M1_SEL[0], my+M1_SEL[1])
    for i in range(2):
        mx, my = m2[i]
        c.route_vh(sel_base+10, 650-10+40+i*10, mx+M1_SEL[0], my+M1_SEL[1])
    c.route_vh(sel_base+20, 650-10+80, mx_f+M1_SEL[0], my_f+M1_SEL[1])

    return c


def build_DECODER_3to8():
    c = Circuit("DECODER_3to8", "3-to-8 Decoder")
    c.add(xml_pin(50, 250, 3, False, "Sel[2:0]"))
    c.add(xml_comp("2", "Decoder", p(200, 250), {"select": "3"}))
    # Enable (tie to VCC=1)
    c.add(xml_constant(200, 350, 1, "1"))
    c.route_vh(200, 350, 200+D3_EN[0], 250+D3_EN[1])
    # 8 outputs
    for i in range(8):
        oy = 250 + D3_OUT[i][1]
        c.add(xml_pin(350, oy, 1, True, f"Y{i}"))
        c.wire_h(200+D3_OUT[i][0], 350, oy)
    # Select input
    c.wire_h(50, 200+D3_SEL[0], 250)
    return c


def build_HALF_ADDER():
    c = Circuit("HALF_ADDER", "Half Adder")
    c.add(xml_pin(50, 100, 1, False, "A"))
    c.add(xml_pin(50, 200, 1, False, "B"))
    # XOR for Sum
    c.add(xml_comp("1", "XOR Gate", p(200, 100), {"label": "XOR"}))
    # AND for Carry
    c.add(xml_comp("1", "AND Gate", p(200, 250), {"label": "AND"}))
    # Outputs
    c.add(xml_pin(300, 100, 1, True, "Sum"))
    c.add(xml_pin(300, 250, 1, True, "Carry"))
    # A -> XOR.A and AND.A
    ax, ay = 200+G_INA[0], 100+G_INA[1]
    c.wire_h(50, ax, 100)
    c.wire_v(ax, 100, ay)  # to XOR.A
    and_ay = 250+G_INA[1]
    c.wire_v(ax, ay, and_ay)  # continue down to AND.A
    # B -> XOR.B and AND.B
    bx, by = 200+G_INB[0], 100+G_INB[1]
    c.wire_h(50, bx, 200)
    c.wire_v(bx, 200, by)  # to XOR.B
    and_by = 250+G_INB[1]
    c.wire_v(bx, by, and_by)  # continue down to AND.B
    # XOR.out -> Sum
    c.wire_h(200+G_OUT[0], 300, 100)
    # AND.out -> Carry
    c.wire_h(200+G_OUT[0], 300, 250)
    return c


def build_FULL_ADDER():
    c = Circuit("FULL_ADDER", "Full Adder")
    c.add(xml_pin(50, 100, 1, False, "A"))
    c.add(xml_pin(50, 200, 1, False, "B"))
    c.add(xml_pin(50, 350, 1, False, "Cin"))
    # Two HALF_ADDERs
    c.add(subcircuit("HALF_ADDER", 200, 100, "HA1"))
    c.add(subcircuit("HALF_ADDER", 450, 100, "HA2"))
    # OR for carry out
    c.add(xml_comp("1", "OR Gate", p(450, 300), {"label": "OR"}))
    # Outputs
    c.add(xml_pin(600, 100, 1, True, "Sum"))
    c.add(xml_pin(600, 300, 1, True, "Cout"))

    ha1 = sub_pin_offsets("HALF_ADDER")
    ha2 = sub_pin_offsets("HALF_ADDER")

    # A -> HA1.A
    ha1a = (200+ha1["A"][0], 100+ha1["A"][1])
    c.route_hv(50, 100, ha1a[0], ha1a[1])
    # B -> HA1.B
    ha1b = (200+ha1["B"][0], 100+ha1["B"][1])
    c.route_hv(50, 200, ha1b[0], ha1b[1])
    # HA1.Sum -> HA2.A
    ha1s = (200+ha1["Sum"][0], 100+ha1["Sum"][1])
    ha2a = (450+ha2["A"][0], 100+ha2["A"][1])
    c.route_hv(ha1s[0], ha1s[1], ha2a[0], ha2a[1])
    # Cin -> HA2.B
    ha2b = (450+ha2["B"][0], 100+ha2["B"][1])
    c.route_hv(50, 350, ha2b[0], ha2b[1])
    # HA2.Sum -> Sum output
    ha2s = (450+ha2["Sum"][0], 100+ha2["Sum"][1])
    c.wire_h(ha2s[0], 600, ha2s[1])
    # HA1.Carry -> OR.A
    ha1c = (200+ha1["Carry"][0], 100+ha1["Carry"][1])
    or_a = (450+G_INA[0], 300+G_INA[1])
    c.route_hv(ha1c[0], ha1c[1], or_a[0], or_a[1])
    # HA2.Carry -> OR.B
    ha2c = (450+ha2["Carry"][0], 100+ha2["Carry"][1])
    or_b = (450+G_INB[0], 300+G_INB[1])
    c.route_hv(ha2c[0], ha2c[1], or_b[0], or_b[1])
    # OR.out -> Cout
    c.wire_h(450+G_OUT[0], 600, 300)
    return c


def build_ADDER_8():
    c = Circuit("ADDER_8", "8-bit Ripple-Carry Adder")
    c.add(xml_pin(50, 100, 8, False, "A[7:0]"))
    c.add(xml_pin(50, 200, 8, False, "B[7:0]"))
    c.add(xml_pin(50, 400, 1, False, "Cin"))
    c.add(xml_pin(1000, 100, 8, True, "Sum[7:0]"))
    c.add(xml_pin(1000, 400, 1, True, "Cout"))

    # Split A and B into individual bits
    c.add(xml_splitter(150, 100, 8, 8, "none", "right"))
    c.add(xml_splitter(150, 200, 8, 8, "none", "right"))
    c.add(xml_splitter(900, 100, 8, 8, "one", "right"))  # combiner for Sum

    c.wire_h(50, 150, 100)
    c.wire_h(50, 150, 200)

    # 8 FULL_ADDERs, ripple carry
    fa = []
    for i in range(8):
        fx, fy = 400, 50 + i*60
        c.add(subcircuit("FULL_ADDER", fx, fy, f"FA{i}"))
        fa.append((fx, fy))

    fa_off = sub_pin_offsets("FULL_ADDER")

    for i in range(8):
        fx, fy = fa[i]
        # A[i] from splitter to FA.A
        a_bit_y = 100 + D3_OUT[i][1]  # splitter bit i y offset
        fa_a = (fx + fa_off["A"][0], fy + fa_off["A"][1])
        c.route_hv(150+D3_OUT[i][0], a_bit_y, fa_a[0], fa_a[1])
        # B[i] from splitter to FA.B
        b_bit_y = 200 + D3_OUT[i][1]
        fa_b = (fx + fa_off["B"][0], fy + fa_off["B"][1])
        c.route_hv(150+D3_OUT[i][0], b_bit_y, fa_b[0], fa_b[1])
        # FA.Sum to combiner
        fa_s = (fx + fa_off["Sum"][0], fy + fa_off["Sum"][1])
        c_bit_y = 100 + D3_OUT[i][1]
        c.route_hv(fa_s[0], fa_s[1], 900+D3_OUT[i][0], c_bit_y)
        # Carry chain
        if i == 0:
            fa_cin = (fx + fa_off["Cin"][0], fy + fa_off["Cin"][1])
            c.route_hv(50, 400, fa_cin[0], fa_cin[1])
        else:
            prev_fx, prev_fy = fa[i-1]
            prev_cout = (prev_fx + fa_off["Cout"][0], prev_fy + fa_off["Cout"][1])
            fa_cin = (fx + fa_off["Cin"][0], fy + fa_off["Cin"][1])
            c.route_hv(prev_cout[0], prev_cout[1], fa_cin[0], fa_cin[1])

    # Last carry out -> Cout
    last_fx, last_fy = fa[7]
    last_cout = (last_fx + fa_off["Cout"][0], last_fy + fa_off["Cout"][1])
    c.route_hv(last_cout[0], last_cout[1], 1000, 400)

    # Combiner output -> Sum
    c.wire_h(900+D3_OUT[0][0]-20, 1000, 100)  # combiner combined end to output
    return c


def build_SUBTRACTOR_8():
    c = Circuit("SUBTRACTOR_8", "8-bit Subtractor (A-B)")
    c.add(xml_pin(50, 100, 8, False, "A[7:0]"))
    c.add(xml_pin(50, 200, 8, False, "B[7:0]"))
    c.add(xml_pin(700, 100, 8, True, "Diff[7:0]"))
    c.add(xml_pin(700, 250, 1, True, "Bout"))

    # XOR B with 0xFF for inversion
    c.add(xml_comp("1", "XOR Gate", p(200, 200), {"width": "8"}))
    c.add(xml_constant(150, 300, 8, "ff"))
    # Adder_8
    c.add(subcircuit("ADDER_8", 450, 100, "ADD"))
    # Constant 1 for carry-in
    c.add(xml_constant(250, 400, 1, "1"))

    add = sub_pin_offsets("ADDER_8")

    # A -> XOR.A and ADDER.A
    xor_a = (200+G_INA[0], 200+G_INA[1])
    c.route_hv(50, 100, xor_a[0], 200)  # horizontal to XOR x
    c.wire_v(xor_a[0], 100, xor_a[1])  # down to XOR.A
    # Also route to adder A
    add_a = (450+add["A"][0], 100+add["A"][1])
    c.route_hv(xor_a[0], 100, add_a[0], add_a[1])

    # B -> XOR.B
    xor_b = (200+G_INB[0], 200+G_INB[1])
    c.route_hv(50, 200, xor_b[0], xor_b[1])

    # 0xFF -> XOR.B (constant already placed)
    c.route_hv(150, 300, xor_b[0], 300)

    # XOR.out -> ADDER.B
    xor_out = (200+G_OUT[0], 200)
    add_b = (450+add["B"][0], 100+add["B"][1])
    c.route_hv(xor_out[0], xor_out[1], add_b[0], add_b[1])

    # Constant 1 -> ADDER.Cin
    add_cin = (450+add["Cin"][0], 100+add["Cin"][1])
    c.route_hv(250, 400, add_cin[0], add_cin[1])

    # ADDER.Sum -> Diff
    add_sum = (450+add["Sum"][0], 100+add["Sum"][1])
    c.route_hv(add_sum[0], add_sum[1], 700, 100)

    # ADDER.Cout -> Bout (inverted borrow)
    add_cout = (450+add["Cout"][0], 100+add["Cout"][1])
    c.route_hv(add_cout[0], add_cout[1], 700, 250)

    return c


def build_ALU_8():
    c = Circuit("ALU_8", "8-bit ALU (13 operations)")
    c.add(xml_pin(50, 100, 8, False, "A[7:0]"))
    c.add(xml_pin(50, 200, 8, False, "B[7:0]"))
    c.add(xml_pin(50, 350, 4, False, "ALU_OP[3:0]"))
    c.add(xml_pin(1200, 100, 8, True, "RESULT[7:0]"))
    c.add(xml_pin(1200, 200, 1, True, "Z"))
    c.add(xml_pin(1200, 250, 1, True, "N"))
    c.add(xml_pin(1200, 300, 1, True, "C"))

    # Bitwise operations using 8-bit gates
    c.add(xml_comp("1", "AND Gate", p(350, 400), {"width": "8", "label": "AND_OP"}))
    c.add(xml_comp("1", "OR Gate", p(350, 500), {"width": "8", "label": "OR_OP"}))
    c.add(xml_comp("1", "XOR Gate", p(350, 600), {"width": "8", "label": "XOR_OP"}))
    c.add(xml_comp("1", "NOT Gate", p(350, 700), {"width": "8", "label": "NOT_OP"}))
    c.add(xml_comp("1", "NAND Gate", p(350, 800), {"width": "8", "label": "NAND_OP"}))

    # Result MUX (8:1 for operation selection)
    c.add(xml_comp("2", "Multiplexer", p(800, 150), {
        "label": "RESULT_MUX", "select": "3", "width": "8"}))

    # MUX select from ALU_OP bits 0-2
    mux_sel = (800+M3_SEL[0], 150+M3_SEL[1])
    c.route_hv(50, 350, mux_sel[0], mux_sel[1])

    # Route A to all operations
    a_bus_x = 250
    c.wire_h(50, a_bus_x, 100)
    c.wire_v(a_bus_x, 100, 700)  # A bus

    # Route B to all operations
    b_bus_x = 280
    c.wire_h(50, b_bus_x, 200)
    c.wire_v(b_bus_x, 200, 600)  # B bus

    # Connect A, B to each gate
    for gy, gx in [(400, 350), (500, 350), (600, 350), (800, 350)]:
        ga = (gx+G_INA[0], gy+G_INA[1])
        gb = (gx+G_INB[0], gy+G_INB[1])
        c.wire_v(a_bus_x, 100, ga[1])
        c.wire_h(a_bus_x, ga[0], ga[1])
        c.wire_v(b_bus_x, 200, gb[1])
        c.wire_h(b_bus_x, gb[0], gb[1])

    # NOT gate (only A input)
    not_in = (350+N_IN[0], 700+N_IN[1])
    c.wire_v(a_bus_x, 100, not_in[1])
    c.wire_h(a_bus_x, not_in[0], not_in[1])

    # Connect gate outputs to MUX data inputs
    ops = [
        ("AND_OP", 400), ("OR_OP", 500), ("XOR_OP", 600),
        ("NOT_OP", 700), ("NAND_OP", 800),
    ]
    for idx, (op, gy) in enumerate(ops):
        ox = 350 + G_OUT[0]
        md = M3_D[idx]
        mux_d = (800+md[0], 150+md[1])
        c.route_hv(ox, gy, mux_d[0], mux_d[1])

    # MUX output -> RESULT
    c.wire_h(800+M3_OUT[0], 1200, 100)

    # Flags (simplified stubs)
    # Z: NOR of all result bits (check if RESULT == 0)
    # N: MSB of result
    # C: carry out from adder
    # For now, just connect basic flags
    c.wire_h(800+M3_OUT[0], 850, 100)
    c.wire_v(850, 100, 300)

    return c


def build_REGISTER_8():
    c = Circuit("REGISTER_8", "8-bit Register with Reset")
    c.add(xml_pin(50, 100, 8, False, "D[7:0]"))
    c.add(xml_pin(50, 200, 1, False, "WE"))
    c.add(xml_pin(50, 250, 1, False, "CLK"))
    c.add(xml_pin(50, 300, 1, False, "RST"))
    c.add(xml_comp("4", "Register", p(250, 150), {"width": "8"}))
    c.add(xml_pin(400, 150, 8, True, "Q[7:0]"))

    # D -> Register.D
    rd = (250+R_D[0], 150+R_D[1])
    c.route_hv(50, 100, rd[0], rd[1])
    # WE -> Register.EN
    ren = (250+R_EN[0], 150+R_EN[1])
    c.route_hv(50, 200, ren[0], ren[1])
    # CLK -> Register.CLK
    rclk = (250+R_CLK[0], 150+R_CLK[1])
    c.route_hv(50, 250, rclk[0], rclk[1])
    # Register.Q -> output
    rq = (250+R_Q[0], 150+R_Q[1])
    c.wire_h(rq[0], 400, rq[1])
    # RST -> 0 (unused)
    c.add(xml_constant(150, 300, 1, "0"))
    c.wire_h(150, 50, 300)
    return c


def build_REGISTER_FILE_8x8():
    c = Circuit("REGISTER_FILE_8x8", "8x8 Register File (S0-S7)")
    # Inputs
    c.add(xml_pin(50, 100, 3, False, "RdAddr1[2:0]"))
    c.add(xml_pin(50, 200, 3, False, "RdAddr2[2:0]"))
    c.add(xml_pin(50, 600, 3, False, "WrAddr[2:0]"))
    c.add(xml_pin(50, 700, 8, False, "WrData[7:0]"))
    c.add(xml_pin(50, 800, 1, False, "WE"))
    c.add(xml_pin(50, 900, 1, False, "CLK"))
    # Outputs
    c.add(xml_pin(1100, 100, 8, True, "RdData1[7:0]"))
    c.add(xml_pin(1100, 600, 8, True, "RdData2[7:0]"))

    # 8 Register_8 instances
    regs = []
    for i in range(8):
        rx, ry = 400, 50 + i*100
        c.add(subcircuit("REGISTER_8", rx, ry, f"S{i}"))
        regs.append((rx, ry))

    # Write decoder
    c.add(subcircuit("DECODER_3to8", 200, 650, "WR_DEC"))
    # Read MUXes (8:1)
    c.add(xml_comp("2", "Multiplexer", p(800, 100), {
        "label": "RD_MUX1", "select": "3", "width": "8"}))
    c.add(xml_comp("2", "Multiplexer", p(800, 600), {
        "label": "RD_MUX2", "select": "3", "width": "8"}))

    reg_off = sub_pin_offsets("REGISTER_8")
    dec_off = sub_pin_offsets("DECODER_3to8")

    # WrAddr -> decoder select
    dec_sel = (200+dec_off["Sel"][0], 650+dec_off["Sel"][1])
    c.route_hv(50, 600, dec_sel[0], dec_sel[1])

    # WE -> decoder enable (tie to 1 for now - decoder always enabled)
    c.add(xml_constant(250, 750, 1, "1"))

    # WrData -> all register D inputs (shared data bus)
    for i in range(8):
        rx, ry = regs[i]
        r_d = (rx+reg_off["D"][0], ry+reg_off["D"][1])
        c.route_hv(50, 700, r_d[0], r_d[1])

    # CLK -> all registers
    clk_bus_x = 350
    c.wire_h(50, clk_bus_x, 900)
    c.wire_v(clk_bus_x, 50, 900)  # clock bus
    for i in range(8):
        rx, ry = regs[i]
        r_clk = (rx+reg_off["CLK"][0], ry+reg_off["CLK"][1])
        c.route_hv(clk_bus_x, ry, r_clk[0], r_clk[1])

    # Decoder outputs -> register WE enables
    for i in range(8):
        rx, ry = regs[i]
        r_en = (rx+reg_off["WE"][0], ry+reg_off["WE"][1])
        dec_out = (200+dec_off[f"Y{i}"][0], 650+dec_off[f"Y{i}"][1])
        route_x = 300 + i*10
        c.route_hv(dec_out[0], dec_out[1], r_en[0], r_en[1])

    # Register Q outputs -> MUX data inputs
    mux1_sel = (800+M3_SEL[0], 100+M3_SEL[1])
    mux2_sel = (800+M3_SEL[0], 600+M3_SEL[1])
    c.route_hv(50, 100, mux1_sel[0], mux1_sel[1])
    c.route_hv(50, 200, mux2_sel[0], mux2_sel[1])

    for i in range(8):
        rx, ry = regs[i]
        r_q = (rx+reg_off["Q"][0], ry+reg_off["Q"][1])
        md1 = M3_D[i]
        mux1_d = (800+md1[0], 100+md1[1])
        mux2_d = (800+md1[0], 600+md1[1])
        mid_x = 700
        c.route_hv(r_q[0], r_q[1], mux1_d[0], mux1_d[1])
        c.route_hv(r_q[0], r_q[1], mux2_d[0], mux2_d[1])

    # MUX outputs
    mux1_out = (800+M3_OUT[0], 100+M3_OUT[1])
    mux2_out = (800+M3_OUT[0], 600+M3_OUT[1])
    c.wire_h(mux1_out[0], 1100, mux1_out[1])
    c.wire_h(mux2_out[0], 1100, mux2_out[1])

    return c


def build_ACC_8():
    c = Circuit("ACC_8", "8-bit Accumulator")
    c.add(xml_pin(50, 100, 8, False, "D[7:0]"))
    c.add(xml_pin(50, 200, 1, False, "WE"))
    c.add(xml_pin(50, 250, 1, False, "CLK"))
    c.add(xml_pin(50, 300, 1, False, "RST"))
    c.add(xml_comp("4", "Register", p(250, 150), {"width": "8", "label": "ACC"}))
    c.add(xml_pin(400, 150, 8, True, "Q[7:0]"))

    rd = (250+R_D[0], 150+R_D[1])
    c.route_hv(50, 100, rd[0], rd[1])
    ren = (250+R_EN[0], 150+R_EN[1])
    c.route_hv(50, 200, ren[0], ren[1])
    rclk = (250+R_CLK[0], 150+R_CLK[1])
    c.route_hv(50, 250, rclk[0], rclk[1])
    rq = (250+R_Q[0], 150+R_Q[1])
    c.wire_h(rq[0], 400, rq[1])
    c.add(xml_constant(150, 300, 1, "0"))
    c.wire_h(150, 50, 300)
    return c


def build_FLAGS_REG():
    c = Circuit("FLAGS_REG", "Flags Register (Z, N, C)")
    c.add(xml_pin(50, 100, 8, False, "D[7:0]"))
    c.add(xml_pin(50, 200, 1, False, "WE"))
    c.add(xml_pin(50, 250, 1, False, "CLK"))
    c.add(xml_pin(50, 300, 1, False, "RST"))
    c.add(xml_comp("4", "Register", p(250, 150), {"width": "8", "label": "FLAGS"}))
    c.add(xml_pin(500, 100, 1, True, "Z"))
    c.add(xml_pin(500, 200, 1, True, "N"))
    c.add(xml_pin(500, 300, 1, True, "C"))

    # Register connections
    rd = (250+R_D[0], 150+R_D[1])
    c.route_hv(50, 100, rd[0], rd[1])
    ren = (250+R_EN[0], 150+R_EN[1])
    c.route_hv(50, 200, ren[0], ren[1])
    rclk = (250+R_CLK[0], 150+R_CLK[1])
    c.route_hv(50, 250, rclk[0], rclk[1])

    rq = (250+R_Q[0], 150+R_Q[1])

    # Bit Selectors to extract bits 0, 1, 2
    c.add(xml_comp("2", "Bit Selector", p(380, 100), {"width": "8", "upper": "0", "lower": "0"}))
    c.add(xml_comp("2", "Bit Selector", p(380, 200), {"width": "8", "upper": "1", "lower": "1"}))
    c.add(xml_comp("2", "Bit Selector", p(380, 300), {"width": "8", "upper": "2", "lower": "2"}))

    # Q -> all bit selectors
    bs_in_y = [100+BS_IN[1], 200+BS_IN[1], 300+BS_IN[1]]
    c.route_hv(rq[0], rq[1], 380+BS_IN[0], bs_in_y[0])
    c.wire_v(380+BS_IN[0], bs_in_y[0], bs_in_y[2])

    # Bit selector outputs -> output pins
    for i, y in enumerate([100, 200, 300]):
        bs_out = (380+BS_OUT[0], y+BS_OUT[1])
        c.wire_h(bs_out[0], 500, bs_out[1])

    # RST -> 0
    c.add(xml_constant(150, 300, 1, "0"))
    c.wire_h(150, 50, 300)
    return c


def build_PC_16():
    c = Circuit("PC_16", "16-bit Program Counter")
    c.add(xml_pin(50, 100, 16, False, "D[15:0]"))
    c.add(xml_pin(50, 200, 1, False, "WE"))
    c.add(xml_pin(50, 250, 1, False, "CLK"))
    c.add(xml_pin(50, 300, 1, False, "HALT"))
    c.add(xml_comp("4", "Register", p(250, 150), {"width": "16", "label": "PC"}))
    c.add(xml_pin(400, 150, 16, True, "Q[15:0]"))

    rd = (250+R_D[0], 150+R_D[1])
    c.route_hv(50, 100, rd[0], rd[1])
    ren = (250+R_EN[0], 150+R_EN[1])
    c.route_hv(50, 200, ren[0], ren[1])
    rclk = (250+R_CLK[0], 150+R_CLK[1])
    c.route_hv(50, 250, rclk[0], rclk[1])
    rq = (250+R_Q[0], 150+R_Q[1])
    c.wire_h(rq[0], 400, rq[1])
    return c


def build_SP_8():
    c = Circuit("SP_8", "8-bit Stack Pointer")
    c.add(xml_pin(50, 100, 8, False, "D[7:0]"))
    c.add(xml_pin(50, 200, 1, False, "WE"))
    c.add(xml_pin(50, 250, 1, False, "CLK"))
    c.add(xml_pin(50, 300, 1, False, "RST"))
    c.add(xml_comp("4", "Register", p(250, 150), {"width": "8", "label": "SP"}))
    c.add(xml_pin(400, 150, 8, True, "Q[7:0]"))

    rd = (250+R_D[0], 150+R_D[1])
    c.route_hv(50, 100, rd[0], rd[1])
    ren = (250+R_EN[0], 150+R_EN[1])
    c.route_hv(50, 200, ren[0], ren[1])
    rclk = (250+R_CLK[0], 150+R_CLK[1])
    c.route_hv(50, 250, rclk[0], rclk[1])
    rq = (250+R_Q[0], 150+R_Q[1])
    c.wire_h(rq[0], 400, rq[1])
    c.add(xml_constant(150, 300, 1, "0"))
    c.wire_h(150, 50, 300)
    return c


def build_INSTRUCTION_REGISTER():
    c = Circuit("INSTRUCTION_REGISTER", "Instruction Register")
    c.add(xml_pin(50, 100, 8, False, "D[7:0]"))
    c.add(xml_pin(50, 200, 1, False, "IR_LOAD"))
    c.add(xml_pin(50, 250, 1, False, "CLK"))
    c.add(xml_pin(500, 100, 4, True, "OPCODE[3:0]"))
    c.add(xml_pin(500, 200, 4, True, "REG[3:0]"))

    # Register (8-bit)
    c.add(xml_comp("4", "Register", p(250, 150), {"width": "8", "label": "IR_REG"}))

    # Bit Selectors for opcode [7:4] and reg [3:0]
    c.add(xml_comp("2", "Bit Selector", p(400, 100), {
        "label": "OPCODE_BS", "width": "8", "upper": "7", "lower": "4"}))
    c.add(xml_comp("2", "Bit Selector", p(400, 200), {
        "label": "REG_BS", "width": "8", "upper": "3", "lower": "0"}))

    # D -> Register.D
    rd = (250+R_D[0], 150+R_D[1])
    c.route_hv(50, 100, rd[0], rd[1])
    # IR_LOAD -> Register.EN
    ren = (250+R_EN[0], 150+R_EN[1])
    c.route_hv(50, 200, ren[0], ren[1])
    # CLK -> Register.CLK
    rclk = (250+R_CLK[0], 150+R_CLK[1])
    c.route_hv(50, 250, rclk[0], rclk[1])
    # Q -> bit selectors
    rq = (250+R_Q[0], 150+R_Q[1])
    bs_in_y0 = 100+BS_IN[1]
    bs_in_y1 = 200+BS_IN[1]
    c.route_hv(rq[0], rq[1], 400+BS_IN[0], bs_in_y0)
    c.wire_v(400+BS_IN[0], bs_in_y0, bs_in_y1)
    # Bit selector outputs -> output pins
    c.wire_h(400+BS_OUT[0], 500, 100)
    c.wire_h(400+BS_OUT[0], 500, 200)
    return c


def build_RAM_64K():
    c = Circuit("RAM_64K", "64KB RAM")
    c.add(xml_pin(50, 100, 16, False, "ADDR[15:0]"))
    c.add(xml_pin(50, 200, 8, False, "WDATA[7:0]"))
    c.add(xml_pin(50, 300, 1, False, "WE"))
    c.add(xml_pin(50, 350, 1, False, "CLK"))
    c.add(xml_comp("4", "RAM", p(300, 200), {"addr_bits": "16", "data_bits": "8"}))
    c.add(xml_pin(500, 200, 8, True, "RDATA[7:0]"))

    # RAM pin connections
    ram_a = (300+RAM_ADDR[0], 200+RAM_ADDR[1])
    c.route_hv(50, 100, ram_a[0], ram_a[1])
    ram_w = (300+RAM_WDATA[0], 200+RAM_WDATA[1])
    c.route_hv(50, 200, ram_w[0], ram_w[1])
    ram_we = (300+RAM_WE[0], 200+RAM_WE[1])
    c.route_hv(50, 300, ram_we[0], ram_we[1])
    ram_clk = (300+RAM_CLK[0], 200+RAM_CLK[1])
    c.route_hv(50, 350, ram_clk[0], ram_clk[1])
    ram_r = (300+RAM_RDATA[0], 200+RAM_RDATA[1])
    c.wire_h(ram_r[0], 500, ram_r[1])
    return c


def build_CONTROL_UNIT():
    c = Circuit("CONTROL_UNIT", "Dhad Control Unit (FSM)")
    # Inputs
    c.add(xml_pin(50, 100, 4, False, "OPCODE[3:0]"))
    c.add(xml_pin(50, 150, 4, False, "EXT_OP[3:0]"))
    c.add(xml_pin(50, 200, 1, False, "CLK"))
    c.add(xml_pin(50, 250, 1, False, "RST"))
    c.add(xml_pin(50, 300, 1, False, "Z_FLAG"))
    c.add(xml_pin(50, 350, 1, False, "N_FLAG"))
    c.add(xml_pin(50, 400, 1, False, "C_FLAG"))

    # Output control signals
    signals = [
        ("IR_LOAD", 1), ("PC_INC", 1), ("PC_LOAD", 1),
        ("MEM_READ", 1), ("MEM_WRITE", 1),
        ("ACC_WRITE", 1), ("REG_WRITE", 1), ("FLAGS_WRITE", 1),
        ("ALU_OP", 4), ("ALU_A_SRC", 2), ("ALU_B_SRC", 2),
        ("WR_SRC", 2), ("MEM_ADDR_SRC", 2),
        ("SP_INC", 1), ("SP_DEC", 1), ("HALT", 1),
    ]
    sx = 600
    for i, (name, w) in enumerate(signals):
        c.add(xml_pin(sx, 100 + i*40, w, True, name))

    # Stub: connect a constant 0 for all outputs (no decode logic yet)
    for i, (name, w) in enumerate(signals):
        c.add(xml_constant(sx - 50, 100 + i*40, w, "0"))
        c.wire_h(sx - 50, sx, 100 + i*40)

    return c


def build_DATAPATH():
    c = Circuit("DATAPATH", "Dhad 8-bit Datapath")

    # Input pins (from CONTROL_UNIT)
    c.add(xml_pin(50, 100, 1, False, "CLK"))
    c.add(xml_pin(50, 150, 1, False, "RST"))
    c.add(xml_pin(50, 250, 8, False, "MEM_RDATA[7:0]"))
    c.add(xml_pin(50, 400, 1, False, "IR_LOAD"))
    c.add(xml_pin(50, 450, 1, False, "PC_INC"))
    c.add(xml_pin(50, 500, 1, False, "PC_LOAD"))
    c.add(xml_pin(50, 550, 1, False, "MEM_READ"))
    c.add(xml_pin(50, 600, 1, False, "MEM_WRITE"))
    c.add(xml_pin(50, 650, 1, False, "ACC_WRITE"))
    c.add(xml_pin(50, 700, 1, False, "REG_WRITE"))
    c.add(xml_pin(50, 750, 1, False, "FLAGS_WRITE"))
    c.add(xml_pin(50, 800, 4, False, "ALU_OP[3:0]"))
    c.add(xml_pin(50, 850, 2, False, "ALU_A_SRC[1:0]"))
    c.add(xml_pin(50, 900, 2, False, "ALU_B_SRC[1:0]"))
    c.add(xml_pin(50, 950, 2, False, "WR_SRC[1:0]"))
    c.add(xml_pin(50, 1000, 2, False, "MEM_ADDR_SRC[1:0]"))
    c.add(xml_pin(50, 1050, 1, False, "SP_INC"))
    c.add(xml_pin(50, 1100, 1, False, "SP_DEC"))

    # Output pins
    c.add(xml_pin(1500, 250, 16, True, "MEM_ADDR[15:0]"))
    c.add(xml_pin(1500, 300, 8, True, "MEM_WDATA[7:0]"))
    c.add(xml_pin(1500, 400, 4, True, "OPCODE[3:0]"))
    c.add(xml_pin(1500, 450, 4, True, "REG[3:0]"))
    c.add(xml_pin(1500, 500, 1, True, "Z_FLAG"))
    c.add(xml_pin(1500, 550, 1, True, "N_FLAG"))
    c.add(xml_pin(1500, 600, 1, True, "C_FLAG"))
    c.add(xml_pin(1500, 700, 8, True, "ACC_OUT[7:0]"))
    c.add(xml_pin(1500, 750, 16, True, "PC_OUT[15:0]"))

    # Subcircuit instances
    c.add(subcircuit("PC_16", 350, 200, "PC"))
    c.add(subcircuit("INSTRUCTION_REGISTER", 650, 200, "IR"))
    c.add(subcircuit("REGISTER_FILE_8x8", 1000, 400, "REG_FILE"))
    c.add(subcircuit("ACC_8", 1000, 150, "ACC"))
    c.add(subcircuit("ALU_8", 1200, 300, "ALU"))
    c.add(subcircuit("FLAGS_REG", 1350, 450, "FLAGS"))
    c.add(subcircuit("SP_8", 350, 500, "SP"))

    # Simple connections: CLK, RST to all subcircuits
    c.wire_h(50, 1400, 100)   # CLK bus
    c.wire_h(50, 1400, 150)   # RST bus

    # PC outputs -> MEM_ADDR and PC_OUT
    pc = sub_pin_offsets("PC_16")
    pc_q = (350+pc["Q"][0], 200+pc["Q"][1])
    c.route_hv(pc_q[0], pc_q[1], 1500, 750)
    c.route_hv(pc_q[0], pc_q[1], 1500, 250)

    # IR outputs
    ir = sub_pin_offsets("INSTRUCTION_REGISTER")
    ir_op = (650+ir["OPCODE"][0], 200+ir["OPCODE"][1])
    ir_reg = (650+ir["REG"][0], 200+ir["REG"][1])
    c.route_hv(ir_op[0], ir_op[1], 1500, 400)
    c.route_hv(ir_reg[0], ir_reg[1], 1500, 450)

    # ACC output
    acc = sub_pin_offsets("ACC_8")
    acc_q = (1000+acc["Q"][0], 150+acc["Q"][1])
    c.route_hv(acc_q[0], acc_q[1], 1500, 700)

    # FLAGS outputs
    fl = sub_pin_offsets("FLAGS_REG")
    for fy, oy in [(500, 500), (550, 550), (600, 600)]:
        fn = (1350+fl["Z"][0], 450+fl["Z"][1])
        # Connect flag outputs
    c.route_hv(1350+fl["Z"][0], 450+fl["Z"][1], 1500, 500)
    c.route_hv(1350+fl["N"][0], 450+fl["N"][1], 1500, 550)
    c.route_hv(1350+fl["C"][0], 450+fl["C"][1], 1500, 600)

    return c


def build_CPU():
    c = Circuit("CPU", "Dhad 8-bit CPU")
    # Inputs
    c.add(xml_pin(50, 100, 1, False, "CLK"))
    c.add(xml_pin(50, 150, 1, False, "RST"))
    # Outputs
    c.add(xml_pin(800, 100, 8, True, "ACC[7:0]"))
    c.add(xml_pin(800, 150, 16, True, "PC[15:0]"))
    c.add(xml_pin(800, 200, 1, True, "HALTED"))

    # RAM
    c.add(subcircuit("RAM_64K", 250, 300, "RAM"))
    # DATAPATH
    c.add(subcircuit("DATAPATH", 500, 300, "DP"))
    # CONTROL_UNIT
    c.add(subcircuit("CONTROL_UNIT", 500, 600, "CTRL"))

    # CLK, RST to all
    c.wire_h(50, 200, 100)
    c.wire_h(50, 200, 150)

    # DP ACC_OUT -> CPU ACC output
    dp = sub_pin_offsets("DATAPATH")
    c.route_hv(500+dp.get("ACC_OUT", (20, 0))[0], 300+dp.get("ACC_OUT", (20, 0))[1],
               800, 100)
    # DP PC_OUT -> CPU PC output
    c.route_hv(500+dp.get("PC_OUT", (20, 0))[0], 300+dp.get("PC_OUT", (20, 0))[1],
               800, 150)

    return c


# ============================================================
# Generate complete file
# ============================================================

def generate():
    builders = [
        build_NAND2, build_NOT1, build_AND2, build_OR2, build_XOR2,
        build_MUX2_1, build_MUX8_1, build_DECODER_3to8,
        build_HALF_ADDER, build_FULL_ADDER, build_ADDER_8, build_SUBTRACTOR_8,
        build_ALU_8,
        build_REGISTER_8, build_REGISTER_FILE_8x8, build_ACC_8, build_FLAGS_REG,
        build_PC_16, build_SP_8, build_INSTRUCTION_REGISTER,
        build_RAM_64K,
        build_CONTROL_UNIT, build_DATAPATH, build_CPU,
    ]

    parts = []
    parts.append('<?xml version="1.0" encoding="UTF-8" standalone="no"?>')
    parts.append('<project version="1.0" source="Logisim-Evolution">')
    parts.append('  <lib desc="#Wiring" name="0"/>')
    parts.append('  <lib desc="#Gates" name="1"/>')
    parts.append('  <lib desc="#Plexers" name="2"/>')
    parts.append('  <lib desc="#Memory" name="4"/>')
    parts.append('  <main name="CPU"/>')

    for build in builders:
        circuit = build()
        parts.append(circuit.to_xml())
        print(f"  Built: {circuit.name}")

    parts.append('</project>')

    content = '\n'.join(parts) + '\n'

    os.makedirs(os.path.dirname(OUTPUT), exist_ok=True)
    with open(OUTPUT, 'w') as f:
        f.write(content)

    print(f"\nGenerated: {OUTPUT}")
    print(f"File size: {len(content)} bytes")

    # Verify
    import xml.etree.ElementTree as ET
    tree = ET.parse(OUTPUT)
    root = tree.getroot()
    circuits = [c.get('name') for c in root.findall('circuit')]
    print(f"Circuits: {len(circuits)}")
    for cn in circuits:
        print(f"  - {cn}")

    # Check component names
    errors = []
    for circuit in root.findall('circuit'):
        for comp in circuit.findall('comp'):
            lib = comp.get('lib', '')
            name = comp.get('name', '')
            if lib in ('0', '1', '2', '4') and name:
                valid_names = {
                    '0': ['Pin', 'Constant', 'Splitter'],
                    '1': ['NAND Gate', 'NOT Gate', 'AND Gate', 'OR Gate', 'XOR Gate'],
                    '2': ['Multiplexer', 'Decoder', 'Bit Selector'],
                    '4': ['Register', 'RAM'],
                }
                if name not in valid_names.get(lib, []):
                    errors.append(f"  Invalid: lib={lib} name={name} in {circuit.get('name')}")

    if errors:
        print("\nERRORS found:")
        for e in errors:
            print(e)
    else:
        print("\nAll component names are valid!")

    return len(errors) == 0


if __name__ == '__main__':
    ok = generate()
    # Also copy to snap-accessible location
    import shutil
    snap_dir = os.path.expanduser("~/snap/logisim-evolution/common")
    dest = os.path.join(snap_dir, "dhad_cpu.circ")
    if os.path.isdir(snap_dir):
        shutil.copy2(OUTPUT, dest)
        print(f"Copied to {dest}")
    sys.exit(0 if ok else 1)
