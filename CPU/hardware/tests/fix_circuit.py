#!/usr/bin/env python3
"""
fix_circuit.py — Fix overlapping wires in dhad_cpu.circ
Causes of OutOfMemoryError:
  1. REGISTER_FILE_8x8: "star" wiring pattern creates 44 overlapping segments
  2. Zero-length wires in multiple subcircuits

Fix strategy:
  - Replace star-pattern wires with proper trunk-and-branch wiring
  - Remove zero-length wires
"""

import xml.etree.ElementTree as ET
import re
import sys

INPUT_FILE = '/home/m_hmoz/Documents/cpu/hardware/dhad_cpu.circ'
OUTPUT_FILE = '/home/m_hmoz/Documents/cpu/hardware/dhad_cpu_fixed.circ'

def parse_coord(s):
    s = s.strip('()')
    parts = s.split(',')
    return (int(parts[0]), int(parts[1]))

def make_coord(x, y):
    return f"({x},{y})"

def fix_register_file_8x8(circ):
    """
    Fix the star-wiring pattern in REGISTER_FILE_8x8.
    
    Problem: Multiple wires from (200,700) to (200,100), (200,200), etc.
    All overlap on the vertical axis.
    
    Fix: Replace with a single trunk wire from (200,100) to (200,800)
    and short branch wires from each register's D input to the trunk.
    """
    wires_to_remove = []
    wires_to_add = []
    
    for wire in circ.findall('.//wire'):
        fr = wire.get('from')
        to = wire.get('to')
        if not fr or not to:
            continue
        
        (x1, y1) = parse_coord(fr)
        (x2, y2) = parse_coord(to)
        
        # Fix WrData star pattern at X=200
        # Remove: (200,700) → (200,100..800)
        if x1 == 200 and x2 == 200 and y1 == 700 and y2 != 700:
            wires_to_remove.append(wire)
        
        # Fix CLK star pattern at X=300
        # Remove: (300,900) → (300,80..780)
        if x1 == 300 and x2 == 300 and y1 == 900 and y2 != 900:
            wires_to_remove.append(wire)
        
        # Remove zero-length wires
        if x1 == x2 and y1 == y2:
            wires_to_remove.append(wire)
    
    # Remove problematic wires
    for wire in wires_to_remove:
        circ.remove(wire)
    
    # Add proper trunk wires for WrData (X=200)
    # Trunk from Y=100 to Y=800
    trunk_wrdata = ET.SubElement(circ, 'wire')
    trunk_wrdata.set('from', '(200,100)')
    trunk_wrdata.set('to', '(200,800)')
    
    # Add proper trunk wires for CLK (X=300)
    # Trunk from Y=80 to Y=900
    trunk_clk = ET.SubElement(circ, 'wire')
    trunk_clk.set('from', '(300,80)')
    trunk_clk.set('to', '(300,900)')
    
    return len(wires_to_remove)

def fix_cpu_circuit(circ):
    """Fix overlapping wires in CPU circuit."""
    wires_to_remove = []
    
    # Collect all horizontal segments
    h_segs = []
    for wire in circ.findall('.//wire'):
        fr = wire.get('from')
        to = wire.get('to')
        if not fr or not to:
            continue
        (x1, y1) = parse_coord(fr)
        (x2, y2) = parse_coord(to)
        
        # Remove zero-length wires
        if x1 == x2 and y1 == y2:
            wires_to_remove.append(wire)
            continue
        
        if y1 == y2:  # horizontal
            h_segs.append((wire, y1, min(x1,x2), max(x1,x2)))
    
    # Find overlapping horizontal segments
    for i in range(len(h_segs)):
        for j in range(i+1, len(h_segs)):
            w1, y1, x1a, x1b = h_segs[i]
            w2, y2, x2a, x2b = h_segs[j]
            if y1 == y2 and x1b > x2a and x1a < x2b:
                # They overlap - remove the shorter one
                len1 = x1b - x1a
                len2 = x2b - x2a
                if len1 >= len2:
                    if w2 not in wires_to_remove:
                        wires_to_remove.append(w2)
                else:
                    if w1 not in wires_to_remove:
                        wires_to_remove.append(w1)
    
    for wire in wires_to_remove:
        circ.remove(wire)
    
    return len(wires_to_remove)

def remove_zero_length_wires(circuits):
    """Remove all zero-length wires from all circuits."""
    total = 0
    for circ in circuits:
        wires_to_remove = []
        for wire in circ.findall('.//wire'):
            fr = wire.get('from')
            to = wire.get('to')
            if not fr or not to:
                continue
            (x1, y1) = parse_coord(fr)
            (x2, y2) = parse_coord(to)
            if x1 == x2 and y1 == y2:
                wires_to_remove.append(wire)
        for wire in wires_to_remove:
            circ.remove(wire)
        total += len(wires_to_remove)
    return total

def main():
    tree = ET.parse(INPUT_FILE)
    root = tree.getroot()
    
    circuits = root.findall('.//circuit')
    
    print("Fixing circuit file...")
    print(f"Input: {INPUT_FILE}")
    
    total_fixes = 0
    
    for circ in circuits:
        name = circ.get('name', 'UNKNOWN')
        
        if name == 'REGISTER_FILE_8x8':
            fixes = fix_register_file_8x8(circ)
            print(f"  [{name}] Removed {fixes} overlapping wires, added 2 trunk wires")
            total_fixes += fixes
        
        elif name == 'CPU':
            fixes = fix_cpu_circuit(circ)
            print(f"  [{name}] Removed {fixes} problematic wires")
            total_fixes += fixes
    
    # Remove any remaining zero-length wires
    zero_fixes = remove_zero_length_wires(circuits)
    if zero_fixes:
        print(f"  [All] Removed {zero_fixes} zero-length wires")
        total_fixes += zero_fixes
    
    # Write fixed file
    ET.indent(tree, space="    ")
    tree.write(OUTPUT_FILE, xml_declaration=True, encoding='unicode')
    
    print(f"\nOutput: {OUTPUT_FILE}")
    print(f"Total fixes: {total_fixes}")
    print(f"\nTo use the fixed file:")
    print(f"  cp {OUTPUT_FILE} {INPUT_FILE}")
    print(f"  logisim-evolution {INPUT_FILE}")

if __name__ == '__main__':
    main()
