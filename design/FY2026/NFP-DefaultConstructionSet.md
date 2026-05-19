Default Construction Set
================

**Kyle Benne, National Laboratory of the Rockies**

 - Original Date: 05/19/2026
 - Revision Date: 05/19/2026

## Justification for New Feature ##

For support of OS <-> E+ Alignment.
`DefaultConstructionSet` is widely used in OpenStudio; breaking API here would have too much impact.

EnergyPlus currently requires a `Construction Name` to be explicitly specified on every surface and sub-surface — potentially hundreds or thousands of objects in a large model. 

In OpenStudio, instead of having to hardcode the Construction on each building Surface/SubSurface, we can assign a `DefaultConstructionSet` at various inheritance levels, from most specific to less specific:

- Space
- SpaceType (no E+ equivalent)
- BuildingStory (no E+ equivalent)
- Building

Constructions are resolved at run time based on surface type and boundary condition. This offers several advantages:

1. **DRY input data** — a single construction assignment at the building or space level avoids redundant repetition across every surface.
2. **Easy global swaps** — changing a construction type (e.g., upgrading wall insulation) requires editing one object instead of hundreds.
3. **Less error-prone** — assignment is based on a surface's semantic role (Exterior Wall, Interior Floor, Exterior Window, etc.) rather than requiring the user to manually track every surface.
4. **OS ↔ E+ Alignment** — the E+ IDD and OpenStudio Model API share the same conceptual shape, reducing the translation surface and maintenance burden, and enabling OpenStudio to work directly on any IDF.

## E-mail and Conference Call Conclusions ##

N/A

## Overview ##

Three new IDD objects are added:

- **`DefaultConstructionSet`** — top-level object referenced by `Building` and/or `Space`. Groups references to `DefaultSurfaceConstructions`, `DefaultSubSurfaceConstructions`, and individual constructions for special cases (interior partitions, adiabatic surfaces).
- **`DefaultSurfaceConstructions`** — holds Floor, Wall, and Roof/Ceiling construction references for one category (exterior, interior, or ground-contact).
- **`DefaultSubSurfaceConstructions`** — holds construction references for each sub-surface type (fixed window, operable window, door, glass door, overhead door, skylight, tubular daylighting) for one category (exterior or interior).

Two existing objects are extended with an optional field:

- **`Building`** — gains `Default Construction Set Name`.
- **`Space`** — gains `Default Construction Set Name`.

The `Construction Name` field on `BuildingSurface:Detailed`, `FenestrationSurface:Detailed`, and `InternalMass` is made optional. If blank, construction is resolved at input processing time by searching Space → Building; if still unresolved, a fatal error is issued.

## Approach ##

### IDD Changes

Add `DefaultConstructionSet`, `DefaultSurfaceConstructions`, and `DefaultSubSurfaceConstructions` objects (see **Input Description** for full IDD text).

Add an optional `Default Construction Set Name` field to `Building` (after the existing `Minimum Number of Warmup Days` field) and to `Space` (before the extensible `Tag` fields).

Remove `\required-field` from `Construction Name` (field A3) in `BuildingSurface:Detailed`, `FenestrationSurface:Detailed`, and `InternalMass`.

Extend `FenestrationSurface:Detailed`'s `Surface Type` field with four new `\key` values: `FixedWindow`, `OperableWindow`, `Skylight`, `OverheadDoor`. These allow users (and the OpenStudio ForwardTranslator) to express sub-surface intent more precisely, eliminating the current lossy collapse of FixedWindow/OperableWindow/Skylight → `Window` and OverheadDoor → `Door`.

### C++ Changes

**New `SurfaceClass` enum values** (`DataSurfaces.hh`):

Add `FixedWindow`, `OperableWindow`, `Skylight`, and `OverheadDoor` to the `SurfaceClass` enum before `Num`. The existing comment at line 151 already warns that downstream data in `ComputeNominalUwithConvCoeffs` (`DataHeatBalance.cc`) must be kept in sync.

