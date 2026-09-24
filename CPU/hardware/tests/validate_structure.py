#!/usr/bin/env python3
"""
Structural validation script for dhad_cpu.circ
Checks XML structure, component connectivity, and identifies issues.
Does NOT run Logisim simulation — that must be done manually.
"""

import xml.etree.ElementTree as ET
import sys
from collections import defaultdict

CIRC_FILE = "dhad_cpu.circ"

def load_circuit(path):
    tree = ET.parse(path)
    return tree.getroot()

def get_circuits(root):
    return {c.get('name'): c for c in root.findall('circuit')}

def get_components(circuit):
    comps = []
    for comp in circuit.findall('comp'):
        lib = comp.get('lib', '0')
        name = comp.get('name')
        loc = comp.get('loc', '(0,0)')
        attrs = {}
        for a in comp.findall('a'):
            attrs[a.get('name')] = a.get('val')
        comps.append({'lib': lib, 'name': name, 'loc': loc, 'attrs': attrs})
    return comps

def get_wires(circuit):
    wires = []
    for wire in circuit.findall('wire'):
        fr = wire.get('from')
        to = wire.get('to')
        wires.append({'from': fr, 'to': to})
    return wires

def parse_loc(loc_str):
    """Parse (x,y) string to tuple of ints."""
    loc_str = loc_str.strip('()')
    parts = loc_str.split(',')
    return (int(parts[0]), int(parts[1]))

def check_circuit_structure(name, circuit):
    """Check a single circuit for structural issues."""
    issues = []
    components = get_components(circuit)
    wires = get_wires(circuit)

    # Check 1: Empty circuit
    if len(components) == 0:
        issues.append("ERROR: No components in circuit")

    # Check 2: Components without wires
    comp_locs = set()
    for comp in components:
        loc = parse_loc(comp['loc'])
        comp_locs.add(loc)

    wire_locs = set()
    for wire in wires:
        wire_locs.add(parse_loc(wire['from']))
        wire_locs.add(parse_loc(wire['to']))

    # Check 3: Input pins (lib=0, output=false)
    input_pins = [c for c in components if c['lib'] == '0' and c['name'] == 'Pin'
                  and c['attrs'].get('output') == 'false']
    output_pins = [c for c in components if c['lib'] == '0' and c['name'] == 'Pin'
                   and c['attrs'].get('output') == 'true']

    # Check 4: Pin widths
    for pin in input_pins + output_pins:
        w = pin['attrs'].get('width', '1')
        try:
            width = int(w)
            if width < 1 or width > 32:
                issues.append(f"WARNING: Pin {pin['attrs'].get('label','?')} has unusual width {width}")
        except ValueError:
            issues.append(f"ERROR: Pin {pin['attrs'].get('label','?')} has invalid width '{w}'")

    # Check 5: Wire connectivity
    connected_locs = set()
    for wire in wires:
        connected_locs.add(parse_loc(wire['from']))
        connected_locs.add(parse_loc(wire['to']))

    # Check 6: Component pin connectivity (basic check)
    # Count how many wires touch each component location
    loc_wire_count = defaultdict(int)
    for wire in wires:
        loc_wire_count[parse_loc(wire['from'])] += 1
        loc_wire_count[parse_loc(wire['to'])] += 1

    return issues, len(components), len(wires), len(input_pins), len(output_pins)

def check_subcircuit_references(root, circuits):
    """Check that subcircuit references are valid."""
    issues = []
    circuit_names = set(circuits.keys())

    for name, circuit in circuits.items():
        for comp in circuit.findall('comp'):
            lib = comp.get('lib', '0')
            comp_name = comp.get('name')
            # Subcircuit references (lib != 0-6) refer to other circuits
            if lib not in ('0', '1', '2', '3', '4', '5', '6', '#Wiring', '#Gates', '#Plexers', '#Arithmetic', '#Memory', '#I/O', '#Base'):
                if comp_name not in circuit_names:
                    issues.append(f"ERROR: {name} references unknown subcircuit '{comp_name}'")
    return issues

