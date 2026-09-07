#!/usr/bin/env python3
"""Generate DhadStudio WiX source for wixl. Deterministic GUIDs via uuid5 (stable across runs)."""
import os, uuid, xml.sax.saxutils as X

PKG = '/tmp/dhad-win7/pkg'
NS5 = uuid.NAMESPACE_URL
def guid(*parts):
    return str(uuid.uuid5(NS5, '|'.join(parts))).upper()

def fid(path):
    s = ''.join(c if c.isalnum() else '_' for c in path)
    return 'F_' + s[:60]

out = []
out.append('<?xml version="1.0" encoding="utf-8"?>')
out.append('<Wix xmlns="http://schemas.microsoft.com/wix/2006/wi">')
out.append(' <Product Id="%s" Name="Dhad Studio" Language="1033" Version="1.0.0" Manufacturer="Dhad Studio" UpgradeCode="%s">' % (
    guid('product','dhad-studio','1.0.0'), guid('upgrade','dhad-studio')))
out.append('  <Package InstallerVersion="200" Compressed="yes" InstallScope="perMachine" Description="Dhad Studio v1.0.0 - Arabic Programming Language (Windows 7 Interview Demo)" />')
out.append('  <Media Id="1" Cabinet="dhad.cab" EmbedCab="yes" />')
out.append('  <Property Id="ARPCOMMENTS" Value="Arabic programming language demo: compiler, programs, CPU simulator, examples and docs." />')
out.append('  <Directory Id="TARGETDIR" Name="SourceDir">')
out.append('   <Directory Id="ProgramFilesFolder">')
out.append('    <Directory Id="INSTALLDIR" Name="DhadStudio">')

# collect payload: (msi_subdir or None for root, relpath)
payload = []
for root, _, files in os.walk(PKG):
    for fn in sorted(files):
        full = os.path.join(root, fn)
        rel = os.path.relpath(full, PKG)
        parts = rel.split(os.sep)
        sub = parts[0] if len(parts) > 1 else None
        payload.append((sub, rel))

subdirs = sorted({s for s, _ in payload if s})
for s in subdirs:
    out.append('     <Directory Id="DIR_%s" Name="%s" />' % (s.upper(), s))
out.append('    </Directory>')
out.append('   </Directory>')
out.append('   <Directory Id="ProgramMenuFolder"><Directory Id="MenuDhad" Name="Dhad Studio" /></Directory>')
out.append('   <Directory Id="DesktopFolder" />')
out.append('  </Directory>')

out.append('  <Feature Id="MainFeature" Title="Dhad Studio" Level="1">')
comp_ids = []
# root files
for sub, rel in payload:
    if sub is not None:
        continue
    cid = 'C_' + fid(rel)
    comp_ids.append(cid)
    src = 'var.PayloadDir/' + rel.replace(os.sep, '/')
    extra = ''
    if rel.lower() == 'demo-dhad-studio.bat':
        extra = ('<Shortcut Id="ShMenu" Directory="MenuDhad" Name="Dhad Studio Demo" '
                 'Target="[INSTALLDIR]Demo-Dhad-Studio.bat" WorkingDirectory="INSTALLDIR" />'
                 '<Shortcut Id="ShDesk" Directory="DesktopFolder" Name="Dhad Studio Demo" '
                 'Target="[INSTALLDIR]Demo-Dhad-Studio.bat" WorkingDirectory="INSTALLDIR" />'
                 '<RemoveFolder Id="RmMenu" Directory="MenuDhad" On="uninstall" />')
    out.append('  <Component Id="%s" Guid="%s" Directory="INSTALLDIR">'
               '<File Id="%s" Source="%s" />%s</Component>' % (cid, guid('comp', rel), fid(rel), src, extra))
# subdir files
for s in subdirs:
    for sub, rel in payload:
        if sub != s:
            continue
        cid = 'C_' + fid(rel)
        comp_ids.append(cid)
        src = 'var.PayloadDir/' + rel.replace(os.sep, '/')
        out.append('  <Component Id="%s" Guid="%s" Directory="DIR_%s">'
                   '<File Id="%s" Source="%s" /></Component>' % (cid, guid('comp', rel), s.upper(), fid(rel), src))
for cid in comp_ids:
    out.append('   <ComponentRef Id="%s" />' % cid)
out.append('  </Feature>')
out.append(' </Product>')
out.append('</Wix>')
open('/tmp/dhad-win7/dhadstudio.wxs', 'w', encoding='utf-8').write('\n'.join(out) + '\n')
print('components:', len(comp_ids))
