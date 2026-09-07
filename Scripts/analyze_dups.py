import sys

# Read hashes
hash_to_files = {}
with open('/tmp/hashes_clean.txt') as f:
    for line in f:
        parts = line.strip().split('  ', 1)
        if len(parts) == 2:
            h, filepath = parts
            if h not in hash_to_files:
                hash_to_files[h] = []
            hash_to_files[h].append(filepath)

# Read duplicate hashes
with open('/tmp/dup_hashes_clean.txt') as f:
    dup_hashes = [line.strip() for line in f if line.strip()]

print(f"Total duplicate hash groups: {len(dup_hashes)}")

# Analyze each group
from collections import defaultdict
pair_counts = defaultdict(int)
within_counts = defaultdict(int)
total_dup_files = 0

for h in dup_hashes:
    files = hash_to_files.get(h, [])
    if len(files) < 2:
        continue
    
    total_dup_files += len(files)
    
    # Get component names
    components = set()
    for fp in files:
        parts = fp.split('/')
        # Find the component name (after the base path)
        for i, p in enumerate(parts):
            if p in ['Daad-Compiler', 'Dhad-Studio-Full', 'Dhad-Studio-Web', 
                      'Dhad-Studio-Electron', 'VSCode-Extension', 'cpu', 'desktop',
                      'daad-studio-v1.0.0-windows-x64']:
                components.add(p)
                break
    
    comps = sorted(components)
    if len(comps) == 1:
        within_counts[comps[0]] += 1
    elif len(comps) == 2:
        pair_key = f"{comps[0]} <-> {comps[1]}"
        pair_counts[pair_key] += 1
    else:
        pair_key = f"Across {len(comps)} dirs"
        pair_counts[pair_key] += 1

print(f"\nTotal files in duplicate groups: {total_dup_files}")
print(f"\n=== Within-component duplicates ===")
for comp, count in sorted(within_counts.items(), key=lambda x: -x[1]):
    print(f"  {count:4d} groups in {comp}")

print(f"\n=== Cross-component duplicates ===")
for pair, count in sorted(pair_counts.items(), key=lambda x: -x[1]):
    print(f"  {count:4d} groups: {pair}")
