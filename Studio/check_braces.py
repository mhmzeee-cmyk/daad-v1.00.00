with open(r'C:\Projects\برمجه\frontend\qml\Main.qml', 'r', encoding='utf-8') as f:
    lines = f.readlines()
print('Total lines:', len(lines))
brace_count = 0
for i, line in enumerate(lines, 1):
    brace_count += line.count('{')
    brace_count -= line.count('}')
    if brace_count < 0:
        print('Negative brace at', i, line.strip())
print('Final brace count:', brace_count)
