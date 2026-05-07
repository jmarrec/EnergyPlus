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
#     publicity, or other promotional activities any name, trade name,
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
Split old-style ElectricEquipment IDF objects embedded in C++ unit test files
into separate ElectricEquipment and ElectricEquipment:Definition objects.

Old format (10-12 fields):
  ElectricEquipment,
    Name,                    !- Name
    Zone,                    !- Zone or ZoneList Name
    Schedule,                !- Schedule Name
    Method,                  !- Design Level Calculation Method
    Level,                   !- Design Level {W}
    WattsArea,               !- Watts per Zone Floor Area {W/m2}
    WattsPerson,             !- Watts per Person {W/person}
    FracLatent,              !- Fraction Latent
    FracRadiant,             !- Fraction Radiant
    FracLost[,               !- Fraction Lost
    FracReplaceable,]        !- Fraction Replaceable  (dropped in new schema)
    [EndUse;]                !- End-Use Subcategory   (moved to ElectricEquipment)

New format:
  ElectricEquipment,
    Name,                    !- Name
    Name Definition,         !- Electric Equipment Definition Name
    Zone,                    !- Zone or ZoneList or Space or SpaceList Name
    Schedule[,               !- Schedule Name
    [EndUse];                !- End-Use Subcategory

  ElectricEquipment:Definition,
    Name Definition,         !- Name
    Method,                  !- Design Level Calculation Method
    Level,                   !- Design Level {W}
    WattsArea,               !- Watts per Floor Area {W/m2}
    WattsPerson,             !- Watts per Person {W/person}
    FracLatent,              !- Fraction Latent
    FracRadiant,             !- Fraction Radiant
    FracLost;                !- Fraction Lost

Already-converted blocks (those with 'Electric Equipment Definition Name' as the
3rd field comment) are left untouched.

Usage:
    python split_electric_equipment_unit_tests.py [file1.unit.cc ...]
    # With no args, processes all *.unit.cc under tst/EnergyPlus/unit/
"""

import argparse
import importlib.util
import re
import sys
from pathlib import Path

# Load the IDF script for its parsing helpers (used inside raw string literals)
_spec = importlib.util.spec_from_file_location(
    "split_space_load_idf",
    Path(__file__).parent / "split_space_load_idf.py",
)
_idf = importlib.util.module_from_spec(_spec)
_spec.loader.exec_module(_idf)

# Matches a C++ line that holds a single quoted IDF string, e.g.:
#   '        "    Name,               !- Name",'
# Group 1: leading whitespace + opening quote  (e.g. '        "')
# Group 2: IDF content between the quotes      (e.g. '    Name,               !- Name')
# Group 3: closing quote + trailing comma/ws   (e.g. '",')
CPP_STRING_RE = re.compile(r'^(\s*")(.*?)("[\s,]*)$')


# ---------------------------------------------------------------------------
# Low-level helpers
# ---------------------------------------------------------------------------


def parse_cpp_line(line):
    """Return (prefix, idf_content, suffix) or None if not a C++ IDF string line."""
    m = CPP_STRING_RE.match(line.rstrip("\n\r"))
    if m:
        return m.group(1), m.group(2), m.group(3)
    return None


def idf_split(idf_content):
    """Return (value_with_terminator, comment_or_None) for an IDF field string."""
    if "!-" in idf_content:
        before, comment = idf_content.split("!-", 1)
        return before.rstrip(), comment.strip()
    return idf_content.rstrip(), None


def get_value(idf_content):
    """Bare field value, stripped of terminator and whitespace."""
    value_str, _ = idf_split(idf_content)
    return value_str.rstrip(",").rstrip(";").strip()


def get_comment(idf_content):
    """Comment text after '!-', or empty string."""
    _, c = idf_split(idf_content)
    return c or ""


def set_terminator(idf_content, new_term):
    """
    Replace the trailing ',' or ';' in idf_content with new_term,
    preserving the spacing before the '!-' comment.
    """
    value_str, comment = idf_split(idf_content)
    # Strip old terminator
    bare = value_str.rstrip(",").rstrip(";")
    new_value_str = bare + new_term
    if comment:
        col = idf_content.index("!-")
        padding = max(1, col - len(new_value_str))
        return f"{new_value_str}{' ' * padding}!- {comment}"
    return new_value_str


def is_ee_class(idf_content, only_class: str | None = None):
    """True iff idf_content is a splittable equipment class name (not a Definition)."""
    classes = [only_class] if only_class is not None else list(_idf.OBJECTS_TO_SPLIT)
    return bool(re.match(r"^\s*(" + "|".join(re.escape(k) for k in classes) + r")\s*,\s*$", idf_content))


def terminates_object(idf_content):
    """True iff this IDF field ends an object (value ends with ';')."""
    value_str, _ = idf_split(idf_content)
    return value_str.endswith(";")


def is_already_converted(block):
    """True if the 3rd block line already carries '<Object> Definition Name'."""
    if len(block) < 3:
        return False
    parsed = parse_cpp_line(block[2])
    return parsed is not None and "Definition Name" in parsed[1]


def find_comment_col(block):
    """Return the median '!-' column among field lines, or 33 as a fallback."""
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


# ---------------------------------------------------------------------------
# People-specific block transformer (C++ quoted-string variant)
# ---------------------------------------------------------------------------


def transform_people_block_cpp(block, filepath_for_warning=""):
    """Convert an old-style People C++ block to People + People:Definition."""
    parsed = [parse_cpp_line(ln) for ln in block]
    if any(p is None for p in parsed):
        return None
    if len(parsed) < 11:
        print(f"  WARNING: People block too short ({len(parsed)} lines) in " f"'{filepath_for_warning}', skipping")
        return None

    eol = "\r\n" if block[0].endswith("\r\n") else "\n"
    class_prefix, class_idf, class_suffix = parsed[0]
    field_prefix, name_idf, field_suffix = parsed[1]
    idf_indent = re.match(r"^(\s*)", name_idf).group(1)
    class_idf_indent = re.match(r"^(\s*)", class_idf).group(1)
    comment_col = find_comment_col(block)

    def cpp_line(idf_content, *, is_class=False):
        p = class_prefix if is_class else field_prefix
        s = class_suffix if is_class else field_suffix
        return f"{p}{idf_content}{s}{eol}"

    def new_idf_field(value, term, comment_text):
        vp = f"{idf_indent}{value}{term}"
        padding = max(1, comment_col - len(vp))
        return f"{vp}{' ' * padding}!- {comment_text}"

    def nfl(value, term, comment_text):
        return cpp_line(new_idf_field(value, term, comment_text))

    def get(field_num):
        if field_num < len(parsed):
            idf = parsed[field_num][1]
            return get_value(idf), get_comment(idf)
        return None

    name_val = get_value(name_idf)
    def_name = f"{name_val} Definition"
    n = len(parsed)

    inst_opt = [fn for fn in _idf._PEOPLE_INST_OPT if fn < n]
    def_opt = [fn for fn in _idf._PEOPLE_DEF_OPT if fn < n]

    result = []

    # --- Instance ---
    result.append(block[0])
    result.append(block[1])
    result.append(nfl(def_name, ",", "People Definition Name"))
    result.append(cpp_line(set_terminator(parsed[2][1], ",")))  # Zone
    result.append(cpp_line(set_terminator(parsed[3][1], ",")))  # Schedule
    if inst_opt:
        result.append(cpp_line(set_terminator(idf_content=parsed[10][1], new_term=",")))  # Activity
        for i, fn in enumerate(inst_opt):
            term = ";" if i == len(inst_opt) - 1 else ","
            result.append(cpp_line(set_terminator(idf_content=parsed[fn][1], new_term=term)))
    else:
        result.append(cpp_line(set_terminator(idf_content=parsed[10][1], new_term=";")))

    result.append(eol)

    # --- Definition ---
    result.append(cpp_line(f"{class_idf_indent}People:Definition,", is_class=True))
    result.append(nfl(def_name, ",", "Name"))
    all_def = _idf._PEOPLE_DEF_REQUIRED + def_opt
    for i, fn in enumerate(all_def):
        result.append(cpp_line(set_terminator(parsed[fn][1], ";" if i == len(all_def) - 1 else ",")))

    return result


# ---------------------------------------------------------------------------
# Lights-specific block transformer (C++ quoted-string variant)
# ---------------------------------------------------------------------------


def transform_lights_block_cpp(block, filepath_for_warning=""):
    """Convert an old-style Lights C++ block to Lights + Lights:Definition."""
    parsed = [parse_cpp_line(ln) for ln in block]
    if any(p is None for p in parsed):
        return None
    # Minimum: class + Name + Zone + Sched + 7 def fields + Replaceable = 12 lines
    if len(parsed) < 12:
        print(f"  WARNING: Lights block too short ({len(parsed)} lines) in " f"'{filepath_for_warning}', skipping")
        return None

    eol = "\r\n" if block[0].endswith("\r\n") else "\n"
    class_prefix, class_idf, class_suffix = parsed[0]
    field_prefix, name_idf, field_suffix = parsed[1]
    idf_indent = re.match(r"^(\s*)", name_idf).group(1)
    class_idf_indent = re.match(r"^(\s*)", class_idf).group(1)
    comment_col = find_comment_col(block)

    def cpp_line(idf_content, *, is_class=False):
        p = class_prefix if is_class else field_prefix
        s = class_suffix if is_class else field_suffix
        return f"{p}{idf_content}{s}{eol}"

    def new_idf_field(value, term, comment_text):
        vp = f"{idf_indent}{value}{term}"
        padding = max(1, comment_col - len(vp))
        return f"{vp}{' ' * padding}!- {comment_text}"

    def nfl(value, term, comment_text):
        return cpp_line(new_idf_field(value, term, comment_text))

    name_val = get_value(name_idf)
    def_name = f"{name_val} Definition"
    n = len(parsed)

    def_opt = [fn for fn in _idf._LIGHTS_DEF_OPT if fn < n]

    result = []

    # --- Instance ---
    result.append(block[0])
    result.append(block[1])
    result.append(nfl(def_name, ",", "Lights Definition Name"))
    result.append(cpp_line(set_terminator(parsed[2][1], ",")))  # Zone
    result.append(cpp_line(set_terminator(parsed[3][1], ",")))  # Schedule
    # Fraction Replaceable + optional instance tail fields
    inst_opt_fns = []
    if n > 12:
        inst_opt_fns.append(12)  # End-Use Subcategory
    if n > 15 and parsed[16][1].strip().strip(",;"):
        inst_opt_fns.append(16)  # Return Air Heat Gain Node Name
    if n > 16 and parsed[17][1].strip().strip(",;"):
        inst_opt_fns.append(17)  # Exhaust Air Heat Gain Node Name
    if inst_opt_fns:
        result.append(cpp_line(set_terminator(parsed[11][1], ",")))
        # result.append(nfl("", ",", "Multiplier"))
        for i, fn in enumerate(inst_opt_fns):
            result.append(cpp_line(set_terminator(parsed[fn][1], ";" if i == len(inst_opt_fns) - 1 else ",")))
    else:
        result.append(cpp_line(set_terminator(parsed[11][1], ";")))  # Fraction Replaceable

    result.append(eol)

    # --- Definition ---
    result.append(cpp_line(f"{class_idf_indent}Lights:Definition,", is_class=True))
    result.append(nfl(def_name, ",", "Name"))
    all_def = _idf._LIGHTS_DEF_REQUIRED + def_opt
    for i, fn in enumerate(all_def):
        result.append(cpp_line(set_terminator(parsed[fn][1], ";" if i == len(all_def) - 1 else ",")))

    return result


# ---------------------------------------------------------------------------
# ElectricEquipment:ITE:AirCooled-specific block transformer (C++ quoted-string variant)
# ---------------------------------------------------------------------------


def transform_ite_aircooled_block_cpp(block, filepath_for_warning=""):
    """Convert an old-style ElectricEquipment:ITE:AirCooled C++ block to instance + definition."""
    parsed = [parse_cpp_line(ln) for ln in block]
    if any(p is None for p in parsed):
        return None

    # Minimum: class + fields 1-28 = 29 lines (old \min-fields 28)
    if len(parsed) < 29:
        print(
            f"  WARNING: ElectricEquipment:ITE:AirCooled block too short ({len(parsed)} lines) in "
            f"'{filepath_for_warning}', skipping"
        )
        return None

    eol = "\r\n" if block[0].endswith("\r\n") else "\n"
    class_prefix, class_idf, class_suffix = parsed[0]
    field_prefix, name_idf, field_suffix = parsed[1]
    idf_indent = re.match(r"^(\s*)", name_idf).group(1)
    class_idf_indent = re.match(r"^(\s*)", class_idf).group(1)
    comment_col = find_comment_col(block)

    def cpp_line(idf_content, *, is_class=False):
        p = class_prefix if is_class else field_prefix
        s = class_suffix if is_class else field_suffix
        return f"{p}{idf_content}{s}{eol}"

    def new_idf_field(value, term, comment_text):
        vp = f"{idf_indent}{value}{term}"
        padding = max(1, comment_col - len(vp))
        return f"{vp}{' ' * padding}!- {comment_text}"

    def nfl(value, term, comment_text):
        return cpp_line(new_idf_field(value, term, comment_text))

    name_val = get_value(name_idf)
    def_name = f"{name_val} Definition"
    n = len(parsed)
    def_opt = [fn for fn in _idf._ITE_DEF_OPT if fn < n]

    result = []

    # --- Instance ---
    result.append(block[0])  # class line
    result.append(block[1])  # Name
    result.append(nfl(def_name, ",", "ElectricEquipment ITE AirCooled Definition Name"))
    result.append(cpp_line(set_terminator(parsed[2][1], ",")))  # Zone or Space Name
    result.append(cpp_line(set_terminator(parsed[6][1], ",")))  # Number of Units
    result.append(cpp_line(set_terminator(parsed[8][1], ",")))  # Design Power Input Schedule
    result.append(cpp_line(set_terminator(parsed[9][1], ",")))  # CPU Loading Schedule
    result.append(cpp_line(set_terminator(parsed[18][1], ",")))  # Air Inlet Room Air Model Node
    result.append(cpp_line(set_terminator(parsed[19][1], ",")))  # Air Outlet Room Air Model Node
    result.append(cpp_line(set_terminator(parsed[20][1], ",")))  # Supply Air Node
    result.append(cpp_line(set_terminator(parsed[26][1], ",")))  # CPU End-Use Subcategory
    result.append(cpp_line(set_terminator(parsed[27][1], ",")))  # Fan End-Use Subcategory
    result.append(cpp_line(set_terminator(parsed[28][1], ";")))  # EPS End-Use Subcategory

    result.append(eol)

    # --- Definition ---
    result.append(cpp_line(f"{class_idf_indent}ElectricEquipment:ITE:AirCooled:Definition,", is_class=True))
    result.append(nfl(def_name, ",", "Name"))
    all_def = _idf._ITE_DEF_REQUIRED + def_opt
    for i, fn in enumerate(all_def):
        result.append(cpp_line(set_terminator(parsed[fn][1], ";" if i == len(all_def) - 1 else ",")))

    return result


# ---------------------------------------------------------------------------
# Equipment block transformer
# ---------------------------------------------------------------------------


def transform_block(block, filepath_for_warning=""):
    """
    Convert an old-style equipment block to the new split format.

    Returns a list of new C++ line strings (with newlines), or None to skip.
    """
    parsed = [parse_cpp_line(ln) for ln in block]
    if any(p is None for p in parsed):
        return None

    eol = "\r\n" if block[0].endswith("\r\n") else "\n"

    class_prefix, class_idf, class_suffix = parsed[0]
    field_prefix, name_idf, field_suffix = parsed[1]

    # IDF indent inside the quotes for field lines (e.g. "    " or "  ")
    idf_indent = re.match(r"^(\s*)", name_idf).group(1)
    # IDF indent inside the class-name quote (may differ from field indent)
    class_idf_indent = re.match(r"^(\s*)", class_idf).group(1)
    # Class name (e.g. "ElectricEquipment" or "SteamEquipment")
    class_name = class_idf.strip().rstrip(",")

    # People has a non-standard interleaved field layout — use dedicated transform
    if class_name == "People":
        return transform_people_block_cpp(block, filepath_for_warning)

    # Lights has a non-standard field layout — use dedicated transform
    if class_name == "Lights":
        return transform_lights_block_cpp(block, filepath_for_warning)

    # ElectricEquipment:ITE:AirCooled has a non-standard field layout — use dedicated transform
    if class_name == "ElectricEquipment:ITE:AirCooled":
        return transform_ite_aircooled_block_cpp(block, filepath_for_warning)

    def_field_label = _idf.OBJECTS_TO_SPLIT[class_name]

    extra_instance_labels = _idf.OBJECT_EXTRA_INSTANCE_FIELDS.get(class_name, [])
    field_offset = len(extra_instance_labels)
    min_def_fields = _idf.OBJECT_MIN_DEF_FIELDS.get(class_name, 7)
    max_def_fields = _idf.OBJECT_MAX_DEF_FIELDS.get(class_name, 7)

    # Minimum: class + Name + extra-instance + Zone + Sched + definition fields
    min_fields = 4 + field_offset + min_def_fields
    if len(parsed) < min_fields:
        print(
            f"  WARNING: block too short ({len(parsed)} lines, need {min_fields}) in "
            f"'{filepath_for_warning}', skipping"
        )
        return None

    comment_col = find_comment_col(block)

    # ---- helpers scoped to this block ----

    def cpp_line(idf_content, *, is_class=False):
        """Wrap idf_content in the correct C++ quoting and return a full line string."""
        p = class_prefix if is_class else field_prefix
        s = class_suffix if is_class else field_suffix
        return f"{p}{idf_content}{s}{eol}"

    def new_idf_field(value, term, comment_text):
        """Build a new IDF field content string (what goes inside the quotes)."""
        value_part = f"{idf_indent}{value}{term}"
        padding = max(1, comment_col - len(value_part))
        return f"{value_part}{' ' * padding}!- {comment_text}"

    def new_field_line(value, term, comment_text):
        return cpp_line(new_idf_field(value, term, comment_text))

    # ---- extract old field values ----

    name = get_value(parsed[1][1])
    def_name = f"{name} Definition"

    # Extra instance fields (e.g. Fuel Type for OtherEquipment), Zone, Schedule
    extra_idf = [parsed[2 + k][1] for k in range(field_offset)]
    zone_idf = parsed[2 + field_offset][1]
    schedule_idf = parsed[3 + field_offset][1]

    # Definition fields start at index 4 + field_offset
    def_idf_contents = []  # IDF content strings for <Object>:Definition fields
    end_use_value = None

    for _, idf, _ in parsed[4 + field_offset :]:
        comment = get_comment(idf)
        if "Replaceable" in comment:
            continue  # dropped in new schema
        if "End-Use" in comment or "Subcategory" in comment:
            end_use_value = get_value(idf)  # moves to instance object
            continue
        if len(def_idf_contents) < max_def_fields:
            def_idf_contents.append(idf)

    if len(def_idf_contents) < min_def_fields:
        print(
            f"  WARNING: only {len(def_idf_contents)} definition fields found in " f"'{filepath_for_warning}', skipping"
        )
        return None
    if len(def_idf_contents) > max_def_fields:
        print(
            f"  WARNING: {len(def_idf_contents)} definition fields found (max {max_def_fields}) in "
            f"'{filepath_for_warning}', skipping"
        )
        return None
    # ---- assemble output ----

    result = []

    # — <Object> instance —
    result.append(block[0])  # class line unchanged
    result.append(block[1])  # Name line unchanged
    result.append(new_field_line(def_name, ",", def_field_label))

    # Extra instance fields (e.g. Fuel Type for OtherEquipment)
    for extra_idf_content in extra_idf:
        result.append(cpp_line(set_terminator(extra_idf_content, ",")))

    result.append(cpp_line(set_terminator(zone_idf, ",")))  # Zone (keep comma)

    if end_use_value:
        result.append(cpp_line(set_terminator(schedule_idf, ",")))  # Schedule → comma
        # result.append(new_field_line("", ",", "Multiplier"))
        result.append(new_field_line(end_use_value, ";", "End-Use Subcategory"))
    else:
        result.append(cpp_line(set_terminator(schedule_idf, ";")))  # Schedule → semicolon

    result.append(eol)  # blank line between objects

    # — <Object>:Definition —
    result.append(cpp_line(f"{class_idf_indent}{class_name}:Definition,", is_class=True))
    result.append(new_field_line(def_name, ",", "Name"))

    for i, idf in enumerate(def_idf_contents):
        is_last = i == len(def_idf_contents) - 1
        result.append(cpp_line(set_terminator(idf, ";" if is_last else ",")))

    return result


# ---------------------------------------------------------------------------
# File-level processing
# ---------------------------------------------------------------------------


def process_file(filepath, only_class: str | None = None):
    """Process a single file in place. Returns number of blocks converted."""
    filepath = Path(filepath)
    lines = filepath.read_text(encoding="utf-8", errors="replace").splitlines(keepends=True)

    output = []
    i = 0
    num_converted = 0
    in_raw_string = False

    while i < len(lines):
        line = lines[i]

        # Track R"IDF(...)IDF" raw string literal boundaries
        if not in_raw_string and line.rstrip().endswith('R"IDF('):
            in_raw_string = True
            output.append(line)
            i += 1
            continue

        if in_raw_string and line.lstrip().startswith(')IDF"'):
            in_raw_string = False
            output.append(line)
            i += 1
            continue

        if in_raw_string:
            # Plain IDF content — delegate to the IDF script's parser
            parsed = _idf.parse_idf_line(line)
            if parsed and _idf.is_ee_class(parsed=parsed, only_class=only_class):
                block = [line]
                j = i + 1
                while j < len(lines):
                    next_line = lines[j]
                    if next_line.lstrip().startswith(')IDF"'):
                        break  # don't cross raw string boundary
                    next_parsed = _idf.parse_idf_line(next_line)
                    if next_parsed:
                        block.append(next_line)
                        if _idf.terminates_object(next_parsed):
                            j += 1
                            break
                        j += 1
                    else:
                        stripped = next_line.strip()
                        if stripped == "" or stripped.startswith("!"):
                            block.append(next_line)
                            j += 1
                        else:
                            break

                if _idf.is_already_converted(block):
                    output.append(line)
                    i += 1
                    continue

                new_block = _idf.transform_block(block, filepath_for_warning=str(filepath))
                if new_block is not None:
                    output.extend(new_block)
                    i = j
                    num_converted += 1
                    continue
        else:
            # C++ per-line quoted string mode
            parsed = parse_cpp_line(line)
            if parsed and is_ee_class(idf_content=parsed[1], only_class=only_class):
                block = [line]
                j = i + 1
                while j < len(lines):
                    next_line = lines[j]
                    next_parsed = parse_cpp_line(next_line)
                    if next_parsed:
                        block.append(next_line)
                        if terminates_object(next_parsed[1]):
                            j += 1
                            break
                        j += 1
                    else:
                        if next_line.strip() == "":
                            j += 1  # blank line between fields, keep looking
                        else:
                            break  # non-blank non-string content ends the block

                if is_already_converted(block):
                    output.append(line)
                    i += 1
                    continue

                new_block = transform_block(block, filepath_for_warning=str(filepath))
                if new_block is not None:
                    output.extend(new_block)
                    i = j
                    num_converted += 1
                    continue

        output.append(line)
        i += 1

    if num_converted > 0:
        filepath.write_text("".join(output), encoding="utf-8")

    return num_converted


# ---------------------------------------------------------------------------
# Entry point
# ---------------------------------------------------------------------------


def main():
    parser = argparse.ArgumentParser(description="Split old-style equipment objects in C++ unit test files.")
    parser.add_argument("files", nargs="*", help="Files to process (default: all *.unit.cc under tst/EnergyPlus/unit/)")
    parser.add_argument(
        "--only-class",
        choices=list(_idf.OBJECTS_TO_SPLIT),
        metavar="CLASS",
        help=f"Limit split to this class only. Choices: {', '.join(_idf.OBJECTS_TO_SPLIT)}",
    )
    args = parser.parse_args()

    if args.files:
        files = [Path(f) for f in args.files]
    else:
        repo_root = Path(__file__).resolve().parents[2]
        unit_dir = repo_root / "tst" / "EnergyPlus" / "unit"
        files = sorted(unit_dir.rglob("*.unit.cc"))

    total = 0
    for filepath in files:
        count = process_file(filepath=filepath, only_class=args.only_class)
        if count:
            print(f"  {filepath.name}: {count} block(s) converted")
            total += count

    print(f"\nTotal: {total} equipment block(s) converted across {len(files)} file(s) checked")


if __name__ == "__main__":
    main()
