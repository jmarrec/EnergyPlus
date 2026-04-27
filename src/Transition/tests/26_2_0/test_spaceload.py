def test_electric_equipment_with_end_use_subcat(run_transition_test):
    idf_text = """
  ElectricEquipment,
    SPACE1-1 ElecEq 1,       !- Name
    SPACE1-1,                !- Zone or ZoneList or Space or SpaceList Name
    EQUIP-1,                 !- Schedule Name
    EquipmentLevel,          !- Design Level Calculation Method
    1056.0,                  !- Design Level {W}
    ,                        !- Watts per Floor Area {W/m2}
    0.0,                     !- Watts per Person {W/person}
    0.1,                     !- Fraction Latent
    0.3,                     !- Fraction Radiant
    0.2,                     !- Fraction Lost
    SpaceLoad;               !- End-Use Subcategory
    """

    expected_idf_text = """
  ElectricEquipment,
    SPACE1-1 ElecEq 1,       !- Name
    SPACE1-1 ElecEq 1 Definition,  !- Electric Equipment Definition Name
    SPACE1-1,                !- Zone or ZoneList or Space or SpaceList Name
    EQUIP-1,                 !- Schedule Name
    SpaceLoad;               !- End-Use Subcategory

  ElectricEquipment:Definition,
    SPACE1-1 ElecEq 1 Definition,  !- Name
    EquipmentLevel,          !- Design Level Calculation Method
    1056.0,                  !- Design Level {W}
    ,                        !- Watts per Floor Area {W/m2}
    0.0,                     !- Watts per Person {W/person}
    0.1,                     !- Fraction Latent
    0.3,                     !- Fraction Radiant
    0.2;                     !- Fraction Lost
"""

    run_transition_test(idf_text, expected_idf_text)


def test_electric_equipment_blank_end_use_subcat(run_transition_test):
    idf_text = """
  ElectricEquipment,
    SPACE1-1 ElecEq 1,       !- Name
    SPACE1-1,                !- Zone or ZoneList or Space or SpaceList Name
    EQUIP-1,                 !- Schedule Name
    EquipmentLevel,          !- Design Level Calculation Method
    1056.0,                  !- Design Level {W}
    ,                        !- Watts per Floor Area {W/m2}
    0.0,                     !- Watts per Person {W/person}
    0.1,                     !- Fraction Latent
    0.3,                     !- Fraction Radiant
    0.2,                     !- Fraction Lost
    ;                        !- End-Use Subcategory
    """

    expected_idf_text = """
  ElectricEquipment,
    SPACE1-1 ElecEq 1,       !- Name
    SPACE1-1 ElecEq 1 Definition,  !- Electric Equipment Definition Name
    SPACE1-1,                !- Zone or ZoneList or Space or SpaceList Name
    EQUIP-1,                 !- Schedule Name
    ;                        !- End-Use Subcategory

  ElectricEquipment:Definition,
    SPACE1-1 ElecEq 1 Definition,  !- Name
    EquipmentLevel,          !- Design Level Calculation Method
    1056.0,                  !- Design Level {W}
    ,                        !- Watts per Floor Area {W/m2}
    0.0,                     !- Watts per Person {W/person}
    0.1,                     !- Fraction Latent
    0.3,                     !- Fraction Radiant
    0.2;                     !- Fraction Lost
"""

    run_transition_test(idf_text, expected_idf_text)


