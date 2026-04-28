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


def test_people_minimal(run_transition_test):
    """People object with only the required fields (old min-fields 10: through Activity Level Schedule)."""
    idf_text = """
  People,
    SPACE1-1 People 1,       !- Name
    SPACE1-1,                !- Zone or ZoneList or Space or SpaceList Name
    PEOPLE-1,                !- Number of People Schedule Name
    People,                  !- Number of People Calculation Method
    10.0,                    !- Number of People {people}
    ,                        !- People per Zone Floor Area {person/m2}
    ,                        !- Zone Floor Area per Person {m2/person}
    0.3,                     !- Fraction Radiant
    autocalculate,           !- Sensible Heat Fraction
    ACT-SCHED;               !- Activity Level Schedule Name
    """

    expected_idf_text = """
  People,
    SPACE1-1 People 1,       !- Name
    SPACE1-1 People 1 Definition,  !- People Definition Name
    SPACE1-1,                !- Zone or ZoneList or Space or SpaceList Name
    PEOPLE-1,                !- Number of People Schedule Name
    ACT-SCHED;               !- Activity Level Schedule Name

  People:Definition,
    SPACE1-1 People 1 Definition,  !- Name
    People,                  !- Number of People Calculation Method
    10.0,                    !- Number of People
    ,                        !- People per Floor Area {person/m2}
    ,                        !- Floor Area per Person {m2/person}
    0.3,                     !- Fraction Radiant
    autocalculate;           !- Sensible Heat Fraction
"""

    run_transition_test(idf_text, expected_idf_text)


def test_people_with_co2_and_ashrae55(run_transition_test):
    """People object with CO2, ASHRAE55, and MRT type fields (fields 11-13)."""
    idf_text = """
  People,
    SPACE1-1 People 1,       !- Name
    SPACE1-1,                !- Zone or ZoneList or Space or SpaceList Name
    PEOPLE-1,                !- Number of People Schedule Name
    People,                  !- Number of People Calculation Method
    10.0,                    !- Number of People {people}
    ,                        !- People per Zone Floor Area {person/m2}
    ,                        !- Zone Floor Area per Person {m2/person}
    0.3,                     !- Fraction Radiant
    autocalculate,           !- Sensible Heat Fraction
    ACT-SCHED,               !- Activity Level Schedule Name
    3.82e-8,                 !- Carbon Dioxide Generation Rate {m3/s-W}
    Yes,                     !- Enable ASHRAE 55 Comfort Warnings
    EnclosureAveraged;       !- Mean Radiant Temperature Calculation Type
    """

    expected_idf_text = """
  People,
    SPACE1-1 People 1,       !- Name
    SPACE1-1 People 1 Definition,  !- People Definition Name
    SPACE1-1,                !- Zone or ZoneList or Space or SpaceList Name
    PEOPLE-1,                !- Number of People Schedule Name
    ACT-SCHED;               !- Activity Level Schedule Name

  People:Definition,
    SPACE1-1 People 1 Definition,  !- Name
    People,                  !- Number of People Calculation Method
    10.0,                    !- Number of People
    ,                        !- People per Floor Area {person/m2}
    ,                        !- Floor Area per Person {m2/person}
    0.3,                     !- Fraction Radiant
    autocalculate,           !- Sensible Heat Fraction
    3.82e-8,                 !- Carbon Dioxide Generation Rate {m3/s-W}
    Yes,                     !- Enable ASHRAE 55 Comfort Warnings
    EnclosureAveraged;       !- Mean Radiant Temperature Calculation Type
"""

    run_transition_test(idf_text, expected_idf_text)


