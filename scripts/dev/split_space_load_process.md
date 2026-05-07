# Space Load Object Split Process

Splitting `XxxEquipment` into `XxxEquipment` (instance) + `XxxEquipment:Definition`.

The instance keeps: Name, `<Xxx> Equipment Definition Name` (new A2), Zone, Schedule, End-Use Subcategory.
The definition holds: Design Level Calculation Method, the three level fields, Fraction Latent/Radiant/Lost.

Completed so far: `ElectricEquipment`, `HotWaterEquipment`, `SteamEquipment`, `GasEquipment`, `OtherEquipment`, `People`, `Lights`, `ElectricEquipment:ITE:AirCooled`.

---

## Lights — differences from the equipment pattern

`Lights` is fully split (IDD, C++, Transition, tests all done), but the split scripts need awareness of its non-standard field layout.

### Why Lights is different

**`Fraction Replaceable` stays in the instance.** Unlike equipment where all fractions belong to the definition, `Fraction Replaceable` is an instance-level field (used by daylighting controls). The definition holds all other fractions (Return Air, Radiant, Visible).

**Return/Exhaust node names stay on the instance.** `Return Air Heat Gain Node Name` and `Exhaust Air Heat Gain Node Name` are zone/HVAC-topology-specific (each zone has its own return air nodes), so they belong on the `Lights` instance, not the definition.

**Old field layout** (1-based, alpha + numeric interleaved by position):

| Old field # | Old name | → New object |
|---|---|---|
| 1 | Name | instance A1 |
| 2 | Zone or ZoneList... | instance A3 |
| 3 | Schedule Name | instance A4 |
| 4 | Design Level Calculation Method | **definition A2** |
| 5–7 | Lighting Level, W/Area, W/Person | **definition N1–N3** |
| 8–10 | Return Air Fraction, Fraction Radiant, Fraction Visible | **definition N4–N6** |
| 11 | Fraction Replaceable | **instance N1** (old `\min-fields 11`) |
| 12 | End-Use Subcategory | instance A5 (opt) |
| 13 | Return Air Fraction Calculated from Plenum Temperature | **definition A3** (opt) |
| 14–15 | Plenum Temp Coefficient 1–2 | **definition N7–N8** (opt) |
| 16–17 | Return Air / Exhaust Air Heat Gain Node Name | **instance A6–A7** (opt) |

**`\min-fields` for old Lights was 11** (up to and including Fraction Replaceable).

### Script support

`split_space_load_idf.py` and `split_space_load_unit_tests.py` have dedicated `transform_lights_block` / `transform_lights_block_cpp` functions dispatched when `transform_block` encounters a `Lights` class line.

`split_space_load_epjson.py` uses per-object `instance_fields` in `OBJECT_CONFIGS["Lights"]` (including `fraction_replaceable`) and lists all 12 definition fields explicitly.

### Production code fix

When a `Lights` definition name doesn't match any `Lights:Definition`, the processing `continue`s before filling the `Lights` array entry, leaving `ZonePtr=0`. A guard `if (zoneNum <= 0) continue;` was added to the post-processing loop in `GetInternalHeatGainsInput` to prevent accessing `ZoneEquipConfig(0)`.

---

## People — differences from the equipment pattern

`People` is fully split (IDD, C++, Transition, tests all done), but the split scripts need awareness of its non-standard field layout.

### Why People is different

**Interleaved fields.** The old monolithic `People` object does not follow the equipment pattern of `Name / Zone / Schedule / [definition fields] / EndUse`. Instead, definition and instance fields are interleaved:

| Old field # | Old name | → New object |
|---|---|---|
| 1 | Name | instance A1 |
| 2 | Zone or ZoneList... | instance A3 |
| 3 | Number of People Schedule Name | instance A4 |
| 4 | Number of People Calculation Method | **definition A2** |
| 5–9 | Number of People, per Floor Area, Floor Area per Person, Fraction Radiant, Sensible Heat Fraction | **definition N1–N5** |
| 10 | Activity Level Schedule Name | instance A5 (old `\min-fields 10`) |
| 11 | Carbon Dioxide Generation Rate | **definition N6** (opt) |
| 12–13 | ASHRAE 55 warnings, MRT Calculation Type | **definition A3–A4** (opt) |
| 14–19 | Surface, Work Efficiency, Clothing×3, Air Velocity | instance A6–A11 (opt) |
| 20–26 | Thermal Comfort Model 1–7 Type | **definition A5–A11** (opt) |
| 27 | Ankle Level Air Velocity Schedule Name | instance A12 (opt) |
| 28–29 | Cold / Heat Stress Temperature Threshold | instance N1–N2 (opt) |