def test_electric_equipment_no_end_use_subcat(run_transition_test):
    idf_text = """
  ElectricEquipment,
    SPACE1-1 ElecEq 1,       !- Name
    SPACE1-1,                !- Zone or ZoneList or Space or SpaceList Name
    EQUIP-1,                 !- Schedule Name
    EquipmentLevel,          !- Design Level Calculation Method
    1056.0,                  !- Design Level {W}
    ,                        !- Watts per Floor Area {W/m2}
    0.0,                     !- Watts per Person {W/person}
    0.1,                     !- Fraction Latent
    0.3,                     !- Fraction Radiant
    0.2;                     !- Fraction Lost
    """

    expected_idf_text = """
  ElectricEquipment,
    SPACE1-1 ElecEq 1,       !- Name
    SPACE1-1 ElecEq 1 Definition,  !- Electric Equipment Definition Name
    SPACE1-1,                !- Zone or ZoneList or Space or SpaceList Name
    EQUIP-1;                 !- Schedule Name

  ElectricEquipment:Definition,
    SPACE1-1 ElecEq 1 Definition,  !- Name
    EquipmentLevel,          !- Design Level Calculation Method
    1056.0,                  !- Design Level {W}
    ,                        !- Watts per Floor Area {W/m2}
    0.0,                     !- Watts per Person {W/person}
    0.1,                     !- Fraction Latent
    0.3,                     !- Fraction Radiant
    0.2;                     !- Fraction Lost
"""

    run_transition_test(idf_text, expected_idf_text)


def test_steam_equipment_with_end_use_subcat(run_transition_test):
    idf_text = """
  SteamEquipment,
    SPACE1-1 StmEq 1,        !- Name
    SPACE1-1,                !- Zone or ZoneList or Space or SpaceList Name
    EQUIP-1,                 !- Schedule Name
    EquipmentLevel,          !- Design Level Calculation Method
    2000.0,                  !- Design Level {W}
    ,                        !- Power per Floor Area {W/m2}
    0.0,                     !- Power per Person {W/person}
    0.1,                     !- Fraction Latent
    0.3,                     !- Fraction Radiant
    0.2,                     !- Fraction Lost
    SpaceLoad;               !- End-Use Subcategory
    """

    expected_idf_text = """
  SteamEquipment,
    SPACE1-1 StmEq 1,        !- Name
    SPACE1-1 StmEq 1 Definition,  !- Steam Equipment Definition Name
    SPACE1-1,                !- Zone or ZoneList or Space or SpaceList Name
    EQUIP-1,                 !- Schedule Name
    SpaceLoad;               !- End-Use Subcategory

  SteamEquipment:Definition,
    SPACE1-1 StmEq 1 Definition,  !- Name
    EquipmentLevel,          !- Design Level Calculation Method
    2000.0,                  !- Design Level {W}
    ,                        !- Power per Floor Area {W/m2}
    0.0,                     !- Power per Person {W/person}
    0.1,                     !- Fraction Latent
    0.3,                     !- Fraction Radiant
    0.2;                     !- Fraction Lost
"""

    run_transition_test(idf_text, expected_idf_text)


def test_steam_equipment_blank_end_use_subcat(run_transition_test):
    idf_text = """
  SteamEquipment,
    SPACE1-1 StmEq 1,        !- Name
    SPACE1-1,                !- Zone or ZoneList or Space or SpaceList Name
    EQUIP-1,                 !- Schedule Name
    EquipmentLevel,          !- Design Level Calculation Method
    2000.0,                  !- Design Level {W}
    ,                        !- Power per Floor Area {W/m2}
    0.0,                     !- Power per Person {W/person}
    0.1,                     !- Fraction Latent
    0.3,                     !- Fraction Radiant
    0.2,                     !- Fraction Lost
    ;                        !- End-Use Subcategory
    """

    expected_idf_text = """
  SteamEquipment,
    SPACE1-1 StmEq 1,        !- Name
    SPACE1-1 StmEq 1 Definition,  !- Steam Equipment Definition Name
    SPACE1-1,                !- Zone or ZoneList or Space or SpaceList Name
    EQUIP-1,                 !- Schedule Name
    ;                        !- End-Use Subcategory

  SteamEquipment:Definition,
    SPACE1-1 StmEq 1 Definition,  !- Name
    EquipmentLevel,          !- Design Level Calculation Method
    2000.0,                  !- Design Level {W}
    ,                        !- Power per Floor Area {W/m2}
    0.0,                     !- Power per Person {W/person}
    0.1,                     !- Fraction Latent
    0.3,                     !- Fraction Radiant
    0.2;                     !- Fraction Lost
"""

    run_transition_test(idf_text, expected_idf_text)


