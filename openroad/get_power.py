# !/usr/bin/env python3
"""
get_power.py
============

Utility to summarise *per-instance* total‐power numbers from an OpenSTA /
OpenROAD text dump created with::

    report_power ... > power_<corner>.txt

Features
--------
* Reads the flat dump **once**, filters only the instances whose full
  hierarchical name starts with a user-supplied *prefix*.
* Sums the “Total Power” column for all matching rows.
* Prints the *N* most power-hungry instances plus the grand total.

Typical usage::

    # default file & prefix, show top-20
    python3 get_power.py

    # choose another file, different prefix, show top-10
    python3 get_power.py -f power_ff.txt -p top/u_core/ -n 10
"""

from __future__ import annotations

import argparse
import pathlib
import sys
from typing import List, Tuple

# ── helpers ──────────────────────────────────────────────────────────

scriptdir = pathlib.Path(__file__).parent.resolve()

def is_numeric(token: str) -> bool:
    """
    Return ``True`` if *token* can be parsed as a float, else ``False``.

    The dump contains many header lines (e.g. ``Clock``, ``Sequential``)
    whose first field is non-numeric.  Those lines must be skipped during
    parsing, and a tiny helper is cleaner than a full try/except chain.
    """
    try:
        float(token)
        return True
    except ValueError:
        return False


def parse_power_dump(
    path: pathlib.Path, prefix: str
) -> Tuple[List[Tuple[float, str]], float]:
    """
    Scan *path* and collect the power numbers for every instance whose
    name begins with *prefix*.

    Parameters
    ----------
    path
        Path to the dump produced by ``report_power > file``.
    prefix
        Hierarchical prefix to match (case-sensitive).

    Returns
    -------
    instances
        List of ``(power, name)`` tuples **sorted in descending power**.
    total
        Sum of the “Total Power” column for all matching rows.
    """
    total = 0.0
    instances: List[Tuple[float, str]] = []

    with path.open() as fh:
        for raw in fh:
            cols = raw.strip().split()
            if len(cols) < 5 or not is_numeric(cols[0]):
                # Skip blank / dashed / header lines
                continue

            try:
                power_val = float(cols[3])  # 4th numeric col = Total
            except ValueError:
                # Should never happen once is_numeric() passed
                continue

            inst_name = cols[4]
            if inst_name.startswith(prefix):
                total += power_val
                instances.append((power_val, inst_name))

    instances.sort(reverse=True)
    return instances, total


def print_report(
    instances: List[Tuple[float, str]],
    total_power: float,
    prefix: str,
    dump_path: pathlib.Path,
    topn: int,
) -> None:
    """
    Nicely format the *instances* list and the *total_power* figure.

    Only the first *topn* entries of *instances* are shown (use ``-n 0``
    on the CLI to suppress the ranking entirely).
    """
    if topn > 0:
        shown = min(topn, len(instances))
        print(
            f"\nTop {shown} power consumers under '{prefix}' "
            f"in '{dump_path}':\n"
        )
        print(f"{'Power (W)':>12} | Instance")
        print("-" * 60)
        for power, inst in instances[:shown]:
            print(f"{power:12.5e} | {inst}")
        print("-" * 60)

    print(f"\nTOTAL power for prefix '{prefix}' = {total_power:.6g} W\n")


# ── main ─────────────────────────────────────────────────────────────


def main() -> None:
    """
    Parse command-line arguments, delegate parsing/printing to helpers.
    """
    parser = argparse.ArgumentParser(
        description=(
            "Summarise per-instance total power for any hierarchy prefix "
            "inside an OpenSTA / OpenROAD power dump."
        )
    )
    parser.add_argument(
        "-f",
        "--file",
        default=f"{scriptdir}/source/power_tt.txt",
        help="dump file from 'report_power > file'",
    )
    parser.add_argument(
        "-p",
        "--prefix",
        default="i_croc_soc/i_croc/i_pulser/",
        help="hierarchy prefix to sum",
    )
    parser.add_argument(
        "-n",
        "--topn",
        type=int,
        default=20,
        help="show N hottest instances (0 → none)",
    )
    args = parser.parse_args()

    dump_path = pathlib.Path(args.file)
    if not dump_path.exists():
        sys.exit(f"Error: dump file '{dump_path}' not found.")

    instances, total_power = parse_power_dump(dump_path, args.prefix)
    print_report(instances, total_power, args.prefix, dump_path, args.topn)


if __name__ == "__main__":
    main()
