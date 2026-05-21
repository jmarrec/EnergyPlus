#!/usr/bin/env python3
# EnergyPlus, Copyright (c) 1996-present, The Board of Trustees of the
# University of Illinois, The Regents of the University of California, through
# Lawrence Berkeley National Laboratory (subject to receipt of any required
# approvals from the U.S. Dept. of Energy), Oak Ridge National Laboratory,
# managed by UT-Battelle, Alliance for Energy Innovation, LLC, and other
# contributors. All rights reserved.
#
# NOTICE: This Software was developed under funding from the U.S. Department of
# Energy and the U.S. Government consequently retains certain rights. As such,
# the U.S. Government has been granted for itself and others acting on its
# behalf a paid-up, nonexclusive, irrevocable, worldwide license in the
# Software to reproduce, distribute copies to the public, prepare derivative
# works, and perform publicly and display publicly, and to permit others to do
# so.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
#
# (1) Redistributions of source code must retain the above copyright notice,
#     this list of conditions and the following disclaimer.
#
# (2) Redistributions in binary form must reproduce the above copyright notice,
#     this list of conditions and the following disclaimer in the documentation
#     and/or other materials provided with the distribution.
#
# (3) Neither the name of the University of California, Lawrence Berkeley
#     National Laboratory, the University of Illinois, U.S. Dept. of Energy nor
#     the names of its contributors may be used to endorse or promote products
#     derived from this software without specific prior written permission.
#
# (4) Use of EnergyPlus(TM) Name. If Licensee (i) distributes the software in
#     stand-alone form without changes from the version obtained under this
#     License, or (ii) Licensee makes a reference solely to the software
#     portion of its product, Licensee must refer to the software as
#     "EnergyPlus version X" software, where "X" is the version number Licensee
#     obtained under this License and may not use a different name for the
#     software. Except as specifically required in this Section (4), Licensee
#     shall not use in a company name, a product name, in advertising,
#     publicity, or other promotional materials any name, trade name,
#     trademark, logo, or other designation of "EnergyPlus", "E+", "e+" or
#     confusingly similar designation, without the U.S. Department of Energy's
#     prior written consent.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
# AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
# ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
# LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
# CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
# SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
# INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
# CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
# ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
# POSSIBILITY OF SUCH DAMAGE.

"""
Insert the new "Default Construction Set Name" blank field (position 7) into
Space objects that have Tag fields.

Old Space layout (1-based data fields; class line is index 0):
  1  Name
  2  Zone Name
  3  Ceiling Height {m}
  4  Volume {m3}
  5  Floor Area {m2}
  6  Space Type              ← min-fields 6 ends here
  7  Tag 1  (extensible, optional)
  8  Tag 2
  9  Tag 3

New Space layout:
  1  Name
  2  Zone Name
  3  Ceiling Height {m}
  4  Volume {m3}
  5  Floor Area {m2}
  6  Space Type
  7  Default Construction Set Name   ← NEW (blank)
  8  Tag 1  (extensible, optional)
  9  Tag 2
  10 Tag 3

Space objects with no tags (exactly 6 data fields) are left unchanged — the
blank field is a trailing optional and the Fortran transition tool leaves those
at 6 fields too.  epJSON files are also left unchanged because fields are
keyed by name; the new optional field is simply absent.

Usage:
    # Process all IDF/IMF testfiles and C++ unit tests (default):
    python scripts/dev/update_space_field.py

    # Process specific files:
    python scripts/dev/update_space_field.py path/to/file.idf path/to/test.unit.cc
"""

import argparse
import re
from pathlib import Path

# ---------------------------------------------------------------------------
# Shared IDF field regex
# ---------------------------------------------------------------------------

# Matches a single IDF field or class-name line, e.g.:
#   "    Zone1 Space1,         !- Name"
#   "  Space,"
#   "    autocalculate,        !- Ceiling Height {m}"
# Group 1: leading whitespace
# Group 2: field value (stripped)
# Group 3: terminator (',' or ';')
# Group 4: comment after '!-' (or None)
IDF_FIELD_RE = re.compile(r"^(\s*)(.*?)\s*([,;])\s*(?:!-\s*(.*))?$")

# Number of non-extensible data fields in the old Space IDD (before our change).
# Tag fields begin at data field 7.
_SPACE_FIXED_FIELDS = 6

# Comment text for the new blank field.
_NEW_FIELD_COMMENT = "Default Construction Set Name"


# ---------------------------------------------------------------------------
# IDF helpers
# ---------------------------------------------------------------------------