def test_steam_equipment_no_end_use_subcat(run_transition_test):
    idf_text = """
  SteamEquipment,
    SPACE1-1 StmEq 1,        !- Name
    SPACE1-1,                !- Zone or ZoneList or Space or SpaceList Name
    EQUIP-1,                 !- Schedule Name
    Power/Area,              !- Design Level Calculation Method
    ,                        !- Design Level {W}
    10.0,                    !- Power per Floor Area {W/m2}
    ,                        !- Power per Person {W/person}
    0.1,                     !- Fraction Latent
    0.3,                     !- Fraction Radiant
    0.2;                     !- Fraction Lost
    """

    expected_idf_text = """
  SteamEquipment,
    SPACE1-1 StmEq 1,        !- Name
    SPACE1-1 StmEq 1 Definition,  !- Steam Equipment Definition Name
    SPACE1-1,                !- Zone or ZoneList or Space or SpaceList Name
    EQUIP-1;                 !- Schedule Name

  SteamEquipment:Definition,
    SPACE1-1 StmEq 1 Definition,  !- Name
    Power/Area,              !- Design Level Calculation Method
    ,                        !- Design Level {W}
    10.0,                    !- Power per Floor Area {W/m2}
    ,                        !- Power per Person {W/person}
    0.1,                     !- Fraction Latent
    0.3,                     !- Fraction Radiant
    0.2;                     !- Fraction Lost
"""

    run_transition_test(idf_text, expected_idf_text)


def test_hot_water_equipment_with_end_use_subcat(run_transition_test):
    idf_text = """
  HotWaterEquipment,
    SPACE1-1 HWEq 1,         !- Name
    SPACE1-1,                !- Zone or ZoneList or Space or SpaceList Name
    EQUIP-1,                 !- Schedule Name
    EquipmentLevel,          !- Design Level Calculation Method
    1500.0,                  !- Design Level {W}
    ,                        !- Power per Floor Area {W/m2}
    0.0,                     !- Power per Person {W/person}
    0.1,                     !- Fraction Latent
    0.3,                     !- Fraction Radiant
    0.2,                     !- Fraction Lost
    SpaceLoad;               !- End-Use Subcategory
    """

    expected_idf_text = """
  HotWaterEquipment,
    SPACE1-1 HWEq 1,         !- Name
    SPACE1-1 HWEq 1 Definition,  !- Hot Water Equipment Definition Name
    SPACE1-1,                !- Zone or ZoneList or Space or SpaceList Name
    EQUIP-1,                 !- Schedule Name
    SpaceLoad;               !- End-Use Subcategory

  HotWaterEquipment:Definition,
    SPACE1-1 HWEq 1 Definition,  !- Name
    EquipmentLevel,          !- Design Level Calculation Method
    1500.0,                  !- Design Level {W}
    ,                        !- Power per Floor Area {W/m2}
    0.0,                     !- Power per Person {W/person}
    0.1,                     !- Fraction Latent
    0.3,                     !- Fraction Radiant
    0.2;                     !- Fraction Lost
"""

    run_transition_test(idf_text, expected_idf_text)


