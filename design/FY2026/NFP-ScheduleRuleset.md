Schedule Ruleset
================

**Joe Robertson, National Laboratory of the Rockies**

 - May 1, 2026 - Initial Draft
 - June 15, 2026 - Revised Draft

## Justification for New Feature ##

EnergyPlus currently requires users to decompose a yearly schedule into a hierarchy of objects: `Schedule:Day:*` → `Schedule:Week:*` → `Schedule:Year`.
This is expressive but verbose and non-intuitive: a typical weekday/weekend/holiday schedule that a user might describe in a few sentences requires many objects and careful date-range bookkeeping.

The OpenStudio SDK has long offered `OS:Schedule:Ruleset` as a higher-level, rule-based abstraction: the user defines a default day schedule and a prioritized list of override rules, each specifying which days of the week and which date range (or specific dates) the rule applies to.
Special design day schedules (summer, winter, holiday, custom) are also first-class fields.
This model is far closer to how schedules are actually specified in building standards, energy codes, and operator manuals.

Today, the OpenStudio ForwardTranslator expands `OS:Schedule:Ruleset` into the `Schedule:Year` + `Schedule:Week:Daily` + `Schedule:Day:Interval` hierarchy before writing an IDF.
Adding `Schedule:Ruleset` natively to EnergyPlus would allow users who author IDF/epJSON files directly to use this intuitive representation, and would eventually allow the OpenStudio ForwardTranslator to emit the compact ruleset form instead of the expanded one.

## E-mail and Conference Call Conclusions ##

Our generalized set of options for OS <-> E+ Alignment:
- Option 1 - Reverse translate from IDF to OSM (one time up front), and then forward translate from OSM to IDF (one time back end)
- Option 2 - Develop and push new objects into E+
- Option 3 - Drop support for an OS model object API; measures need to deal with the deprecation
- Option 4 - Data is backed; RT/FT on the fly (probably very expensive)
 
For handling ScheduleRuleset, we ultimately arrived at Option 2:
- It has widespread use and popularity in OS
- We'd just be adding new standalone object(s) in E+
- It is convenient and easily extensible (i.e., an attractive schedule type)
- The advantages outweigh the other options from above

## Overview ##

Add a new detailed schedule type.
The new schedule type is an alternative to, e.g., `Schedule:Year` and `Schedule:Compact`, for describing detailed schedules.
The new schedule type involves a parent "ruleset" object along with 1 to many "rule" objects.

The parent `Schedule:Ruleset` object:
- Requires references to schedule type limits and a default day schedule
- Optionally references summer, winter, holiday, custom 1, and custom 2 design day schedules

The child(ren) `Schedule:Rule` object(s):
- Requires reference to the parent `Schedule:Ruleset` object
- Requires an "order" be specified for determining rule index amongst other rules
- Requires reference to a day schedule
- Specifies which day(s) of the week for which the rule applies
- Specifies the start month/day and end month/day (date range) or month/day (specific dates) for which the rule applies

During input processing, EnergyPlus will resolve the ruleset into the same internal `ScheduleDetailed` structure used by `Schedule:Year` and `Schedule:Compact`.
No downstream code changes are required; the new objects are transparent to all schedule consumers.

## Approach ##

For the most part, follows the logic and implementation in OS.

Add new `Schedule:Ruleset` and `Schedule:Rule` objects to the IDD.

Make updates and additions to ScheduleManager.hh and ScheduleManager.cc:
- Get all `Schedule:Rule` objects up front; move field values into structs
- Loop through each `Schedule:Ruleset` object
  - Call `AddScheduleDetailed` for creating a new detailed schedule
  - For every day of the year:
    - Get the "priority" schedule rule, i.e., the one whose (A) applicable dates contain the day and (B) has the least rule order value (0 = highest priority)
    - Either get an existing, or add a new, week schedule
    - Update the (12) day schedules according to the ruleset's special day schedules and rule's properties
    - Assign the week schedule to the week schedule's array for the year

It is not required that a day has a rule defined for it (i.e., a `Schedule:Ruleset` may have no attached `Schedule:Rule` objects).
In this case, either:
- The day is Feb 29 and is just made equal to Feb 28, or
- The day falls back to the default day schedule as defined by the ruleset.

Special design day slots (summer, winter, holiday, custom 1, custom 2) are set on all `WeekSchedule` objects.
If a special day schedule is not provided, the default day schedule is used as fallback for that slot.

