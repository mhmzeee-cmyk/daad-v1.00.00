
#!/usr/bin/env python3
"""
Spatial-RT architecture simulator
-------------------------------
Educational / architectural model, NOT a model of proprietary NVIDIA RT hardware.

Simulates:
  * 1 / 1K / 1M / 10M triangles
  * binary BVH depth and node storage
  * 128-byte memory transactions
  * configurable cache size / line size / hit rate
  * DRAM bandwidth
  * PCIe transfer of compact ray results
  * SSD asset loading
  * decode bandwidth
  * BLAS build as a configurable bandwidth+overhead model
  * traditional pipeline vs persistent spatial accelerator

Run:
  python3 spatial_sim.py
"""

from dataclasses import dataclass
import argparse
import math

@dataclass
class Config:
    # Geometry model
    position_bytes: int = 12
    index_bytes: int = 4
    vertex_reuse: float = 1.5

    # Spatial structure model
    bvh_node_bytes: int = 64
    fetch_bytes: int = 128
    avg_node_visits: int | None = None  # if None -> ceil(log2(N)) for N>1

    # Memory / links
    dram_bw_gbps: float = 448.0
    accelerator_dram_bw_gbps: float = 256.0
    pcie_bw_gbps: float = 15.75       # PCIe 3 x16 usable-ish model
    ssd_bw_gbps: float = 5.0
    decode_bw_gbps: float = 50.0

    # Cache model
    cache_hit_rate: float = 0.50

    # Build model
    blas_extra_read_multiplier: float = 1.5
    blas_fixed_ms: float = 0.10

    # Frame/query model
    rays_per_frame: int = 1920 * 1080
    fps: int = 60
    bytes_per_result: int = 8

    # Accelerator assumptions
    spatial_blocks_per_ray: int = 3
    block_bytes: int = 128

def geometry_bytes(triangles, c: Config | None = None):
    c = c or Config()
    vertices = math.ceil((3 * triangles) / c.vertex_reuse)
    positions = vertices * c.position_bytes
    indices = triangles * 3 * c.index_bytes
    return positions + indices

def bvh_nodes(triangles):
    return max(1, 2 * triangles - 1)

def bvh_bytes(triangles, c: Config | None = None):
    c = c or Config()
    return bvh_nodes(triangles) * c.bvh_node_bytes

def compressed_bytes(triangles, bytes_per_triangle=3.5):
    return triangles * bytes_per_triangle

def node_visits(triangles, c: Config | None = None):
    c = c or Config()
    if triangles <= 1:
        return 1
    if c.avg_node_visits is not None:
        return c.avg_node_visits
    return math.ceil(math.log2(triangles))

def ms_for_bytes(nbytes, gbps):
    return (nbytes / (gbps * 1e9)) * 1000.0

def traditional(triangles, c: Config | None = None):
    c = c or Config()
    raw = geometry_bytes(triangles, c)
    compressed = compressed_bytes(triangles)

    # SSD -> memory
    ssd_ms = ms_for_bytes(compressed, c.ssd_bw_gbps)

    # compressed transfer to GPU over PCIe
    pcie_upload_ms = ms_for_bytes(compressed, c.pcie_bw_gbps)

    # decode compressed -> raw
    decode_ms = ms_for_bytes(raw, c.decode_bw_gbps)

    # Simplified BLAS model:
    # read geometry multiple times + construction overhead
    blas_read = raw * c.blas_extra_read_multiplier
    blas_ms = ms_for_bytes(blas_read, c.dram_bw_gbps) + c.blas_fixed_ms

    # TLAS for one asset
    tlas_ms = 0.05

    visits = node_visits(triangles, c)

    # Every cache miss is modeled as one 128B transaction.
    miss_rate = 1.0 - c.cache_hit_rate
    bvh_transactions_per_ray = visits * miss_rate
    bvh_bytes_per_ray = bvh_transactions_per_ray * c.fetch_bytes

    triangle_bytes_per_ray = c.fetch_bytes
    rt_bytes_per_ray = bvh_bytes_per_ray + triangle_bytes_per_ray

    rt_bytes_per_frame = rt_bytes_per_ray * c.rays_per_frame
    rt_bw_required = rt_bytes_per_frame * c.fps / 1e9

    return {
        "triangles": triangles,
        "raw_mb": raw / 1e6,
        "compressed_mb": compressed / 1e6,
        "bvh_mb": bvh_bytes(triangles, c) / 1e6,
        "ssd_ms": ssd_ms,
        "pcie_upload_ms": pcie_upload_ms,
        "decode_ms": decode_ms,
        "blas_ms": blas_ms,
        "tlas_ms": tlas_ms,
        "node_visits": visits,
        "bvh_bytes_per_ray": bvh_bytes_per_ray,
        "rt_bytes_per_ray": rt_bytes_per_ray,
        "rt_gbps_required": rt_bw_required,
        "rt_frame_gb": rt_bytes_per_frame / 1e9,
    }