def test_hot_water_equipment_blank_end_use_subcat(run_transition_test):
    idf_text = """
  HotWaterEquipment,
    SPACE1-1 HWEq 1,         !- Name
    SPACE1-1,                !- Zone or ZoneList or Space or SpaceList Name
    EQUIP-1,                 !- Schedule Name
    EquipmentLevel,          !- Design Level Calculation Method
    1500.0,                  !- Design Level {W}
    ,                        !- Power per Floor Area {W/m2}
    0.0,                     !- Power per Person {W/person}
    0.1,                     !- Fraction Latent
    0.3,                     !- Fraction Radiant
    0.2,                     !- Fraction Lost
    ;                        !- End-Use Subcategory
    """

    expected_idf_text = """
  HotWaterEquipment,
    SPACE1-1 HWEq 1,         !- Name
    SPACE1-1 HWEq 1 Definition,  !- Hot Water Equipment Definition Name
    SPACE1-1,                !- Zone or ZoneList or Space or SpaceList Name
    EQUIP-1,                 !- Schedule Name
    ;                        !- End-Use Subcategory

  HotWaterEquipment:Definition,
    SPACE1-1 HWEq 1 Definition,  !- Name
    EquipmentLevel,          !- Design Level Calculation Method
    1500.0,                  !- Design Level {W}
    ,                        !- Power per Floor Area {W/m2}
    0.0,                     !- Power per Person {W/person}
    0.1,                     !- Fraction Latent
    0.3,                     !- Fraction Radiant
    0.2;                     !- Fraction Lost
"""

    run_transition_test(idf_text, expected_idf_text)


def test_hot_water_equipment_no_end_use_subcat(run_transition_test):
    idf_text = """
  HotWaterEquipment,
    SPACE1-1 HWEq 1,         !- Name
    SPACE1-1,                !- Zone or ZoneList or Space or SpaceList Name
    EQUIP-1,                 !- Schedule Name
    Power/Area,              !- Design Level Calculation Method
    ,                        !- Design Level {W}
    10.0,                    !- Power per Floor Area {W/m2}
    ,                        !- Power per Person {W/person}
    0.1,                     !- Fraction Latent
    0.3,                     !- Fraction Radiant
    0.2;                     !- Fraction Lost
    """

    expected_idf_text = """
  HotWaterEquipment,
    SPACE1-1 HWEq 1,         !- Name
    SPACE1-1 HWEq 1 Definition,  !- Hot Water Equipment Definition Name
    SPACE1-1,                !- Zone or ZoneList or Space or SpaceList Name
    EQUIP-1;                 !- Schedule Name

  HotWaterEquipment:Definition,
    SPACE1-1 HWEq 1 Definition,  !- Name
    Power/Area,              !- Design Level Calculation Method
    ,                        !- Design Level {W}
    10.0,                    !- Power per Floor Area {W/m2}
    ,                        !- Power per Person {W/person}
    0.1,                     !- Fraction Latent
    0.3,                     !- Fraction Radiant
    0.2;                     !- Fraction Lost
"""

    run_transition_test(idf_text, expected_idf_text)


def test_other_equipment_with_end_use_subcat(run_transition_test):
    idf_text = """
  OtherEquipment,
    SPACE1-1 OthEq 1,        !- Name
    None,                    !- Fuel Type
    SPACE1-1,                !- Zone or ZoneList or Space or SpaceList Name
    EQUIP-1,                 !- Schedule Name
    EquipmentLevel,          !- Design Level Calculation Method
    1500.0,                  !- Design Level {W}
    ,                        !- Power per Floor Area {W/m2}
    0.0,                     !- Power per Person {W/person}
    0.1,                     !- Fraction Latent
    0.3,                     !- Fraction Radiant
    0.2,                     !- Fraction Lost
    3.45e-8,                 !- Carbon Dioxide Generation Rate {m3/s-W}
    SpaceLoad;               !- End-Use Subcategory
    """

    expected_idf_text = """
  OtherEquipment,
    SPACE1-1 OthEq 1,        !- Name
    SPACE1-1 OthEq 1 Definition,  !- Other Equipment Definition Name
    None,                    !- Fuel Type
    SPACE1-1,                !- Zone or ZoneList or Space or SpaceList Name
    EQUIP-1,                 !- Schedule Name
    SpaceLoad;               !- End-Use Subcategory

  OtherEquipment:Definition,
    SPACE1-1 OthEq 1 Definition,  !- Name
    EquipmentLevel,          !- Design Level Calculation Method
    1500.0,                  !- Design Level {W}
    ,                        !- Power per Floor Area {W/m2}
    0.0,                     !- Power per Person {W/person}
    0.1,                     !- Fraction Latent
    0.3,                     !- Fraction Radiant
    0.2,                     !- Fraction Lost
    3.45e-8;                 !- Carbon Dioxide Generation Rate {m3/s-W}
"""

    run_transition_test(idf_text, expected_idf_text)