**No End-Use Subcategory.** People has no end-use subcategory field — the instance terminates after Activity Level Schedule (or the last optional instance field).

**Different definition fractions.** No `Fraction Latent` or `Fraction Lost`; instead `Fraction Radiant` and `Sensible Heat Fraction` (`autocalculate` by default).

**TC model types in definition.** Up to 7 Thermal Comfort Model Type fields belong to the definition, not the instance.

### Script support

Because of the interleaved layout, `split_space_load_idf.py` and `split_space_load_unit_tests.py` have a dedicated `transform_people_block` / `transform_people_block_cpp` function that is dispatched automatically when `transform_block` encounters a `People` class line.

`split_space_load_epjson.py` uses per-object `instance_fields` in `OBJECT_CONFIGS["People"]` (instead of the shared `_INSTANCE_FIELDS`) and lists all 16 definition fields explicitly.

---

## 1. IDD — `idd/Energy+.idd.in`

Replace the old monolithic object with two objects.

**Instance object** (example: `SteamEquipment`):
- Change `\min-fields` to `4`
- Add to A1: `\reference SteamEquipmentNames`, `\reference SpaceItemNames`, `\reference SpaceLoadNames`, `\reference SpaceComponentInstanceNames`
- Insert new A2: `\field Steam Equipment Definition Name` with `\type object-list` + `\object-list SteamEquipmentDefinitionNames`
- Renumber old A2→A3 (Zone), old A3→A4 (Schedule)
- Keep A5 End-Use Subcategory (terminate with `;`)
- Remove the calculation method / level / fraction fields

