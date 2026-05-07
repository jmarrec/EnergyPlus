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
Split old-style ElectricEquipment IDF objects in .idf files into separate
ElectricEquipment and ElectricEquipment:Definition objects.

Old format (11-13 fields including class line):
  ElectricEquipment,
    Name,                    !- Name
    Zone,                    !- Zone or ZoneList or Space or SpaceList Name
    Schedule,                !- Schedule Name
    Method,                  !- Design Level Calculation Method
    Level,                   !- Design Level {W}
    WattsArea,               !- Watts per Floor Area {W/m2}
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
    ,]                       !- Multiplier
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

Already-converted blocks (those whose 3rd field comment is
'Electric Equipment Definition Name') are left untouched.

Usage:
    python split_electric_equipment_idf.py [file1.idf ...]
    # With no args, processes all *.idf and *.imf under testfiles/,
    # performance_tests/, and datasets/
"""

import argparse
import re
import sys
from pathlib import Path

# Matches an IDF field or class-name line, e.g.:
#   "    West Zone ElecEq 1,      !- Name"
#   "  ElectricEquipment,"
#   "    ,                        !- Watts per Floor Area {W/m2}"
# Group 1: leading whitespace (indent)
# Group 2: field value (stripped of trailing spaces)
# Group 3: terminator (',' or ';')
# Group 4: comment after '!-' (or None)
IDF_FIELD_RE = re.compile(r"^(\s*)(.*?)\s*([,;])\s*(?:!-\s*(.*))?$")

# Objects handled by this script (class name → definition field label)
OBJECTS_TO_SPLIT = {
    "ElectricEquipment": "Electric Equipment Definition Name",
    "ElectricEquipment:ITE:AirCooled": "ElectricEquipment ITE AirCooled Definition Name",
    "GasEquipment": "Gas Equipment Definition Name",
    "HotWaterEquipment": "Hot Water Equipment Definition Name",
    "Lights": "Lights Definition Name",
    "OtherEquipment": "Other Equipment Definition Name",
    "People": "People Definition Name",
    "SteamEquipment": "Steam Equipment Definition Name",
}

# For objects with extra instance-only fields between Name and Zone in the old format
# (listed in order by label).  These fields stay in the instance and are NOT moved
# to the Definition.
OBJECT_EXTRA_INSTANCE_FIELDS = {
    "OtherEquipment": ["Fuel Type"],
}

# Maximum number of definition fields to collect (Method counts as the first).
# Standard is 7 (Method + Level + area + person + 3 fractions).
# OtherEquipment and GasEquipment add CO2 Generation Rate as an 8th definition field.
OBJECT_MAX_DEF_FIELDS = {
    "GasEquipment": 8,
    "OtherEquipment": 8,
}

# Minimum number of definition fields required to accept a block for conversion.
# Falls back to OBJECT_MAX_DEF_FIELDS (or 7) when absent.
# OtherEquipment's and GasEquipment's CO2 Generation Rate (field 8) have a default of 0.0,
# so blocks without it are still valid and produce correct Definition objects.
OBJECT_MIN_DEF_FIELDS = {
    "GasEquipment": 7,
    "OtherEquipment": 7,
}


# ---------------------------------------------------------------------------
# Low-level helpers
# ---------------------------------------------------------------------------


def parse_idf_line(line):
    """Return (indent, value, terminator, comment_or_None) or None."""
    m = IDF_FIELD_RE.match(line.rstrip("\n\r"))
    if m:
        return m.group(1), m.group(2), m.group(3), m.group(4)
    return None


def is_ee_class(parsed, only_class: str | None = None):
    """True iff parsed line is a splittable equipment class-name line (not a Definition).

    Field-value lines that happen to contain an equipment name as a value always
    carry a '!- ...' comment (e.g. '!- Actuated Component Type'), so excluding
    lines with a comment reliably avoids false positives.
    """
    indent, value, term, comment = parsed
    if term != "," or comment is not None:
        return False
    if only_class is not None:
        return value == only_class
    return value in OBJECTS_TO_SPLIT


def terminates_object(parsed):
    """True iff this IDF field ends an object (terminator is ';')."""
    return parsed[2] == ";"


def is_already_converted(block):
    """True if the 3rd block line already has '<Object> Definition Name' as comment."""
    if len(block) < 3:
        return False
    parsed = parse_idf_line(block[2])
    return parsed is not None and parsed[3] is not None and "Definition Name" in parsed[3]


def find_comment_col(block):
    """Return the median '!-' column among field lines, or 33 as a fallback."""
    cols = []
    for line in block[1:]:
        idx = line.find("!-")
        if idx > 0:
            cols.append(idx)
    if cols:
        cols.sort()
        return cols[len(cols) // 2]
    return 33


# ---------------------------------------------------------------------------
# Block transformer
# ---------------------------------------------------------------------------


# ---------------------------------------------------------------------------
# People-specific block transformer
# ---------------------------------------------------------------------------
# Old People IDD field layout (1-based; class line is index 0 in parsed):
#  1  Name                                      → instance A1
#  2  Zone or ZoneList...                        → instance A3
#  3  Number of People Schedule Name             → instance A4
#  4  Number of People Calculation Method        → definition A2
#  5  Number of People                           → definition N1
#  6  People per Floor Area                      → definition N2
#  7  Floor Area per Person                      → definition N3
#  8  Fraction Radiant                           → definition N4
#  9  Sensible Heat Fraction                     → definition N5
# 10  Activity Level Schedule Name               → instance A5   (old min-fields 10)
# 11  Carbon Dioxide Generation Rate             → definition N6  (opt)
# 12  Enable ASHRAE 55 Comfort Warnings          → definition A3  (opt)
# 13  Mean Radiant Temperature Calculation Type  → definition A4  (opt)
# 14  Surface Name/Angle Factor List Name        → instance A6   (opt)
# 15  Work Efficiency Schedule Name              → instance A7   (opt)
# 16  Clothing Insulation Calculation Method     → instance A8   (opt)
# 17  Clothing Insulation Calculation Method Schedule → instance A9  (opt)
# 18  Clothing Insulation Schedule Name          → instance A10  (opt)
# 19  Air Velocity Schedule Name                 → instance A11  (opt)
# 20  Thermal Comfort Model 1 Type               → definition A5 (opt)
# 21–26 Thermal Comfort Model 2–7 Type           → definition A6–A11 (opt)
# 27  Ankle Level Air Velocity Schedule Name     → instance A12  (opt)
# 28  Cold Stress Temperature Threshold          → instance N1   (opt)
# 29  Heat Stress Temperature Threshold          → instance N2   (opt)

_PEOPLE_DEF_REQUIRED = [4, 5, 6, 7, 8, 9]
_PEOPLE_DEF_OPT = [11, 12, 13, 20, 21, 22, 23, 24, 25, 26]
_PEOPLE_INST_OPT = [14, 15, 16, 17, 18, 19, 27, 28, 29]


def transform_people_block(block, filepath_for_warning=""):
    """Convert an old-style People IDF block to People + People:Definition."""
    parsed = [parse_idf_line(ln) for ln in block]
    if any(p is None for p in parsed):
        return None

    if len(parsed) < 11:
        print(f"  WARNING: People block too short ({len(parsed)} lines) in " f"'{filepath_for_warning}', skipping")
        return None

    eol = "\r\n" if block[0].endswith("\r\n") else "\n"
    comment_col = find_comment_col(block)
    class_indent = parsed[0][0]
    field_indent = parsed[1][0]
    name_val = parsed[1][1]
    def_name = f"{name_val} Definition"

    def idf_line(indent, val, term, comment_text):
        vp = f"{indent}{val}{term}"
        if comment_text:
            pad = max(2, comment_col - len(vp))
            return f"{vp}{' ' * pad}!- {comment_text}{eol}"
        return f"{vp}{eol}"

    def fl(val, term, comment_text):
        return idf_line(field_indent, val, term, comment_text)

    def get(field_num):
        if field_num < len(parsed):
            p = parsed[field_num]
            return p[1], p[3] or ""
        return None

    n = len(parsed)
    inst_opt = [fn for fn in _PEOPLE_INST_OPT if fn < n]
    def_opt = [fn for fn in _PEOPLE_DEF_OPT if fn < n]

    result = []

    # --- Instance ---
    result.append(block[0])
    result.append(block[1])
    result.append(fl(def_name, ",", "People Definition Name"))
    zone_v, zone_c = get(2)
    result.append(fl(zone_v, ",", zone_c))
    sched_v, sched_c = get(3)
    result.append(fl(sched_v, ",", sched_c))
    act_v, act_c = get(10)
    if inst_opt:
        result.append(fl(act_v, ",", act_c))
        for i, fn in enumerate(inst_opt):
            v, c = get(fn)
            result.append(fl(v, ",", c))
        result.append(fl("", ";", "Multiplier"))
    else:
        result.append(fl(act_v, ",", act_c))
        result.append(fl("", ";", "Multiplier"))

    result.append(eol)

    # --- Definition ---
    result.append(idf_line(class_indent, "People:Definition", ",", None))
    result.append(fl(def_name, ",", "Name"))
    all_def = _PEOPLE_DEF_REQUIRED + def_opt
    for i, fn in enumerate(all_def):
        v, c = get(fn)
        result.append(fl(v, ";" if i == len(all_def) - 1 else ",", c))

    return result


# ---------------------------------------------------------------------------
# Lights-specific block transformer
# ---------------------------------------------------------------------------
# Old Lights IDD field layout (1-based; class line is index 0 in parsed):
#  1  Name                                            → instance A1
#  2  Zone or ZoneList...                             → instance A3
#  3  Schedule Name                                   → instance A4
#  4  Design Level Calculation Method                 → definition A2
#  5  Lighting Level                                  → definition N1
#  6  Watts per Floor Area                            → definition N2
#  7  Watts per Person                                → definition N3
#  8  Return Air Fraction                             → definition N4
#  9  Fraction Radiant                                → definition N5
# 10  Fraction Visible                                → definition N6
# 11  Fraction Replaceable                            → instance N1  (old \min-fields 11)
# 12  End-Use Subcategory                             → instance A5  (opt)
# 13  Return Air Fraction Calculated from Plenum Temp → definition A3 (opt)
# 14  Return Air Fraction Function of Plenum Temp Coeff 1 → definition N7 (opt)
# 15  Return Air Fraction Function of Plenum Temp Coeff 2 → definition N8 (opt)
# 16  Return Air Heat Gain Node Name                  → definition A4 (opt)
# 17  Exhaust Air Heat Gain Node Name                 → definition A5 (opt)

_LIGHTS_DEF_REQUIRED = [4, 5, 6, 7, 8, 9, 10]
_LIGHTS_DEF_OPT = [13, 14, 15]


def transform_lights_block(block, filepath_for_warning=""):
    """Convert an old-style Lights IDF block to Lights + Lights:Definition."""
    parsed = [parse_idf_line(ln) for ln in block]
    if any(p is None for p in parsed):
        return None

    # Minimum: class + Name + Zone + Sched + 7 def fields + Replaceable = 12 lines
    if len(parsed) < 12:
        print(f"  WARNING: Lights block too short ({len(parsed)} lines) in " f"'{filepath_for_warning}', skipping")
        return None

    eol = "\r\n" if block[0].endswith("\r\n") else "\n"
    comment_col = find_comment_col(block)
    class_indent = parsed[0][0]
    field_indent = parsed[1][0]
    name_val = parsed[1][1]
    def_name = f"{name_val} Definition"

    def idf_line(indent, val, term, comment_text):
        vp = f"{indent}{val}{term}"
        if comment_text:
            pad = max(2, comment_col - len(vp))
            return f"{vp}{' ' * pad}!- {comment_text}{eol}"
        return f"{vp}{eol}"

    def fl(val, term, comment_text):
        return idf_line(field_indent, val, term, comment_text)

    def get(field_num):
        if field_num < len(parsed):
            p = parsed[field_num]
            return p[1], p[3] or ""
        return None

    n = len(parsed)
    def_opt = [fn for fn in _LIGHTS_DEF_OPT if fn < n]

    result = []

    # --- Instance ---
    result.append(block[0])  # class line
    result.append(block[1])  # Name
    result.append(fl(def_name, ",", "Lights Definition Name"))
    zone_v, zone_c = get(2)
    result.append(fl(zone_v, ",", zone_c))
    sched_v, sched_c = get(3)
    result.append(fl(sched_v, ",", sched_c))
    repl_v, repl_c = get(11)
    # build instance tail: EndUse (opt), RetNode (opt), ExhaustNode (opt)
    inst_opt = []
    if n > 12:
        inst_opt.append((get(12), "End-Use Subcategory"))
    if n > 15 and get(16) is not None:
        inst_opt.append((get(16), "Return Air Heat Gain Node Name"))
    if n > 16 and get(17) is not None:
        inst_opt.append((get(17), "Exhaust Air Heat Gain Node Name"))

    if inst_opt:
        result.append(fl(repl_v, ",", repl_c))
        result.append(fl("", ",", "Multiplier"))
        for i, ((v, c), label) in enumerate(inst_opt):
            term = ";" if i == len(inst_opt) - 1 else ","
            result.append(fl(v, term, c or label))
    else:
        result.append(fl(repl_v, ";", repl_c))

    result.append(eol)

    # --- Definition ---
    result.append(idf_line(class_indent, "Lights:Definition", ",", None))
    result.append(fl(def_name, ",", "Name"))
    all_def = _LIGHTS_DEF_REQUIRED + def_opt
    for i, fn in enumerate(all_def):
        v, c = get(fn)
        result.append(fl(v, ";" if i == len(all_def) - 1 else ",", c))

    return result


# ---------------------------------------------------------------------------
# ElectricEquipment:ITE:AirCooled-specific block transformer
# ---------------------------------------------------------------------------
# Old ITE:AirCooled field layout (1-based; class line is index 0 in parsed):
#  1  Name                                            → instance A1
#  2  Zone or Space Name                              → instance A3
#  3  Air Flow Calculation Method                     → definition A2
#  4  Design Power Input Calculation Method           → definition A3
#  5  Watts per Unit                                  → definition N1
#  6  Number of Units                                 → instance N1
#  7  Watts per Floor Area                            → definition N2
#  8  Design Power Input Schedule Name                → instance A4
#  9  CPU Loading Schedule Name                       → instance A5
# 10  CPU Power Input Function curve                  → definition A4
# 11  Design Fan Power Input Fraction                 → definition N3
# 12  Design Fan Air Flow Rate per Power Input        → definition N4
# 13  Air Flow Function curve                         → definition A5
# 14  Fan Power Input Function of Flow curve          → definition A6
# 15  Design Entering Air Temperature                 → definition N5
# 16  Environmental Class                             → definition A7
# 17  Air Inlet Connection Type                       → definition A8
# 18  Air Inlet Room Air Model Node Name              → instance A6
# 19  Air Outlet Room Air Model Node Name             → instance A7
# 20  Supply Air Node Name                            → instance A8
# 21  Design Recirculation Fraction                   → definition N6
# 22  Recirculation Function curve                    → definition A9
# 23  Design Electric Power Supply Efficiency         → definition N7
# 24  EPS Efficiency Function of PLR curve            → definition A10
# 25  Fraction of EPS Losses to Zone                  → definition N8
# 26  CPU End-Use Subcategory                         → instance A9
# 27  Fan End-Use Subcategory                         → instance A10
# 28  EPS End-Use Subcategory                         → instance A11   (old \min-fields 28)
# 29  Supply Temperature Difference                   → definition N9  (optional)
# 30  Supply Temperature Difference Schedule          → definition A11 (optional)
# 31  Return Temperature Difference                   → definition N10 (optional)
# 32  Return Temperature Difference Schedule          → definition A12 (optional)

_ITE_DEF_REQUIRED = [3, 4, 5, 7, 10, 11, 12, 13, 14, 15, 16, 17, 21, 22, 23, 24, 25]
_ITE_DEF_OPT = [29, 30, 31, 32]


def transform_ite_aircooled_block(block, filepath_for_warning=""):
    """Convert an old-style ElectricEquipment:ITE:AirCooled block to instance + definition."""
    parsed = [parse_idf_line(ln) for ln in block]
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
    comment_col = find_comment_col(block)
    class_indent = parsed[0][0]
    field_indent = parsed[1][0]
    name_val = parsed[1][1]
    def_name = f"{name_val} Definition"

    def idf_line(indent, val, term, comment_text):
        vp = f"{indent}{val}{term}"
        if comment_text:
            pad = max(2, comment_col - len(vp))
            return f"{vp}{' ' * pad}!- {comment_text}{eol}"
        return f"{vp}{eol}"

    def fl(val, term, comment_text):
        return idf_line(field_indent, val, term, comment_text)

    def get(field_num):
        if field_num < len(parsed):
            p = parsed[field_num]
            return p[1], p[3] or ""
        return "", ""

    n = len(parsed)
    def_opt = [fn for fn in _ITE_DEF_OPT if fn < n]

    result = []

    # --- Instance ---
    result.append(block[0])  # class line
    result.append(block[1])  # Name
    result.append(fl(def_name, ",", "ElectricEquipment ITE AirCooled Definition Name"))
    zone_v, zone_c = get(2)
    result.append(fl(zone_v, ",", zone_c))
    num_v, num_c = get(6)
    result.append(fl(num_v, ",", num_c))
    sched_v, sched_c = get(8)
    result.append(fl(sched_v, ",", sched_c))
    csched_v, csched_c = get(9)
    result.append(fl(csched_v, ",", csched_c))
    inlet_v, inlet_c = get(18)
    result.append(fl(inlet_v, ",", inlet_c))
    outlet_v, outlet_c = get(19)
    result.append(fl(outlet_v, ",", outlet_c))
    supply_v, supply_c = get(20)
    result.append(fl(supply_v, ",", supply_c))
    cpu_eu_v, cpu_eu_c = get(26)
    result.append(fl(cpu_eu_v, ",", cpu_eu_c))
    fan_eu_v, fan_eu_c = get(27)
    result.append(fl(fan_eu_v, ",", fan_eu_c))
    eps_eu_v, eps_eu_c = get(28)
    result.append(fl(eps_eu_v, ";", eps_eu_c))

    result.append(eol)

    # --- Definition ---
    result.append(idf_line(class_indent, "ElectricEquipment:ITE:AirCooled:Definition", ",", None))
    result.append(fl(def_name, ",", "Name"))
    all_def = _ITE_DEF_REQUIRED + def_opt
    for i, fn in enumerate(all_def):
        v, c = get(fn)
        result.append(fl(v, ";" if i == len(all_def) - 1 else ",", c))

    return result


# ---------------------------------------------------------------------------
# Equipment block transformer
# ---------------------------------------------------------------------------


def transform_block(block, filepath_for_warning=""):
    """
    Convert an old-style equipment IDF block to the new split format.

    Returns a list of new line strings (with newlines), or None to skip.
    """
    parsed = [parse_idf_line(ln) for ln in block]
    if any(p is None for p in parsed):
        return None

    # People has a non-standard interleaved field layout — use dedicated transform
    if parsed[0][1] == "People":
        return transform_people_block(block, filepath_for_warning)

    # Lights has a non-standard field layout — use dedicated transform
    if parsed[0][1] == "Lights":
        return transform_lights_block(block, filepath_for_warning)

    # ElectricEquipment:ITE:AirCooled has a non-standard field layout — use dedicated transform
    if parsed[0][1] == "ElectricEquipment:ITE:AirCooled":
        return transform_ite_aircooled_block(block, filepath_for_warning)

    # Minimum: class + Name + Zone + Sched + Method + Level +
    # WattsArea + WattsPerson + FracLatent + FracRadiant + FracLost = 11 lines
    if len(parsed) < 11:
        print(f"  WARNING: block too short ({len(parsed)} lines) in " f"'{filepath_for_warning}', skipping")
        return None

    eol = "\r\n" if block[0].endswith("\r\n") else "\n"

    class_indent, class_name, _, _ = parsed[0]
    field_indent, name_val, _, _ = parsed[1]
    def_field_label = OBJECTS_TO_SPLIT[class_name]

    extra_instance_labels = OBJECT_EXTRA_INSTANCE_FIELDS.get(class_name, [])
    field_offset = len(extra_instance_labels)
    min_def_fields = OBJECT_MIN_DEF_FIELDS.get(class_name, 7)
    max_def_fields = OBJECT_MAX_DEF_FIELDS.get(class_name, 7)

    # Minimum: class + Name + extra-instance + Zone + Sched + definition fields
    min_fields = 4 + field_offset + min_def_fields
    if len(parsed) < min_fields:
        print(
            f"  WARNING: block too short ({len(parsed)} lines, need {min_fields}) in "
            f"'{filepath_for_warning}', skipping"
        )
        print(block)
        return None

    comment_col = find_comment_col(block)

    # ---- helper: build one IDF line ----

    def idf_line(indent, value, term, comment_text):
        """Build a full IDF line string."""
        value_part = f"{indent}{value}{term}"
        if comment_text:
            padding = max(2, comment_col - len(value_part))
            return f"{value_part}{' ' * padding}!- {comment_text}{eol}"
        return f"{value_part}{eol}"

    def field_line(value, term, comment_text):
        return idf_line(field_indent, value, term, comment_text)

    # ---- extract old field values ----

    name_val = parsed[1][1]
    def_name = f"{name_val} Definition"

    # Extra instance fields (e.g. Fuel Type for OtherEquipment), Zone, Schedule
    extra_parsed = [parsed[2 + k] for k in range(field_offset)]
    zone_parsed = parsed[2 + field_offset]
    sched_parsed = parsed[3 + field_offset]

    # Definition fields start at index 4 + field_offset
    def_fields = []  # (value, comment) for <Object>:Definition
    end_use_value = None

    for indent, val, term, comment in parsed[4 + field_offset :]:
        comment = comment or ""
        if "Replaceable" in comment:
            continue  # dropped in new schema
        if "End-Use" in comment or "Subcategory" in comment:
            end_use_value = val  # moves to instance object
            continue
        if len(def_fields) < min_def_fields:
            def_fields.append((val, comment))

    if len(def_fields) < min_def_fields:
        print(f"  WARNING: only {len(def_fields)} definition fields found in " f"'{filepath_for_warning}', skipping")
        return None
    if len(def_fields) > max_def_fields:
        print(
            f"  WARNING: {len(def_fields)} definition fields found (max {max_def_fields}) in "
            f"'{filepath_for_warning}', skipping"
        )
        return None

    # ---- assemble output ----

    result = []

    # — <Object> instance —
    result.append(block[0])  # class line unchanged
    result.append(block[1])  # Name line unchanged
    result.append(field_line(def_name, ",", def_field_label))

    # Extra instance fields (e.g. Fuel Type for OtherEquipment)
    for ep in extra_parsed:
        ep_indent, ep_val, _, ep_comment = ep
        result.append(idf_line(ep_indent, ep_val, ",", ep_comment))

    # Zone: keep value verbatim, just ensure terminator is ','
    zone_indent, zone_val, _, zone_comment = zone_parsed
    result.append(idf_line(zone_indent, zone_val, ",", zone_comment))

    sched_indent, sched_val, _, sched_comment = sched_parsed
    if end_use_value:
        result.append(idf_line(sched_indent, sched_val, ",", sched_comment))
        result.append(field_line("", ",", "Multiplier"))
        result.append(field_line(end_use_value, ";", "End-Use Subcategory"))
    else:
        result.append(idf_line(sched_indent, sched_val, ";", sched_comment))

    result.append(eol)  # blank line between objects

    # — <Object>:Definition —
    result.append(idf_line(class_indent, f"{class_name}:Definition", ",", None))
    result.append(field_line(def_name, ",", "Name"))

    for i, (val, comment) in enumerate(def_fields):
        is_last = i == len(def_fields) - 1
        result.append(field_line(val, ";" if is_last else ",", comment))

    return result


# ---------------------------------------------------------------------------
# File-level processing
# ---------------------------------------------------------------------------


def process_file(filepath, only_class: str | None = None):
    """Process a single .idf file in place. Returns number of blocks converted."""
    filepath = Path(filepath)
    lines = filepath.read_text(encoding="utf-8", errors="replace").splitlines(keepends=True)

    output = []
    i = 0
    num_converted = 0

    while i < len(lines):
        line = lines[i]
        parsed = parse_idf_line(line)

        if parsed and is_ee_class(parsed=parsed, only_class=only_class):
            # Collect the complete IDF object (until the terminating ';' field)
            block = [line]
            j = i + 1
            while j < len(lines):
                next_line = lines[j]
                next_parsed = parse_idf_line(next_line)
                if next_parsed:
                    block.append(next_line)
                    if terminates_object(next_parsed):
                        j += 1
                        break
                    j += 1
                else:
                    # Blank line or pure comment line inside object — keep collecting
                    # only if we haven't hit a new object keyword
                    stripped = next_line.strip()
                    if stripped == "" or stripped.startswith("!"):
                        block.append(next_line)
                        j += 1
                    else:
                        break  # some other content ends the block

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
    parser = argparse.ArgumentParser(description="Split old-style equipment IDF objects into instance + definition.")
    parser.add_argument("files", nargs="*", help="Files or directories to process (default: all IDF/IMF under repo)")
    parser.add_argument(
        "--only-class",
        choices=list(OBJECTS_TO_SPLIT),
        metavar="CLASS",
        help=f"Limit split to this class only. Choices: {', '.join(OBJECTS_TO_SPLIT)}",
    )
    args = parser.parse_args()

    only_class = args.only_class

    if args.files:
        files = []
        for arg in args.files:
            p = Path(arg)
            if p.is_dir():
                files.extend(sorted(p.rglob("*.idf")))
                files.extend(sorted(p.rglob("*.imf")))
            else:
                files.append(p)
    else:
        repo_root = Path(__file__).resolve().parents[2]
        files = []
        for subdir in ("testfiles", "performance_tests", "datasets", "tst/EnergyPlus/unit/Resources"):
            d = repo_root / subdir
            files += sorted(d.rglob("*.idf"))
            files += sorted(d.rglob("*.imf"))

    total = 0
    for filepath in files:
        count = process_file(filepath, only_class=only_class)
        if count:
            print(f"  {filepath.name}: {count} block(s) converted")
            total += count

    print(f"\nTotal: {total} equipment block(s) converted across {len(files)} file(s) checked")


if __name__ == "__main__":
    main()
