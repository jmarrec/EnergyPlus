Schedule Ruleset
================

**Joe Robertson, National Laboratory of the Rockies**

 - May 1, 2026 - Initial Draft

## Justification for New Feature ##

For support of OS <-> E+ Alignment.
ScheduleRuleset is widely used; breaking API here would have too much impact.
See additional details below.

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
- Optionally references summer, winter, holiday, custom 1, and custom 2 day schedules

The child(ren) `Schedule:Rule` object(s):
- Requires reference to the parent `Schedule:Ruleset` object
- Requires an "order" be specified for determining rule index amongst other rules
- Requires reference to a day schedule
- Specifies which day(s) of the week for which the rule applies
- Specifies the start month/day and end month/day (date range) or month/day (specific dates) for which the rule applies

## Approach ##

For the most part, follows the logic and implementation in OS.

Add new `Schedule:Ruleset` and `Schedule:Rule` objects to the IDD.

Make updates and additions to ScheduleManager.hh and ScheduleManager.cc:
- Get all `Schedule:Rule` objects up front; move field values into structs
- Loop through each `Schedule:Ruleset` object
  - Call `AddScheduleDetailed` for creating a new detailed schedule
  - For every day of the year:
    - Get the "priority" schedule rule (i.e., the one that applies and has the least rule order value)
    - Either get an existing, or add a new, week schedule
    - Update the (12) day schedules according to the ruleset's special day schedules and rule's properties
    - Assign the week schedule to the week schedule's array for the year

That's it. Downstream `GetSchedule` will find the detailed schedules created from `Schedule:Ruleset` and `Schedule:Rule`.

## Testing/Validation/Data Sources ##

Several new unit tests in tst/EnergyPlus/unit/ScheduleManager.unit.cc.

A new test file _ResidentialBaseScheduleRuleset.idf, where all `Schedule:Year` / `Schedule:Week:Daily` objects are replaced with equivalent `Schedule:Ruleset` / `Schedule:Rule` objects.

## Input Output Reference Documentation ##

Update doc/input-output-reference/src/overview/group-schedules.tex with a new `Schedule:Ruleset` subsection directly following `Schedule:Year` and `Schedule:Compact`.

## Input Description ##

```
  Schedule:Ruleset,
    occupants schedule ruleset              !- Name
    Fractional,                             !- Schedule Type Limits Name
    occupants schedule default day;         !- Default Day Schedule Name
    occupants schedule default day,         !- Summer Design Day Schedule Name
    occupants schedule default day,         !- Winter Design Day Schedule Name
    occupants schedule default day,         !- Holiday Schedule Name
    occupants schedule default day,         !- Custom Day 1 Schedule Name
    occupants schedule default day;         !- Custom Day 2 Schedule Name

  Schedule:Rule,
    occupants schedule rule,                !- Name
    occupants schedule,                     !- Schedule Ruleset Name
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

TODO

## Engineering Reference ##

NA

## Example File and Transition Changes ##

No transition rules.

## References ##

OpenStudio Model API - `OS:Schedule:Ruleset` / `OS:Schedule:Rule`.