**Definition object** (`SteamEquipment:Definition`):
- Add `\min-fields 8` (N2/N3 have no `\default`; without this the IDF reader won't guarantee all trailing fields are parsed)
- A1 Name: `\reference SteamEquipmentDefinitionNames`, `\reference SpaceComponentDefinitionNames`
- A2 Design Level Calculation Method (choices + `\default EquipmentLevel`)
- N1 Design Level, N2 area field, N3 person field, N4 Fraction Latent, N5 Fraction Radiant, N6 Fraction Lost (terminate N6 with `;`)

Field names vary per object type (see `space_loads.py` / `space_load_fields.csv`):

| Object | area field | person field |
|---|---|---|
| ElectricEquipment | Watts per Floor Area | Watts per Person |
| SteamEquipment | Power per Floor Area | Power per Person |

> **Note on Watts/Area vs Power/Area aliases**: Some objects (e.g., SteamEquipment) accept both `Watts/Area` and `Power/Area` as valid keys for the calculation method, but their definition schema only has the `power_per_floor_area` field (not `watts_per_floor_area`). `GetSpaceLoadDefinition` handles this automatically by checking `objectSchemaProps` and remapping to the correct alias. **No extra C++ work needed** when adding new objects — the alias resolution is already in place.

---

## 2. C++ — `src/EnergyPlus/InternalHeatGains.cc`

Find the `// XxxEquipment` block inside `GetInternalHeatGainsInput`.

**Add before the `if (TotXxxEquip > 0)` block:**
```cpp
std::vector<ZoneEquipDefinitionData> xxxLoadDefs = GetSpaceLoadDefinition(state, "XxxEquipment:Definition");
```

**Inside the per-instance loop, replace:**
- Schedule lookup: `IHGAlphas(3)` → `IHGAlphas(4)`
- Remove the `levelMethod`/`fieldNum` switch block and `IHGNumbers(fieldNum)` reads
- Add definition lookup:
```cpp
std::string defName = IHGAlphas(2);
auto itDef = std::find_if(xxxLoadDefs.begin(), xxxLoadDefs.end(),
    [&defName](ZoneEquipDefinitionData const &d) { return d.Name == defName; });
if (itDef == xxxLoadDefs.end()) {
    ShowSevereItemNotFound(state, eoh, IHGAlphaFieldNames(2), defName);
    ErrorsFound = true;
    continue;
}
```
- Pass `itDef->designLevelMethod`, `itDef->levelValue`, `itDef->levelIsBlank`, `itDef->levelField` to `setDesignLevel()`
- Replace `IHGNumbers(4/5/6)` with `itDef->FractionLatent/Radiant/Lost`

`ZoneEquipDefinitionData` and `GetSpaceLoadDefinition` are already declared in `InternalHeatGains.hh` (added for ElectricEquipment). **No header changes needed.**

---

## 3. Transition — `src/Transition/CreateNewIDFUsingRulesV26_2_0.f90`

Add under `! If your original object starts with <letter>`:

```fortran
CASE('XXXEQUIPMENT')
  CALL GetNewObjectDefInIDD(ObjectName,NwNumArgs,NwAorN,NwReqFld,NwObjMinFlds,NwFldNames,NwFldDefaults,NwFldUnits)
  nodiff=.false.
  OutArgs(1) = InArgs(1)                          ! A1 Name
  OutArgs(2) = TRIM(InArgs(1)) // ' Definition'   ! A2 new def ref
  OutArgs(3) = InArgs(2)                          ! A3 Zone (was A2)
  OutArgs(4) = InArgs(3)                          ! A4 Schedule (was A3)
  IF (CurArgs >= 11) THEN
    OutArgs(5) = InArgs(11)                       ! A5 End-Use Subcategory (was A5/field 11)
    CurArgs = 5
  ELSE
    CurArgs = 4
  END IF
  CALL WriteOutIDFLines(DifLfn,'XxxEquipment',CurArgs,OutArgs,NwFldNames,NwFldUnits)

  ObjectName = 'XxxEquipment:Definition'
  CALL GetNewObjectDefInIDD(ObjectName,NwNumArgs,NwAorN,NwReqFld,NwObjMinFlds,NwFldNames,NwFldDefaults,NwFldUnits)
  OutArgs(1) = TRIM(InArgs(1)) // ' Definition'   ! A1 Name
  OutArgs(2) = InArgs(4)   ! Design Level Calculation Method (was A4/field 4)
  OutArgs(3) = InArgs(5)   ! Design Level         (was N1/field 5)
  OutArgs(4) = InArgs(6)   ! Area field            (was N2/field 6)
  OutArgs(5) = InArgs(7)   ! Person field          (was N3/field 7)
  OutArgs(6) = InArgs(8)   ! Fraction Latent       (was N4/field 8)
  OutArgs(7) = InArgs(9)   ! Fraction Radiant      (was N5/field 9)
  OutArgs(8) = InArgs(10)  ! Fraction Lost         (was N6/field 10)
  CurArgs = 8
  CALL WriteOutIDFLines(DifLfn,ObjectName,CurArgs,OutArgs,NwFldNames,NwFldUnits)
  Written = .true.
```

Old field numbering (all types follow the same layout):
`InArgs(1..4)` = A1 Name, A2 Zone, A3 Schedule, A4 Method; `InArgs(5..10)` = N1–N6; `InArgs(11)` = A5 End-Use.

---

## 4. Transition rules doc — `src/Transition/InputRulesFiles/Rules26-1-0-to-26-2-0.md`

Add a section:
```markdown
# Object Change: XxxEquipment and new XxxEquipment:Definition

The object was split into two: `XxxEquipment` and `XxxEquipment:Definition`.

Moved to `XxxEquipment:Definition`:
- old A4 `Design Level Calculation Method` → new A2
- old N1–N6 (level + fractions) → new N1–N6

Renumbered in `XxxEquipment`:
- old A2 Zone → new A3 / old A3 Schedule → new A4 / old A5 End-Use → new A5 (unchanged)

New A2 `Xxx Equipment Definition Name` inserted in `XxxEquipment`.
```

---

## 5. Transition tests — `src/Transition/tests/26_2_0/test_spaceload.py`

Add 3 or 4 tests following the pattern used for the other equipment types:

| Test name | What it checks |
|---|---|
| `test_xxx_equipment_with_end_use_subcat` | Full old object (including CO2 rate if applicable) → split into instance + definition; End-Use Subcategory preserved |
| `test_xxx_equipment_blank_end_use_subcat` | Blank End-Use Subcategory field (explicit `;`) round-trips correctly |
| `test_xxx_equipment_no_end_use_subcat_no_co2` | Old object without optional trailing fields → instance terminates at Schedule, definition terminates at Fraction Lost |
| `test_xxx_equipment_no_end_use_subcat` | CO2 rate present but no End-Use Subcategory → instance terminates at Schedule, definition terminates at CO2 rate |

The last two tests apply to objects **with a CO2 rate field** (GasEquipment, OtherEquipment). For objects **without a CO2 rate field** (HotWaterEquipment, SteamEquipment), use only 3 tests (`_with_end_use_subcat`, `_blank_end_use_subcat`, `_no_end_use_subcat`).

---

## 6. Unit tests — `tst/EnergyPlus/unit/InternalHeatGains.unit.cc`

Add 5 tests after the last `ElectricEquipment` test group, following the exact same pattern:

| Test name | What it checks |
|---|---|
| `InternalHeatGains_XxxEquipment` | Two instances sharing one definition; verifies name, zone, schedule, end-use, level, fractions |
| `InternalHeatGains_XxxEquipment_InvalidDefinition` | Typo in definition name → `FatalError` thrown, correct `Severe` messages |
| `InternalHeatGains_XxxEquipment_PerArea` | `Watts/Area` (or `Power/Area`) method; zone floor area set manually |
| `InternalHeatGains_XxxEquipment_PerPerson` | `Watts/Person` method; `TotOccupants` set manually |
| `InternalHeatGains_XxxEquipment_MissingLevelField` | Method/field mismatch → two warnings, `DesignLevel == 0` |

For `_MissingLevelField`, the expected warning strings mention the **actual schema field name** after alias resolution, not the method key the user wrote. For objects whose schema uses `power_per_floor_area` (e.g., SteamEquipment), even if the user wrote `Watts/Area`, the warning says `power_per_floor_area`:
```
getSpaceLoadDefinition: XxxEquipment:Definition="DEFNAME", specifies Method=WATTS/AREA, but the corresponding field "<resolved_field>"is blank. 0 will result.
GetInternalHeatGains: XxxEquipment="INSTNAME", specifies <resolved_field>, but that field is blank.  0 XxxEquipment will result.
```
Where `<resolved_field>` is `watts_per_floor_area` for ElectricEquipment and `power_per_floor_area` for SteamEquipment.
(Note: no space before `is blank` in the first message — matches the existing warning format.)

---

## 7. Update split scripts

### `split_space_load_idf.py` and `split_space_load_unit_tests.py`

Add the new type to `OBJECTS_TO_SPLIT` in `split_space_load_idf.py`:
```python
OBJECTS_TO_SPLIT = {
    "ElectricEquipment": "Electric Equipment Definition Name",
    "SteamEquipment":    "Steam Equipment Definition Name",
    "XxxEquipment":      "Xxx Equipment Definition Name",   # add here
}
```
The IDF and unit-test scripts read this dict from the imported `_idf` module — no other changes needed.

For objects that include a CO2 generation rate field in the definition (currently `GasEquipment` and `OtherEquipment`), also add an entry to `OBJECT_MAX_DEF_FIELDS` (set to `8`) and `OBJECT_MIN_DEF_FIELDS` (set to `7`, because the CO2 field has a default and may be absent in old files).

### `split_space_load_epjson.py`

Add an entry to `OBJECT_CONFIGS`:
```python
"XxxEquipment": {
    "def_type": "XxxEquipment:Definition",
    "def_ref_field": "xxx_equipment_definition_name",
    "definition_fields": {
        "design_level_calculation_method",
        "design_level",
        "<area_field>",    # e.g. watts_per_floor_area or power_per_floor_area
        "<person_field>",  # e.g. watts_per_person or power_per_person
        "fraction_latent",
        "fraction_radiant",
        "fraction_lost",
        # "carbon_dioxide_generation_rate",  # only for GasEquipment / OtherEquipment
    },
},
```

---

## 8. Run the scripts

```bash
# From repo root:
python scripts/dev/split_space_load_idf.py
python scripts/dev/split_space_load_unit_tests.py
python scripts/dev/split_space_load_epjson.py
```

Default (no args) covers `testfiles/`, `performance_tests/`, `datasets/` for IDF/IMF/epJSON, and `tst/EnergyPlus/unit/` for unit tests.

To limit the split to a single class (useful when adding one new type at a time):

```bash
python scripts/dev/split_space_load_idf.py          --only-class ElectricEquipment
python scripts/dev/split_space_load_unit_tests.py   --only-class ElectricEquipment
python scripts/dev/split_space_load_epjson.py       --only-class ElectricEquipment
```
