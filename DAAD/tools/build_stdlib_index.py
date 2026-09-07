#!/usr/bin/env python3
"""م1: محلل تواقيع المكتبة → stdlib_index.json"""
import os, re, json, sys

ROOT = os.path.join(os.path.dirname(os.path.abspath(__file__)), '..')
TYPES = r'[\w\u0600-\u06FF\[\]]+'
NAME  = r'[\w\u0600-\u06FF]+'
SIG = re.compile(rf'دالة\s+({NAME})\s*\(([^)]*)\)\s*(?:->\s*({TYPES}))?\s*\{{')

def parse_params(raw):
    if not raw.strip(): return []
    out = []
    for part in raw.split('،') if '،' in raw else raw.split(','):
        toks = part.split()
        if not toks: continue
        if len(toks) >= 2:
            # نمطا الصيغتين: "النوع الاسم" أو "الاسم : النوع"
            out.append({'type': toks[0], 'name': toks[1]})
        else:
            out.append({'type': 'صحيح', 'name': toks[0]})  # كلمة واحدة = اسم
    return out

index = []
for root in ['stdlib', 'stdlib_arabic']:
    base = os.path.join(ROOT, root)
    for dp, _, fs in os.walk(base):
        for fn in sorted(fs):
            if not fn.endswith('.ض'): continue
            path = os.path.join(dp, fn)
            rel  = os.path.relpath(path, ROOT)
            src  = open(path, encoding='utf-8').read()
            for m in SIG.finditer(src):
                ret = m.group(3) or 'فراغ'
                index.append({
                    'name': m.group(1), 'file': rel.replace('\\','/'),
                    'params': parse_params(m.group(2)), 'returns': ret,
                })
out_path = os.path.join(ROOT, 'stdlib_index.json')
json.dump(index, open(out_path,'w',encoding='utf-8'), ensure_ascii=False, indent=1)
cats = {}
for e in index: cats[e['returns']] = cats.get(e['returns'],0)+1
print(f"دوال مفهرسة: {len(index)} | ملفات: {len(set(e['file'] for e in index))}")
print("أعلى أنواع الإرجاع:", sorted(cats.items(), key=lambda x:-x[1])[:5])
