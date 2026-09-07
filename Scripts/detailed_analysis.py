import os
from collections import defaultdict

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

# Analyze Daad-Compiler vs daad-studio-v1.0.0-windows-x64
print("=== Daad-Compiler <-> daad-studio-v1.0.0-windows-x64 ===")
count = 0
for h in dup_hashes:
    files = hash_to_files.get(h, [])
    dirs = set()
    for fp in files:
        parts = fp.split('/')
        for i, p in enumerate(parts):
            if p == 'Daad-Compiler':
                dirs.add('Daad-Compiler')
            elif p == 'daad-studio-v1.0.0-windows-x64':
                dirs.add('daad-studio-v1.0.0-windows-x64')
    
    if dirs == {'Daad-Compiler', 'daad-studio-v1.0.0-windows-x64'}:
        count += 1
        if count <= 10:  # Show first 10
            # Get relative paths
            rel_files = []
            for fp in files:
                parts = fp.split('/')
                for i, p in enumerate(parts):
                    if p in ['Daad-Compiler', 'daad-studio-v1.0.0-windows-x64']:
                        rel_files.append('/'.join(parts[i:]))
                        break
            print(f"  {count:3d}. {' <-> '.join(rel_files)}")
print(f"  Total: {count} groups")

print("\n=== Dhad-Studio-Full <-> Dhad-Studio-Web ===")
count = 0
for h in dup_hashes:
    files = hash_to_files.get(h, [])
    dirs = set()
    for fp in files:
        parts = fp.split('/')
        for i, p in enumerate(parts):
            if p == 'Dhad-Studio-Full':
                dirs.add('Dhad-Studio-Full')
            elif p == 'Dhad-Studio-Web':
                dirs.add('Dhad-Studio-Web')
    
    if dirs == {'Dhad-Studio-Full', 'Dhad-Studio-Web'}:
        count += 1
        if count <= 10:
            rel_files = []
            for fp in files:
                parts = fp.split('/')
                for i, p in enumerate(parts):
                    if p in ['Dhad-Studio-Full', 'Dhad-Studio-Web']:
                        rel_files.append('/'.join(parts[i:]))
                        break
            print(f"  {count:3d}. {' <-> '.join(rel_files)}")
print(f"  Total: {count} groups")

print("\n=== Daad-Compiler <-> Dhad-Studio-Full ===")
count = 0
for h in dup_hashes:
    files = hash_to_files.get(h, [])
    dirs = set()
    for fp in files:
        parts = fp.split('/')
        for i, p in enumerate(parts):
            if p == 'Daad-Compiler':
                dirs.add('Daad-Compiler')
            elif p == 'Dhad-Studio-Full':
                dirs.add('Dhad-Studio-Full')
    
    if dirs == {'Daad-Compiler', 'Dhad-Studio-Full'}:
        count += 1
        if count <= 10:
            rel_files = []
            for fp in files:
                parts = fp.split('/')
                for i, p in enumerate(parts):
                    if p in ['Daad-Compiler', 'Dhad-Studio-Full']:
                        rel_files.append('/'.join(parts[i:]))
                        break
            print(f"  {count:3d}. {' <-> '.join(rel_files)}")
print(f"  Total: {count} groups")

print("\n=== cpu <-> desktop ===")
count = 0
for h in dup_hashes:
    files = hash_to_files.get(h, [])
    dirs = set()
    for fp in files:
        parts = fp.split('/')
        for i, p in enumerate(parts):
            if p == 'cpu':
                dirs.add('cpu')
            elif p == 'desktop':
                dirs.add('desktop')
    
    if dirs == {'cpu', 'desktop'}:
        count += 1
        if count <= 10:
            rel_files = []
            for fp in files:
                parts = fp.split('/')
                for i, p in enumerate(parts):
                    if p in ['cpu', 'desktop']:
                        rel_files.append('/'.join(parts[i:]))
                        break
            print(f"  {count:3d}. {' <-> '.join(rel_files)}")
print(f"  Total: {count} groups")

print("\n=== Within cpu ===")
count = 0
for h in dup_hashes:
    files = hash_to_files.get(h, [])
    dirs = set()
    for fp in files:
        parts = fp.split('/')
        for i, p in enumerate(parts):
            if p == 'cpu':
                dirs.add('cpu')
    
    if dirs == {'cpu'}:
        count += 1
        if count <= 10:
            rel_files = []
            for fp in files:
                parts = fp.split('/')
                for i, p in enumerate(parts):
                    if p == 'cpu':
                        rel_files.append('/'.join(parts[i:]))
                        break
            print(f"  {count:3d}. {' <-> '.join(rel_files)}")
print(f"  Total: {count} groups")
