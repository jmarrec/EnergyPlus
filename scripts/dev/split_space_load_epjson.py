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
Split old-style ElectricEquipment epJSON objects into separate
ElectricEquipment and ElectricEquipment:Definition objects.

Old ElectricEquipment object (all fields inline):
  {
    "zone_or_zonelist_or_space_or_spacelist_name": "...",
    "schedule_name": "...",
    "design_level_calculation_method": "...",
    "design_level": ...,
    "watts_per_floor_area": ...,
    "watts_per_person": ...,
    "fraction_latent": ...,
    "fraction_radiant": ...,
    "fraction_lost": ...,
    "end_use_subcategory": "..."
  }

New ElectricEquipment object:
  {
    "electric_equipment_definition_name": "<Name> Definition",
    "zone_or_zonelist_or_space_or_spacelist_name": "...",
    "schedule_name": "...",
    "end_use_subcategory": "..."           (only if present)
  }

New ElectricEquipment:Definition object:
  {
    "design_level_calculation_method": "...",
    "design_level": ...,                   (only if present)
    "watts_per_floor_area": ...,           (only if present)
    "watts_per_person": ...,               (only if present)
    "fraction_latent": ...,
    "fraction_radiant": ...,
    "fraction_lost": ...
  }

Files that already have an 'ElectricEquipment:Definition' key are left untouched.

Usage:
    python split_electric_equipment_epjson.py [file1.epJSON ...]
    # With no args, processes all *.epJSON under testfiles/
