from pathlib import Path

import openstudio
import pandas as pd
from openstudio.openstudiomodel import *

# Taking an unmodified copy of the IDD file, so that it does not YET have the split of ElectricEquipment
EP_IDD_PATH = Path("/home/julien/Software/Others/EnergyPlus2/idd/Energy+.idd.in")

EP_IDD = openstudio.IddFile.load(EP_IDD_PATH).get()


# Get all IddFields for an IddObject
#
# @param iddObject [OpenStudio::IddObject] the IddObject to scan
# @return [Array[OpenStudio::IddField]
def get_idd_fields(iddObject: "openstudio.IddObject") -> list["openstudio.IddField"]:
    """Get all IddFields for an IddObject.

    Parameters
    ----------
    iddObject : openstudio.IddObject
        the IddObject to scan

    Returns
    -------
    fields : List[openstudio.IddField]
    """
    num_fields = iddObject.numFields() + iddObject.properties().numExtensible
    return [iddObject.getField(i).get() for i in range(num_fields)]


def get_fields(iddObject: "openstudio.IddObject") -> dict[str, int]:
    """Scans all fields for an IddObject and return a dict of name to index.

    Parameters
    ----------
    iddObject : openstudio.IddObject
        the IddObject to scan

    Returns
    -------
    fields : Dict[str, int]
        keys are the field names, values are the indices (0-indexed)
    """
    result = {}
    for i, f in enumerate(get_idd_fields(iddObject=iddObject)):
        result[f.name()] = i
    return result


if __name__ == "__main__":

    ep_names = [
        "People",
        "Lights",
        "ElectricEquipment",
        "GasEquipment",
        "HotWaterEquipment",
        "SteamEquipment",
        "OtherEquipment",
    ]

    all_ep_s = []
    all_choices_s = []
    for object_name in ep_names:
        objs = [x for x in EP_IDD.objects() if x.name() == object_name]
        assert len(objs) == 1
        iddObject = objs[0]
        all_ep_s.append(pd.Series(get_fields(iddObject).keys(), name=object_name))

        field = next(
            f
            for i in range(iddObject.numFields() + iddObject.properties().numExtensible)
            if "Calculation Method" in (f := iddObject.getField(i).get()).name()
        )
        all_choices_s.append(pd.Series([k.name() for k in field.keys()], name=object_name))
    df_fields = pd.concat(all_ep_s, axis=1)
    print("Fields:")
    print(df_fields)
    print("Saved df_fields to space_load_fields.csv")
    df_fields.to_csv("space_load_fields.csv", index=False)
    print("\nChoices for Calculation Method:")
    df_choices = pd.concat(all_choices_s, axis=1)
    print(df_choices)
    print(
        "Note: Power/Area is exactly the same as Watts/Area, and Power/Person is exactly the same as Watts/Person. I'll probably delete the Power versions later."
    )