def test_other_equipment_blank_end_use_subcat(run_transition_test):
    idf_text = """
  OtherEquipment,
    SPACE1-1 OthEq 1,        !- Name
    ,                        !- Fuel Type
    SPACE1-1,                !- Zone or ZoneList or Space or SpaceList Name
    EQUIP-1,                 !- Schedule Name
    EquipmentLevel,          !- Design Level Calculation Method
    1500.0,                  !- Design Level {W}
    ,                        !- Power per Floor Area {W/m2}
    0.0,                     !- Power per Person {W/person}
    0.1,                     !- Fraction Latent
    0.3,                     !- Fraction Radiant
    0.2,                     !- Fraction Lost
    ,                        !- Carbon Dioxide Generation Rate {m3/s-W}
    ;                        !- End-Use Subcategory
    """

    expected_idf_text = """
  OtherEquipment,
    SPACE1-1 OthEq 1,        !- Name
    SPACE1-1 OthEq 1 Definition,  !- Other Equipment Definition Name
    ,                        !- Fuel Type
    SPACE1-1,                !- Zone or ZoneList or Space or SpaceList Name
    EQUIP-1,                 !- Schedule Name
    ;                        !- End-Use Subcategory

  OtherEquipment:Definition,
    SPACE1-1 OthEq 1 Definition,  !- Name
    EquipmentLevel,          !- Design Level Calculation Method
    1500.0,                  !- Design Level {W}
    ,                        !- Power per Floor Area {W/m2}
    0.0,                     !- Power per Person {W/person}
    0.1,                     !- Fraction Latent
    0.3,                     !- Fraction Radiant
    0.2,                     !- Fraction Lost
    ;                        !- Carbon Dioxide Generation Rate {m3/s-W}
"""

    run_transition_test(idf_text, expected_idf_text)


def test_other_equipment_no_end_use_subcat_no_co2(run_transition_test):
    idf_text = """
  OtherEquipment,
    SPACE1-1 OthEq 1,        !- Name
    FuelOilNo1,              !- Fuel Type
    SPACE1-1,                !- Zone or ZoneList or Space or SpaceList Name
    EQUIP-1,                 !- Schedule Name
    Power/Area,              !- Design Level Calculation Method
    ,                        !- Design Level {W}
    10.0,                    !- Power per Floor Area {W/m2}
    ,                        !- Power per Person {W/person}
    0.1,                     !- Fraction Latent
    0.3,                     !- Fraction Radiant
    0.2;                     !- Fraction Lost
    """

    expected_idf_text = """
  OtherEquipment,
    SPACE1-1 OthEq 1,        !- Name
    SPACE1-1 OthEq 1 Definition,  !- Other Equipment Definition Name
    FuelOilNo1,              !- Fuel Type
    SPACE1-1,                !- Zone or ZoneList or Space or SpaceList Name
    EQUIP-1;                 !- Schedule Name

  OtherEquipment:Definition,
    SPACE1-1 OthEq 1 Definition,  !- Name
    Power/Area,              !- Design Level Calculation Method
    ,                        !- Design Level {W}
    10.0,                    !- Power per Floor Area {W/m2}
    ,                        !- Power per Person {W/person}
    0.1,                     !- Fraction Latent
    0.3,                     !- Fraction Radiant
    0.2;                     !- Fraction Lost
"""

    run_transition_test(idf_text, expected_idf_text)


