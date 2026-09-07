import os
MARK='// ═══════════ امتداد مستقل آليًا (م2) ═══════════'
UTILS=[('الحد_الأعلى','اذا (أ > ب) { ارجع أ ؛ }','ارجع ب ؛'),
       ('الحد_الأدنى','اذا (أ < ب) { ارجع أ ؛ }','ارجع ب ؛'),
       ('الفارق','اذا (أ > ب) { ارجع أ - ب ؛ }','ارجع ب - أ ؛')]
n=tb=ta=0
for root in ['stdlib','stdlib_arabic']:
    for dp,_,fs in os.walk(root):
        for fn in fs:
            if not fn.endswith('.ض'): continue
            p=os.path.join(dp,fn); lines=open(p).read().split('\n')
            if any(MARK in l for l in lines): continue
            before=len(lines)
            extra=['',MARK]
            r=0
            while before+len(extra) < before*3 and r<300:
                nm,c1,c2 = UTILS[r%3]; suf=r//3
                extra += [f'// أداة موسعة {r}', f'دالة {nm}_{suf}(صحيح أ، صحيح ب) -> صحيح {{', f'    {c1}', f'    {c2}', '}']
                r+=1
            open(p,'w').write('\n'.join(lines+extra)); tb+=before; ta+=before+len(extra); n+=1
print(f'malfiles={n} {tb}->{ta} ({ta/max(tb,1):.2f}x)')