**`OriginalClass` / `Class` remapping** (`SurfaceGeometry.cc` ~line 2469):

EnergyPlus already uses this pattern for `GlassDoor` and `TDD_Diffuser`: `OriginalClass` stores the IDD-declared type while `Class` is remapped for thermal calculations. Extend the existing loop with:

```cpp
if (surf.Class == SurfaceClass::FixedWindow || surf.Class == SurfaceClass::OperableWindow ||
    surf.Class == SurfaceClass::Skylight) {
    surf.Class = SurfaceClass::Window;
}
if (surf.Class == SurfaceClass::OverheadDoor) {
    surf.Class = SurfaceClass::Door;
}
```

**Downstream touch-points** — several places test `OriginalClass` against an explicit set of classes and must be extended to include the new types:

- `AirflowNetwork/Solver.cpp` — multiple checks for `OriginalClass == Window | Door | GlassDoor` that determine whether a sub-surface participates in airflow; the new types must be included.
- `HeatBalanceSurfaceManager.cc` line 1255 — counting logic that special-cases `GlassDoor` and `TDD_Diffuser` by `OriginalClass`; extend to handle new types.
- `OutputReports.cc` lines 651–660 and 950–959 — reporting blocks that enumerate `Window | GlassDoor | TDD_Dome | TDD_Diffuser` by `OriginalClass`; extend for new types.
- `DataHeatBalance.cc` — `ComputeNominalUwithConvCoeffs` per the existing enum comment.

All of these are additive `||` extensions to existing enum checks — no structural changes required.

**Reading new objects** (new function, likely in `SurfaceGeometry.cc` or a dedicated helper):

- Parse all `DefaultSurfaceConstructions` and `DefaultSubSurfaceConstructions` objects into structs.
- Parse all `DefaultConstructionSet` objects, resolving the nested object-list references.
- Read the new `Default Construction Set Name` field from `Building` and each `Space`.

**Construction resolution** (`GetHTSurfaceData` in `SurfaceGeometry.cc`):

For each `BuildingSurface:Detailed` or `FenestrationSurface:Detailed` with a blank `Construction Name`, apply the following resolution in priority order:

1. Hardcoded construction on the surface (existing behavior — unchanged).
2. `DefaultConstructionSet` on the surface's `Space` (if the Space has one).
3. `DefaultConstructionSet` on the `Building`.
4. If still unresolved → fatal error.

The lookup within a `DefaultConstructionSet` uses `surf.OriginalClass` (the IDD-declared type, set before the thermal remapping) and the outside boundary condition:

| `surf.OriginalClass` | Outside BC | Slot in `DefaultConstructionSet` |
|---|---|---|
| Wall / Floor / Roof | Outdoors | Exterior Surface Constructions → Wall/Floor/Roof slot |
| Wall / Floor / Roof | Surface / Zone / Space | Interior Surface Constructions → Wall/Floor/Roof slot |
| Wall / Floor / Roof | Ground* / Foundation | Ground Contact Surface Constructions → Wall/Floor/Roof slot |
| Wall / Floor / Roof | Adiabatic | Adiabatic Surface Construction (direct) |
| FixedWindow / OperableWindow / Skylight / GlassDoor / TDD_Dome / TDD_Diffuser | Exterior parent | Exterior SubSurface Constructions → matching slot |
| FixedWindow / OperableWindow / Skylight / GlassDoor / TDD_Dome / TDD_Diffuser | Interior parent | Interior SubSurface Constructions → matching slot |
| Door / OverheadDoor | Exterior parent | Exterior SubSurface Constructions → matching slot |
| Door / OverheadDoor | Interior parent | Interior SubSurface Constructions → matching slot |
| IntMass | — | Interior Partition Construction (direct) |

Note: `Ceiling` maps to `Roof` in EnergyPlus (`SurfaceGeometry.cc` line 3895–3897); `DefaultSurfaceConstructions` uses a `Roof Ceiling Construction Name` slot for both.

### Scope Limitations