"""

import argparse
import json
import sys
from pathlib import Path

# Per-object configuration: instance type → (definition type, def ref field, definition fields)
# Equipment fields that stay in the instance object are the same for all types.
_INSTANCE_FIELDS = {
    "zone_or_zonelist_or_space_or_spacelist_name",
    "schedule_name",
    "multiplier",
    "end_use_subcategory",
}

OBJECT_CONFIGS = {
    "ElectricEquipment": {
        "def_type": "ElectricEquipment:Definition",
        "def_ref_field": "electric_equipment_definition_name",
        "definition_fields": {
            "design_level_calculation_method",
            "design_level",
            "watts_per_floor_area",
            "watts_per_person",
            "fraction_latent",
            "fraction_radiant",
            "fraction_lost",
        },
    },
    "ElectricEquipment:ITE:AirCooled": {
        "def_type": "ElectricEquipment:ITE:AirCooled:Definition",
        "def_ref_field": "electricequipment_ite_aircooled_definition_name",
        "definition_fields": {
            "air_flow_calculation_method",
            "design_power_input_calculation_method",
            "watts_per_unit",
            "watts_per_floor_area",
            "cpu_power_input_function_of_loading_and_air_temperature_curve_name",
            "design_fan_power_input_fraction",
            "design_fan_air_flow_rate_per_power_input",
            "air_flow_function_of_loading_and_air_temperature_curve_name",
            "fan_power_input_function_of_flow_curve_name",
            "design_entering_air_temperature",
            "environmental_class",
            "air_inlet_connection_type",
            "design_recirculation_fraction",
            "recirculation_function_of_loading_and_supply_temperature_curve_name",
            "design_electric_power_supply_efficiency",
            "electric_power_supply_efficiency_function_of_part_load_ratio_curve_name",
            "fraction_of_electric_power_supply_losses_to_zone",
            "supply_temperature_difference",
            "supply_temperature_difference_schedule",
            "return_temperature_difference",
            "return_temperature_difference_schedule",
        },
        "instance_fields": {
            "zone_or_space_name",
            "number_of_units",
            "design_power_input_schedule_name",
            "cpu_loading_schedule_name",
            "air_inlet_room_air_model_node_name",
            "air_outlet_room_air_model_node_name",
            "supply_air_node_name",
            "cpu_end_use_subcategory",
            "fan_end_use_subcategory",
            "electric_power_supply_end_use_subcategory",
        },
    },
    "GasEquipment": {
        "def_type": "GasEquipment:Definition",
        "def_ref_field": "gas_equipment_definition_name",
        "definition_fields": {
            "design_level_calculation_method",
            "design_level",
            "power_per_floor_area",
            "power_per_person",
            "fraction_latent",
            "fraction_radiant",
            "fraction_lost",
            "carbon_dioxide_generation_rate",
        },
    },
    "HotWaterEquipment": {
        "def_type": "HotWaterEquipment:Definition",
        "def_ref_field": "hot_water_equipment_definition_name",
        "definition_fields": {
            "design_level_calculation_method",
            "design_level",
            "power_per_floor_area",
            "power_per_person",
            "fraction_latent",
            "fraction_radiant",
            "fraction_lost",
        },
    },
    "Lights": {
        "def_type": "Lights:Definition",
        "def_ref_field": "lights_definition_name",
        "definition_fields": {
            "design_level_calculation_method",
            "lighting_level",
            "watts_per_floor_area",
            "watts_per_person",
            "return_air_fraction",
            "fraction_radiant",
            "fraction_visible",
            "return_air_fraction_calculated_from_plenum_temperature",
            "return_air_fraction_function_of_plenum_temperature_coefficient_1",
            "return_air_fraction_function_of_plenum_temperature_coefficient_2",
        },
        "instance_fields": {
            "zone_or_zonelist_or_space_or_spacelist_name",
            "schedule_name",
            "fraction_replaceable",
            "multiplier",
            "end_use_subcategory",
            "return_air_heat_gain_node_name",
            "exhaust_air_heat_gain_node_name",
        },
    },
    "SteamEquipment": {
        "def_type": "SteamEquipment:Definition",
        "def_ref_field": "steam_equipment_definition_name",
        "definition_fields": {
            "design_level_calculation_method",
            "design_level",
            "power_per_floor_area",
            "power_per_person",
            "fraction_latent",
            "fraction_radiant",
            "fraction_lost",
        },
    },
    "OtherEquipment": {
        "def_type": "OtherEquipment:Definition",
        "def_ref_field": "other_equipment_definition_name",
        "definition_fields": {
            "design_level_calculation_method",
            "design_level",
            "power_per_floor_area",
            "power_per_person",
            "fraction_latent",
            "fraction_radiant",
            "fraction_lost",
            "carbon_dioxide_generation_rate",
        },
    },
    "People": {
        "def_type": "People:Definition",
        "def_ref_field": "people_definition_name",
        "definition_fields": {
            "number_of_people_calculation_method",
            "number_of_people",
            "people_per_floor_area",
            "floor_area_per_person",
            "fraction_radiant",
            "sensible_heat_fraction",
            "carbon_dioxide_generation_rate",
            "enable_ashrae_55_comfort_warnings",
            "mean_radiant_temperature_calculation_type",
            "thermal_comfort_model_1_type",
            "thermal_comfort_model_2_type",
            "thermal_comfort_model_3_type",
            "thermal_comfort_model_4_type",
            "thermal_comfort_model_5_type",
            "thermal_comfort_model_6_type",
            "thermal_comfort_model_7_type",
        },
        "instance_fields": {
            "zone_or_zonelist_or_space_or_spacelist_name",
            "number_of_people_schedule_name",
            "activity_level_schedule_name",
            "surface_name_angle_factor_list_name",
            "work_efficiency_schedule_name",
            "clothing_insulation_calculation_method",
            "clothing_insulation_calculation_method_schedule_name",
            "clothing_insulation_schedule_name",
            "air_velocity_schedule_name",
            "ankle_level_air_velocity_schedule_name",
            "cold_stress_temperature_threshold",
            "heat_stress_temperature_threshold",
            "multiplier",
        },
    },
}


def process_object_type(data, obj_type, cfg):
    """Split one object type in a parsed epJSON dict. Returns count of objects converted."""
    def_type = cfg["def_type"]
    def_ref_field = cfg["def_ref_field"]
    definition_fields = cfg["definition_fields"]
    inst_fields = cfg.get("instance_fields", _INSTANCE_FIELDS)

    if obj_type not in data:
        return 0
    if def_type in data:
        return 0  # already converted

    old_objs = data[obj_type]
    new_instances = {}
    new_defs = {}

    for name, obj in old_objs.items():
        def_name = f"{name} Definition"

        instance_entry = {def_ref_field: def_name}
        for field in inst_fields:
            if field in obj:
                instance_entry[field] = obj[field]
        new_instances[name] = instance_entry

        def_entry = {}
        for field in definition_fields:
            if field in obj:
                def_entry[field] = obj[field]
        new_defs[def_name] = def_entry

    # Rebuild data dict, inserting the definition type right after the instance type
    new_data = {}
    for key, value in data.items():
        if key == obj_type:
            new_data[obj_type] = new_instances
            new_data[def_type] = new_defs
        else:
            new_data[key] = value
    data.clear()
    data.update(new_data)
    return len(old_objs)


def process_file(filepath, only_class: str | None = None):
    """Process a single epJSON file in place. Returns number of objects converted."""
    filepath = Path(filepath)
    text = filepath.read_text(encoding="utf-8")
    data = json.loads(text)

    total = 0
    for obj_type, cfg in OBJECT_CONFIGS.items():
        if only_class is not None and obj_type != only_class:
            continue
        total += process_object_type(data=data, obj_type=obj_type, cfg=cfg)

    if total:
        filepath.write_text(json.dumps(data, indent=4) + "\n", encoding="utf-8")
    return total


def main():
    parser = argparse.ArgumentParser(description="Split old-style equipment epJSON objects into instance + definition.")
    parser.add_argument("files", nargs="*", help="Files or directories to process (default: all epJSON under repo)")
    parser.add_argument(
        "--only-class",
        choices=list(OBJECT_CONFIGS),
        metavar="CLASS",
        help=f"Limit split to this class only. Choices: {', '.join(OBJECT_CONFIGS)}",
    )
    args = parser.parse_args()

    if args.files:
        files = []
        for arg in args.files:
            p = Path(arg)
            if p.is_dir():
                files.extend(sorted(p.rglob("*.epJSON")))
            else:
                files.append(p)
    else:
        repo_root = Path(__file__).resolve().parents[2]
        files = sorted((repo_root / "testfiles").rglob("*.epJSON"))
        files += sorted((repo_root / "performance_tests").rglob("*.epJSON"))
        files += sorted((repo_root / "datasets").rglob("*.epJSON"))

    total = 0
    for filepath in files:
        count = process_file(filepath=filepath, only_class=args.only_class)
        if count:
            print(f"  {filepath.name}: {count} object(s) converted")
            total += count

    print(f"\nTotal: {total} equipment object(s) converted across {len(files)} file(s) checked")


if __name__ == "__main__":
    main()
