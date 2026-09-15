#!/bin/bash
# Original: export BASE="/run/media/m_hmoz/UBUNTU 26_0/"$'\xc2\xa0'  (external drive)
export BASE="${DAAD_BASE:-$(cd "$(dirname "$0")/.." && pwd)}"
find "$BASE" -type f \
    -not -path "*/node_modules/*" \
    -not -path "*/.Trash-1000/*" \
    -not -path "*/System Volume Information/*" \
    -not -name "*.zip" \
    -not -name "*.lnk" \
    -not -name "desktop.ini" \
    -not -name "IndexerVolumeGuid" \
    2>/dev/null | while IFS= read -r f; do
        sha256sum "$f" 2>/dev/null
    done > /tmp/hashes_clean.txt