def test_people_with_tc_models(run_transition_test):
    """People object with thermal comfort schedules and TC model types."""
    idf_text = """
  People,
    SPACE1-1 People 1,       !- Name
    SPACE1-1,                !- Zone or ZoneList or Space or SpaceList Name
    PEOPLE-1,                !- Number of People Schedule Name
    People,                  !- Number of People Calculation Method
    10.0,                    !- Number of People {people}
    ,                        !- People per Zone Floor Area {person/m2}
    ,                        !- Zone Floor Area per Person {m2/person}
    0.3,                     !- Fraction Radiant
    autocalculate,           !- Sensible Heat Fraction
    ACT-SCHED,               !- Activity Level Schedule Name
    3.82e-8,                 !- Carbon Dioxide Generation Rate {m3/s-W}
    No,                      !- Enable ASHRAE 55 Comfort Warnings
    EnclosureAveraged,       !- Mean Radiant Temperature Calculation Type
    ,                        !- Surface Name/Angle Factor List Name
    WORK-EFF-SCHED,          !- Work Efficiency Schedule Name
    ClothingInsulationSchedule,  !- Clothing Insulation Calculation Method
    ,                        !- Clothing Insulation Calculation Method Schedule Name
    CLO-SCHED,               !- Clothing Insulation Schedule Name
    AIR-VEL-SCHED,           !- Air Velocity Schedule Name
    Fanger,                  !- Thermal Comfort Model 1 Type
    Pierce;                  !- Thermal Comfort Model 2 Type
    """

    expected_idf_text = """
  People,
    SPACE1-1 People 1,       !- Name
    SPACE1-1 People 1 Definition,  !- People Definition Name
    SPACE1-1,                !- Zone or ZoneList or Space or SpaceList Name
    PEOPLE-1,                !- Number of People Schedule Name
    ACT-SCHED,               !- Activity Level Schedule Name
    ,                        !- Surface Name/Angle Factor List Name
    WORK-EFF-SCHED,          !- Work Efficiency Schedule Name
    ClothingInsulationSchedule,  !- Clothing Insulation Calculation Method
    ,                        !- Clothing Insulation Calculation Method Schedule Name
    CLO-SCHED,               !- Clothing Insulation Schedule Name
    AIR-VEL-SCHED;           !- Air Velocity Schedule Name

  People:Definition,
    SPACE1-1 People 1 Definition,  !- Name
    People,                  !- Number of People Calculation Method
    10.0,                    !- Number of People
    ,                        !- People per Floor Area {person/m2}
    ,                        !- Floor Area per Person {m2/person}
    0.3,                     !- Fraction Radiant
    autocalculate,           !- Sensible Heat Fraction
    3.82e-8,                 !- Carbon Dioxide Generation Rate {m3/s-W}
    No,                      !- Enable ASHRAE 55 Comfort Warnings
    EnclosureAveraged,       !- Mean Radiant Temperature Calculation Type
    Fanger,                  !- Thermal Comfort Model 1 Type
    Pierce;                  !- Thermal Comfort Model 2 Type
"""

    run_transition_test(idf_text, expected_idf_text)


def test_lights_with_end_use_subcat(run_transition_test):
    """Lights object with End-Use Subcategory → split into instance + definition."""
    idf_text = """
  Lights,
    SPACE1-1 Lights 1,       !- Name
    SPACE1-1,                !- Zone or ZoneList or Space or SpaceList Name
    LIGHTS-1,                !- Schedule Name
    LightingLevel,           !- Design Level Calculation Method
    1000.0,                  !- Lighting Level {W}
    ,                        !- Watts per Floor Area {W/m2}
    ,                        !- Watts per Person {W/person}
    0.0,                     !- Return Air Fraction
    0.32,                    !- Fraction Radiant
    0.25,                    !- Fraction Visible
    1.0,                     !- Fraction Replaceable
    General;                 !- End-Use Subcategory
    """

    expected_idf_text = """
  Lights,
    SPACE1-1 Lights 1,       !- Name
    SPACE1-1 Lights 1 Definition,  !- Lights Definition Name
    SPACE1-1,                !- Zone or ZoneList or Space or SpaceList Name
    LIGHTS-1,                !- Schedule Name
    1.0,                     !- Fraction Replaceable
    General;                 !- End-Use Subcategory

  Lights:Definition,
    SPACE1-1 Lights 1 Definition,  !- Name
    LightingLevel,           !- Design Level Calculation Method
    1000.0,                  !- Lighting Level {W}
    ,                        !- Watts per Floor Area {W/m2}
    ,                        !- Watts per Person {W/person}
    0.0,                     !- Return Air Fraction
    0.32,                    !- Fraction Radiant
    0.25;                    !- Fraction Visible
"""

    run_transition_test(idf_text, expected_idf_text)