Shading-related fields present in the OS IDD (`Space Shading Construction Name`, `Building Shading Construction Name`, `Site Shading Construction Name`) are **not** included in this initial implementation. These map to `ShadingProperty:Reflectance` objects in E+ and involve a more complex translation path.

`SpaceType` and `BuildingStory` inheritance levels from OS are not included (no E+ equivalent).

### Backward Compatibility

No existing behavior changes for models that hardcode `Construction Name` on every surface. 

## Testing/Validation/Data Sources ##

New unit tests in `tst/EnergyPlus/unit/SurfaceGeometry.unit.cc` covering:

- Resolution via Building-level `DefaultConstructionSet` only.
- Resolution via Space-level `DefaultConstructionSet`, with Building-level fallback for unset slots.
- Space-level set overrides Building-level set for a specific surface type.
- Adiabatic and interior partition construction resolution.
- Fatal error when construction cannot be resolved at any level.
- Surfaces with hardcoded `Construction Name` are unaffected by any `DefaultConstructionSet`.
- `FenestrationSurface:Detailed` with `Surface Type = FixedWindow / OperableWindow / Skylight / OverheadDoor` is parsed correctly, `OriginalClass` is set, and `Class` is remapped to `Window` / `Door` as appropriate.

A new shoebox example file (see below) will serve as an integration test via the regression suite.

## Input Output Reference Documentation ##

Add new sections to `doc/input-output-reference/src/overview/group-surface-construction-elements.tex` for `DefaultConstructionSet`, `DefaultSurfaceConstructions`, and `DefaultSubSurfaceConstructions`, placed after the existing `Construction` section.

Document the resolution priority (hardcoded → Space → Building), the mapping from surface type + boundary condition to slot, and the fatal error behavior.

Document the new optional `Default Construction Set Name` fields on `Building` and `Space`.

Update `BuildingSurface:Detailed`, `FenestrationSurface:Detailed`, and `InternalMass` field descriptions to note that `Construction Name` may now be blank when a `DefaultConstructionSet` is in effect.*


## Input Description ##

```
DefaultConstructionSet,
       \memo Defines a set of default constructions to be assigned to surfaces and sub-surfaces
       \memo based on their type and outside boundary condition. Can be referenced by Building
       \memo or Space objects. Space-level assignments take precedence over Building-level.
  A1 , \field Name
       \type alpha
       \required-field
       \reference DefaultConstructionSetNames
  A2 , \field Default Exterior Surface Constructions Name
       \note Constructions for exterior walls, floors, and roofs (Outside Boundary Condition = Outdoors).
       \type object-list
       \object-list DefaultSurfaceConstructionsNames
  A3 , \field Default Interior Surface Constructions Name
       \note Constructions for interior surfaces (Outside Boundary Condition = Surface, Zone, or Space).
       \type object-list
       \object-list DefaultSurfaceConstructionsNames
  A4 , \field Default Ground Contact Surface Constructions Name
       \note Constructions for ground-contact surfaces (Outside Boundary Condition = Ground*, Foundation).
       \type object-list
       \object-list DefaultSurfaceConstructionsNames
  A5 , \field Default Exterior SubSurface Constructions Name
       \note Constructions for sub-surfaces on exterior base surfaces.
       \type object-list
       \object-list DefaultSubSurfaceConstructionsNames
  A6 , \field Default Interior SubSurface Constructions Name
       \note Constructions for sub-surfaces on interior base surfaces.
       \type object-list
       \object-list DefaultSubSurfaceConstructionsNames
  A7 , \field Interior Partition Construction Name
       \note Construction for InternalMass objects in spaces referencing this set.
       \type object-list
       \object-list ConstructionNames
  A8 ; \field Adiabatic Surface Construction Name
       \note Construction for surfaces with Outside Boundary Condition = Adiabatic.
       \type object-list
       \object-list ConstructionNames

DefaultSurfaceConstructions,
       \memo Defines constructions for the three opaque surface types (Floor, Wall, Roof/Ceiling)
       \memo for a single boundary-condition category (exterior, interior, or ground-contact).
  A1 , \field Name
       \type alpha
       \required-field
       \reference DefaultSurfaceConstructionsNames
  A2 , \field Floor Construction Name
       \type object-list
       \object-list ConstructionNames
  A3 , \field Wall Construction Name
       \type object-list
       \object-list ConstructionNames
  A4 ; \field Roof Ceiling Construction Name
       \type object-list
       \object-list ConstructionNames

DefaultSubSurfaceConstructions,
       \memo Defines constructions for each sub-surface type for a single boundary-condition
       \memo category (exterior or interior).
  A1 , \field Name
       \type alpha
       \required-field
       \reference DefaultSubSurfaceConstructionsNames
  A2 , \field Fixed Window Construction Name
       \type object-list
       \object-list ConstructionNames
  A3 , \field Operable Window Construction Name
       \type object-list
       \object-list ConstructionNames
  A4 , \field Door Construction Name
       \type object-list
       \object-list ConstructionNames
  A5 , \field Glass Door Construction Name
       \type object-list
       \object-list ConstructionNames
  A6 , \field Overhead Door Construction Name
       \type object-list
       \object-list ConstructionNames
  A7 , \field Skylight Construction Name
       \type object-list
       \object-list ConstructionNames
  A8 , \field Tubular Daylight Dome Construction Name
       \type object-list
       \object-list ConstructionNames
  A9 ; \field Tubular Daylight Diffuser Construction Name
       \type object-list
       \object-list ConstructionNames
```