def test_gas_equipment_with_end_use_subcat(run_transition_test):
    idf_text = """
  GasEquipment,
    SPACE1-1 GasEq 1,        !- Name
    SPACE1-1,                !- Zone or ZoneList or Space or SpaceList Name
    EQUIP-1,                 !- Schedule Name
    EquipmentLevel,          !- Design Level Calculation Method
    1500.0,                  !- Design Level {W}
    ,                        !- Power per Floor Area {W/m2}
    0.0,                     !- Power per Person {W/person}
    0.1,                     !- Fraction Latent
    0.3,                     !- Fraction Radiant
    0.2,                     !- Fraction Lost
    3.45e-8,                 !- Carbon Dioxide Generation Rate {m3/s-W}
    SpaceLoad;               !- End-Use Subcategory
    """

    expected_idf_text = """
  GasEquipment,
    SPACE1-1 GasEq 1,        !- Name
    SPACE1-1 GasEq 1 Definition,  !- Gas Equipment Definition Name
    SPACE1-1,                !- Zone or ZoneList or Space or SpaceList Name
    EQUIP-1,                 !- Schedule Name
    SpaceLoad;               !- End-Use Subcategory

  GasEquipment:Definition,
    SPACE1-1 GasEq 1 Definition,  !- Name
    EquipmentLevel,          !- Design Level Calculation Method
    1500.0,                  !- Design Level {W}
    ,                        !- Power per Floor Area {W/m2}
    0.0,                     !- Power per Person {W/person}
    0.1,                     !- Fraction Latent
    0.3,                     !- Fraction Radiant
    0.2,                     !- Fraction Lost
    3.45e-8;                 !- Carbon Dioxide Generation Rate {m3/s-W}
"""

    run_transition_test(idf_text, expected_idf_text)


def test_gas_equipment_blank_end_use_subcat(run_transition_test):
    idf_text = """
  GasEquipment,
    SPACE1-1 GasEq 1,        !- Name
    SPACE1-1,                !- Zone or ZoneList or Space or SpaceList Name
    EQUIP-1,                 !- Schedule Name
    EquipmentLevel,          !- Design Level Calculation Method
    1500.0,                  !- Design Level {W}
    ,                        !- Power per Floor Area {W/m2}
    0.0,                     !- Power per Person {W/person}
    0.1,                     !- Fraction Latent
    0.3,                     !- Fraction Radiant
    0.2,                     !- Fraction Lost
    ,                        !- Carbon Dioxide Generation Rate {m3/s-W}
    ;                        !- End-Use Subcategory
    """

    expected_idf_text = """
  GasEquipment,
    SPACE1-1 GasEq 1,        !- Name
    SPACE1-1 GasEq 1 Definition,  !- Gas Equipment Definition Name
    SPACE1-1,                !- Zone or ZoneList or Space or SpaceList Name
    EQUIP-1,                 !- Schedule Name
    ;                        !- End-Use Subcategory

  GasEquipment:Definition,
    SPACE1-1 GasEq 1 Definition,  !- Name
    EquipmentLevel,          !- Design Level Calculation Method
    1500.0,                  !- Design Level {W}
    ,                        !- Power per Floor Area {W/m2}
    0.0,                     !- Power per Person {W/person}
    0.1,                     !- Fraction Latent
    0.3,                     !- Fraction Radiant
    0.2,                     !- Fraction Lost
    ;                        !- Carbon Dioxide Generation Rate {m3/s-W}
"""

    run_transition_test(idf_text, expected_idf_text)


def test_gas_equipment_no_end_use_subcat_no_co2(run_transition_test):
    idf_text = """
  GasEquipment,
    SPACE1-1 GasEq 1,        !- Name
    SPACE1-1,                !- Zone or ZoneList or Space or SpaceList Name
    EQUIP-1,                 !- Schedule Name
    Power/Area,              !- Design Level Calculation Method
    ,                        !- Design Level {W}
    10.0,                    !- Power per Floor Area {W/m2}
    ,                        !- Power per Person {W/person}
    0.1,                     !- Fraction Latent
    0.3,                     !- Fraction Radiant
    0.2;                     !- Fraction Lost
    """

    expected_idf_text = """
  GasEquipment,
    SPACE1-1 GasEq 1,        !- Name
    SPACE1-1 GasEq 1 Definition,  !- Gas Equipment Definition Name
    SPACE1-1,                !- Zone or ZoneList or Space or SpaceList Name
    EQUIP-1;                 !- Schedule Name

  GasEquipment:Definition,
    SPACE1-1 GasEq 1 Definition,  !- Name
    Power/Area,              !- Design Level Calculation Method
    ,                        !- Design Level {W}
    10.0,                    !- Power per Floor Area {W/m2}
    ,                        !- Power per Person {W/person}
    0.1,                     !- Fraction Latent
    0.3,                     !- Fraction Radiant
    0.2;                     !- Fraction Lost
"""

    run_transition_test(idf_text, expected_idf_text)