def accelerator(triangles, c: Config | None = None):
    c = c or Config()
    compressed = compressed_bytes(triangles)
    # Only cold-storage load is paid when the asset is first resident.
    ssd_ms = ms_for_bytes(compressed, c.ssd_bw_gbps)

    # No geometry upload over PCIe; compact query results only.
    pcie_result_gbps = (c.rays_per_frame * c.fps * c.bytes_per_result) / 1e9
    pcie_result_ms_per_frame = ms_for_bytes(
        c.rays_per_frame * c.bytes_per_result, c.pcie_bw_gbps
    )

    # On-demand spatial blocks.
    bytes_per_ray = c.spatial_blocks_per_ray * c.block_bytes
    frame_bytes = bytes_per_ray * c.rays_per_frame
    required_bw = frame_bytes * c.fps / 1e9

    # Accelerator DRAM lower-bound time for one frame.
    dram_ms = ms_for_bytes(frame_bytes, c.accelerator_dram_bw_gbps)

    return {
        "triangles": triangles,
        "compressed_mb": compressed / 1e6,
        "ssd_ms": ssd_ms,
        "pcie_result_gbps": pcie_result_gbps,
        "pcie_result_ms_per_frame": pcie_result_ms_per_frame,
        "spatial_bytes_per_ray": bytes_per_ray,
        "spatial_gbps_required": required_bw,
        "spatial_frame_gb": frame_bytes / 1e9,
        "accelerator_dram_lower_bound_ms": dram_ms,
    }

def fmt(x):
    if isinstance(x, int):
        return str(x)
    if abs(x) >= 1000:
        return f"{x:,.0f}"
    if abs(x) >= 100:
        return f"{x:,.1f}"
    return f"{x:,.3f}"

def print_report(c):
    cases = [1, 1_000, 1_000_000, 10_000_000]
    print("\n=== SPATIAL RT SIMULATOR ===")
    print("Model assumptions:")
    print(f"  fetch line:              {c.fetch_bytes} B")
    print(f"  cache hit rate:          {c.cache_hit_rate:.0%}")
    print(f"  GPU/DRAM bandwidth:      {c.dram_bw_gbps} GB/s")
    print(f"  accelerator DRAM:        {c.accelerator_dram_bw_gbps} GB/s")
    print(f"  PCIe:                    {c.pcie_bw_gbps} GB/s")
    print(f"  SSD:                     {c.ssd_bw_gbps} GB/s")
    print(f"  decode:                  {c.decode_bw_gbps} GB/s")
    print(f"  rays/frame:              {c.rays_per_frame:,}")
    print(f"  FPS:                     {c.fps}")
    print()

    header = (
        "Triangles | Raw MB | Comp MB | BVH MB | "
        "RT B/ray | RT GB/s | SSD ms | Decode ms | BLAS ms"
    )
    print(header)
    print("-" * len(header))

    for n in cases:
        t = traditional(n, c)
        print(
            f"{n:>9,} | {fmt(t['raw_mb']):>7} | {fmt(t['compressed_mb']):>8} | "
            f"{fmt(t['bvh_mb']):>7} | {fmt(t['rt_bytes_per_ray']):>9} | "
            f"{fmt(t['rt_gbps_required']):>8} | {fmt(t['ssd_ms']):>7} | "
            f"{fmt(t['decode_ms']):>10} | {fmt(t['blas_ms']):>8}"
        )

    print("\n=== ACCELERATOR MODEL ===")
    print("Assumes 3 x 128B spatial blocks/ray and 8B result/ray.")
    for n in cases:
        a = accelerator(n, c)
        print(
            f"{n:>9,} triangles: "
            f"{a['spatial_bytes_per_ray']} B/ray, "
            f"{a['spatial_gbps_required']:.1f} GB/s spatial traffic, "
            f"{a['pcie_result_gbps']:.2f} GB/s PCIe results, "
            f"{a['accelerator_dram_lower_bound_ms']:.3f} ms DRAM lower bound/frame"
        )

    print("\n=== CACHE SWEEP: 1M TRIANGLES ===")
    print("Hit rate | RT B/ray | Required RT GB/s @60 FPS")
    print("-" * 48)
    for hit in [0.0, 0.25, 0.50, 0.75, 0.90, 0.95, 0.99]:
        cc = Config()
        cc.cache_hit_rate = hit
        t = traditional(1_000_000, cc)
        print(f"{hit:>7.0%} | {t['rt_bytes_per_ray']:>9,.0f} | {t['rt_gbps_required']:>22.1f}")

    print("\nNOTE:")
    print("This is a first-order architecture simulator. It does NOT claim")
    print("to reproduce proprietary GPU cache/BVH internals or real benchmark timings.")
    print("Use it to compare architectures and identify bottlenecks before hardware.")

if __name__ == "__main__":
    p = argparse.ArgumentParser()
    p.add_argument("--cache-hit", type=float, default=0.50,
                   help="cache hit rate, e.g. 0.75")
    p.add_argument("--pcie", type=float, default=15.75,
                   help="PCIe usable GB/s")
    p.add_argument("--dram", type=float, default=448.0,
                   help="GPU DRAM GB/s")
    p.add_argument("--accel-dram", type=float, default=256.0,
                   help="accelerator local DRAM GB/s")
    p.add_argument("--ssd", type=float, default=5.0,
                   help="SSD sequential GB/s")
    p.add_argument("--decode", type=float, default=50.0,
                   help="decode throughput GB/s")
    args = p.parse_args()

    c = Config()
    c.cache_hit_rate = args.cache_hit
    c.pcie_bw_gbps = args.pcie
    c.dram_bw_gbps = args.dram
    c.accelerator_dram_bw_gbps = args.accel_dram
    c.ssd_bw_gbps = args.ssd
    c.decode_bw_gbps = args.decode

    print_report(c)