def test_lights_blank_end_use_subcat(run_transition_test):
    """Lights with blank End-Use Subcategory round-trips correctly."""
    idf_text = """
  Lights,
    SPACE1-1 Lights 1,       !- Name
    SPACE1-1,                !- Zone or ZoneList or Space or SpaceList Name
    LIGHTS-1,                !- Schedule Name
    LightingLevel,           !- Design Level Calculation Method
    1000.0,                  !- Lighting Level {W}
    ,                        !- Watts per Floor Area {W/m2}
    ,                        !- Watts per Person {W/person}
    0.0,                     !- Return Air Fraction
    0.32,                    !- Fraction Radiant
    0.25,                    !- Fraction Visible
    1.0,                     !- Fraction Replaceable
    ;                        !- End-Use Subcategory
    """

    expected_idf_text = """
  Lights,
    SPACE1-1 Lights 1,       !- Name
    SPACE1-1 Lights 1 Definition,  !- Lights Definition Name
    SPACE1-1,                !- Zone or ZoneList or Space or SpaceList Name
    LIGHTS-1,                !- Schedule Name
    1.0,                     !- Fraction Replaceable
    ;                        !- End-Use Subcategory

  Lights:Definition,
    SPACE1-1 Lights 1 Definition,  !- Name
    LightingLevel,           !- Design Level Calculation Method
    1000.0,                  !- Lighting Level {W}
    ,                        !- Watts per Floor Area {W/m2}
    ,                        !- Watts per Person {W/person}
    0.0,                     !- Return Air Fraction
    0.32,                    !- Fraction Radiant
    0.25;                    !- Fraction Visible
"""

    run_transition_test(idf_text, expected_idf_text)


def test_lights_no_end_use_subcat(run_transition_test):
    """Old Lights without End-Use Subcategory → instance terminates at Fraction Replaceable."""
    idf_text = """
  Lights,
    SPACE1-1 Lights 1,       !- Name
    SPACE1-1,                !- Zone or ZoneList or Space or SpaceList Name
    LIGHTS-1,                !- Schedule Name
    LightingLevel,           !- Design Level Calculation Method
    1000.0,                  !- Lighting Level {W}
    ,                        !- Watts per Floor Area {W/m2}
    ,                        !- Watts per Person {W/person}
    0.0,                     !- Return Air Fraction
    0.32,                    !- Fraction Radiant
    0.25,                    !- Fraction Visible
    1.0;                     !- Fraction Replaceable
    """

    expected_idf_text = """
  Lights,
    SPACE1-1 Lights 1,       !- Name
    SPACE1-1 Lights 1 Definition,  !- Lights Definition Name
    SPACE1-1,                !- Zone or ZoneList or Space or SpaceList Name
    LIGHTS-1,                !- Schedule Name
    1.0;                     !- Fraction Replaceable

  Lights:Definition,
    SPACE1-1 Lights 1 Definition,  !- Name
    LightingLevel,           !- Design Level Calculation Method
    1000.0,                  !- Lighting Level {W}
    ,                        !- Watts per Floor Area {W/m2}
    ,                        !- Watts per Person {W/person}
    0.0,                     !- Return Air Fraction
    0.32,                    !- Fraction Radiant
    0.25;                    !- Fraction Visible
"""

    run_transition_test(idf_text, expected_idf_text)


