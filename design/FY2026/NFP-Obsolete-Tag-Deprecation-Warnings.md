Modify `obsolete` Tag Handling for Deprecation Warnings
=======================================================

**Jason W. DeGraw, ORNL**

 - Original Date: 06/18/2026
 - Revision Date: N/A


## Justification for New Feature ##

EnergyPlus currently documents a Level 2 deprecation process in `Deprecation.html`, but the existing IDD tagging behavior does not map cleanly onto that process. The relevant statement is:

> If a release cycle is completed, and a capability is still planned for deprecation, then the relevant input objects will be marked with a deprecation tag, and the object will be marked as Level 2 in this document.

In practice, the IDD has two related mechanisms:

- `\deprecated`, a field-level tag indicating that a field is no longer used
- `\obsolete`, an object-level tag indicating that an object is no longer used and naming a replacement object

The `\deprecated` tag is too narrow for object-level deprecation, and the current `\obsolete` behavior assumes there is always a replacement object. That makes it difficult to mark an object as planned for removal when there is no direct successor.

This proposal updates behavior so that the existing `\obsolete` tag can also support a "to be removed, no replacement" case, enabling EnergyPlus to better follow its own documented deprecation process without introducing new schema constructs.

## E-mail and Conference Call Conclusions ##

N/A

## Overview ##

This proposal changes the behavior associated with the object-level `obsolete` tag in the IDD and generated JSON schema.

Today, the `obsolete` tag is interpreted as a replacement mapping:

- Transition warns that the object is obsolete and identifies the replacement object.
- The EnergyPlus engine does not issue a corresponding warning.

The proposed behavior adds a second interpretation:

- If the value of the `obsolete` tag is different from the object name, keep the current meaning: the object is obsolete and should be replaced by the named object.
- If the value of the `obsolete` tag is the same as the object name, interpret it as "this object is obsolete and will be removed in the future with no replacement object."

This allows the existing tag to express both:

- "replace this object with another object"
- "this object is going away and there is no automatic path forward"

No schema format changes are proposed. The change is strictly behavioral.

## Approach ##

The proposed implementation is:

1. Preserve the existing `obsolete` field in both IDD-derived JSON and legacy IDD processing.
2. Modify transition warning behavior so the warning depends on the `obsolete` value:
   - If `obsolete != object name`, warn that the object is obsolete and should be replaced with the named object.
   - If `obsolete == object name`, warn that the object is obsolete, will be removed in the future, and has no replacement.
3. Add an EnergyPlus runtime warning for obsolete objects encountered by the engine.
4. Reuse the same "same name means no replacement" interpretation in the runtime warning path.

The proposed transition behavior becomes:

- Replacement case:
  - `Obsolete object=GoingAway encountered. Should be replaced with new object=NewObject`
- No-replacement case:
  - Warn that the object is obsolete and will be removed in the future.

The proposed engine behavior becomes:

- Issue a warning when an obsolete object is encountered.
- For self-referential obsolete tags, warn that the object will be removed in the future.
- For replacement mappings, warn that the object is obsolete and optionally identify the replacement object.

One motivating example is `Daylighting:DELight:ComplexFenestration`, which has been identified for possible deprecation in `Deprecation.html`. Under this proposal, it can be marked as:

```text
Daylighting:DELight:ComplexFenestration,
       \obsolete Daylighting:DELight:ComplexFenestration
```

and in JSON as:

```json
"obsolete": "Daylighting:DELight:ComplexFenestration"
```

This keeps the schema unchanged while signaling that the object is on a removal path without naming a successor object.

## Testing/Validation/Data Sources ##

Testing in the near term will be relatively easy, particularly for the runtime warnings. Both unit testing and regression testing for the runtime warnings is easy while an object is marked for deprecation, but once the object is removed, the unit test is no longer legitimate. As such, the recommended approach to testing is to rely on regression testing:

- the addition of the tag will lead to warnings and diffs that will need to be justified, and
- the removal of the object will similarly lead to fewer warnings and thus diffs that will need to be justified.

This approach will have the least impact on developers.

## Input Output Reference Documentation ##

No Input Output Reference changes are expected for object syntax itself because no new input fields, no new objects, and no new outputs are introduced.

However, related user-facing documentation should be updated:

- `Deprecation.html` should clarify that a self-referential `obsolete` tag means the object is marked for future removal with no designated replacement object.
- If desired, warning messages can direct users to `Deprecation.html` and/or the relevant object documentation for more context.

## Input Description ##

No new input syntax is proposed. This feature changes only the interpretation of an existing object-level tag.

Proposed meanings:

```text
OldObject
    \obsolete NewObject
```

Means:

- This object is obsolete.
- Transition and runtime warnings may identify `NewObject` as the replacement.

```text
SameObjectName
    \obsolete SameObjectName
```

Means:

- This object is obsolete.
- The object will be removed in the future.
- There is no replacement object.

## Outputs Description ##

No new report variables or tabular outputs are proposed.

The user-visible output change is new or revised warning behavior in:

- Transition output
- EnergyPlus runtime warning output

An example runtime warning for the no-replacement case could be:

```text
** Warning ** warnObsoleteObjects: Object Daylighting:DELight:ComplexFenestration is obsolete and will be removed in the future.
```

Possible future refinement:

- Modify warning text in both transition and runtime paths to direct users to `Deprecation.html` and/or the Input Output Reference.

## Engineering Reference ##

No Engineering Reference.

## Example File and Transition Changes ##

No transition of object data is required for the no-replacement case because there is, by definition, no replacement object.

Transition changes are limited to warning behavior:

- Replacement case:
  - Keep current warning semantics.
- No-replacement case:
  - Warn that the object is obsolete and will be removed in the future.

Example IDD tagging for a no-replacement object:

```text
Daylighting:DELight:ComplexFenestration,
       \min-fields 5
       \memo Used for DElight Complex Fenestration of all types
       \obsolete Daylighting:DELight:ComplexFenestration
```

Example JSON representation:

```json
"Daylighting:DELight:ComplexFenestration": {
    "patternProperties": {
        "^.*\\S.*$": {
            "type": "object",
            "obsolete": "Daylighting:DELight:ComplexFenestration"
        }
    }
}
```

An example file using `Daylighting:DELight:ComplexFenestration` could be used as a regression case to validate warning behavior in both transition and runtime execution.

This proposal does not resolve how future transition should behave once an object is actually removed. At that point, there may be no guaranteed automatic path forward. That question is outside the scope of this proposal, which is focused on better marking and warning during the deprecation period.

## References ##

N/A