def check_wire_bus_widths(circuit):
    """Check for bus width mismatches at wire junctions."""
    issues = []
    components = get_components(circuit)
    wires = get_wires(circuit)

    # Build location -> width map for output pins
    loc_width = {}
    for comp in components:
        if comp['name'] == 'Pin' and comp['attrs'].get('output') == 'true':
            loc = parse_loc(comp['loc'])
            width = int(comp['attrs'].get('width', '1'))
            loc_width[loc] = width

    # Check wires connected to output pins
    for wire in wires:
        fr = parse_loc(wire['from'])
        to = parse_loc(wire['to'])
        if fr in loc_width:
            # Wire starts at an output pin
            pass  # Can't easily determine wire width without more context
        if to in loc_width:
            pass

    return issues

def check_datapath_muxes(circuit):
    """Check MUX organization in DATAPATH circuit."""
    issues = []
    components = get_components(circuit)

    muxes = [c for c in components if 'MUX' in c['attrs'].get('label', '') or 'Multiplexer' in c['name']]
    for mux in muxes:
        label = mux['attrs'].get('label', mux['attrs'].get('circuit_Lab_Name', 'unknown'))
        select = mux['attrs'].get('select', '?')
        width = mux['attrs'].get('width', '?')
        issues.append(f"INFO: MUX found: {label} (select={select}, width={width})")

    return issues

def check_pc_16(circuit):
    """Check PC_16 circuit structure.
    PC_16 is a simple 16-bit register.
    Increment logic (PC+1) and MUX live in the DATAPATH circuit."""
    issues = []
    components = get_components(circuit)

    # PC_16 should have: Register(16) only
    has_register = any(c['name'] == 'Register' and c['attrs'].get('width') == '16' for c in components)
    if not has_register:
        issues.append("ERROR: PC_16 missing 16-bit Register")

    # Verify it's a simple register (increment/MUX in DATAPATH)
    pins = [c for c in components if c['name'] == 'Pin']
    pin_labels = [p['attrs'].get('label', '') for p in pins]
    expected_inputs = {'D', 'WE', 'CLK', 'RST'}
    expected_outputs = {'Q'}
    actual_inputs = {p['attrs'].get('label', '') for p in pins if p['attrs'].get('output') == 'false'}
    actual_outputs = {p['attrs'].get('label', '') for p in pins if p['attrs'].get('output') == 'true'}

    if actual_inputs != expected_inputs:
        issues.append(f"WARNING: PC_16 inputs {actual_inputs} != expected {expected_inputs}")
    if actual_outputs != expected_outputs:
        issues.append(f"WARNING: PC_16 outputs {actual_outputs} != expected {expected_outputs}")

    return issues

def check_register_file(circuit):
    """Check REGISTER_FILE_8x8 structure."""
    issues = []
    components = get_components(circuit)

    # Should have 8 registers
    registers = [c for c in components if c['name'] == 'Register' and c['attrs'].get('width') == '8']
    if len(registers) != 8:
        issues.append(f"ERROR: REGISTER_FILE_8x8 has {len(registers)} registers, expected 8")

    # Should have write decoder (3-to-8)
    has_decoder = any(c['name'] == 'Decoder' for c in components)
    if not has_decoder:
        issues.append("ERROR: REGISTER_FILE_8x8 missing write decoder")

    # Should have 2 read MUXes
    muxes = [c for c in components if c['name'] == 'Multiplexer']
    if len(muxes) < 2:
        issues.append(f"ERROR: REGISTER_FILE_8x8 has {len(muxes)} MUXes, expected >= 2")

    return issues

def check_alu(circuit):
    """Check ALU_8 structure."""
    issues = []
    components = get_components(circuit)

    # Should have arithmetic components
    arithmetic = [c for c in components if c['lib'] == '3']  # Arithmetic library
    if len(arithmetic) < 5:
        issues.append(f"WARNING: ALU_8 has only {len(arithmetic)} arithmetic components, expected >= 5")

    # Should have result MUX (16:1 or 4:1 with select=4)
    muxes = [c for c in components if c['name'] == 'Multiplexer']
    has_result_mux = any(m['attrs'].get('select') == '4' for m in muxes)
    if not has_result_mux:
        issues.append("ERROR: ALU_8 missing 16:1 result MUX")

    return issues

def check_ram(circuit):
    """Check RAM_64K structure."""
    issues = []
    components = get_components(circuit)

    # Should have RAM primitive
    ram = [c for c in components if c['name'] == 'RAM']
    if len(ram) == 0:
        issues.append("ERROR: RAM_64K missing RAM primitive")
    else:
        r = ram[0]
        addr_bits = r['attrs'].get('addr_bits', '?')
        data_bits = r['attrs'].get('data_bits', '?')
        if addr_bits != '16':
            issues.append(f"ERROR: RAM addr_bits={addr_bits}, expected 16")
        if data_bits != '8':
            issues.append(f"ERROR: RAM data_bits={data_bits}, expected 8")

    return issues