Allow `Schedule:Ruleset` objects to be actuated by adding the following at the end of the `CurrentModuleObject = "Schedule:Ruleset";` block:
```
  if (s_glob->AnyEnergyManagementSystemInModel) { // setup constant schedules as actuators
      SetupEMSActuator(state, "Schedule:Ruleset", sched->Name, "Schedule Value", "[ ]", sched->EMSActuatedOn, sched->EMSVal);
  }
```

Downstream `GetSchedule` will find the detailed schedules created from `Schedule:Ruleset` and `Schedule:Rule`.

No new data structures are needed in `ScheduleManager.hh`.
The resolution is purely a parse-time operation that flattens the ruleset into the existing `ScheduleDetailed.weekScheds[367]` array.

No existing objects are changed.
No transition is required.

## Testing/Validation/Data Sources ##

Several new unit tests in `tst/EnergyPlus/unit/ScheduleManager.unit.cc` covering:

- A ruleset with two rules demonstrating priority (lower `Rule Order` wins when date ranges overlap).
- `DateRange` specification: correct day assignment across a date range, including wrap-around (e.g., Nov 1 – Jan 31).
- `SpecificDates` specification: only the listed dates get the rule's day schedule.
- Fallback to default day schedule when no rule matches.
- Summer/winter/holiday design day overrides; fallback to default day schedule when not specified.
- Validation errors: unknown `Day Schedule Name`, duplicate `Rule Order` values within a ruleset (warning), missing Default Day Schedule.

A new test file `_ResidentialBaseScheduleRuleset.idf`, where all `Schedule:Year` / `Schedule:Week:Daily` objects are replaced with equivalent `Schedule:Ruleset` / `Schedule:Rule` objects.

## Input Output Reference Documentation ##

Update `doc/input-output-reference/src/overview/group-schedules.tex` with new `Schedule:Ruleset` and `Schedule:Rule` subsections placed directly following `Schedule:Year` and `Schedule:Compact`.
Key points to document:

- The priority model: rules are evaluated in ascending `Rule Order`; the first matching rule wins.
- The `DateRange` vs `SpecificDates` date specification types and their respective fields.
- The special-day schedule fields and their fallback behavior.
- The relationship between `Schedule:Ruleset`, `Schedule:Rule`, and `Schedule:Day:*` objects.

## Input Description ##

IDD:

```
Schedule:Ruleset,
       \memo A Schedule:Ruleset defines a yearly schedule using a default day profile and
       \memo a prioritized list of override rules (Schedule:Rule objects). Rules are evaluated
       \memo in ascending Rule Order; the first matching rule for a given day is used.
       \memo If no rule matches, the Default Day Schedule is used.
       \min-fields 3
  A1 , \field Name
       \type alpha
       \required-field
       \reference ScheduleNames
       \reference ScheduleRulesetNames
  A2 , \field Schedule Type Limits Name
       \type object-list
       \object-list ScheduleTypeLimitsNames
  A3 , \field Default Day Schedule Name
       \required-field
       \type object-list
       \object-list DayScheduleNames
  A4 , \field Summer Design Day Schedule Name
       \note If blank, the Default Day Schedule is used for Summer Design Days.
       \type object-list
       \object-list DayScheduleNames
  A5 , \field Winter Design Day Schedule Name
       \note If blank, the Default Day Schedule is used for Winter Design Days.
       \type object-list
       \object-list DayScheduleNames
  A6 , \field Holiday Schedule Name
       \note If blank, the Default Day Schedule is used for Holidays.
       \type object-list
       \object-list DayScheduleNames
  A7 , \field Custom Day 1 Schedule Name
       \note If blank, the Default Day Schedule is used for Custom Day 1.
       \type object-list
       \object-list DayScheduleNames
  A8 ; \field Custom Day 2 Schedule Name
       \note If blank, the Default Day Schedule is used for Custom Day 2.
       \type object-list
       \object-list DayScheduleNames

Schedule:Rule,
       \memo A Schedule:Rule defines one override rule for a Schedule:Ruleset.
       \memo Rules are evaluated in ascending Rule Order; the first matching rule wins.
       \memo A rule matches a day if: (a) the day falls within the date specification,
       \memo AND (b) the corresponding Apply <DayOfWeek> field is Yes.
       \extensible:2 - repeat last two fields (Specific Month, Specific Day)
       \min-fields 12
  A1 , \field Name
       \type alpha
       \required-field
       \reference ScheduleRuleNames
  A2 , \field Schedule Ruleset Name
       \required-field
       \type object-list
       \object-list ScheduleRulesetNames
  N1 , \field Rule Order
       \note Lower values have higher priority. Must be unique within a Schedule:Ruleset.
       \type integer
       \required-field
       \minimum 0
  A3 , \field Day Schedule Name
       \required-field
       \type object-list
       \object-list DayScheduleNames
  A4 , \field Apply Sunday
       \type choice
       \default No
       \key Yes
       \key No
  A5 , \field Apply Monday
       \type choice
       \default No
       \key Yes
       \key No
  A6 , \field Apply Tuesday
       \type choice
       \default No
       \key Yes
       \key No
  A7 , \field Apply Wednesday
       \type choice
       \default No
       \key Yes
       \key No
  A8 , \field Apply Thursday
       \type choice
       \default No
       \key Yes
       \key No
  A9 , \field Apply Friday
       \type choice
       \default No
       \key Yes
       \key No
  A10, \field Apply Saturday
       \type choice
       \default No
       \key Yes
       \key No
  A11, \field Date Specification Type
       \type choice
       \default DateRange
       \key DateRange
       \key SpecificDates
  N2 , \field Start Month
       \note Used only when Date Specification Type = DateRange.
       \type integer
       \minimum 1
       \maximum 12
       \default 1
  N3 , \field Start Day
       \note Used only when Date Specification Type = DateRange.
       \type integer
       \minimum 1
       \maximum 31
       \default 1
  N4 , \field End Month
       \note Used only when Date Specification Type = DateRange.
       \type integer
       \minimum 1
       \maximum 12
       \default 12
  N5 , \field End Day
       \note Used only when Date Specification Type = DateRange.
       \type integer
       \minimum 1
       \maximum 31
       \default 31
  N6 , \field Specific Month
       \note Used only when Date Specification Type = SpecificDates.
       \note Repeat the Specific Month / Specific Day pair for each date.
       \begin-extensible
       \type integer
       \minimum 1
       \maximum 12
  N7 ; \field Specific Day
       \note Used only when Date Specification Type = SpecificDates.
       \type integer
       \minimum 1
       \maximum 31
```

Example IDF snippet:

```
  Schedule:Ruleset,
    occupants schedule ruleset,             !- Name
    Fractional,                             !- Schedule Type Limits Name
    occupants schedule default day,         !- Default Day Schedule Name
    occupants schedule default day,         !- Summer Design Day Schedule Name
    occupants schedule default day,         !- Winter Design Day Schedule Name
    occupants schedule default day,         !- Holiday Schedule Name
    occupants schedule default day,         !- Custom Day 1 Schedule Name
    occupants schedule default day;         !- Custom Day 2 Schedule Name

  Schedule:Rule,
    occupants schedule rule,                !- Name
    occupants schedule ruleset,             !- Schedule Ruleset Name
    0,                                      !- Rule Order
    occupants schedule day,                 !- Day Schedule Name
    No,                                     !- Apply Sunday
    Yes,                                    !- Apply Monday
    No,                                     !- Apply Tuesday
    Yes,                                    !- Apply Wednesday
    No,                                     !- Apply Thursday
    Yes,                                    !- Apply Friday
    No,                                     !- Apply Saturday
    DateRange,                              !- Date Specification Type
    1,                                      !- Start Month
    1,                                      !- Start Day
    12,                                     !- End Month
    31;                                     !- End Day
```

## Outputs Description ##

No new output variables are added.
`Schedule:Ruleset` objects will appear in existing schedule reporting (e.g., `Output:Schedules`) under their name, identical to `Schedule:Year`.

## Engineering Reference ##

No changes to the Engineering Reference are required.
The rule-resolution algorithm is a straightforward date and day-of-week lookup with no new physics.

## Example File and Transition Changes ##

A new example file `_ResidentialBaseScheduleRuleset.idf` will be added, replacing all `Schedule:Year` / `Schedule:Week:Daily` objects with equivalent `Schedule:Ruleset` / `Schedule:Rule` objects to demonstrate parity.

No transition rules are required; this is a purely additive feature.

## References ##

- OpenStudio Model API - `OS:Schedule:Ruleset` / `OS:Schedule:Rule`
- OpenStudio ForwardTranslator — current expansion to `Schedule:Year`: `ForwardTranslateScheduleRuleset.cpp`
- EnergyPlus Engineering Reference, Schedules chapter