New fields on existing objects:

```
Building,
  [existing fields...]
  A4 ; \field Default Construction Set Name
       \note Optional. Provides default constructions for surfaces and sub-surfaces in the building.
       \note Space-level Default Construction Set assignments take precedence.
       \type object-list
       \object-list DefaultConstructionSetNames

Space,
  [existing fields, before extensible Tag fields...]
  A4 , \field Default Construction Set Name
       \note Optional. Overrides the Building-level Default Construction Set for surfaces in this Space.
       \type object-list
       \object-list DefaultConstructionSetNames
  A5,  \field Tag 1   ! (renumbered from A4)
       \begin-extensible
  [...]
```

The `Construction Name` field on `BuildingSurface:Detailed`, `FenestrationSurface:Detailed`, and `InternalMass` is made optional (remove `\required-field`).

`FenestrationSurface:Detailed` `Surface Type` gains four new keys:

```
  A2 , \field Surface Type
       \type choice
       \key Window
       \key Door
       \key GlassDoor
       \key TubularDaylightDome
       \key TubularDaylightDiffuser
       \key FixedWindow          ! new
       \key OperableWindow       ! new
       \key Skylight             ! new
       \key OverheadDoor         ! new
```


## Outputs Description ##

No new output variables are added.

## Engineering Reference ##

No changes to the Engineering Reference are required. Construction resolution is a bookkeeping operation with no new physics.

## Example File and Transition Changes ##

A new shoebox example file will be added demonstrating:

- A `Building` with a `DefaultConstructionSet` covering all surface/sub-surface types.
- One `Space` with a partial `DefaultConstructionSet` that overrides only the Exterior and Ground-contact constructions for that space.
- The remaining spaces relying on the building-level set.
- At least one surface per category (exterior wall, interior wall, ground floor, adiabatic, exterior window, interior door).
- A few surfaces with hardcoded `Construction Name` to verify they are unaffected.

**Transition:** No transition rules are needed. Existing files with explicit construction names continue to work without modification.

## References ##

- OpenStudio Model API — `OS:DefaultConstructionSet` / `OS:DefaultSurfaceConstructions`
- OpenStudio `Space_Impl::getDefaultConstructionWithSearchDistance` (`src/model/Space.cpp`)
- OpenStudio `DefaultConstructionSet_Impl::getDefaultConstruction` (`src/model/DefaultConstructionSet.cpp`)
- OpenStudio ForwardTranslator — `ForwardTranslateInteriorPartitionSurface.cpp`
