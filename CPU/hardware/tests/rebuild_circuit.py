#!/usr/bin/env python3
"""Rebuild dhad_cpu.circ from scratch with clean wiring."""
import xml.etree.ElementTree as ET

def pin(parent, x, y, w=1, out=False, label=""):
    c = ET.SubElement(parent, 'comp')
    c.set('lib', '0')
    c.set('loc', f'({x},{y})')
    c.set('name', 'Pin')
    a = ET.SubElement(c, 'a')
    a.set('name', 'width'); a.set('val', str(w))
    a = ET.SubElement(c, 'a')
    a.set('name', 'output'); a.set('val', 'true' if out else 'false')
    a = ET.SubElement(c, 'a')
    a.set('name', 'label'); a.set('val', label)

def gate(parent, lib, name, x, y, attrs=None, label=""):
    c = ET.SubElement(parent, 'comp')
    c.set('lib', str(lib))
    c.set('loc', f'({x},{y})')
    c.set('name', name)
    if label:
        a = ET.SubElement(c, 'a')
        a.set('name', 'label'); a.set('val', label)
    if attrs:
        for k, v in attrs.items():
            a = ET.SubElement(c, 'a')
            a.set('name', k); a.set('val', str(v))

def wire(parent, x1, y1, x2, y2):
    w = ET.SubElement(parent, 'wire')
    w.set('from', f'({x1},{y1})')
    w.set('to', f'({x2},{y2})')

def circuit(root, name, label=None, lab_name=None):
    c = ET.SubElement(root, 'circuit')
    c.set('name', name)
    a = ET.SubElement(c, 'a'); a.set('name', 'circuit'); a.set('val', name)
    a = ET.SubElement(c, 'a'); a.set('name', 'label'); a.set('val', label or name)
    if lab_name:
        a = ET.SubElement(c, 'a'); a.set('name', 'circuit_Lab_Name'); a.set('val', lab_name)
    return c

root = ET.Element('project')
root.set('version', '1.0')
a = ET.SubElement(root, 'a'); a.set('name', 'source'); a.set('val', 'Dhad 8-bit CPU')
lib = ET.SubElement(root, 'lib'); lib.set('desc', 'User Library'); lib.set('name', '7')
a = ET.SubElement(lib, 'a'); a.set('name', 'desc'); a.set('val', 'User Library')

# ============================================================
# 1. NAND2
# ============================================================
c = circuit(root, 'NAND2', 'NAND2', 'NAND Gate')
pin(c, 100, 100, 1, False, 'A')
pin(c, 100, 200, 1, False, 'B')
pin(c, 320, 150, 1, True, 'Q')
gate(c, 6, 'Nand', 200, 150)
wire(c, 100, 100, 170, 100)
wire(c, 170, 100, 170, 130)
wire(c, 100, 200, 170, 200)
wire(c, 170, 200, 170, 170)
wire(c, 230, 150, 320, 150)

# ============================================================
# 2. NOT1
# ============================================================
c = circuit(root, 'NOT1', 'NOT1', 'NOT Gate')
pin(c, 100, 150, 1, False, 'A')
pin(c, 320, 150, 1, True, 'Q')
gate(c, 6, 'Not', 200, 150)
wire(c, 100, 150, 170, 150)
wire(c, 230, 150, 320, 150)

# ============================================================
# 3. AND2
# ============================================================
c = circuit(root, 'AND2', 'AND2', 'AND Gate')
pin(c, 100, 100, 1, False, 'A')
pin(c, 100, 200, 1, False, 'B')
pin(c, 320, 150, 1, True, 'Q')
gate(c, 6, 'And', 200, 150)
wire(c, 100, 100, 170, 100)
wire(c, 170, 100, 170, 130)
wire(c, 100, 200, 170, 200)
wire(c, 170, 200, 170, 170)
wire(c, 230, 150, 320, 150)

# ============================================================
# 4. OR2
# ============================================================
c = circuit(root, 'OR2', 'OR2', 'OR Gate')
pin(c, 100, 100, 1, False, 'A')
pin(c, 100, 200, 1, False, 'B')
pin(c, 320, 150, 1, True, 'Q')
gate(c, 6, 'Or', 200, 150)
wire(c, 100, 100, 170, 100)
wire(c, 170, 100, 170, 130)
wire(c, 100, 200, 170, 200)
wire(c, 170, 200, 170, 170)
wire(c, 230, 150, 320, 150)

# ============================================================
# 5. XOR2
# ============================================================
c = circuit(root, 'XOR2', 'XOR2', 'XOR Gate')
pin(c, 100, 100, 1, False, 'A')
pin(c, 100, 200, 1, False, 'B')
pin(c, 320, 150, 1, True, 'Q')
gate(c, 6, 'Xor', 200, 150)
wire(c, 100, 100, 170, 100)
wire(c, 170, 100, 170, 130)
wire(c, 100, 200, 170, 200)
wire(c, 170, 200, 170, 170)
wire(c, 230, 150, 320, 150)