def parse_idf_line(line):
    """Return (indent, value, terminator, comment_or_None) or None."""
    m = IDF_FIELD_RE.match(line.rstrip("\n\r"))
    if m:
        return m.group(1), m.group(2), m.group(3), m.group(4)
    return None


def is_space_class_line(parsed):
    """True iff this line is the 'Space,' class opener (not a field value)."""
    indent, value, term, comment = parsed
    # Class-name lines have no comment; field-value lines always have '!-'
    return term == "," and comment is None and value == "Space"


def terminates_object(parsed):
    return parsed[2] == ";"


def is_already_converted(block):
    """True if data field 7 (block index 7) already has the new field comment."""
    if len(block) <= 7:
        return False
    p = parse_idf_line(block[7])
    return p is not None and p[3] is not None and _NEW_FIELD_COMMENT in p[3]


def find_comment_col(block):
    """Return median '!-' column among field lines, or 33 as fallback."""
    cols = []
    for line in block[1:]:
        idx = line.find("!-")
        if idx > 0:
            cols.append(idx)
    if cols:
        cols.sort()
        return cols[len(cols) // 2]
    return 33


def transform_space_block(block):
    """
    Insert a blank 'Default Construction Set Name' field at data-field position 7.

    Returns a new list of line strings, or None if the block should not be
    modified (no tags, or already converted, or parse error).
    """
    parsed = [parse_idf_line(ln) for ln in block]
    if any(p is None for p in parsed):
        return None

    # block[0] = class line; block[1..6] = data fields 1-6; block[7+] = tags
    # Only modify if there are tag fields (total lines > 7).
    if len(parsed) <= 7:
        return None

    if is_already_converted(block):
        return None

    eol = "\r\n" if block[0].endswith("\r\n") else "\n"
    comment_col = find_comment_col(block)
    field_indent = parsed[1][0]

    def make_field_line(value, term, comment_text):
        vp = f"{field_indent}{value}{term}"
        if comment_text:
            pad = max(2, comment_col - len(vp))
            return f"{vp}{' ' * pad}!- {comment_text}{eol}"
        return f"{vp}{eol}"

    # Fields 1-6 (indices 0-6 in block) stay identical, but field 6 (Space Type,
    # block index 6) may have been the last field (';') — change to ',' since the
    # new blank field now follows it.
    result = list(block[:6])  # class line + data fields 1-5 unchanged

    # Field 6: Space Type — ensure terminator is ','
    st_indent, st_val, st_term, st_comment = parsed[6]
    st_line = f"{st_indent}{st_val},"
    if st_comment:
        pad = max(2, comment_col - len(st_line))
        st_line = f"{st_line}{' ' * pad}!- {st_comment}{eol}"
    else:
        st_line = st_line + eol
    result.append(st_line)

    # New blank field 7: Default Construction Set Name (always ',', tags follow)
    result.append(make_field_line("", ",", _NEW_FIELD_COMMENT))

    # Tag fields (old block indices 7+) appended unchanged
    result.extend(block[7:])

    return result


# ---------------------------------------------------------------------------
# epJSON file processing
# ---------------------------------------------------------------------------


def process_epjson_file(filepath):
    """
    Process a single .epJSON file.  Returns number of Space objects found with tags.

    epJSON fields are keyed by name, so no positional insertion is needed — the new
    optional 'default_construction_set_name' field is simply absent from existing
    objects, which is valid.  This function exists so that future replays of this
    script on files that do contain Space objects are handled explicitly rather than
    silently skipped.
    """
    import json

    filepath = Path(filepath)
    try:
        data = json.loads(filepath.read_text(encoding="utf-8"))
    except json.JSONDecodeError as exc:
        print(f"  WARNING: could not parse {filepath.name} as JSON: {exc}")
        return 0

    spaces = data.get("Space", {})
    if not spaces:
        return 0

    tagged = 0
    for name, obj in spaces.items():
        has_tags = any(k in obj for k in ("tag_1", "tag_2", "tag_3"))
        has_dcs = "default_construction_set_name" in obj
        if has_tags and not has_dcs:
            # No action needed: JSON is name-keyed; the absent field is fine.
            tagged += 1

    if tagged:
        print(
            f"  {filepath.name}: {tagged} Space object(s) with tags — "
            f"no change needed (epJSON fields are name-keyed)"
        )
    return tagged


# ---------------------------------------------------------------------------
# IDF/IMF file processing
# ---------------------------------------------------------------------------


def process_idf_file(filepath):
    """Process a single .idf/.imf file in place. Returns number of Space blocks updated."""
    filepath = Path(filepath)
    lines = filepath.read_text(encoding="utf-8", errors="replace").splitlines(keepends=True)

    output = []
    i = 0
    num_updated = 0

    while i < len(lines):
        line = lines[i]
        parsed = parse_idf_line(line)

        if parsed and is_space_class_line(parsed):
            # Collect the complete object until the terminating ';' field
            block = [line]
            j = i + 1
            while j < len(lines):
                next_line = lines[j]
                next_parsed = parse_idf_line(next_line)
                if next_parsed:
                    block.append(next_line)
                    j += 1
                    if terminates_object(next_parsed):
                        break
                else:
                    stripped = next_line.strip()
                    if stripped == "" or stripped.startswith("!"):
                        block.append(next_line)
                        j += 1
                    else:
                        break

            new_block = transform_space_block(block)
            if new_block is not None:
                output.extend(new_block)
                num_updated += 1
            else:
                output.extend(block)
            i = j
            continue

        output.append(line)
        i += 1

    if num_updated > 0:
        filepath.write_text("".join(output), encoding="utf-8")

    return num_updated


# ---------------------------------------------------------------------------
# C++ unit test file processing
# ---------------------------------------------------------------------------

# Matches a C++ line containing a single quoted IDF string, e.g.:
#   '        "    Zone1 Space1,         !- Name",'
# Group 1: leading whitespace + opening quote
# Group 2: IDF content between the quotes
# Group 3: closing quote + trailing comma/whitespace
CPP_STRING_RE = re.compile(r'^(\s*")(.*?)("[\s,]*)$')


def parse_cpp_line(line):
    """Return (prefix, idf_content, suffix) or None."""
    m = CPP_STRING_RE.match(line.rstrip("\n\r"))
    if m:
        return m.group(1), m.group(2), m.group(3)
    return None


def is_space_class_line_cpp(idf_content):
    """True iff the IDF content of a C++ string line is the 'Space,' class opener."""
    return bool(re.match(r"^\s*Space\s*,\s*$", idf_content))


def terminates_object_cpp(idf_content):
    """True iff the field in this C++ string line ends with ';'."""
    before_comment = idf_content.split("!-")[0] if "!-" in idf_content else idf_content
    return before_comment.rstrip().endswith(";")


def is_already_converted_cpp(block):
    """True if data field 7 (block index 7) already has the new field comment."""
    if len(block) <= 7:
        return False
    p = parse_cpp_line(block[7])
    return p is not None and _NEW_FIELD_COMMENT in p[1]


def find_comment_col_cpp(block):
    """Return median '!-' column (within the IDF content) among block lines."""
    cols = []
    for line in block[1:]:
        p = parse_cpp_line(line)
        if p:
            idx = p[1].find("!-")
            if idx > 0:
                cols.append(idx)
    if cols:
        cols.sort()
        return cols[len(cols) // 2]
    return 33


def make_cpp_field_line(prefix, value, term, comment_text, comment_col, suffix, eol):
    """Build a C++ string line wrapping an IDF field."""
    vp = f"{value}{term}"
    if comment_text:
        pad = max(2, comment_col - len(vp))
        idf_content = f"{vp}{' ' * pad}!- {comment_text}"
    else:
        idf_content = vp
    return f"{prefix}{idf_content}{suffix}{eol}"


def transform_space_block_cpp(block):
    """
    Insert a blank 'Default Construction Set Name' field at data-field position 7
    within a C++ unit test string block.

    Returns new list of line strings, or None if not applicable.
    """
    if len(block) <= 7:
        return None

    if is_already_converted_cpp(block):
        return None

    eol = "\r\n" if block[0].endswith("\r\n") else "\n"
    comment_col = find_comment_col_cpp(block)

    result = list(block[:6])  # class line + data fields 1-5 unchanged

    # Field 6: Space Type — ensure terminator is ','
    st_parsed = parse_cpp_line(block[6])
    if st_parsed is None:
        return None
    st_prefix, st_idf, st_suffix = st_parsed
    before_comment = st_idf.split("!-")[0] if "!-" in st_idf else st_idf
    st_comment = st_idf.split("!-", 1)[1].strip() if "!-" in st_idf else None
    st_bare = before_comment.rstrip().rstrip(";").rstrip(",")
    new_st_vp = f"{st_bare},"
    if st_comment:
        pad = max(2, comment_col - len(new_st_vp))
        new_st_idf = f"{new_st_vp}{' ' * pad}!- {st_comment}"
    else:
        new_st_idf = new_st_vp
    result.append(f"{st_prefix}{new_st_idf}{st_suffix}{eol}")

    # New blank field 7 — infer prefix/suffix from an existing field line
    ref_parsed = parse_cpp_line(block[1])
    if ref_parsed is None:
        return None
    ref_prefix, _, ref_suffix = ref_parsed
    result.append(make_cpp_field_line(ref_prefix, "", ",", _NEW_FIELD_COMMENT, comment_col, ref_suffix, eol))

    # Tag fields unchanged
    result.extend(block[7:])

    return result


def process_cpp_file(filepath):
    """Process a single .cc file in place. Returns number of Space blocks updated."""
    filepath = Path(filepath)
    lines = filepath.read_text(encoding="utf-8", errors="replace").splitlines(keepends=True)

    output = []
    i = 0
    num_updated = 0

    while i < len(lines):
        line = lines[i]
        cpp_parsed = parse_cpp_line(line)

        if cpp_parsed and is_space_class_line_cpp(cpp_parsed[1]):
            block = [line]
            j = i + 1
            while j < len(lines):
                next_line = lines[j]
                next_cpp = parse_cpp_line(next_line)
                if next_cpp:
                    block.append(next_line)
                    j += 1
                    if terminates_object_cpp(next_cpp[1]):
                        break
                else:
                    stripped = next_line.strip()
                    if stripped == "" or stripped.startswith("//") or stripped.startswith("!"):
                        block.append(next_line)
                        j += 1
                    else:
                        break

            new_block = transform_space_block_cpp(block)
            if new_block is not None:
                output.extend(new_block)
                num_updated += 1
            else:
                output.extend(block)
            i = j
            continue

        output.append(line)
        i += 1

    if num_updated > 0:
        filepath.write_text("".join(output), encoding="utf-8")

    return num_updated


# ---------------------------------------------------------------------------
# Entry point
# ---------------------------------------------------------------------------


def main():
    parser = argparse.ArgumentParser(
        description=(
            "Insert blank 'Default Construction Set Name' field (position 7) into "
            "Space objects that have Tag fields, in IDF/IMF files and C++ unit tests. "
            "Space objects without tags are left unchanged. epJSON files are skipped "
            "(fields are keyed by name; the new optional field is simply absent)."
        )
    )
    parser.add_argument(
        "files",
        nargs="*",
        help="Files or directories to process (default: testfiles/, datasets/, performance_tests/, tst/)",
    )
    args = parser.parse_args()

    if args.files:
        idf_files = []
        epjson_files = []
        cpp_files = []
        for arg in args.files:
            p = Path(arg)
            if p.is_dir():
                idf_files += sorted(p.rglob("*.idf")) + sorted(p.rglob("*.imf"))
                epjson_files += sorted(p.rglob("*.epJSON"))
                cpp_files += sorted(p.rglob("*.unit.cc"))
            elif p.suffix in (".idf", ".imf"):
                idf_files.append(p)
            elif p.suffix in (".epJSON", ".json"):
                epjson_files.append(p)
            elif p.suffix == ".cc":
                cpp_files.append(p)
            else:
                print(f"  Skipping {p} (unknown extension)")
    else:
        repo_root = Path(__file__).resolve().parents[2]
        idf_files = []
        epjson_files = []
        for subdir in ("testfiles", "performance_tests", "datasets", "tst/EnergyPlus/unit/Resources"):
            d = repo_root / subdir
            if d.exists():
                idf_files += sorted(d.rglob("*.idf"))
                idf_files += sorted(d.rglob("*.imf"))
                epjson_files += sorted(d.rglob("*.epJSON"))
        cpp_files = sorted((repo_root / "tst/EnergyPlus/unit").rglob("*.unit.cc"))

    total_idf = 0
    for filepath in idf_files:
        count = process_idf_file(filepath)
        if count:
            print(f"  {filepath.name}: {count} Space block(s) updated")
            total_idf += count

    total_epjson = 0
    for filepath in epjson_files:
        count = process_epjson_file(filepath)
        total_epjson += count

    total_cpp = 0
    for filepath in cpp_files:
        count = process_cpp_file(filepath)
        if count:
            print(f"  {filepath.name}: {count} Space block(s) updated")
            total_cpp += count

    print(
        f"\nTotal: {total_idf} IDF/IMF block(s) updated, "
        f"{total_epjson} epJSON Space-with-tags object(s) noted (no change needed), "
        f"{total_cpp} C++ block(s) updated "
        f"across {len(idf_files) + len(epjson_files) + len(cpp_files)} file(s) checked"
    )


if __name__ == "__main__":
    main()