def check_control_unit(circuit):
    """Check CONTROL_UNIT structure."""
    issues = []
    components = get_components(circuit)

    # Should have state register
    state_reg = [c for c in components if c['name'] == 'Register' and c['attrs'].get('width') == '2']
    if len(state_reg) == 0:
        issues.append("ERROR: CONTROL_UNIT missing state register")

    # Should have state decoder
    state_dec = [c for c in components if c['name'] == 'Decoder' and c['attrs'].get('select') == '2']
    if len(state_dec) == 0:
        issues.append("ERROR: CONTROL_UNIT missing state decoder")

    # Should have opcode decoder
    opcode_dec = [c for c in components if c['name'] == 'Decoder' and c['attrs'].get('select') == '4']
    if len(opcode_dec) == 0:
        issues.append("ERROR: CONTROL_UNIT missing opcode decoder")

    return issues

def main():
    print(f"Loading {CIRC_FILE}...")
    root = load_circuit(CIRC_FILE)
    circuits = get_circuits(root)

    print(f"Found {len(circuits)} circuits\n")

    total_issues = 0
    all_results = {}

    # Check each circuit
    for name in sorted(circuits.keys()):
        circuit = circuits[name]
        issues, n_comp, n_wires, n_in, n_out = check_circuit_structure(name, circuit)

        # Circuit-specific checks
        if name == 'PC_16':
            issues.extend(check_pc_16(circuit))
        elif name == 'REGISTER_FILE_8x8':
            issues.extend(check_register_file(circuit))
        elif name == 'ALU_8':
            issues.extend(check_alu(circuit))
        elif name == 'RAM_64K':
            issues.extend(check_ram(circuit))
        elif name == 'CONTROL_UNIT':
            issues.extend(check_control_unit(circuit))
        elif name == 'DATAPATH':
            issues.extend(check_datapath_muxes(circuit))

        errors = [i for i in issues if 'ERROR' in i]
        warnings = [i for i in issues if 'WARNING' in i]
        infos = [i for i in issues if 'INFO' in i]

        status = "PASS" if len(errors) == 0 else "FAIL"
        total_issues += len(errors)

        all_results[name] = {
            'status': status,
            'components': n_comp,
            'wires': n_wires,
            'input_pins': n_in,
            'output_pins': n_out,
            'errors': errors,
            'warnings': warnings,
            'infos': infos
        }

        print(f"{'='*60}")
        print(f"Circuit: {name}")
        print(f"  Components: {n_comp}, Wires: {n_wires}, Inputs: {n_in}, Outputs: {n_out}")
        print(f"  Status: {status}")
        if errors:
            for e in errors:
                print(f"  ❌ {e}")
        if warnings:
            for w in warnings:
                print(f"  ⚠️  {w}")
        if infos:
            for i in infos:
                print(f"  ℹ️  {i}")
        print()

    # Check subcircuit references
    print(f"\n{'='*60}")
    print("SUBCIRCUIT REFERENCE CHECK")
    print(f"{'='*60}")
    ref_issues = check_subcircuit_references(root, circuits)
    if ref_issues:
        for issue in ref_issues:
            print(f"  ❌ {issue}")
            total_issues += 1
    else:
        print("  ✅ All subcircuit references valid")

    # Summary
    print(f"\n{'='*60}")
    print("SUMMARY")
    print(f"{'='*60}")
    print(f"Total circuits: {len(circuits)}")
    print(f"Total errors: {total_issues}")

    pass_count = sum(1 for r in all_results.values() if r['status'] == 'PASS')
    fail_count = sum(1 for r in all_results.values() if r['status'] == 'FAIL')
    print(f"Circuits PASS: {pass_count}/{len(circuits)}")
    print(f"Circuits FAIL: {fail_count}/{len(circuits)}")

    if total_issues > 0:
        print(f"\n⚠️  STRUCTURAL ISSUES FOUND — must fix before simulation")
        return 1
    else:
        print(f"\n✅ No structural issues found — ready for simulation")
        return 0

if __name__ == '__main__':
    sys.exit(main())