# ============================================================
# 6. MUX2_1
# ============================================================
c = circuit(root, 'MUX2_1', 'MUX2_1', '2:1 Multiplexer')
pin(c, 60, 100, 1, False, 'I0')
pin(c, 60, 200, 1, False, 'I1')
pin(c, 60, 300, 1, False, 'Sel')
pin(c, 400, 150, 1, True, 'Q')
gate(c, 2, 'Multiplexer', 200, 150, {'select': '1', 'width': '1'})
wire(c, 60, 100, 150, 100)
wire(c, 150, 100, 150, 130)
wire(c, 60, 200, 150, 200)
wire(c, 150, 200, 150, 170)
wire(c, 60, 300, 200, 180)
wire(c, 250, 150, 400, 150)

# ============================================================
# 7. MUX8_1
# ============================================================
c = circuit(root, 'MUX8_1', 'MUX8_1', '8:1 Multiplexer')
for i in range(8):
    pin(c, 60, 100+i*80, 8, False, f'I{i}')
pin(c, 60, 800, 3, False, 'Sel[2:0]')
pin(c, 900, 400, 8, True, 'Q[7:0]')
# 7 MUX2_1 tree
for i in range(4):
    gate(c, 2, 'Multiplexer', 400, 200+i*160, {'select': '1', 'width': '8'}, f'M{i}')
for i in range(2):
    gate(c, 2, 'Multiplexer', 600, 280+i*160, {'select': '1', 'width': '8'}, f'M{4+i}')
