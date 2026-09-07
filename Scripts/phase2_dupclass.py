hash_to_files = {}
with open('/tmp/phase2_hashes.txt', encoding='utf-8', errors='replace') as f:
    for line in f:
        parts = line.rstrip('\n').split('  ', 1)
        if len(parts) == 2:
            hash_to_files.setdefault(parts[0], []).append(parts[1])
with open('/tmp/phase2_dups.txt') as f:
    dups = [l.strip() for l in f if l.strip()]
from collections import Counter
cats = Counter()
examples = {}
for h in dups:
    files = hash_to_files[h]
    comps = set()
    for fp in files:
        for c in ['Daad-Compiler','Dhad-Studio-Full','Dhad-Studio-Web','Dhad-Studio-Electron','VSCode-Extension','cpu','daad-studio-v1.0.0-windows-x64']:
            if ('/'+c+'/') in fp or fp.endswith('/'+c):
                comps.add(c); break
        else:
            comps.add('ROOT-OR-OTHER')
    key = ' <-> '.join(sorted(comps))
    cats[key] += 1
    examples.setdefault(key, []).append(files)
print(f"groups={len(dups)} files_in_dups={sum(len(hash_to_files[h]) for h in dups)}")
for k, v in cats.most_common():
    print(f"{v:4d} : {k}")
    for fs in examples[k][:2]:
        print("     e.g. " + " | ".join(x.split('26_0/\u00a0/')[-1][:80] for x in fs))