def test_ite_aircooled_watts_per_unit(run_transition_test):
    """ElectricEquipment:ITE:AirCooled (Watts/Unit) → split into instance + definition."""
    idf_text = """
  ElectricEquipment:ITE:AirCooled,
    Data Center Servers,     !- Name
    Main Zone,               !- Zone or Space Name
    FlowFromSystem,          !- Air Flow Calculation Method
    Watts/Unit,              !- Design Power Input Calculation Method
    500,                     !- Watts per Unit {W}
    100,                     !- Number of Units
    ,                        !- Watts per Floor Area {W/m2}
    OperatingSchedule,       !- Design Power Input Schedule Name
    CPULoadSchedule,         !- CPU Loading Schedule Name
    CPU_Power_fLoadTemp,     !- CPU Power Input Function of Loading and Air Temperature Curve Name
    0.4,                     !- Design Fan Power Input Fraction
    0.0001,                  !- Design Fan Air Flow Rate per Power Input {m3/s-W}
    AirFlow_fLoadTemp,       !- Air Flow Function of Loading and Air Temperature Curve Name
    FanPower_fFlow,          !- Fan Power Input Function of Flow Curve Name
    15,                      !- Design Entering Air Temperature {C}
    A3,                      !- Environmental Class
    AdjustedSupply,          !- Air Inlet Connection Type
    ,                        !- Air Inlet Room Air Model Node Name
    ,                        !- Air Outlet Room Air Model Node Name
    Main Zone Inlet Node,    !- Supply Air Node Name
    0.1,                     !- Design Recirculation Fraction
    ,                        !- Recirculation Function of Loading and Supply Temperature Curve Name
    0.9,                     !- Design Electric Power Supply Efficiency
    ,                        !- Electric Power Supply Efficiency Function of Part Load Ratio Curve Name
    1.0,                     !- Fraction of Electric Power Supply Losses to Zone
    ITE-CPU,                 !- CPU End-Use Subcategory
    ITE-Fans,                !- Fan End-Use Subcategory
    ITE-UPS;                 !- Electric Power Supply End-Use Subcategory
    """

    expected_idf_text = """
  ElectricEquipment:ITE:AirCooled,
    Data Center Servers,     !- Name
    Data Center Servers Definition,  !- ElectricEquipment ITE AirCooled Definition Name
    Main Zone,               !- Zone or Space Name
    100,                     !- Number of Units
    OperatingSchedule,       !- Design Power Input Schedule Name
    CPULoadSchedule,         !- CPU Loading Schedule Name
    ,                        !- Air Inlet Room Air Model Node Name
    ,                        !- Air Outlet Room Air Model Node Name
    Main Zone Inlet Node,    !- Supply Air Node Name
    ITE-CPU,                 !- CPU End-Use Subcategory
    ITE-Fans,                !- Fan End-Use Subcategory
    ITE-UPS;                 !- Electric Power Supply End-Use Subcategory

  ElectricEquipment:ITE:AirCooled:Definition,
    Data Center Servers Definition,  !- Name
    FlowFromSystem,          !- Air Flow Calculation Method
    Watts/Unit,              !- Design Power Input Calculation Method
    500,                     !- Watts per Unit {W}
    ,                        !- Watts per Floor Area {W/m2}
    CPU_Power_fLoadTemp,     !- CPU Power Input Function of Loading and Air Temperature Curve Name
    0.4,                     !- Design Fan Power Input Fraction
    0.0001,                  !- Design Fan Air Flow Rate per Power Input {m3/s-W}
    AirFlow_fLoadTemp,       !- Air Flow Function of Loading and Air Temperature Curve Name
    FanPower_fFlow,          !- Fan Power Input Function of Flow Curve Name
    15,                      !- Design Entering Air Temperature {C}
    A3,                      !- Environmental Class
    AdjustedSupply,          !- Air Inlet Connection Type
    0.1,                     !- Design Recirculation Fraction
    ,                        !- Recirculation Function of Loading and Supply Temperature Curve Name
    0.9,                     !- Design Electric Power Supply Efficiency
    ,                        !- Electric Power Supply Efficiency Function of Part Load Ratio Curve Name
    1.0;                     !- Fraction of Electric Power Supply Losses to Zone
"""

    run_transition_test(idf_text, expected_idf_text)


