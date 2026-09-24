# Dependency Review Process — ض (Daad)

## Purpose
This document defines the process for reviewing and approving new
third-party dependencies before they are added to the ض project.

## Review Steps
### 1. License Identification
- Identify the exact license of the dependency.
- Check the SPDX identifier.

### 2. Compatibility Check
- Cross-reference with the compatibility matrix in `THIRD_PARTY_NOTICES.md`.
- **Auto-reject**: GPL-2.0, GPL-3.0, AGPL-3.0, SSPL, EUPL (for core components).
- **Auto-approve**: MIT, BSD-2, BSD-3, Apache-2.0, ISC, Zlib, Unlicense.