def test_gas_equipment_no_end_use_subcat(run_transition_test):
    idf_text = """
  GasEquipment,
    SPACE1-1 GasEq 1,        !- Name
    SPACE1-1,                !- Zone or ZoneList or Space or SpaceList Name
    EQUIP-1,                 !- Schedule Name
    Power/Area,              !- Design Level Calculation Method
    ,                        !- Design Level {W}
    10.0,                    !- Power per Floor Area {W/m2}
    ,                        !- Power per Person {W/person}
    0.1,                     !- Fraction Latent
    0.3,                     !- Fraction Radiant
    0.2,                     !- Fraction Lost
    3.45e-8;                 !- Carbon Dioxide Generation Rate {m3/s-W}
    """

    expected_idf_text = """
  GasEquipment,
    SPACE1-1 GasEq 1,        !- Name
    SPACE1-1 GasEq 1 Definition,  !- Gas Equipment Definition Name
    SPACE1-1,                !- Zone or ZoneList or Space or SpaceList Name
    EQUIP-1;                 !- Schedule Name

  GasEquipment:Definition,
    SPACE1-1 GasEq 1 Definition,  !- Name
    Power/Area,              !- Design Level Calculation Method
    ,                        !- Design Level {W}
    10.0,                    !- Power per Floor Area {W/m2}
    ,                        !- Power per Person {W/person}
    0.1,                     !- Fraction Latent
    0.3,                     !- Fraction Radiant
    0.2,                     !- Fraction Lost
    3.45e-8;                 !- Carbon Dioxide Generation Rate {m3/s-W}
"""

    run_transition_test(idf_text, expected_idf_text)


def test_other_equipment_no_end_use_subcat(run_transition_test):
    idf_text = """
  OtherEquipment,
    SPACE1-1 OthEq 1,        !- Name
    FuelOilNo1,              !- Fuel Type
    SPACE1-1,                !- Zone or ZoneList or Space or SpaceList Name
    EQUIP-1,                 !- Schedule Name
    Power/Area,              !- Design Level Calculation Method
    ,                        !- Design Level {W}
    10.0,                    !- Power per Floor Area {W/m2}
    ,                        !- Power per Person {W/person}
    0.1,                     !- Fraction Latent
    0.3,                     !- Fraction Radiant
    0.2,                     !- Fraction Lost
    3.45e-8;                 !- Carbon Dioxide Generation Rate {m3/s-W}
    """

    expected_idf_text = """
  OtherEquipment,
    SPACE1-1 OthEq 1,        !- Name
    SPACE1-1 OthEq 1 Definition,  !- Other Equipment Definition Name
    FuelOilNo1,              !- Fuel Type
    SPACE1-1,                !- Zone or ZoneList or Space or SpaceList Name
    EQUIP-1;                 !- Schedule Name

  OtherEquipment:Definition,
    SPACE1-1 OthEq 1 Definition,  !- Name
    Power/Area,              !- Design Level Calculation Method
    ,                        !- Design Level {W}
    10.0,                    !- Power per Floor Area {W/m2}
    ,                        !- Power per Person {W/person}
    0.1,                     !- Fraction Latent
    0.3,                     !- Fraction Radiant
    0.2,                     !- Fraction Lost
    3.45e-8;                 !- Carbon Dioxide Generation Rate {m3/s-W}
"""

    run_transition_test(idf_text, expected_idf_text)