gate(c, 2, 'Multiplexer', 800, 400, {'select': '1', 'width': '8'}, 'M6')
# Wire inputs to first-level MUXes
for i in range(4):
    y_in = 100+i*160
    wire(c, 60, y_in, 350, y_in)
    wire(c, 60, y_in+80, 350, y_in+80)
    wire(c, 450, 200+i*160, 550, 280+(i//2)*160)
    wire(c, 60, 800, 400, 230+i*160)
# Wire second-level to final
wire(c, 650, 280, 750, 370)
wire(c, 650, 440, 750, 430)
wire(c, 60, 800, 800, 430)
wire(c, 850, 400, 900, 400)

# ============================================================
# 8. DECODER_3to8
# ============================================================
c = circuit(root, 'DECODER_3to8', 'DECODER_3to8', '3-to-8 Decoder')
pin(c, 60, 400, 3, False, 'Sel[2:0]')
for i in range(8):
    pin(c, 700, 100+i*80, 1, True, f'Y{i}')
gate(c, 2, 'Decoder', 300, 400, {'select': '3'})
wire(c, 60, 400, 260, 400)
for i in range(8):
    wire(c, 340, 370+i*10, 600, 100+i*80)
    wire(c, 600, 100+i*80, 700, 100+i*80)

# ============================================================
# 9. HALF_ADDER
# ============================================================
c = circuit(root, 'HALF_ADDER', 'HA', 'Half Adder')
pin(c, 60, 100, 1, False, 'A')
pin(c, 60, 200, 1, False, 'B')
pin(c, 500, 100, 1, True, 'Sum')
pin(c, 500, 200, 1, True, 'Carry')
gate(c, 7, 'XOR2', 200, 100, label='XOR')
gate(c, 7, 'AND2', 200, 250, label='AND')
wire(c, 60, 100, 150, 100)
wire(c, 150, 100, 150, 80)
wire(c, 60, 200, 150, 200)
wire(c, 150, 200, 150, 120)
wire(c, 250, 100, 500, 100)
wire(c, 60, 100, 150, 230)
wire(c, 150, 230, 150, 250)
wire(c, 60, 200, 150, 270)
wire(c, 150, 270, 150, 290)
wire(c, 250, 250, 500, 200)

# ============================================================
# 10. FULL_ADDER
# ============================================================
c = circuit(root, 'FULL_ADDER', 'FA', 'Full Adder')
pin(c, 60, 100, 1, False, 'A')
pin(c, 60, 200, 1, False, 'B')
pin(c, 60, 350, 1, False, 'Cin')
pin(c, 600, 100, 1, True, 'Sum')
pin(c, 600, 300, 1, True, 'Cout')
gate(c, 7, 'HALF_ADDER', 200, 100, label='HA1')
gate(c, 7, 'HALF_ADDER', 400, 100, label='HA2')
gate(c, 7, 'OR2', 400, 300, label='OR')
wire(c, 60, 100, 130, 100)
wire(c, 130, 100, 130, 80)
wire(c, 60, 200, 130, 200)
wire(c, 130, 200, 130, 120)
wire(c, 280, 100, 330, 100)
wire(c, 330, 100, 330, 80)
wire(c, 60, 350, 330, 350)
wire(c, 330, 350, 330, 120)
wire(c, 480, 100, 600, 100)
wire(c, 280, 150, 330, 280)
wire(c, 330, 280, 330, 300)
wire(c, 480, 150, 330, 320)
wire(c, 330, 320, 330, 340)
wire(c, 450, 300, 600, 300)

# ============================================================
# 11. ADDER_8
# ============================================================
c = circuit(root, 'ADDER_8', 'ADDER_8', '8-bit Adder')
pin(c, 60, 100, 8, False, 'A[7:0]')
pin(c, 60, 200, 8, False, 'B[7:0]')
pin(c, 60, 350, 1, False, 'Cin')
pin(c, 700, 100, 8, True, 'Sum[7:0]')
pin(c, 700, 350, 1, True, 'Cout')
gate(c, 7, 'FULL_ADDER', 300, 100, label='FA0')
gate(c, 4, 'Bit Selector', 150, 100, {'width': '8', 'upper': '0', 'lower': '0'})
gate(c, 4, 'Bit Selector', 150, 200, {'width': '8', 'upper': '0', 'lower': '0'})
gate(c, 4, 'Bit Selector', 600, 100, {'width': '8', 'upper': '0', 'lower': '0'})
wire(c, 60, 100, 110, 100)
wire(c, 60, 200, 110, 200)
wire(c, 380, 100, 450, 100)
wire(c, 60, 350, 250, 350)
wire(c, 250, 350, 250, 130)
wire(c, 450, 150, 700, 350)
wire(c, 640, 100, 700, 100)

# ============================================================
# 12. SUBTRACTOR_8
# ============================================================
c = circuit(root, 'SUBTRACTOR_8', 'SUB_8', '8-bit Subtractor')
pin(c, 60, 100, 8, False, 'A[7:0]')
pin(c, 60, 200, 8, False, 'B[7:0]')
pin(c, 700, 100, 8, True, 'Diff[7:0]')
pin(c, 700, 350, 1, True, 'Bout')
gate(c, 7, 'ADDER_8', 400, 100, label='ADD')
gate(c, 6, 'Constant', 200, 300, {'value': '1', 'width': '8'})
gate(c, 6, 'Xor', 200, 200, {'width': '8'})
wire(c, 60, 100, 350, 100)
wire(c, 350, 100, 350, 80)
wire(c, 60, 200, 150, 200)
wire(c, 150, 200, 150, 180)
wire(c, 200, 300, 150, 220)
wire(c, 250, 200, 350, 200)
wire(c, 350, 200, 350, 120)
wire(c, 480, 100, 700, 100)
wire(c, 480, 350, 700, 350)
wire(c, 200, 300, 350, 350)

# ============================================================
# 13. ALU_8
# ============================================================
c = circuit(root, 'ALU_8', 'ALU', '8-bit ALU (13 operations)')
pin(c, 60, 100, 8, False, 'A[7:0]')
pin(c, 60, 200, 8, False, 'B[7:0]')
pin(c, 60, 350, 4, False, 'ALU_OP[3:0]')
pin(c, 700, 100, 8, True, 'RESULT[7:0]')
pin(c, 700, 200, 1, True, 'Z')
pin(c, 700, 250, 1, True, 'N')
pin(c, 700, 300, 1, True, 'C')
gate(c, 7, 'ADDER_8', 300, 80, label='ADD')
gate(c, 7, 'SUBTRACTOR_8', 300, 200, label='SUB')
gate(c, 4, 'Register', 500, 100, {'width': '8', 'label': 'RESULT_REG'})
wire(c, 60, 100, 250, 80)
wire(c, 250, 80, 250, 60)
wire(c, 60, 200, 250, 200)
wire(c, 250, 200, 250, 100)
wire(c, 60, 100, 250, 180)
wire(c, 60, 200, 250, 220)
wire(c, 380, 80, 450, 80)
wire(c, 450, 80, 450, 100)
wire(c, 380, 200, 450, 200)
wire(c, 450, 200, 450, 100)
wire(c, 500, 100, 700, 100)
wire(c, 60, 100, 600, 200)
wire(c, 60, 350, 500, 350)

# ============================================================
# 14. REGISTER_8
# ============================================================
c = circuit(root, 'REGISTER_8', 'REG8', '8-bit Register')
pin(c, 100, 100, 8, False, 'D')
pin(c, 100, 200, 1, False, 'WE')
pin(c, 100, 250, 1, False, 'CLK')
pin(c, 100, 300, 1, False, 'RST')
pin(c, 400, 150, 8, True, 'Q')
gate(c, 4, 'Register', 250, 150, {'width': '8'})
wire(c, 100, 100, 220, 120)
wire(c, 100, 200, 250, 180)
wire(c, 100, 250, 250, 210)
wire(c, 100, 300, 250, 230)
wire(c, 280, 150, 400, 150)

# ============================================================
# 15. REGISTER_FILE_8x8 (CLEAN WIRING)
# ============================================================
c = circuit(root, 'REGISTER_FILE_8x8', 'REG_FILE', '8x8 Register File (S0-S7)')
# Inputs
pin(c, 60, 100, 3, False, 'RdAddr1[2:0]')
pin(c, 60, 200, 3, False, 'RdAddr2[2:0]')
pin(c, 60, 600, 3, False, 'WrAddr[2:0]')
pin(c, 60, 700, 8, False, 'WrData[7:0]')
pin(c, 60, 800, 1, False, 'WE')
pin(c, 60, 900, 1, False, 'CLK')
# Outputs
pin(c, 900, 200, 8, True, 'RdData1[7:0]')
pin(c, 900, 600, 8, True, 'RdData2[7:0]')

# 8 registers (S0-S7) at X=400, Y=100,200,...,800
for i in range(8):
    gate(c, 7, 'REGISTER_8', 400, 100+i*100, label=f'S{i}')

# Write address decoder
gate(c, 2, 'Decoder', 200, 650, {'select': '3'}, 'WR_DEC')
# Read MUX 1
gate(c, 2, 'Multiplexer', 700, 200, {'select': '3', 'width': '8'}, 'RD_MUX1')
# Read MUX 2
gate(c, 2, 'Multiplexer', 700, 600, {'select': '3', 'width': '8'}, 'RD_MUX2')

# --- WrData TRUNK wire (vertical at X=350, from Y=100 to Y=800) ---
wire(c, 60, 700, 350, 700)
wire(c, 350, 100, 350, 800)
# Branch from trunk to each register D input (short horizontal)
for i in range(8):
    wire(c, 350, 100+i*100, 370, 100+i*100)

# --- CLK TRUNK wire (vertical at X=360, from Y=100 to Y=800) ---
wire(c, 60, 900, 360, 900)
wire(c, 360, 100, 360, 800)
# Branch from trunk to each register CLK
for i in range(8):
    wire(c, 360, 100+i*100, 370, 250+i*100)

# --- WE decoder output to each register WE ---
wire(c, 60, 800, 160, 800)
wire(c, 160, 650, 160, 700)
# Each decoder output → register WE (short wires)
for i in range(8):
    wire(c, 240, 620+i*10, 370, 200+i*100)

# --- Register Q outputs → Read MUX 1 inputs ---
for i in range(8):
    wire(c, 430, 100+i*100, 650, 170+i*20)

# --- Register Q outputs → Read MUX 2 inputs ---
for i in range(8):
    wire(c, 430, 100+i*100, 650, 570+i*20)

# --- Read Address to MUX select ---
wire(c, 60, 100, 700, 230)
wire(c, 60, 200, 700, 630)

# --- MUX outputs → Pins ---
wire(c, 750, 200, 900, 200)
wire(c, 750, 600, 900, 600)

# ============================================================
# 16. ACC_8
# ============================================================
c = circuit(root, 'ACC_8', 'ACC', '8-bit Accumulator')
pin(c, 100, 100, 8, False, 'D')
pin(c, 100, 200, 1, False, 'WE')
pin(c, 100, 250, 1, False, 'CLK')
pin(c, 100, 300, 1, False, 'RST')
pin(c, 400, 150, 8, True, 'Q')
gate(c, 4, 'Register', 250, 150, {'width': '8', 'label': 'ACC'})
wire(c, 100, 100, 220, 120)
wire(c, 100, 200, 250, 180)
wire(c, 100, 250, 250, 210)
wire(c, 100, 300, 250, 230)
wire(c, 280, 150, 400, 150)

# ============================================================
# 17. FLAGS_REG
# ============================================================
c = circuit(root, 'FLAGS_REG', 'FLAGS', 'Flags Register (Z, N, C)')
pin(c, 100, 100, 8, False, 'D')
pin(c, 100, 200, 1, False, 'WE')
pin(c, 100, 250, 1, False, 'CLK')
pin(c, 100, 300, 1, False, 'RST')
pin(c, 400, 100, 1, True, 'Z')
pin(c, 400, 150, 1, True, 'N')
pin(c, 400, 200, 1, True, 'C')
gate(c, 4, 'Register', 250, 150, {'width': '8', 'label': 'FLAGS'})
gate(c, 6, 'Bit Selector', 350, 100, {'width': '8', 'upper': '0', 'lower': '0'})
gate(c, 6, 'Bit Selector', 350, 150, {'width': '8', 'upper': '1', 'lower': '1'})
gate(c, 6, 'Bit Selector', 350, 200, {'width': '8', 'upper': '2', 'lower': '2'})
wire(c, 100, 100, 220, 120)
wire(c, 100, 200, 250, 180)
wire(c, 100, 250, 250, 210)
wire(c, 100, 300, 250, 230)
wire(c, 280, 150, 320, 100)
wire(c, 320, 100, 320, 200)
wire(c, 380, 100, 400, 100)
wire(c, 380, 150, 400, 150)
wire(c, 380, 200, 400, 200)

# ============================================================
# 18. PC_16
# ============================================================
c = circuit(root, 'PC_16', 'PC', '16-bit Program Counter')
pin(c, 100, 100, 16, False, 'D')
pin(c, 100, 200, 1, False, 'WE')
pin(c, 100, 250, 1, False, 'CLK')
pin(c, 100, 300, 1, False, 'RST')
pin(c, 400, 150, 16, True, 'Q')
gate(c, 4, 'Register', 250, 150, {'width': '16', 'label': 'PC'})
wire(c, 100, 100, 220, 120)
wire(c, 100, 200, 250, 180)
wire(c, 100, 250, 250, 210)
wire(c, 100, 300, 250, 230)
wire(c, 280, 150, 400, 150)

# ============================================================
# 19. SP_8
# ============================================================
c = circuit(root, 'SP_8', 'SP', '8-bit Stack Pointer')
pin(c, 100, 100, 8, False, 'D')
pin(c, 100, 200, 1, False, 'WE')
pin(c, 100, 250, 1, False, 'CLK')
pin(c, 100, 300, 1, False, 'RST')
pin(c, 400, 150, 8, True, 'Q')
gate(c, 4, 'Register', 250, 150, {'width': '8', 'label': 'SP'})
wire(c, 100, 100, 220, 120)
wire(c, 100, 200, 250, 180)
wire(c, 100, 250, 250, 210)
wire(c, 100, 300, 250, 230)
wire(c, 280, 150, 400, 150)

# ============================================================
# 20. INSTRUCTION_REGISTER
# ============================================================
c = circuit(root, 'INSTRUCTION_REGISTER', 'IR', 'Instruction Register')
pin(c, 100, 100, 8, False, 'D')
pin(c, 100, 200, 1, False, 'IR_LOAD')
pin(c, 100, 250, 1, False, 'CLK')
pin(c, 500, 100, 4, True, 'OPCODE[3:0]')
pin(c, 500, 200, 4, True, 'REG[3:0]')
gate(c, 4, 'Register', 250, 150, {'width': '8', 'label': 'IR_REG'})
gate(c, 6, 'Bit Selector', 400, 100, {'width': '8', 'upper': '7', 'lower': '4'}, 'OPCODE_EXT')
gate(c, 6, 'Bit Selector', 400, 200, {'width': '8', 'upper': '3', 'lower': '0'}, 'REG_EXT')
wire(c, 100, 100, 220, 120)
wire(c, 100, 200, 250, 180)
wire(c, 100, 250, 250, 210)
wire(c, 280, 150, 370, 100)
wire(c, 370, 100, 370, 200)
wire(c, 430, 100, 500, 100)
wire(c, 430, 200, 500, 200)

# ============================================================
# 21. RAM_64K
# ============================================================
c = circuit(root, 'RAM_64K', 'RAM', '64KB RAM')
pin(c, 60, 100, 16, False, 'ADDR[15:0]')
pin(c, 60, 200, 8, False, 'WDATA[7:0]')
pin(c, 60, 300, 1, False, 'WE')
pin(c, 60, 350, 1, False, 'CLK')
pin(c, 500, 200, 8, True, 'RDATA[7:0]')
gate(c, 4, 'RAM', 300, 200, {'addr_bits': '16', 'data_bits': '8'})
wire(c, 60, 100, 260, 170)
wire(c, 60, 200, 260, 200)
wire(c, 60, 300, 260, 230)
wire(c, 60, 350, 300, 240)
wire(c, 340, 200, 500, 200)

# ============================================================
# 22. CONTROL_UNIT
# ============================================================
c = circuit(root, 'CONTROL_UNIT', 'CTRL', 'Dhad Control Unit (FSM)')
# Inputs
pin(c, 60, 100, 4, False, 'OPCODE[3:0]')
pin(c, 60, 200, 4, False, 'EXT_OP[3:0]')
pin(c, 60, 300, 1, False, 'CLK')
pin(c, 60, 350, 1, False, 'RST')
pin(c, 60, 400, 1, False, 'Z_FLAG')
pin(c, 60, 450, 1, False, 'N_FLAG')
pin(c, 60, 500, 1, False, 'C_FLAG')
# Outputs
outputs = [
    ('IR_LOAD', 1), ('PC_INC', 1), ('PC_LOAD', 1), ('MEM_READ', 1),
    ('MEM_WRITE', 1), ('ACC_WRITE', 1), ('REG_WRITE', 1), ('FLAGS_WRITE', 1),
    ('ALU_OP', 4), ('ALU_A_SRC', 2), ('ALU_B_SRC', 2), ('WR_SRC', 2),
    ('MEM_ADDR_SRC', 2), ('SP_INC', 1), ('SP_DEC', 1), ('HALT', 1)
]
for i, (name, w) in enumerate(outputs):
    pin(c, 1600, 100+i*50, w, True, name)

# State register (2-bit)
gate(c, 4, 'Register', 300, 100, {'width': '2', 'label': 'STATE'})
# State decoder (2:4)
gate(c, 2, 'Decoder', 500, 100, {'select': '2'}, 'STATE_DEC')
# Opcode decoder (4:16)
gate(c, 2, 'Decoder', 500, 300, {'select': '4'}, 'OPCODE_DEC')
# Next state MUX
gate(c, 2, 'Multiplexer', 700, 100, {'select': '2', 'width': '2'}, 'NEXT_STATE_MUX')
# Next state constants
gate(c, 6, 'Constant', 550, 150, {'value': '1', 'width': '2'}, 'NEXT_01')
gate(c, 6, 'Constant', 550, 200, {'value': '2', 'width': '2'}, 'NEXT_10')
gate(c, 6, 'Constant', 550, 250, {'value': '0', 'width': '2'}, 'NEXT_00')

# Wiring
wire(c, 60, 350, 300, 130)   # RST
wire(c, 60, 300, 300, 110)   # CLK
wire(c, 330, 100, 450, 100)  # State → decoder
wire(c, 60, 100, 450, 300)   # OPCODE → decoder
# State decoder outputs → MUX inputs
wire(c, 540, 70, 650, 80)    # S0 → MUX in0
wire(c, 540, 80, 650, 110)   # S1 → MUX in1
wire(c, 540, 90, 650, 140)   # S2 → MUX in2
wire(c, 550, 150, 650, 80)   # const 01 → MUX in0
wire(c, 550, 200, 650, 110)  # const 10 → MUX in1
wire(c, 550, 250, 650, 140)  # const 00 → MUX in2
wire(c, 330, 100, 700, 130)  # State → MUX select
wire(c, 750, 100, 270, 70)   # MUX out → State D
# FETCH state outputs (S0)
wire(c, 540, 70, 1600, 100)  # IR_LOAD = S0
wire(c, 540, 70, 1600, 250)  # MEM_READ = S0
wire(c, 540, 70, 1600, 150)  # PC_INC = S0

# ============================================================
# 23. DATAPATH
# ============================================================
c = circuit(root, 'DATAPATH', 'DATAPATH', 'Dhad 8-bit Datapath')

# --- INPUT PINS ---
pin(c, 60, 100, 1, False, 'CLK')
pin(c, 60, 150, 1, False, 'RST')
pin(c, 60, 250, 8, False, 'MEM_RDATA[7:0]')
pin(c, 60, 300, 8, False, 'IMM[7:0]')
pin(c, 60, 400, 1, False, 'IR_LOAD')
pin(c, 60, 450, 1, False, 'PC_INC')
pin(c, 60, 500, 1, False, 'PC_LOAD')
pin(c, 60, 550, 1, False, 'MEM_READ')
pin(c, 60, 600, 1, False, 'MEM_WRITE')
pin(c, 60, 650, 1, False, 'ACC_WRITE')
pin(c, 60, 700, 1, False, 'REG_WRITE')
pin(c, 60, 750, 1, False, 'FLAGS_WRITE')
pin(c, 60, 800, 4, False, 'ALU_OP[3:0]')
pin(c, 60, 850, 2, False, 'ALU_A_SRC[1:0]')
pin(c, 60, 900, 2, False, 'ALU_B_SRC[1:0]')
pin(c, 60, 950, 2, False, 'WR_SRC[1:0]')
pin(c, 60, 1000, 2, False, 'MEM_ADDR_SRC[1:0]')
pin(c, 60, 1050, 1, False, 'SP_INC')
pin(c, 60, 1100, 1, False, 'SP_DEC')

# --- OUTPUT PINS ---
pin(c, 1800, 250, 16, True, 'MEM_ADDR[15:0]')
pin(c, 1800, 300, 8, True, 'MEM_WDATA[7:0]')
pin(c, 1800, 400, 4, True, 'OPCODE[3:0]')
pin(c, 1800, 450, 4, True, 'REG[3:0]')
pin(c, 1800, 500, 1, True, 'Z_FLAG')
pin(c, 1800, 550, 1, True, 'N_FLAG')
pin(c, 1800, 600, 1, True, 'C_FLAG')
pin(c, 1800, 700, 8, True, 'ACC_OUT[7:0]')
pin(c, 1800, 750, 16, True, 'PC_OUT[15:0]')
pin(c, 1800, 800, 8, True, 'FLAGS_OUT[7:0]')
pin(c, 1800, 850, 8, True, 'SP_OUT[7:0]')
pin(c, 1800, 900, 8, True, 'REG_RDDATA1[7:0]')

# --- SUBCIRCUIT INSTANCES ---
# PC_16 at (500, 200)
gate(c, 7, 'PC_16', 500, 200, label='PC')
# INSTRUCTION_REGISTER at (800, 200)
gate(c, 7, 'INSTRUCTION_REGISTER', 800, 200, label='IR')
# REGISTER_FILE_8x8 at (1200, 400)
gate(c, 7, 'REGISTER_FILE_8x8', 1200, 400, label='REG_FILE')
# ACC_8 at (1200, 150)
gate(c, 7, 'ACC_8', 1200, 150, label='ACC')
# ALU_8 at (1500, 350)
gate(c, 7, 'ALU_8', 1500, 350, label='ALU')
# FLAGS_REG at (1700, 450)
gate(c, 7, 'FLAGS_REG', 1700, 450, label='FLAGS')
# SP_8 at (500, 550)
gate(c, 7, 'SP_8', 500, 550, label='SP')

# MUXes
gate(c, 2, 'Multiplexer', 350, 200, {'select': '1', 'width': '16'}, 'MUX_PC_NEXT')
gate(c, 2, 'Multiplexer', 1350, 300, {'select': '2', 'width': '8'}, 'MUX_ALU_A')
gate(c, 2, 'Multiplexer', 1350, 500, {'select': '2', 'width': '8'}, 'MUX_ALU_B')
gate(c, 2, 'Multiplexer', 1650, 200, {'select': '2', 'width': '8'}, 'MUX_WR_SRC')
gate(c, 2, 'Multiplexer', 650, 100, {'select': '2', 'width': '16'}, 'MUX_MEM_ADDR')
gate(c, 2, 'Multiplexer', 1650, 300, {'select': '1', 'width': '8'}, 'MUX_MEM_WDATA')

# Constants
gate(c, 6, 'Constant', 150, 250, {'value': '1', 'width': '16'}, 'ONE_16')
gate(c, 6, 'Constant', 1250, 550, {'value': '1', 'width': '8'}, 'ONE_8')

# OR gates for WE
gate(c, 1, 'Or', 350, 280, {'width': '1'}, 'PC_WE_OR')
gate(c, 1, 'Or', 400, 600, {'width': '1'}, 'SP_WE_OR')

# --- WIRING ---
# PC connections
wire(c, 60, 100, 500, 210)     # CLK
wire(c, 60, 150, 500, 230)     # RST
wire(c, 400, 200, 470, 170)    # MUX_PC_NEXT out → PC D
wire(c, 60, 450, 320, 270)     # PC_INC → OR in0
wire(c, 60, 500, 320, 290)     # PC_LOAD → OR in1
wire(c, 380, 280, 470, 190)    # OR out → PC WE
wire(c, 530, 200, 600, 200)    # PC Q → junction
wire(c, 600, 200, 1800, 750)   # PC Q → PC_OUT
# PC+1 calculation
wire(c, 530, 200, 180, 200)    # PC Q → adder A
wire(c, 150, 250, 180, 220)    # ONE_16 → adder B
wire(c, 230, 200, 300, 180)    # adder out → MUX in0
wire(c, 60, 500, 350, 230)     # PC_LOAD → MUX sel

# IR connections
wire(c, 60, 250, 740, 170)     # MEM_RDATA → IR D
wire(c, 60, 400, 800, 180)     # IR_LOAD → IR WE
wire(c, 60, 100, 800, 210)     # CLK
wire(c, 860, 170, 900, 170)    # OPCODE out
wire(c, 860, 200, 900, 200)    # REG out
wire(c, 900, 170, 1800, 400)   # OPCODE → output
wire(c, 900, 200, 1800, 450)   # REG → output

# Register File connections
wire(c, 900, 200, 1140, 370)   # REG → RdAddr1
wire(c, 900, 200, 1140, 470)   # REG → RdAddr2
wire(c, 900, 200, 1140, 570)   # REG → WrAddr
wire(c, 60, 700, 1140, 670)    # REG_WRITE → WE
wire(c, 60, 100, 1140, 770)    # CLK
wire(c, 1260, 370, 1300, 280)  # RdData1 → MUX_ALU_A in0
wire(c, 1260, 370, 1300, 480)  # RdData1 → MUX_ALU_B in0
wire(c, 1260, 370, 1600, 280)  # RdData1 → MUX_MEM_WDATA in0
wire(c, 1260, 370, 1800, 900)  # RdData1 → output
wire(c, 1260, 570, 1300, 570)  # RdData2 → MUX_ALU_B in0

# ACC connections
wire(c, 60, 650, 1200, 130)    # ACC_WRITE → WE
wire(c, 60, 100, 1200, 160)    # CLK
wire(c, 60, 150, 1200, 180)    # RST
wire(c, 1230, 150, 1300, 310)  # ACC Q → MUX_ALU_A in1
wire(c, 1230, 150, 1600, 310)  # ACC Q → MUX_MEM_WDATA in1
wire(c, 1230, 150, 1800, 700)  # ACC Q → output

# ALU A MUX
wire(c, 60, 850, 1350, 330)    # ALU_A_SRC → MUX sel
wire(c, 1400, 300, 1470, 320)  # MUX_ALU_A out → ALU A
# ALU B MUX
wire(c, 60, 900, 1350, 530)    # ALU_B_SRC → MUX sel
wire(c, 60, 300, 1300, 510)    # IMM → MUX_ALU_B in1
wire(c, 1250, 550, 1300, 540)  # ONE_8 → MUX_ALU_B in2
wire(c, 1400, 500, 1470, 370)  # MUX_ALU_B out → ALU B

# ALU connections
wire(c, 60, 800, 1500, 380)    # ALU_OP → ALU
wire(c, 1560, 350, 1600, 180)  # RESULT → MUX_WR_SRC in0
wire(c, 1560, 400, 1620, 370)  # Z → splitter
wire(c, 1560, 450, 1620, 400)  # N → splitter
wire(c, 1560, 500, 1620, 430)  # C → splitter

# FLAGS_REG connections (via splitter to combine Z,N,C into 8-bit)
gate(c, 0, 'Splitter', 1650, 400, {'fanout': '8', 'width': '8', 'combine': '1', 'bit0': '0', 'bit1': '1', 'bit2': '2'}, 'FLAG_COMBINE')
wire(c, 1650, 400, 1670, 400)  # splitter out
wire(c, 1670, 400, 1670, 430)  # → FLAGS D
wire(c, 60, 750, 1670, 440)    # FLAGS_WRITE → WE
wire(c, 60, 100, 1700, 460)    # CLK
wire(c, 60, 150, 1700, 480)    # RST
wire(c, 1730, 400, 1800, 500)  # Z → output
wire(c, 1730, 450, 1800, 550)  # N → output
wire(c, 1730, 500, 1800, 600)  # C → output
wire(c, 1730, 400, 1800, 800)  # FLAGS → output (8-bit bus)

# WR_SRC MUX
wire(c, 60, 950, 1650, 230)    # WR_SRC → MUX sel
wire(c, 60, 250, 1600, 210)    # MEM_RDATA → MUX in1
wire(c, 600, 200, 1600, 240)   # PC[7:0] → MUX in2
wire(c, 1700, 200, 1750, 200)  # MUX out → junction
wire(c, 1750, 200, 1200, 120)  # → ACC D
wire(c, 1750, 200, 1140, 600)  # → REG WrData

# SP connections
wire(c, 60, 1050, 370, 590)    # SP_INC → OR in0
wire(c, 60, 1100, 370, 610)    # SP_DEC → OR in1
wire(c, 430, 600, 470, 570)    # OR out → SP WE
wire(c, 60, 100, 500, 560)     # CLK
wire(c, 60, 150, 500, 580)     # RST
wire(c, 530, 550, 600, 550)    # SP Q → junction
wire(c, 600, 550, 1300, 340)   # SP → MUX_ALU_A in2
wire(c, 600, 550, 1800, 850)   # SP → output

# MEM_ADDR MUX
wire(c, 60, 1000, 650, 130)    # MEM_ADDR_SRC → MUX sel
wire(c, 600, 200, 600, 80)     # PC → MUX in0
wire(c, 700, 100, 1800, 250)   # MUX out → MEM_ADDR output

# MEM_WDATA MUX
wire(c, 1700, 300, 1800, 300)  # MUX out → MEM_WDATA output

# ============================================================
# 24. CPU
# ============================================================
c = circuit(root, 'CPU', 'DHAD_CPU', 'Dhad 8-bit CPU')
pin(c, 60, 100, 1, False, 'CLK')
pin(c, 60, 150, 1, False, 'RST')
pin(c, 1400, 100, 8, True, 'ACC[7:0]')
pin(c, 1400, 150, 16, True, 'PC[15:0]')
pin(c, 1400, 200, 1, True, 'HALTED')
pin(c, 1400, 250, 8, True, 'FLAGS[7:0]')

gate(c, 7, 'RAM_64K', 500, 300, label='RAM')
gate(c, 7, 'DATAPATH', 900, 300, label='DP')
gate(c, 7, 'CONTROL_UNIT', 900, 600, label='CTRL')

# Placeholder wiring
wire(c, 60, 100, 200, 100)
wire(c, 60, 150, 200, 150)
wire(c, 1300, 100, 1400, 100)
wire(c, 1300, 150, 1400, 150)
wire(c, 1300, 200, 1400, 200)
wire(c, 1300, 250, 1400, 250)

# Write
ET.indent(root, space="  ")
tree = ET.ElementTree(root)
tree.write('/home/m_hmoz/Documents/cpu/hardware/dhad_cpu.circ',
           xml_declaration=True, encoding='unicode')
print("Circuit rebuilt successfully!")
print("24 circuits generated.")