def test_ite_aircooled_with_approach_temps(run_transition_test):
    """ITE:AirCooled with FlowControlWithApproachTemperatures and approach temp fields → split."""
    idf_text = """
  ElectricEquipment:ITE:AirCooled,
    DC Servers,              !- Name
    DC Zone,                 !- Zone or Space Name
    FlowControlWithApproachTemperatures, !- Air Flow Calculation Method
    Watts/Unit,              !- Design Power Input Calculation Method
    1000,                    !- Watts per Unit {W}
    50,                      !- Number of Units
    ,                        !- Watts per Floor Area {W/m2}
    ,                        !- Design Power Input Schedule Name
    ,                        !- CPU Loading Schedule Name
    CPU_Power_fLoadTemp,     !- CPU Power Input Function of Loading and Air Temperature Curve Name
    0.3,                     !- Design Fan Power Input Fraction
    0.00008,                 !- Design Fan Air Flow Rate per Power Input {m3/s-W}
    AirFlow_fLoadTemp,       !- Air Flow Function of Loading and Air Temperature Curve Name
    FanPower_fFlow,          !- Fan Power Input Function of Flow Curve Name
    20,                      !- Design Entering Air Temperature {C}
    None,                    !- Environmental Class
    AdjustedSupply,          !- Air Inlet Connection Type
    ,                        !- Air Inlet Room Air Model Node Name
    ,                        !- Air Outlet Room Air Model Node Name
    DC Zone Supply Node,     !- Supply Air Node Name
    0.0,                     !- Design Recirculation Fraction
    ,                        !- Recirculation Function of Loading and Supply Temperature Curve Name
    1.0,                     !- Design Electric Power Supply Efficiency
    ,                        !- Electric Power Supply Efficiency Function of Part Load Ratio Curve Name
    1.0,                     !- Fraction of Electric Power Supply Losses to Zone
    ITE-CPU,                 !- CPU End-Use Subcategory
    ITE-Fans,                !- Fan End-Use Subcategory
    ITE-UPS,                 !- Electric Power Supply End-Use Subcategory
    5.0,                     !- Supply Temperature Difference {deltaC}
    ,                        !- Supply Temperature Difference Schedule
    3.0,                     !- Return Temperature Difference {deltaC}
    ;                        !- Return Temperature Difference Schedule
    """

    expected_idf_text = """
  ElectricEquipment:ITE:AirCooled,
    DC Servers,              !- Name
    DC Servers Definition,   !- ElectricEquipment ITE AirCooled Definition Name
    DC Zone,                 !- Zone or Space Name
    50,                      !- Number of Units
    ,                        !- Design Power Input Schedule Name
    ,                        !- CPU Loading Schedule Name
    ,                        !- Air Inlet Room Air Model Node Name
    ,                        !- Air Outlet Room Air Model Node Name
    DC Zone Supply Node,     !- Supply Air Node Name
    ITE-CPU,                 !- CPU End-Use Subcategory
    ITE-Fans,                !- Fan End-Use Subcategory
    ITE-UPS;                 !- Electric Power Supply End-Use Subcategory

  ElectricEquipment:ITE:AirCooled:Definition,
    DC Servers Definition,   !- Name
    FlowControlWithApproachTemperatures,  !- Air Flow Calculation Method
    Watts/Unit,              !- Design Power Input Calculation Method
    1000,                    !- Watts per Unit {W}
    ,                        !- Watts per Floor Area {W/m2}
    CPU_Power_fLoadTemp,     !- CPU Power Input Function of Loading and Air Temperature Curve Name
    0.3,                     !- Design Fan Power Input Fraction
    0.00008,                 !- Design Fan Air Flow Rate per Power Input {m3/s-W}
    AirFlow_fLoadTemp,       !- Air Flow Function of Loading and Air Temperature Curve Name
    FanPower_fFlow,          !- Fan Power Input Function of Flow Curve Name
    20,                      !- Design Entering Air Temperature {C}
    None,                    !- Environmental Class
    AdjustedSupply,          !- Air Inlet Connection Type
    0.0,                     !- Design Recirculation Fraction
    ,                        !- Recirculation Function of Loading and Supply Temperature Curve Name
    1.0,                     !- Design Electric Power Supply Efficiency
    ,                        !- Electric Power Supply Efficiency Function of Part Load Ratio Curve Name
    1.0,                     !- Fraction of Electric Power Supply Losses to Zone
    5.0,                     !- Supply Temperature Difference {deltaC}
    ,                        !- Supply Temperature Difference Schedule
    3.0,                     !- Return Temperature Difference {deltaC}
    ;                        !- Return Temperature Difference Schedule
"""

    run_transition_test(idf_text, expected_idf_text)
