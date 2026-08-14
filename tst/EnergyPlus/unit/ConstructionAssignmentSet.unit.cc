// EnergyPlus, Copyright (c) 1996-present, The Board of Trustees of the University of Illinois,
// The Regents of the University of California, through Lawrence Berkeley National Laboratory
// (subject to receipt of any required approvals from the U.S. Dept. of Energy), Oak Ridge
// National Laboratory, managed by UT-Battelle, Alliance for Energy Innovation, LLC, and other
// contributors. All rights reserved.
//
// NOTICE: This Software was developed under funding from the U.S. Department of Energy and the
// U.S. Government consequently retains certain rights. As such, the U.S. Government has been
// granted for itself and others acting on its behalf a paid-up, nonexclusive, irrevocable,
// worldwide license in the Software to reproduce, distribute copies to the public, prepare
// derivative works, and perform publicly and display publicly, and to permit others to do so.
//
// Redistribution and use in source and binary forms, with or without modification, are permitted
// provided that the following conditions are met:
//
// (1) Redistributions of source code must retain the above copyright notice, this list of
//     conditions and the following disclaimer.
//
// (2) Redistributions in binary form must reproduce the above copyright notice, this list of
//     conditions and the following disclaimer in the documentation and/or other materials
//     provided with the distribution.
//
// (3) Neither the name of the University of California, Lawrence Berkeley National Laboratory,
//     the University of Illinois, U.S. Dept. of Energy nor the names of its contributors may be
//     used to endorse or promote products derived from this software without specific prior
//     written permission.
//
// (4) Use of EnergyPlus(TM) Name. If Licensee (i) distributes the software in stand-alone form
//     without changes from the version obtained under this License, or (ii) Licensee makes a
//     reference solely to the software portion of its product, Licensee must refer to the
//     software as "EnergyPlus version X" software, where "X" is the version number Licensee
//     obtained under this License and may not use a different name for the software. Except as
//     specifically required in this Section (4), Licensee shall not use in a company name, a
//     product name, in advertising, publicity, or other promotional activities any name, trade
//     name, trademark, logo, or other designation of "EnergyPlus", "E+", "e+" or confusingly
//     similar designation, without the U.S. Department of Energy's prior written consent.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR
// IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY
// AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
// CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
// SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
// OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.

// Google Test Headers
#include <gtest/gtest.h>

// C++ Headers
#include <sstream>

// EnergyPlus Headers
#include <EnergyPlus/Construction.hh>
#include <EnergyPlus/ConstructionAssignmentSet.hh>
#include <EnergyPlus/Data/EnergyPlusData.hh>
#include <EnergyPlus/DataHeatBalance.hh>
#include <EnergyPlus/DataSurfaces.hh>
#include <EnergyPlus/HeatBalanceManager.hh>
#include <EnergyPlus/Material.hh>
#include <EnergyPlus/SurfaceGeometry.hh>
#include <EnergyPlus/UtilityRoutines.hh>

#include "Fixtures/EnergyPlusFixture.hh"

using namespace EnergyPlus;

namespace {

// Minimal IDF preamble: a few NoMass constructions covering each surface/subsurface type.
// Using NoMass to keep it short; the parser only needs valid construction names.
constexpr std::string_view idf_constructions = R"(
  Material:NoMass,
    Mat Opaque,              !- Name
    Rough,                   !- Roughness
    0.5,                     !- Thermal Resistance {m2-K/W}
    0.9, 0.9, 0.9;           !- Thermal/Solar/Visible Absorptance

  WindowMaterial:SimpleGlazingSystem,
    Mat Glazing,             !- Name
    2.0,                     !- U-Factor {W/m2-K}
    0.4;                     !- Solar Heat Gain Coefficient

  Construction,
    Constr Wall,             !- Name
    Mat Opaque;              !- Layer 1

  Construction,
    Constr Floor,            !- Name
    Mat Opaque;              !- Layer 1

  Construction,
    Constr Roof,             !- Name
    Mat Opaque;              !- Layer 1

  Construction,
    Constr Door,             !- Name
    Mat Opaque;              !- Layer 1

  Construction,
    Constr Window,           !- Name
    Mat Glazing;             !- Layer 1

  Construction,
    Constr Adiabatic,        !- Name
    Mat Opaque;              !- Layer 1

  Construction,
    Constr IntPartition,     !- Name
    Mat Opaque;              !- Layer 1
)";

constexpr std::string_view idf_dcs_all_types = R"(
ConstructionAssignmentSet,
  Default Construction Set,               !- Name
  Exterior Surface Constructions,         !- Exterior Surface Construction Assignments Name
  Interior Surface Constructions,         !- Interior Surface Construction Assignments Name
  Ground Surface Constructions,           !- Ground Contact Surface Construction Assignments Name
  Exterior SubSurface Constructions,      !- Exterior SubSurface Construction Assignments Name
  Interior SubSurface Constructions,      !- Interior SubSurface Construction Assignments Name
  Interior Partition Construction,        !- Interior Partition Construction Name
  Adiabatic Surface Construction;         !- Adiabatic Surface Construction Name

SurfaceConstructionAssignments,
  Exterior Surface Constructions,         !- Name
  Exterior Floor Construction,            !- Floor Construction Name
  Exterior Wall Construction,             !- Wall Construction Name
  Exterior Roof Construction;             !- Roof Ceiling Construction Name

SurfaceConstructionAssignments,
  Interior Surface Constructions,         !- Name
  Interior Floor Construction,            !- Floor Construction Name
  Interior Wall Construction,             !- Wall Construction Name
  Interior Roof Construction;             !- Roof Ceiling Construction Name

SurfaceConstructionAssignments,
  Ground Surface Constructions,           !- Name
  Ground Floor Construction,              !- Floor Construction Name
  Ground Wall Construction,               !- Wall Construction Name
  Ground Roof Construction;               !- Roof Ceiling Construction Name

SubSurfaceConstructionAssignments,
  Exterior SubSurface Constructions,      !- Name
  Exterior FixedWindow Construction,      !- Fixed Window Construction Name
  Exterior OperableWindow Construction,   !- Operable Window Construction Name
  Exterior Door Construction,             !- Door Construction Name
  Exterior GlassDoor Construction,        !- Glass Door Construction Name
  Exterior OverheadDoor Construction,     !- Overhead Door Construction Name
  Exterior Skylight Construction,         !- Skylight Construction Name
  Exterior TubularDaylightDome Construction, !- Tubular Daylight Dome Construction Name
  Exterior TubularDaylightDiffuser Construction; !- Tubular Daylight Diffuser Construction Name

SubSurfaceConstructionAssignments,
  Interior SubSurface Constructions,      !- Name
  Interior FixedWindow Construction,      !- Fixed Window Construction Name
  Interior OperableWindow Construction,   !- Operable Window Construction Name
  Interior Door Construction,             !- Door Construction Name
  Interior GlassDoor Construction,        !- Glass Door Construction Name
  Interior OverheadDoor Construction,     !- Overhead Door Construction Name
  Interior Skylight Construction,         !- Skylight Construction Name
  Interior TubularDaylightDome Construction, !- Tubular Daylight Dome Construction Name
  Interior TubularDaylightDiffuser Construction; !- Tubular Daylight Diffuser Construction Name

Construction,
  Exterior Floor Construction,            !- Name
  C5 - 4 IN HW CONCRETE,                  !- Outside Layer
  R13LAYER;                               !- Layer 2

Construction,
  Exterior Wall Construction,             !- Name
  C5 - 4 IN HW CONCRETE,                  !- Outside Layer
  R13LAYER;                               !- Layer 2

Construction,
  Exterior Roof Construction,             !- Name
  C5 - 4 IN HW CONCRETE,                  !- Outside Layer
  R13LAYER;                               !- Layer 2

Construction,
  Interior Floor Construction,            !- Name
  R13LAYER,                               !- Outside Layer
  C5 - 4 IN HW CONCRETE;                  !- Layer 2

Construction,
  Interior Wall Construction,             !- Name
  G01a 19mm gypsum board,                 !- Outside Layer
  C5 - 4 IN HW CONCRETE,                  !- Layer 2
  G01a 19mm gypsum board;                 !- Layer 3

Construction,
  Interior Roof Construction,             !- Name
  C5 - 4 IN HW CONCRETE,                  !- Outside Layer
  R13LAYER;                               !- Layer 2

Construction,
  Ground Floor Construction,              !- Name
  C5 - 4 IN HW CONCRETE,                  !- Outside Layer
  R13LAYER;                               !- Layer 2

Construction,
  Ground Wall Construction,               !- Name
  C5 - 4 IN HW CONCRETE,                  !- Outside Layer
  R13LAYER;                               !- Layer 2

Construction,
  Ground Roof Construction,               !- Name
  C5 - 4 IN HW CONCRETE,                  !- Outside Layer
  R13LAYER;                               !- Layer 2

Construction,
  Exterior FixedWindow Construction,      !- Name
  Mat Glazing;                            !- Outside Layer

Construction,
  Exterior OperableWindow Construction,   !- Name
  Mat Glazing;                            !- Outside Layer

Construction,
  Exterior Door Construction,             !- Name
  C5 - 4 IN HW CONCRETE,                  !- Outside Layer
  R13LAYER;                               !- Layer 2

Construction,
  Exterior GlassDoor Construction,        !- Name
  Mat Glazing;                            !- Outside Layer

Construction,
  Exterior OverheadDoor Construction,     !- Name
  C5 - 4 IN HW CONCRETE,                  !- Outside Layer
  R13LAYER;                               !- Layer 2

Construction,
  Exterior Skylight Construction,         !- Name
  Mat Glazing;                            !- Outside Layer

Construction,
  Exterior TubularDaylightDome Construction, !- Name
  Mat Glazing;                            !- Outside Layer

Construction,
  Exterior TubularDaylightDiffuser Construction, !- Name
  Mat Glazing;                            !- Outside Layer

Construction,
  Interior FixedWindow Construction,      !- Name
  Mat Interior Glazing;                   !- Outside Layer

Construction,
  Interior OperableWindow Construction,   !- Name
  Mat Interior Glazing;                   !- Outside Layer

Construction,
  Interior Door Construction,             !- Name
  G05 25mm wood;                          !- Outside Layer

Construction,
  Interior GlassDoor Construction,        !- Name
  Mat Interior Glazing;                   !- Outside Layer

Construction,
  Interior OverheadDoor Construction,     !- Name
  C5 - 4 IN HW CONCRETE,                  !- Outside Layer
  R13LAYER;                               !- Layer 2

Construction,
  Interior Skylight Construction,         !- Name
  Mat Interior Glazing;                   !- Outside Layer

Construction,
  Interior TubularDaylightDome Construction, !- Name
  Mat Interior Glazing;                   !- Outside Layer

Construction,
  Interior TubularDaylightDiffuser Construction, !- Name
  Mat Interior Glazing;                   !- Outside Layer

Construction,
  Interior Partition Construction,        !- Name
  C5 - 4 IN HW CONCRETE,                  !- Outside Layer
  R13LAYER;                               !- Layer 2

Construction,
  Adiabatic Surface Construction,         !- Name
  C5 - 4 IN HW CONCRETE,                  !- Outside Layer
  R13LAYER;                               !- Layer 2

Construction,
  Exterior Wall Construction - Space3,    !- Name
  C5 - 4 IN HW CONCRETE,                  !- Outside Layer
  R13LAYER;                               !- Layer 2

Material,
  C5 - 4 IN HW CONCRETE,                  !- Name
  MediumRough,                            !- Roughness
  0.1014984,                              !- Thickness {m}
  1.729577,                               !- Conductivity {W/m-K}
  2242.585,                               !- Density {kg/m3}
  836.8,                                  !- Specific Heat {J/kg-K}
  0.9,                                    !- Thermal Absorptance
  0.65,                                   !- Solar Absorptance
  0.65;                                   !- Visible Absorptance

Material:NoMass,
  R13LAYER,                               !- Name
  Rough,                                  !- Roughness
  2.290965,                               !- Thermal Resistance {m2-K/W}
  0.9,                                    !- Thermal Absorptance
  0.75,                                   !- Solar Absorptance
  0.75;                                   !- Visible Absorptance

Material,
  G01a 19mm gypsum board,                 !- Name
  MediumSmooth,                           !- Roughness
  0.019,                                  !- Thickness {m}
  0.16,                                   !- Conductivity {W/m-K}
  800,                                    !- Density {kg/m3}
  1090,                                   !- Specific Heat {J/kg-K}
  0.9,                                    !- Thermal Absorptance
  0.7,                                    !- Solar Absorptance
  0.7;                                    !- Visible Absorptance

WindowMaterial:SimpleGlazingSystem,
  Mat Glazing,                            !- Name
  0.1,                                    !- U-Factor {W/m2-K}
  0.65;                                   !- Solar Heat Gain Coefficient

WindowMaterial:SimpleGlazingSystem,
  Mat Interior Glazing,                   !- Name
  0.9,                                    !- U-Factor {W/m2-K}
  0.8;                                    !- Solar Heat Gain Coefficient

Material,
  G05 25mm wood,                          !- Name
  MediumSmooth,                           !- Roughness
  0.0254,                                 !- Thickness {m}
  0.15,                                   !- Conductivity {W/m-K}
  608,                                    !- Density {kg/m3}
  1630,                                   !- Specific Heat {J/kg-K}
  0.9,                                    !- Thermal Absorptance
  0.7,                                    !- Solar Absorptance
  0.7;                                    !- Visible Absorptance
)";

void loadConstructions(EnergyPlusData &state)
{
    bool ErrorsFound = false;
    HeatBalanceManager::GetFrameAndDividerData(state);
    HeatBalanceManager::SetPreConstructionInputParameters(state);
    Material::GetMaterialData(state, ErrorsFound);
    ASSERT_FALSE(ErrorsFound);
    HeatBalanceManager::GetConstructData(state, ErrorsFound);
    ASSERT_FALSE(ErrorsFound);
}

} // namespace

TEST_F(EnergyPlusFixture, ConstructionAssignmentSet_ParseAll)
{
    // Happy path: all three objects parse and ConstructionAssignmentSet links correctly.
    std::string const idf_objects = std::string(idf_constructions) + R"(
  SubSurfaceConstructionAssignments,
    Ext SubSurf Constrs,     !- Name
    Constr Window,           !- Fixed Window Construction Name
    Constr Window,           !- Operable Window Construction Name
    Constr Door,             !- Door Construction Name
    Constr Window,           !- Glass Door Construction Name
    Constr Door,             !- Overhead Door Construction Name
    Constr Window;           !- Skylight Construction Name

  SurfaceConstructionAssignments,
    Ext Surf Constrs,        !- Name
    Constr Floor,            !- Floor Construction Name
    Constr Wall,             !- Wall Construction Name
    Constr Roof;             !- Roof Ceiling Construction Name

  SurfaceConstructionAssignments,
    Int Surf Constrs,        !- Name
    Constr Floor,            !- Floor Construction Name
    Constr Wall,             !- Wall Construction Name
    Constr Roof;             !- Roof Ceiling Construction Name

  ConstructionAssignmentSet,
    My DCS,                  !- Name
    Ext Surf Constrs,        !- Exterior Surface Construction Assignments Name
    Int Surf Constrs,        !- Interior Surface Construction Assignments Name
    ,                        !- Ground Contact Surface Construction Assignments Name
    Ext SubSurf Constrs,     !- Exterior SubSurface Construction Assignments Name
    ,                        !- Interior SubSurface Construction Assignments Name
    Constr IntPartition,     !- Interior Partition Construction Name
    Constr Adiabatic;        !- Adiabatic Surface Construction Name
)";

    ASSERT_TRUE(process_idf(idf_objects));
    state->init_state(*state);
    loadConstructions(*state);

    bool ErrorsFound = false;
    ConstructionAssignments::GetConstructionAssignmentSetData(*state, ErrorsFound);
    ASSERT_FALSE(ErrorsFound);

    auto &s_dc = state->dataConstructionAssignments;

    // Sub-surface set
    ASSERT_EQ(1u, s_dc->subSurfaceConstructionAssignments.size());
    auto const &ssc = s_dc->subSurfaceConstructionAssignments[0];
    EXPECT_EQ("EXT SUBSURF CONSTRS", ssc->Name);
    EXPECT_GT(ssc->fixedWindowConstrNum, 0);
    EXPECT_GT(ssc->operableWindowConstrNum, 0);
    EXPECT_GT(ssc->doorConstrNum, 0);
    EXPECT_GT(ssc->glassDoorConstrNum, 0);
    EXPECT_GT(ssc->overheadDoorConstrNum, 0);
    EXPECT_GT(ssc->skylightConstrNum, 0);
    EXPECT_EQ(0, ssc->tddDomeConstrNum);     // not specified
    EXPECT_EQ(0, ssc->tddDiffuserConstrNum); // not specified

    // Surface sets
    ASSERT_EQ(2u, s_dc->surfaceConstructionAssignments.size());
    EXPECT_EQ("EXT SURF CONSTRS", s_dc->surfaceConstructionAssignments[0]->Name);
    EXPECT_EQ("INT SURF CONSTRS", s_dc->surfaceConstructionAssignments[1]->Name);

    // Construction set
    ASSERT_EQ(1u, s_dc->constructionAssignmentSets.size());
    auto const &dcs = s_dc->constructionAssignmentSets[0];
    EXPECT_EQ("MY DCS", dcs.Name);

    ASSERT_NE(nullptr, dcs.exteriorSurfConstructions);
    EXPECT_EQ("EXT SURF CONSTRS", dcs.exteriorSurfConstructions->Name);
    EXPECT_GT(dcs.exteriorSurfConstructions->wallConstrNum, 0);
    EXPECT_GT(dcs.exteriorSurfConstructions->floorConstrNum, 0);
    EXPECT_GT(dcs.exteriorSurfConstructions->roofCeilingConstrNum, 0);

    ASSERT_NE(nullptr, dcs.interiorSurfConstructions);
    EXPECT_EQ("INT SURF CONSTRS", dcs.interiorSurfConstructions->Name);

    EXPECT_EQ(nullptr, dcs.groundContactSurfConstructions); // blank -> nullptr

    ASSERT_NE(nullptr, dcs.exteriorSubSurfConstructions);
    EXPECT_EQ("EXT SUBSURF CONSTRS", dcs.exteriorSubSurfConstructions->Name);

    EXPECT_EQ(nullptr, dcs.interiorSubSurfConstructions); // blank -> nullptr

    EXPECT_GT(dcs.interiorPartitionConstrNum, 0);
    EXPECT_GT(dcs.adiabaticSurfConstrNum, 0);
}

TEST_F(EnergyPlusFixture, ConstructionAssignmentSet_BadConstructionName)
{
    // Bad construction name in SubSurfaceConstructionAssignments produces an error.
    std::string const idf_objects = std::string(idf_constructions) + R"(
  SubSurfaceConstructionAssignments,
    Bad SubSurf,             !- Name
    NONEXISTENT CONSTR,      !- Fixed Window Construction Name
    ,                        !- Operable Window Construction Name
    ,                        !- Door Construction Name
    ,                        !- Glass Door Construction Name
    ,                        !- Overhead Door Construction Name
    ;                        !- Skylight Construction Name
)";

    ASSERT_TRUE(process_idf(idf_objects));
    state->init_state(*state);
    loadConstructions(*state);

    bool ErrorsFound = false;
    ConstructionAssignments::GetConstructionAssignmentSetData(*state, ErrorsFound);
    EXPECT_TRUE(ErrorsFound);
    EXPECT_TRUE(
        compare_err_stream_substring(R"(** Severe  ** GetConstructionAssignmentSetData: SubSurfaceConstructionAssignments="BAD SUBSURF")", false));
    EXPECT_TRUE(compare_err_stream_substring(R"(invalid fixed_window_construction_name="NONEXISTENT CONSTR" not found.)", false));
}

TEST_F(EnergyPlusFixture, ConstructionAssignmentSet_BadSurfConstrRef)
{
    // Bad SurfaceConstructionAssignments name referenced from ConstructionAssignmentSet produces an error.
    std::string const idf_objects = std::string(idf_constructions) + R"(
  ConstructionAssignmentSet,
    Bad DCS,                 !- Name
    NONEXISTENT SURF CONSTRS, !- Exterior Surface Construction Assignments Name
    ,                        !- Interior Surface Construction Assignments Name
    ,                        !- Ground Contact Surface Construction Assignments Name
    ,                        !- Exterior SubSurface Construction Assignments Name
    ,                        !- Interior SubSurface Construction Assignments Name
    ,                        !- Interior Partition Construction Name
    ;                        !- Adiabatic Surface Construction Name
)";

    ASSERT_TRUE(process_idf(idf_objects));
    state->init_state(*state);
    loadConstructions(*state);

    bool ErrorsFound = false;
    ConstructionAssignments::GetConstructionAssignmentSetData(*state, ErrorsFound);
    EXPECT_TRUE(ErrorsFound);
    EXPECT_TRUE(compare_err_stream_substring(R"(** Severe  ** GetConstructionAssignmentSetData: ConstructionAssignmentSet="BAD DCS")", false));
    EXPECT_TRUE(
        compare_err_stream_substring(R"(invalid exterior_surface_construction_assignments_name="NONEXISTENT SURF CONSTRS" not found.)", false));
}

TEST_F(EnergyPlusFixture, ConstructionAssignmentSet_WindowConstrInOpaqueSurfaceField)
{
    // Using a window construction for an opaque surface field (wall) should produce an error.
    std::string const idf_objects = std::string(idf_constructions) + R"(
  SurfaceConstructionAssignments,
    Bad Surf Type,            !- Name
    Constr Floor,             !- Floor Construction Name
    Constr Window,            !- Wall Construction Name
    Constr Roof;              !- Roof Ceiling Construction Name
)";

    ASSERT_TRUE(process_idf(idf_objects));
    state->init_state(*state);
    loadConstructions(*state);

    bool ErrorsFound = false;
    ConstructionAssignments::GetConstructionAssignmentSetData(*state, ErrorsFound);
    EXPECT_TRUE(ErrorsFound);
    EXPECT_TRUE(
        compare_err_stream_substring(R"(SurfaceConstructionAssignments="BAD SURF TYPE", invalid wall_construction_name="CONSTR WINDOW")", false));
    EXPECT_TRUE(compare_err_stream_substring(R"(has Window material)", false));
}

TEST_F(EnergyPlusFixture, ConstructionAssignmentSet_WrongConstrTypeInSubSurfaceFields)
{
    // Opaque construction in a glazed field → "not a window or air boundary construction".
    // Window construction in a door (non-glazed) field → "has Window material".
    std::string const idf_objects = std::string(idf_constructions) + R"(
  SubSurfaceConstructionAssignments,
    Bad SubSurf Type,         !- Name
    Constr Door,              !- Fixed Window Construction Name
    ,                         !- Operable Window Construction Name
    Constr Window,            !- Door Construction Name
    ,                         !- Glass Door Construction Name
    ,                         !- Overhead Door Construction Name
    ;                         !- Skylight Construction Name
)";

    ASSERT_TRUE(process_idf(idf_objects));
    state->init_state(*state);
    loadConstructions(*state);

    bool ErrorsFound = false;
    ConstructionAssignments::GetConstructionAssignmentSetData(*state, ErrorsFound);
    EXPECT_TRUE(ErrorsFound);
    EXPECT_TRUE(compare_err_stream_substring(
        R"(SubSurfaceConstructionAssignments="BAD SUBSURF TYPE", invalid fixed_window_construction_name="CONSTR DOOR")", false));
    EXPECT_TRUE(compare_err_stream_substring(R"(has an opaque surface construction; it should have a window construction)", false));
    EXPECT_TRUE(compare_err_stream_substring(
        R"(SubSurfaceConstructionAssignments="BAD SUBSURF TYPE", invalid door_construction_name="CONSTR WINDOW")", false));
    EXPECT_TRUE(compare_err_stream_substring(R"(has Window material)", false));
}

TEST(ConstructionAssignmentSet_GetDefaultConstruction, AllCases)
{
    // Verify getConstructionAssignment returns the correct slot for every surface type and BC.
    // No EnergyPlus state needed — builds the struct directly.
    // Unique sentinel per slot so any wrong return is immediately obvious.
    auto extSurf = std::make_shared<ConstructionAssignments::SurfaceConstructionAssignmentsData>();
    extSurf->wallConstrNum = 1;
    extSurf->floorConstrNum = 2;
    extSurf->roofCeilingConstrNum = 3;

    auto intSurf = std::make_shared<ConstructionAssignments::SurfaceConstructionAssignmentsData>();
    intSurf->wallConstrNum = 4;
    intSurf->floorConstrNum = 5;
    intSurf->roofCeilingConstrNum = 6;

    auto gndSurf = std::make_shared<ConstructionAssignments::SurfaceConstructionAssignmentsData>();
    gndSurf->wallConstrNum = 7;
    gndSurf->floorConstrNum = 8;
    gndSurf->roofCeilingConstrNum = 9;

    auto extSubSurf = std::make_shared<ConstructionAssignments::SubSurfaceConstructionAssignmentsData>();
    extSubSurf->fixedWindowConstrNum = 10;
    extSubSurf->operableWindowConstrNum = 11;
    extSubSurf->doorConstrNum = 12;
    extSubSurf->glassDoorConstrNum = 13;
    extSubSurf->overheadDoorConstrNum = 14;
    extSubSurf->skylightConstrNum = 15;
    extSubSurf->tddDomeConstrNum = 16;
    extSubSurf->tddDiffuserConstrNum = 17;

    auto intSubSurf = std::make_shared<ConstructionAssignments::SubSurfaceConstructionAssignmentsData>();
    intSubSurf->fixedWindowConstrNum = 18;
    intSubSurf->operableWindowConstrNum = 19;

    ConstructionAssignments::ConstructionAssignmentSetData dcs;
    dcs.exteriorSurfConstructions = extSurf;
    dcs.interiorSurfConstructions = intSurf;
    dcs.groundContactSurfConstructions = gndSurf;
    dcs.exteriorSubSurfConstructions = extSubSurf;
    dcs.interiorSubSurfConstructions = intSubSurf;
    dcs.interiorPartitionConstrNum = 20;
    dcs.adiabaticSurfConstrNum = 21;

    auto makeSurf = [](DataSurfaces::SurfaceClass cls, int extBC, const std::string &name = "S", const std::string &bcName = "") {
        DataSurfaces::SurfaceData surf;
        surf.Class = cls;

        surf.OriginalClass = surf.Class;
        if (DataSurfaces::SurfaceClassIsGlazed(surf.Class) || surf.Class == DataSurfaces::SurfaceClass::TDD_Diffuser) {
            surf.Class = DataSurfaces::SurfaceClass::Window;
        }
        surf.ExtBoundCond = extBC;
        surf.Name = name;
        surf.ExtBoundCondName = bcName.empty() ? name : bcName;
        return surf;
    };

    // Base surfaces — exterior
    EXPECT_EQ(1, dcs.getConstructionAssignment(makeSurf(DataSurfaces::SurfaceClass::Wall, DataSurfaces::ExternalEnvironment)));
    EXPECT_EQ(2, dcs.getConstructionAssignment(makeSurf(DataSurfaces::SurfaceClass::Floor, DataSurfaces::ExternalEnvironment)));
    EXPECT_EQ(3, dcs.getConstructionAssignment(makeSurf(DataSurfaces::SurfaceClass::Roof, DataSurfaces::ExternalEnvironment)));
    // OtherSideCoeff -> exterior
    EXPECT_EQ(1, dcs.getConstructionAssignment(makeSurf(DataSurfaces::SurfaceClass::Wall, DataSurfaces::OtherSideCoefNoCalcExt)));
    EXPECT_EQ(1, dcs.getConstructionAssignment(makeSurf(DataSurfaces::SurfaceClass::Wall, DataSurfaces::OtherSideCoefCalcExt)));

    // Ground contact
    EXPECT_EQ(7, dcs.getConstructionAssignment(makeSurf(DataSurfaces::SurfaceClass::Wall, DataSurfaces::Ground)));
    EXPECT_EQ(8, dcs.getConstructionAssignment(makeSurf(DataSurfaces::SurfaceClass::Floor, DataSurfaces::Ground)));
    EXPECT_EQ(8, dcs.getConstructionAssignment(makeSurf(DataSurfaces::SurfaceClass::Floor, DataSurfaces::GroundFCfactorMethod)));
    EXPECT_EQ(8, dcs.getConstructionAssignment(makeSurf(DataSurfaces::SurfaceClass::Floor, DataSurfaces::KivaFoundation)));

    // Interior (pre-reconciliation: ExtBoundCond < -6, ExtBoundCondName != Name)
    EXPECT_EQ(4, dcs.getConstructionAssignment(makeSurf(DataSurfaces::SurfaceClass::Wall, -999, "S", "Other")));
    EXPECT_EQ(5, dcs.getConstructionAssignment(makeSurf(DataSurfaces::SurfaceClass::Floor, -999, "S", "Other")));
    EXPECT_EQ(6, dcs.getConstructionAssignment(makeSurf(DataSurfaces::SurfaceClass::Roof, -999, "S", "Other")));

    // Adiabatic (ExtBoundCondName == Name, ExtBoundCond < -6)
    EXPECT_EQ(21, dcs.getConstructionAssignment(makeSurf(DataSurfaces::SurfaceClass::Wall, -999)));

    // Interior mass
    EXPECT_EQ(20, dcs.getConstructionAssignment(makeSurf(DataSurfaces::SurfaceClass::IntMass, -999)));

    // Exterior subsurfaces
    EXPECT_EQ(10, dcs.getConstructionAssignment(makeSurf(DataSurfaces::SurfaceClass::FixedWindow, DataSurfaces::ExternalEnvironment)));
    EXPECT_EQ(10,
              dcs.getConstructionAssignment(
                  makeSurf(DataSurfaces::SurfaceClass::Window, DataSurfaces::ExternalEnvironment))); // generic -> FixedWindow slot
    EXPECT_EQ(11, dcs.getConstructionAssignment(makeSurf(DataSurfaces::SurfaceClass::OperableWindow, DataSurfaces::ExternalEnvironment)));
    EXPECT_EQ(12, dcs.getConstructionAssignment(makeSurf(DataSurfaces::SurfaceClass::Door, DataSurfaces::ExternalEnvironment)));
    EXPECT_EQ(13, dcs.getConstructionAssignment(makeSurf(DataSurfaces::SurfaceClass::GlassDoor, DataSurfaces::ExternalEnvironment)));
    EXPECT_EQ(14, dcs.getConstructionAssignment(makeSurf(DataSurfaces::SurfaceClass::OverheadDoor, DataSurfaces::ExternalEnvironment)));
    EXPECT_EQ(15, dcs.getConstructionAssignment(makeSurf(DataSurfaces::SurfaceClass::Skylight, DataSurfaces::ExternalEnvironment)));
    EXPECT_EQ(16, dcs.getConstructionAssignment(makeSurf(DataSurfaces::SurfaceClass::TDD_Dome, DataSurfaces::ExternalEnvironment)));
    EXPECT_EQ(17, dcs.getConstructionAssignment(makeSurf(DataSurfaces::SurfaceClass::TDD_Diffuser, DataSurfaces::ExternalEnvironment)));

    // Interior subsurfaces
    EXPECT_EQ(18, dcs.getConstructionAssignment(makeSurf(DataSurfaces::SurfaceClass::FixedWindow, -999, "S", "Other")));
    EXPECT_EQ(19, dcs.getConstructionAssignment(makeSurf(DataSurfaces::SurfaceClass::OperableWindow, -999, "S", "Other")));

    // nullptr sub-objects -> 0
    ConstructionAssignments::ConstructionAssignmentSetData dcsEmpty;
    EXPECT_EQ(0, dcsEmpty.getConstructionAssignment(makeSurf(DataSurfaces::SurfaceClass::Wall, DataSurfaces::ExternalEnvironment)));
    EXPECT_EQ(0, dcsEmpty.getConstructionAssignment(makeSurf(DataSurfaces::SurfaceClass::FixedWindow, DataSurfaces::ExternalEnvironment)));
    EXPECT_EQ(0, dcsEmpty.getConstructionAssignment(makeSurf(DataSurfaces::SurfaceClass::Floor, DataSurfaces::Ground)));
}

TEST_F(EnergyPlusFixture, ConstructionAssignmentSet_GetDefaultConstruction)
{
    // Tests getConstructionAssignment() against real E+ parsed surfaces, verifying that
    // the BC values produced by GetHTSurfaceData/GetHTSubSurfaceData map to the correct
    // DCS slot both before and after BC reconciliation.
    //
    // The geometry covers every BC category (exterior, ground, adiabatic, interior) and
    // every fenestration type. Explicit constructions are provided for all surfaces so the
    // model loads; the DCS uses distinct named constructions per slot for unambiguous assertions.

    std::string const geometry = delimited_string({
        "Zone,",
        "  Zone1,                                  !- Name",
        "  0,                                      !- Direction of Relative North {deg}",
        "  0,                                      !- X Origin {m}",
        "  0,                                      !- Y Origin {m}",
        "  0,                                      !- Z Origin {m}",
        "  ,                                       !- Type",
        "  1,                                      !- Multiplier",
        "  ,                                       !- Ceiling Height {m}",
        "  ,                                       !- Volume {m3}",
        "  ,                                       !- Floor Area {m2}",
        "  ,                                       !- Zone Inside Convection Algorithm",
        "  ,                                       !- Zone Outside Convection Algorithm",
        "  Yes;                                    !- Part of Total Floor Area",

        "BuildingSurface:Detailed,",
        "  Space1 Floor,                           !- Name",
        "  Floor,                                  !- Surface Type",
        "  Exterior Floor Construction,            !- Construction Name",
        "  Zone1,                                  !- Zone Name",
        "  ,                                       !- Space Name",
        "  Outdoors,                               !- Outside Boundary Condition",
        "  ,                                       !- Outside Boundary Condition Object",
        "  SunExposed,                             !- Sun Exposure",
        "  WindExposed,                            !- Wind Exposure",
        "  ,                                       !- View Factor to Ground",
        "  ,                                       !- Number of Vertices",
        "  10, 10, 0,                              !- X,Y,Z Vertex 1 {m}",
        "  10, 0, 0,                               !- X,Y,Z Vertex 2 {m}",
        "  0, 0, 0,                                !- X,Y,Z Vertex 3 {m}",
        "  0, 10, 0;                               !- X,Y,Z Vertex 4 {m}",

        "BuildingSurface:Detailed,",
        "  Space1 RoofCeiling,                     !- Name",
        "  Ceiling,                                !- Surface Type",
        "  Interior Roof Construction,             !- Construction Name",
        "  Zone1,                                  !- Zone Name",
        "  ,                                       !- Space Name",
        "  Surface,                                !- Outside Boundary Condition",
        "  Space3 Floor,                           !- Outside Boundary Condition Object",
        "  NoSun,                                  !- Sun Exposure",
        "  NoWind,                                 !- Wind Exposure",
        "  ,                                       !- View Factor to Ground",
        "  ,                                       !- Number of Vertices",
        "  0, 10, 3,                               !- X,Y,Z Vertex 1 {m}",
        "  0, 0, 3,                                !- X,Y,Z Vertex 2 {m}",
        "  10, 0, 3,                               !- X,Y,Z Vertex 3 {m}",
        "  10, 10, 3;                              !- X,Y,Z Vertex 4 {m}",

        "BuildingSurface:Detailed,",
        "  Space1 Wall 1,                          !- Name",
        "  Wall,                                   !- Surface Type",
        "  Interior Wall Construction,             !- Construction Name",
        "  Zone1,                                  !- Zone Name",
        "  ,                                       !- Space Name",
        "  Surface,                                !- Outside Boundary Condition",
        "  Space2 Wall 3,                          !- Outside Boundary Condition Object",
        "  NoSun,                                  !- Sun Exposure",
        "  NoWind,                                 !- Wind Exposure",
        "  ,                                       !- View Factor to Ground",
        "  ,                                       !- Number of Vertices",
        "  10, 10, 3,                              !- X,Y,Z Vertex 1 {m}",
        "  10, 0, 3,                               !- X,Y,Z Vertex 2 {m}",
        "  10, 0, 0,                               !- X,Y,Z Vertex 3 {m}",
        "  10, 10, 0;                              !- X,Y,Z Vertex 4 {m}",

        "FenestrationSurface:Detailed,",
        "  InteriorDoor - Door,                    !- Name",
        "  Door,                                   !- Surface Type",
        "  Interior Door Construction,             !- Construction Name",
        "  Space1 Wall 1,                          !- Building Surface Name",
        "  InteriorDoor - Door - Reversed,         !- Outside Boundary Condition Object",
        "  ,                                       !- View Factor to Ground",
        "  ,                                       !- Frame and Divider Name",
        "  ,                                       !- Multiplier",
        "  ,                                       !- Number of Vertices",
        "  10,                                     !- Vertex 1 X-coordinate {m}",
        "  4.6,                                    !- Vertex 1 Y-coordinate {m}",
        "  2,                                      !- Vertex 1 Z-coordinate {m}",
        "  10,                                     !- Vertex 2 X-coordinate {m}",
        "  4.6,                                    !- Vertex 2 Y-coordinate {m}",
        "  0,                                      !- Vertex 2 Z-coordinate {m}",
        "  10,                                     !- Vertex 3 X-coordinate {m}",
        "  5.4,                                    !- Vertex 3 Y-coordinate {m}",
        "  0,                                      !- Vertex 3 Z-coordinate {m}",
        "  10,                                     !- Vertex 4 X-coordinate {m}",
        "  5.4,                                    !- Vertex 4 Y-coordinate {m}",
        "  2;                                      !- Vertex 4 Z-coordinate {m}",

        "BuildingSurface:Detailed,",
        "  Space1 Wall 2,                          !- Name",
        "  Wall,                                   !- Surface Type",
        "  Exterior Wall Construction,             !- Construction Name",
        "  Zone1,                                  !- Zone Name",
        "  ,                                       !- Space Name",
        "  Outdoors,                               !- Outside Boundary Condition",
        "  ,                                       !- Outside Boundary Condition Object",
        "  SunExposed,                             !- Sun Exposure",
        "  WindExposed,                            !- Wind Exposure",
        "  ,                                       !- View Factor to Ground",
        "  ,                                       !- Number of Vertices",
        "  10, 0, 3,                               !- X,Y,Z Vertex 1 {m}",
        "  0, 0, 3,                                !- X,Y,Z Vertex 2 {m}",
        "  0, 0, 0,                                !- X,Y,Z Vertex 3 {m}",
        "  10, 0, 0;                               !- X,Y,Z Vertex 4 {m}",

        "FenestrationSurface:Detailed,",
        "  ExteriorDoor - Door,                    !- Name",
        "  Door,                                   !- Surface Type",
        "  Exterior Door Construction,             !- Construction Name",
        "  Space1 Wall 2,                          !- Building Surface Name",
        "  ,                                       !- Outside Boundary Condition Object",
        "  ,                                       !- View Factor to Ground",
        "  ,                                       !- Frame and Divider Name",
        "  ,                                       !- Multiplier",
        "  ,                                       !- Number of Vertices",
        "  4.6,                                    !- Vertex 1 X-coordinate {m}",
        "  0,                                      !- Vertex 1 Y-coordinate {m}",
        "  2,                                      !- Vertex 1 Z-coordinate {m}",
        "  4.6,                                    !- Vertex 2 X-coordinate {m}",
        "  0,                                      !- Vertex 2 Y-coordinate {m}",
        "  0,                                      !- Vertex 2 Z-coordinate {m}",
        "  5.4,                                    !- Vertex 3 X-coordinate {m}",
        "  0,                                      !- Vertex 3 Y-coordinate {m}",
        "  0,                                      !- Vertex 3 Z-coordinate {m}",
        "  5.4,                                    !- Vertex 4 X-coordinate {m}",
        "  0,                                      !- Vertex 4 Y-coordinate {m}",
        "  2;                                      !- Vertex 4 Z-coordinate {m}",

        "BuildingSurface:Detailed,",
        "  Space1 Wall 3,                          !- Name",
        "  Wall,                                   !- Surface Type",
        "  Exterior Wall Construction,             !- Construction Name",
        "  Zone1,                                  !- Zone Name",
        "  ,                                       !- Space Name",
        "  Outdoors,                               !- Outside Boundary Condition",
        "  ,                                       !- Outside Boundary Condition Object",
        "  SunExposed,                             !- Sun Exposure",
        "  WindExposed,                            !- Wind Exposure",
        "  ,                                       !- View Factor to Ground",
        "  ,                                       !- Number of Vertices",
        "  0, 0, 3,                                !- X,Y,Z Vertex 1 {m}",
        "  0, 10, 3,                               !- X,Y,Z Vertex 2 {m}",
        "  0, 10, 0,                               !- X,Y,Z Vertex 3 {m}",
        "  0, 0, 0;                                !- X,Y,Z Vertex 4 {m}",

        "FenestrationSurface:Detailed,",
        "  ExteriorDoor - OverheadDoor,            !- Name",
        "  OverheadDoor,                           !- Surface Type",
        "  Exterior OverheadDoor Construction,     !- Construction Name",
        "  Space1 Wall 3,                          !- Building Surface Name",
        "  ,                                       !- Outside Boundary Condition Object",
        "  ,                                       !- View Factor to Ground",
        "  ,                                       !- Frame and Divider Name",
        "  ,                                       !- Multiplier",
        "  ,                                       !- Number of Vertices",
        "  0,                                      !- Vertex 1 X-coordinate {m}",
        "  7.5,                                    !- Vertex 1 Y-coordinate {m}",
        "  2.5,                                    !- Vertex 1 Z-coordinate {m}",
        "  0,                                      !- Vertex 2 X-coordinate {m}",
        "  7.5,                                    !- Vertex 2 Y-coordinate {m}",
        "  0,                                      !- Vertex 2 Z-coordinate {m}",
        "  0,                                      !- Vertex 3 X-coordinate {m}",
        "  2.5,                                    !- Vertex 3 Y-coordinate {m}",
        "  0,                                      !- Vertex 3 Z-coordinate {m}",
        "  0,                                      !- Vertex 4 X-coordinate {m}",
        "  2.5,                                    !- Vertex 4 Y-coordinate {m}",
        "  2.5;                                    !- Vertex 4 Z-coordinate {m}",

        "BuildingSurface:Detailed,",
        "  Space1 Wall 4,                          !- Name",
        "  Wall,                                   !- Surface Type",
        "  Exterior Wall Construction,             !- Construction Name",
        "  Zone1,                                  !- Zone Name",
        "  ,                                       !- Space Name",
        "  Outdoors,                               !- Outside Boundary Condition",
        "  ,                                       !- Outside Boundary Condition Object",
        "  SunExposed,                             !- Sun Exposure",
        "  WindExposed,                            !- Wind Exposure",
        "  ,                                       !- View Factor to Ground",
        "  ,                                       !- Number of Vertices",
        "  0, 10, 3,                               !- X,Y,Z Vertex 1 {m}",
        "  10, 10, 3,                              !- X,Y,Z Vertex 2 {m}",
        "  10, 10, 0,                              !- X,Y,Z Vertex 3 {m}",
        "  0, 10, 0;                               !- X,Y,Z Vertex 4 {m}",

        "FenestrationSurface:Detailed,",
        "  ExteriorDoor - GlassDoor,               !- Name",
        "  GlassDoor,                              !- Surface Type",
        "  Exterior GlassDoor Construction,        !- Construction Name",
        "  Space1 Wall 4,                          !- Building Surface Name",
        "  ,                                       !- Outside Boundary Condition Object",
        "  ,                                       !- View Factor to Ground",
        "  ,                                       !- Frame and Divider Name",
        "  ,                                       !- Multiplier",
        "  ,                                       !- Number of Vertices",
        "  5.4,                                    !- Vertex 1 X-coordinate {m}",
        "  10,                                     !- Vertex 1 Y-coordinate {m}",
        "  2,                                      !- Vertex 1 Z-coordinate {m}",
        "  5.4,                                    !- Vertex 2 X-coordinate {m}",
        "  10,                                     !- Vertex 2 Y-coordinate {m}",
        "  0,                                      !- Vertex 2 Z-coordinate {m}",
        "  4.6,                                    !- Vertex 3 X-coordinate {m}",
        "  10,                                     !- Vertex 3 Y-coordinate {m}",
        "  0,                                      !- Vertex 3 Z-coordinate {m}",
        "  4.6,                                    !- Vertex 4 X-coordinate {m}",
        "  10,                                     !- Vertex 4 Y-coordinate {m}",
        "  2;                                      !- Vertex 4 Z-coordinate {m}",

        "Zone,",
        "  Zone2,                                  !- Name",
        "  0,                                      !- Direction of Relative North {deg}",
        "  10,                                     !- X Origin {m}",
        "  0,                                      !- Y Origin {m}",
        "  0,                                      !- Z Origin {m}",
        "  ,                                       !- Type",
        "  1,                                      !- Multiplier",
        "  ,                                       !- Ceiling Height {m}",
        "  ,                                       !- Volume {m3}",
        "  ,                                       !- Floor Area {m2}",
        "  ,                                       !- Zone Inside Convection Algorithm",
        "  ,                                       !- Zone Outside Convection Algorithm",
        "  Yes;                                    !- Part of Total Floor Area",

        "BuildingSurface:Detailed,",
        "  Space2 Floor,                           !- Name",
        "  Floor,                                  !- Surface Type",
        "  Ground Floor Construction,              !- Construction Name",
        "  Zone2,                                  !- Zone Name",
        "  ,                                       !- Space Name",
        "  Ground,                                 !- Outside Boundary Condition",
        "  ,                                       !- Outside Boundary Condition Object",
        "  NoSun,                                  !- Sun Exposure",
        "  NoWind,                                 !- Wind Exposure",
        "  ,                                       !- View Factor to Ground",
        "  ,                                       !- Number of Vertices",
        "  10, 10, 0,                              !- X,Y,Z Vertex 1 {m}",
        "  10, 0, 0,                               !- X,Y,Z Vertex 2 {m}",
        "  0, 0, 0,                                !- X,Y,Z Vertex 3 {m}",
        "  0, 10, 0;                               !- X,Y,Z Vertex 4 {m}",

        "BuildingSurface:Detailed,",
        "  Space2 RoofCeiling,                     !- Name",
        "  Ceiling,                                !- Surface Type",
        "  Interior Roof Construction,             !- Construction Name",
        "  Zone2,                                  !- Zone Name",
        "  ,                                       !- Space Name",
        "  Surface,                                !- Outside Boundary Condition",
        "  Space4 Floor,                           !- Outside Boundary Condition Object",
        "  NoSun,                                  !- Sun Exposure",
        "  NoWind,                                 !- Wind Exposure",
        "  ,                                       !- View Factor to Ground",
        "  ,                                       !- Number of Vertices",
        "  0, 10, 3,                               !- X,Y,Z Vertex 1 {m}",
        "  0, 0, 3,                                !- X,Y,Z Vertex 2 {m}",
        "  10, 0, 3,                               !- X,Y,Z Vertex 3 {m}",
        "  10, 10, 3;                              !- X,Y,Z Vertex 4 {m}",

        "BuildingSurface:Detailed,",
        "  Space2 Wall 1,                          !- Name",
        "  Wall,                                   !- Surface Type",
        "  Adiabatic Surface Construction,         !- Construction Name",
        "  Zone2,                                  !- Zone Name",
        "  ,                                       !- Space Name",
        "  Adiabatic,                              !- Outside Boundary Condition",
        "  ,                                       !- Outside Boundary Condition Object",
        "  NoSun,                                  !- Sun Exposure",
        "  NoWind,                                 !- Wind Exposure",
        "  ,                                       !- View Factor to Ground",
        "  ,                                       !- Number of Vertices",
        "  10, 10, 3,                              !- X,Y,Z Vertex 1 {m}",
        "  10, 0, 3,                               !- X,Y,Z Vertex 2 {m}",
        "  10, 0, 0,                               !- X,Y,Z Vertex 3 {m}",
        "  10, 10, 0;                              !- X,Y,Z Vertex 4 {m}",

        "BuildingSurface:Detailed,",
        "  Space2 Wall 2,                          !- Name",
        "  Wall,                                   !- Surface Type",
        "  Ground Wall Construction,               !- Construction Name",
        "  Zone2,                                  !- Zone Name",
        "  ,                                       !- Space Name",
        "  Ground,                                 !- Outside Boundary Condition",
        "  ,                                       !- Outside Boundary Condition Object",
        "  NoSun,                                  !- Sun Exposure",
        "  NoWind,                                 !- Wind Exposure",
        "  ,                                       !- View Factor to Ground",
        "  ,                                       !- Number of Vertices",
        "  10, 0, 3,                               !- X,Y,Z Vertex 1 {m}",
        "  0, 0, 3,                                !- X,Y,Z Vertex 2 {m}",
        "  0, 0, 0,                                !- X,Y,Z Vertex 3 {m}",
        "  10, 0, 0;                               !- X,Y,Z Vertex 4 {m}",

        "BuildingSurface:Detailed,",
        "  Space2 Wall 3,                          !- Name",
        "  Wall,                                   !- Surface Type",
        "  Interior Wall Construction,             !- Construction Name",
        "  Zone2,                                  !- Zone Name",
        "  ,                                       !- Space Name",
        "  Surface,                                !- Outside Boundary Condition",
        "  Space1 Wall 1,                          !- Outside Boundary Condition Object",
        "  NoSun,                                  !- Sun Exposure",
        "  NoWind,                                 !- Wind Exposure",
        "  ,                                       !- View Factor to Ground",
        "  ,                                       !- Number of Vertices",
        "  0, 0, 3,                                !- X,Y,Z Vertex 1 {m}",
        "  0, 10, 3,                               !- X,Y,Z Vertex 2 {m}",
        "  0, 10, 0,                               !- X,Y,Z Vertex 3 {m}",
        "  0, 0, 0;                                !- X,Y,Z Vertex 4 {m}",

        "FenestrationSurface:Detailed,",
        "  InteriorDoor - Door - Reversed,         !- Name",
        "  Door,                                   !- Surface Type",
        "  Interior Door Construction,             !- Construction Name",
        "  Space2 Wall 3,                          !- Building Surface Name",
        "  InteriorDoor - Door,                    !- Outside Boundary Condition Object",
        "  ,                                       !- View Factor to Ground",
        "  ,                                       !- Frame and Divider Name",
        "  ,                                       !- Multiplier",
        "  ,                                       !- Number of Vertices",
        "  10,                                     !- Vertex 1 X-coordinate {m}",
        "  5.4,                                    !- Vertex 1 Y-coordinate {m}",
        "  2,                                      !- Vertex 1 Z-coordinate {m}",
        "  10,                                     !- Vertex 2 X-coordinate {m}",
        "  5.4,                                    !- Vertex 2 Y-coordinate {m}",
        "  0,                                      !- Vertex 2 Z-coordinate {m}",
        "  10,                                     !- Vertex 3 X-coordinate {m}",
        "  4.6,                                    !- Vertex 3 Y-coordinate {m}",
        "  0,                                      !- Vertex 3 Z-coordinate {m}",
        "  10,                                     !- Vertex 4 X-coordinate {m}",
        "  4.6,                                    !- Vertex 4 Y-coordinate {m}",
        "  2;                                      !- Vertex 4 Z-coordinate {m}",

        "BuildingSurface:Detailed,",
        "  Space2 Wall 4,                          !- Name",
        "  Wall,                                   !- Surface Type",
        "  Exterior Wall Construction,             !- Construction Name",
        "  Zone2,                                  !- Zone Name",
        "  ,                                       !- Space Name",
        "  Outdoors,                               !- Outside Boundary Condition",
        "  ,                                       !- Outside Boundary Condition Object",
        "  SunExposed,                             !- Sun Exposure",
        "  WindExposed,                            !- Wind Exposure",
        "  ,                                       !- View Factor to Ground",
        "  ,                                       !- Number of Vertices",
        "  0, 10, 3,                               !- X,Y,Z Vertex 1 {m}",
        "  10, 10, 3,                              !- X,Y,Z Vertex 2 {m}",
        "  10, 10, 0,                              !- X,Y,Z Vertex 3 {m}",
        "  0, 10, 0;                               !- X,Y,Z Vertex 4 {m}",

        "FenestrationSurface:Detailed,",
        "  ExteriorWindow - FixedWindow,           !- Name",
        "  FixedWindow,                            !- Surface Type",
        "  Exterior FixedWindow Construction,      !- Construction Name",
        "  Space2 Wall 4,                          !- Building Surface Name",
        "  ,                                       !- Outside Boundary Condition Object",
        "  ,                                       !- View Factor to Ground",
        "  ,                                       !- Frame and Divider Name",
        "  ,                                       !- Multiplier",
        "  ,                                       !- Number of Vertices",
        "  9.9746,                                 !- Vertex 1 X-coordinate {m}",
        "  10,                                     !- Vertex 1 Y-coordinate {m}",
        "  1.96812712579906,                       !- Vertex 1 Z-coordinate {m}",
        "  9.9746,                                 !- Vertex 2 X-coordinate {m}",
        "  10,                                     !- Vertex 2 Y-coordinate {m}",
        "  0.762,                                  !- Vertex 2 Z-coordinate {m}",
        "  0.0254000000000012,                     !- Vertex 3 X-coordinate {m}",
        "  10,                                     !- Vertex 3 Y-coordinate {m}",
        "  0.762,                                  !- Vertex 3 Z-coordinate {m}",
        "  0.0254000000000012,                     !- Vertex 4 X-coordinate {m}",
        "  10,                                     !- Vertex 4 Y-coordinate {m}",
        "  1.96812712579906;                       !- Vertex 4 Z-coordinate {m}",

        "Zone,",
        "  Zone3,                                  !- Name",
        "  0,                                      !- Direction of Relative North {deg}",
        "  0,                                      !- X Origin {m}",
        "  0,                                      !- Y Origin {m}",
        "  3,                                      !- Z Origin {m}",
        "  ,                                       !- Type",
        "  1,                                      !- Multiplier",
        "  ,                                       !- Ceiling Height {m}",
        "  ,                                       !- Volume {m3}",
        "  ,                                       !- Floor Area {m2}",
        "  ,                                       !- Zone Inside Convection Algorithm",
        "  ,                                       !- Zone Outside Convection Algorithm",
        "  Yes;                                    !- Part of Total Floor Area",

        "BuildingSurface:Detailed,",
        "  Space3 Floor,                           !- Name",
        "  Floor,                                  !- Surface Type",
        "  Interior Floor Construction,            !- Construction Name",
        "  Zone3,                                  !- Zone Name",
        "  ,                                       !- Space Name",
        "  Surface,                                !- Outside Boundary Condition",
        "  Space1 RoofCeiling,                     !- Outside Boundary Condition Object",
        "  NoSun,                                  !- Sun Exposure",
        "  NoWind,                                 !- Wind Exposure",
        "  ,                                       !- View Factor to Ground",
        "  ,                                       !- Number of Vertices",
        "  10, 10, 0,                              !- X,Y,Z Vertex 1 {m}",
        "  10, 0, 0,                               !- X,Y,Z Vertex 2 {m}",
        "  0, 0, 0,                                !- X,Y,Z Vertex 3 {m}",
        "  0, 10, 0;                               !- X,Y,Z Vertex 4 {m}",

        "BuildingSurface:Detailed,",
        "  Space3 RoofCeiling,                     !- Name",
        "  Roof,                                   !- Surface Type",
        "  Exterior Roof Construction,             !- Construction Name",
        "  Zone3,                                  !- Zone Name",
        "  ,                                       !- Space Name",
        "  Outdoors,                               !- Outside Boundary Condition",
        "  ,                                       !- Outside Boundary Condition Object",
        "  SunExposed,                             !- Sun Exposure",
        "  WindExposed,                            !- Wind Exposure",
        "  ,                                       !- View Factor to Ground",
        "  ,                                       !- Number of Vertices",
        "  0, 10, 3,                               !- X,Y,Z Vertex 1 {m}",
        "  0, 0, 3,                                !- X,Y,Z Vertex 2 {m}",
        "  10, 0, 3,                               !- X,Y,Z Vertex 3 {m}",
        "  10, 10, 3;                              !- X,Y,Z Vertex 4 {m}",

        "BuildingSurface:Detailed,",
        "  Space3 Wall 1,                          !- Name",
        "  Wall,                                   !- Surface Type",
        "  Interior Wall Construction,             !- Construction Name",
        "  Zone3,                                  !- Zone Name",
        "  ,                                       !- Space Name",
        "  Surface,                                !- Outside Boundary Condition",
        "  Space4 Wall 3,                          !- Outside Boundary Condition Object",
        "  NoSun,                                  !- Sun Exposure",
        "  NoWind,                                 !- Wind Exposure",
        "  ,                                       !- View Factor to Ground",
        "  ,                                       !- Number of Vertices",
        "  10, 10, 3,                              !- X,Y,Z Vertex 1 {m}",
        "  10, 0, 3,                               !- X,Y,Z Vertex 2 {m}",
        "  10, 0, 0,                               !- X,Y,Z Vertex 3 {m}",
        "  10, 10, 0;                              !- X,Y,Z Vertex 4 {m}",

        "FenestrationSurface:Detailed,",
        "  InteriorWindow - FixedWindow,           !- Name",
        "  FixedWindow,                            !- Surface Type",
        "  Interior FixedWindow Construction,      !- Construction Name",
        "  Space3 Wall 1,                          !- Building Surface Name",
        "  InteriorWindow - FixedWindow - Reversed, !- Outside Boundary Condition Object",
        "  ,                                       !- View Factor to Ground",
        "  ,                                       !- Frame and Divider Name",
        "  ,                                       !- Multiplier",
        "  ,                                       !- Number of Vertices",
        "  10,                                     !- Vertex 1 X-coordinate {m}",
        "  0.0254,                                 !- Vertex 1 Y-coordinate {m}",
        "  1.96812712579906,                       !- Vertex 1 Z-coordinate {m}",
        "  10,                                     !- Vertex 2 X-coordinate {m}",
        "  0.0254,                                 !- Vertex 2 Y-coordinate {m}",
        "  0.762,                                  !- Vertex 2 Z-coordinate {m}",
        "  10,                                     !- Vertex 3 X-coordinate {m}",
        "  9.9746,                                 !- Vertex 3 Y-coordinate {m}",
        "  0.762,                                  !- Vertex 3 Z-coordinate {m}",
        "  10,                                     !- Vertex 4 X-coordinate {m}",
        "  9.9746,                                 !- Vertex 4 Y-coordinate {m}",
        "  1.96812712579906;                       !- Vertex 4 Z-coordinate {m}",

        "BuildingSurface:Detailed,",
        "  Space3 Wall 2,                          !- Name",
        "  Wall,                                   !- Surface Type",
        "  Exterior Wall Construction,             !- Construction Name",
        "  Zone3,                                  !- Zone Name",
        "  ,                                       !- Space Name",
        "  Outdoors,                               !- Outside Boundary Condition",
        "  ,                                       !- Outside Boundary Condition Object",
        "  SunExposed,                             !- Sun Exposure",
        "  WindExposed,                            !- Wind Exposure",
        "  ,                                       !- View Factor to Ground",
        "  ,                                       !- Number of Vertices",
        "  10, 0, 3,                               !- X,Y,Z Vertex 1 {m}",
        "  0, 0, 3,                                !- X,Y,Z Vertex 2 {m}",
        "  0, 0, 0,                                !- X,Y,Z Vertex 3 {m}",
        "  10, 0, 0;                               !- X,Y,Z Vertex 4 {m}",

        "FenestrationSurface:Detailed,",
        "  ExteriorWindow - OperableWindow,        !- Name",
        "  OperableWindow,                         !- Surface Type",
        "  Exterior OperableWindow Construction,   !- Construction Name",
        "  Space3 Wall 2,                          !- Building Surface Name",
        "  ,                                       !- Outside Boundary Condition Object",
        "  ,                                       !- View Factor to Ground",
        "  ,                                       !- Frame and Divider Name",
        "  ,                                       !- Multiplier",
        "  ,                                       !- Number of Vertices",
        "  0.0254,                                 !- Vertex 1 X-coordinate {m}",
        "  0,                                      !- Vertex 1 Y-coordinate {m}",
        "  1.96812712579906,                       !- Vertex 1 Z-coordinate {m}",
        "  0.0254,                                 !- Vertex 2 X-coordinate {m}",
        "  0,                                      !- Vertex 2 Y-coordinate {m}",
        "  0.762,                                  !- Vertex 2 Z-coordinate {m}",
        "  9.9746,                                 !- Vertex 3 X-coordinate {m}",
        "  0,                                      !- Vertex 3 Y-coordinate {m}",
        "  0.762,                                  !- Vertex 3 Z-coordinate {m}",
        "  9.9746,                                 !- Vertex 4 X-coordinate {m}",
        "  0,                                      !- Vertex 4 Y-coordinate {m}",
        "  1.96812712579906;                       !- Vertex 4 Z-coordinate {m}",

        "BuildingSurface:Detailed,",
        "  Space3 Wall 3,                          !- Name",
        "  Wall,                                   !- Surface Type",
        "  Exterior Wall Construction,             !- Construction Name",
        "  Zone3,                                  !- Zone Name",
        "  ,                                       !- Space Name",
        "  Outdoors,                               !- Outside Boundary Condition",
        "  ,                                       !- Outside Boundary Condition Object",
        "  SunExposed,                             !- Sun Exposure",
        "  WindExposed,                            !- Wind Exposure",
        "  ,                                       !- View Factor to Ground",
        "  ,                                       !- Number of Vertices",
        "  0, 0, 3,                                !- X,Y,Z Vertex 1 {m}",
        "  0, 10, 3,                               !- X,Y,Z Vertex 2 {m}",
        "  0, 10, 0,                               !- X,Y,Z Vertex 3 {m}",
        "  0, 0, 0;                                !- X,Y,Z Vertex 4 {m}",

        "FenestrationSurface:Detailed,",
        "  ExteriorWindow - Window,                !- Name",
        "  Window,                                 !- Surface Type",
        "  Exterior FixedWindow Construction,      !- Construction Name",
        "  Space3 Wall 3,                          !- Building Surface Name",
        "  ,                                       !- Outside Boundary Condition Object",
        "  ,                                       !- View Factor to Ground",
        "  ,                                       !- Frame and Divider Name",
        "  ,                                       !- Multiplier",
        "  ,                                       !- Number of Vertices",
        "  0,                                      !- Vertex 1 X-coordinate {m}",
        "  9.9746,                                 !- Vertex 1 Y-coordinate {m}",
        "  1.96812712579906,                       !- Vertex 1 Z-coordinate {m}",
        "  0,                                      !- Vertex 2 X-coordinate {m}",
        "  9.9746,                                 !- Vertex 2 Y-coordinate {m}",
        "  0.762,                                  !- Vertex 2 Z-coordinate {m}",
        "  0,                                      !- Vertex 3 X-coordinate {m}",
        "  0.0254000000000012,                     !- Vertex 3 Y-coordinate {m}",
        "  0.762,                                  !- Vertex 3 Z-coordinate {m}",
        "  0,                                      !- Vertex 4 X-coordinate {m}",
        "  0.0254000000000012,                     !- Vertex 4 Y-coordinate {m}",
        "  1.96812712579906;                       !- Vertex 4 Z-coordinate {m}",

        "BuildingSurface:Detailed,",
        "  Space3 Wall 4,                          !- Name",
        "  Wall,                                   !- Surface Type",
        "  Exterior Wall Construction,             !- Construction Name",
        "  Zone3,                                  !- Zone Name",
        "  ,                                       !- Space Name",
        "  Outdoors,                               !- Outside Boundary Condition",
        "  ,                                       !- Outside Boundary Condition Object",
        "  SunExposed,                             !- Sun Exposure",
        "  WindExposed,                            !- Wind Exposure",
        "  ,                                       !- View Factor to Ground",
        "  ,                                       !- Number of Vertices",
        "  0, 10, 3,                               !- X,Y,Z Vertex 1 {m}",
        "  10, 10, 3,                              !- X,Y,Z Vertex 2 {m}",
        "  10, 10, 0,                              !- X,Y,Z Vertex 3 {m}",
        "  0, 10, 0;                               !- X,Y,Z Vertex 4 {m}",

        "Zone,",
        "  Zone4,                                  !- Name",
        "  0,                                      !- Direction of Relative North {deg}",
        "  10,                                     !- X Origin {m}",
        "  0,                                      !- Y Origin {m}",
        "  3,                                      !- Z Origin {m}",
        "  ,                                       !- Type",
        "  1,                                      !- Multiplier",
        "  ,                                       !- Ceiling Height {m}",
        "  ,                                       !- Volume {m3}",
        "  ,                                       !- Floor Area {m2}",
        "  ,                                       !- Zone Inside Convection Algorithm",
        "  ,                                       !- Zone Outside Convection Algorithm",
        "  Yes;                                    !- Part of Total Floor Area",

        "BuildingSurface:Detailed,",
        "  Space4 Floor,                           !- Name",
        "  Floor,                                  !- Surface Type",
        "  Interior Floor Construction,            !- Construction Name",
        "  Zone4,                                  !- Zone Name",
        "  ,                                       !- Space Name",
        "  Surface,                                !- Outside Boundary Condition",
        "  Space2 RoofCeiling,                     !- Outside Boundary Condition Object",
        "  NoSun,                                  !- Sun Exposure",
        "  NoWind,                                 !- Wind Exposure",
        "  ,                                       !- View Factor to Ground",
        "  ,                                       !- Number of Vertices",
        "  10, 10, 0,                              !- X,Y,Z Vertex 1 {m}",
        "  10, 0, 0,                               !- X,Y,Z Vertex 2 {m}",
        "  0, 0, 0,                                !- X,Y,Z Vertex 3 {m}",
        "  0, 10, 0;                               !- X,Y,Z Vertex 4 {m}",

        "BuildingSurface:Detailed,",
        "  Space4 RoofCeiling,                     !- Name",
        "  Roof,                                   !- Surface Type",
        "  Exterior Roof Construction,             !- Construction Name",
        "  Zone4,                                  !- Zone Name",
        "  ,                                       !- Space Name",
        "  Outdoors,                               !- Outside Boundary Condition",
        "  ,                                       !- Outside Boundary Condition Object",
        "  SunExposed,                             !- Sun Exposure",
        "  WindExposed,                            !- Wind Exposure",
        "  ,                                       !- View Factor to Ground",
        "  ,                                       !- Number of Vertices",
        "  0, 10, 3,                               !- X,Y,Z Vertex 1 {m}",
        "  0, 0, 3,                                !- X,Y,Z Vertex 2 {m}",
        "  10, 0, 3,                               !- X,Y,Z Vertex 3 {m}",
        "  10, 10, 3;                              !- X,Y,Z Vertex 4 {m}",

        "FenestrationSurface:Detailed,",
        "  ExteriorWindow - Skylight,              !- Name",
        "  Skylight,                               !- Surface Type",
        "  Exterior Skylight Construction,         !- Construction Name",
        "  Space4 RoofCeiling,                     !- Building Surface Name",
        "  ,                                       !- Outside Boundary Condition Object",
        "  ,                                       !- View Factor to Ground",
        "  ,                                       !- Frame and Divider Name",
        "  ,                                       !- Multiplier",
        "  ,                                       !- Number of Vertices",
        "  1.83772233983162,                       !- Vertex 1 X-coordinate {m}",
        "  8.16227766016838,                       !- Vertex 1 Y-coordinate {m}",
        "  3,                                      !- Vertex 1 Z-coordinate {m}",
        "  1.83772233983162,                       !- Vertex 2 X-coordinate {m}",
        "  1.83772233983162,                       !- Vertex 2 Y-coordinate {m}",
        "  3,                                      !- Vertex 2 Z-coordinate {m}",
        "  8.16227766016838,                       !- Vertex 3 X-coordinate {m}",
        "  1.83772233983162,                       !- Vertex 3 Y-coordinate {m}",
        "  3,                                      !- Vertex 3 Z-coordinate {m}",
        "  8.16227766016838,                       !- Vertex 4 X-coordinate {m}",
        "  8.16227766016838,                       !- Vertex 4 Y-coordinate {m}",
        "  3;                                      !- Vertex 4 Z-coordinate {m}",

        "BuildingSurface:Detailed,",
        "  Space4 Wall 1,                          !- Name",
        "  Wall,                                   !- Surface Type",
        "  Exterior Wall Construction,             !- Construction Name",
        "  Zone4,                                  !- Zone Name",
        "  ,                                       !- Space Name",
        "  Outdoors,                               !- Outside Boundary Condition",
        "  ,                                       !- Outside Boundary Condition Object",
        "  SunExposed,                             !- Sun Exposure",
        "  WindExposed,                            !- Wind Exposure",
        "  ,                                       !- View Factor to Ground",
        "  ,                                       !- Number of Vertices",
        "  10, 10, 3,                              !- X,Y,Z Vertex 1 {m}",
        "  10, 0, 3,                               !- X,Y,Z Vertex 2 {m}",
        "  10, 0, 0,                               !- X,Y,Z Vertex 3 {m}",
        "  10, 10, 0;                              !- X,Y,Z Vertex 4 {m}",

        "BuildingSurface:Detailed,",
        "  Space4 Wall 2,                          !- Name",
        "  Wall,                                   !- Surface Type",
        "  Exterior Wall Construction,             !- Construction Name",
        "  Zone4,                                  !- Zone Name",
        "  ,                                       !- Space Name",
        "  Outdoors,                               !- Outside Boundary Condition",
        "  ,                                       !- Outside Boundary Condition Object",
        "  SunExposed,                             !- Sun Exposure",
        "  WindExposed,                            !- Wind Exposure",
        "  ,                                       !- View Factor to Ground",
        "  ,                                       !- Number of Vertices",
        "  10, 0, 3,                               !- X,Y,Z Vertex 1 {m}",
        "  0, 0, 3,                                !- X,Y,Z Vertex 2 {m}",
        "  0, 0, 0,                                !- X,Y,Z Vertex 3 {m}",
        "  10, 0, 0;                               !- X,Y,Z Vertex 4 {m}",

        "BuildingSurface:Detailed,",
        "  Space4 Wall 3,                          !- Name",
        "  Wall,                                   !- Surface Type",
        "  Interior Wall Construction,             !- Construction Name",
        "  Zone4,                                  !- Zone Name",
        "  ,                                       !- Space Name",
        "  Surface,                                !- Outside Boundary Condition",
        "  Space3 Wall 1,                          !- Outside Boundary Condition Object",
        "  NoSun,                                  !- Sun Exposure",
        "  NoWind,                                 !- Wind Exposure",
        "  ,                                       !- View Factor to Ground",
        "  ,                                       !- Number of Vertices",
        "  0, 0, 3,                                !- X,Y,Z Vertex 1 {m}",
        "  0, 10, 3,                               !- X,Y,Z Vertex 2 {m}",
        "  0, 10, 0,                               !- X,Y,Z Vertex 3 {m}",
        "  0, 0, 0;                                !- X,Y,Z Vertex 4 {m}",

        "FenestrationSurface:Detailed,",
        "  InteriorWindow - FixedWindow - Reversed, !- Name",
        "  FixedWindow,                            !- Surface Type",
        "  Interior FixedWindow Construction,      !- Construction Name",
        "  Space4 Wall 3,                          !- Building Surface Name",
        "  InteriorWindow - FixedWindow,           !- Outside Boundary Condition Object",
        "  ,                                       !- View Factor to Ground",
        "  ,                                       !- Frame and Divider Name",
        "  ,                                       !- Multiplier",
        "  ,                                       !- Number of Vertices",
        "  10,                                     !- Vertex 1 X-coordinate {m}",
        "  9.9746,                                 !- Vertex 1 Y-coordinate {m}",
        "  1.96812712579906,                       !- Vertex 1 Z-coordinate {m}",
        "  10,                                     !- Vertex 2 X-coordinate {m}",
        "  9.9746,                                 !- Vertex 2 Y-coordinate {m}",
        "  0.762,                                  !- Vertex 2 Z-coordinate {m}",
        "  10,                                     !- Vertex 3 X-coordinate {m}",
        "  0.0254,                                 !- Vertex 3 Y-coordinate {m}",
        "  0.762,                                  !- Vertex 3 Z-coordinate {m}",
        "  10,                                     !- Vertex 4 X-coordinate {m}",
        "  0.0254,                                 !- Vertex 4 Y-coordinate {m}",
        "  1.96812712579906;                       !- Vertex 4 Z-coordinate {m}",

        "BuildingSurface:Detailed,",
        "  Space4 Wall 4,                          !- Name",
        "  Wall,                                   !- Surface Type",
        "  Exterior Wall Construction,             !- Construction Name",
        "  Zone4,                                  !- Zone Name",
        "  ,                                       !- Space Name",
        "  Outdoors,                               !- Outside Boundary Condition",
        "  ,                                       !- Outside Boundary Condition Object",
        "  SunExposed,                             !- Sun Exposure",
        "  WindExposed,                            !- Wind Exposure",
        "  ,                                       !- View Factor to Ground",
        "  ,                                       !- Number of Vertices",
        "  0, 10, 3,                               !- X,Y,Z Vertex 1 {m}",
        "  10, 10, 3,                              !- X,Y,Z Vertex 2 {m}",
        "  10, 10, 0,                              !- X,Y,Z Vertex 3 {m}",
        "  0, 10, 0;                               !- X,Y,Z Vertex 4 {m}",
    });

    ASSERT_TRUE(process_idf(std::string(idf_dcs_all_types) + geometry));
    state->init_state(*state);

    bool ErrorsFound = false;
    Material::GetMaterialData(*state, ErrorsFound);
    ASSERT_FALSE(ErrorsFound);
    HeatBalanceManager::GetFrameAndDividerData(*state);
    HeatBalanceManager::SetPreConstructionInputParameters(*state);
    HeatBalanceManager::GetConstructData(*state, ErrorsFound);
    ASSERT_FALSE(ErrorsFound);
    ConstructionAssignments::GetConstructionAssignmentSetData(*state, ErrorsFound);
    ASSERT_FALSE(ErrorsFound);
    HeatBalanceManager::GetZoneData(*state, ErrorsFound);
    ASSERT_FALSE(ErrorsFound);
    SurfaceGeometry::GetGeometryParameters(*state, ErrorsFound);
    ASSERT_FALSE(ErrorsFound);
    SurfaceGeometry::SetupZoneGeometry(*state, ErrorsFound);
    ASSERT_FALSE(ErrorsFound);

    ASSERT_EQ(1u, state->dataConstructionAssignments->constructionAssignmentSets.size());
    auto const &dcs = state->dataConstructionAssignments->constructionAssignmentSets[0];

    auto checkDefaultConstruction = [&](const std::string &expectedConstrName, const std::string &surfName) {
        const std::string &expectedConstrNameUpper = Util::makeUPPER(expectedConstrName);
        int const surfIdx = Util::FindItemInList(Util::makeUPPER(surfName), state->dataSurface->Surface);
        ASSERT_GT(surfIdx, 0) << "Surface not found: " << surfName;
        DataSurfaces::SurfaceData const &surf = state->dataSurface->Surface(surfIdx);

        int const defaultConstrIdx = dcs.getConstructionAssignment(surf);
        ASSERT_GT(defaultConstrIdx, 0) << "No default construction for surface: " << surf.Name;
        EXPECT_EQ(expectedConstrNameUpper, state->dataConstruction->Construct(defaultConstrIdx).Name)
            << "Default construction mismatch for surface: " << surf.Name;

        int const hardAssignedConstruction = surf.Construction;
        ASSERT_GT(hardAssignedConstruction, 0) << "Surface has no assigned construction: " << surf.Name;
        EXPECT_EQ(expectedConstrNameUpper, state->dataConstruction->Construct(hardAssignedConstruction).Name);
    };

    // Exterior Surfaces
    /// Exterior Floor
    checkDefaultConstruction("Exterior Floor Construction", "Space1 Floor");

    /// Exterior Wall
    checkDefaultConstruction("Exterior Wall Construction", "Space4 Wall 4");
    checkDefaultConstruction("Exterior Wall Construction", "Space4 Wall 2");
    checkDefaultConstruction("Exterior Wall Construction", "Space4 Wall 1");
    checkDefaultConstruction("Exterior Wall Construction", "Space3 Wall 4");
    checkDefaultConstruction("Exterior Wall Construction", "Space3 Wall 3");
    checkDefaultConstruction("Exterior Wall Construction", "Space3 Wall 2");
    checkDefaultConstruction("Exterior Wall Construction", "Space2 Wall 4");
    checkDefaultConstruction("Exterior Wall Construction", "Space1 Wall 4");
    checkDefaultConstruction("Exterior Wall Construction", "Space1 Wall 3");
    checkDefaultConstruction("Exterior Wall Construction", "Space1 Wall 2");

    /// Exterior Roof
    checkDefaultConstruction("Exterior Roof Construction", "Space4 RoofCeiling");
    checkDefaultConstruction("Exterior Roof Construction", "Space3 RoofCeiling");

    // Interior Surfaces
    /// Interior Floor
    checkDefaultConstruction("Interior Floor Construction", "Space4 Floor");
    checkDefaultConstruction("Interior Floor Construction", "Space3 Floor");

    /// Interior Wall
    checkDefaultConstruction("Interior Wall Construction", "Space4 Wall 3");
    checkDefaultConstruction("Interior Wall Construction", "Space3 Wall 1");
    checkDefaultConstruction("Interior Wall Construction", "Space2 Wall 3");
    checkDefaultConstruction("Interior Wall Construction", "Space1 Wall 1");

    /// Interior Roof
    checkDefaultConstruction("Interior Roof Construction", "Space2 RoofCeiling");
    checkDefaultConstruction("Interior Roof Construction", "Space1 RoofCeiling");

    // Ground Surfaces
    /// Ground Floor
    checkDefaultConstruction("Ground Floor Construction", "Space2 Floor");

    /// Ground Wall
    checkDefaultConstruction("Ground Wall Construction", "Space2 Wall 2");

    // Adiabatic Surfaces
    /// Adiabatic Wall
    checkDefaultConstruction("Adiabatic Surface Construction", "Space2 Wall 1");

    // ==============================================================
    // Exterior SubSurfaces
    /// Exterior Window
    checkDefaultConstruction("Exterior FixedWindow Construction", "ExteriorWindow - Window");

    /// Exterior FixedWindow
    checkDefaultConstruction("Exterior FixedWindow Construction", "ExteriorWindow - FixedWindow");

    /// Exterior OperableWindow
    checkDefaultConstruction("Exterior OperableWindow Construction", "ExteriorWindow - OperableWindow");

    /// Exterior Door
    checkDefaultConstruction("Exterior Door Construction", "ExteriorDoor - Door");

    /// Exterior GlassDoor
    checkDefaultConstruction("Exterior GlassDoor Construction", "ExteriorDoor - GlassDoor");

    /// Exterior OverheadDoor
    checkDefaultConstruction("Exterior OverheadDoor Construction", "ExteriorDoor - OverheadDoor");

    /// Exterior Skylight
    checkDefaultConstruction("Exterior Skylight Construction", "ExteriorWindow - Skylight");

    // Interior SubSurfaces
    /// Interior FixedWindow
    checkDefaultConstruction("Interior FixedWindow Construction", "InteriorWindow - FixedWindow - Reversed");
    checkDefaultConstruction("Interior FixedWindow Construction", "InteriorWindow - FixedWindow");

    /// Interior Door
    checkDefaultConstruction("Interior Door Construction", "InteriorDoor - Door - Reversed");
    checkDefaultConstruction("Interior Door Construction", "InteriorDoor - Door");
}

TEST_F(EnergyPlusFixture, ConstructionAssignmentSet_SpaceResolvesIndex)
{
    // Verify that a Space with construction_assignment_set_name has its constructionAssignmentSetIndex
    // set to the correct 0-based index into dataConstructionAssignments->constructionAssignmentSets.
    std::string const idf_objects = std::string(idf_constructions) + R"(
  SurfaceConstructionAssignments,
    Ext Surf Constrs,        !- Name
    Constr Floor,            !- Floor Construction Name
    Constr Wall,             !- Wall Construction Name
    Constr Roof;             !- Roof Ceiling Construction Name

  ConstructionAssignmentSet,
    DCS A,                   !- Name
    Ext Surf Constrs,        !- Exterior Surface Construction Assignments Name
    ,                        !- Interior Surface Construction Assignments Name
    ,                        !- Ground Contact Surface Construction Assignments Name
    ,                        !- Exterior SubSurface Construction Assignments Name
    ,                        !- Interior SubSurface Construction Assignments Name
    ,                        !- Interior Partition Construction Name
    ;                        !- Adiabatic Surface Construction Name

  ConstructionAssignmentSet,
    DCS B,                   !- Name
    ,                        !- Exterior Surface Construction Assignments Name
    ,                        !- Interior Surface Construction Assignments Name
    ,                        !- Ground Contact Surface Construction Assignments Name
    ,                        !- Exterior SubSurface Construction Assignments Name
    ,                        !- Interior SubSurface Construction Assignments Name
    ,                        !- Interior Partition Construction Name
    ;                        !- Adiabatic Surface Construction Name

  Zone,
    TestZone,                               !- Name
    ,                                       !- Direction of Relative North {deg}
    0,                                      !- X Origin {m}
    0,                                      !- Y Origin {m}
    0,                                      !- Z Origin {m}
    ,                                       !- Type
    1,                                      !- Multiplier
    ,                                       !- Ceiling Height {m}
    ,                                       !- Volume {m3}
    ,                                       !- Floor Area {m2}
    ,                                       !- Zone Inside Convection Algorithm
    ,                                       !- Zone Outside Convection Algorithm
    Yes;                                    !- Part of Total Floor Area

  Space,
    SpaceWithDCS,                           !- Name
    TestZone,                               !- Zone Name
    ,                                       !- Ceiling Height {m}
    ,                                       !- Volume {m3}
    ,                                       !- Floor Area {m2}
    Space Type 1,                           !- Space Type
    DCS B;                                  !- Construction Assignment Set Name

  Space,
    SpaceWithoutDCS,                           !- Name
    TestZone,                               !- Zone Name
)";

    ASSERT_TRUE(process_idf(idf_objects));
    state->init_state(*state);
    loadConstructions(*state);

    bool ErrorsFound = false;
    ConstructionAssignments::GetConstructionAssignmentSetData(*state, ErrorsFound);
    ASSERT_FALSE(ErrorsFound);

    HeatBalanceManager::GetZoneData(*state, ErrorsFound);
    ASSERT_FALSE(ErrorsFound);

    auto &s_dc = state->dataConstructionAssignments;
    ASSERT_EQ(2u, s_dc->constructionAssignmentSets.size());

    // Find the index of "DCS B" — it may not be 1 due to JSON key ordering.
    int dcsBIndex = -1;
    for (int i = 0; i < static_cast<int>(s_dc->constructionAssignmentSets.size()); ++i) {
        if (s_dc->constructionAssignmentSets[i].Name == "DCS B") {
            dcsBIndex = i;
            break;
        }
    }
    ASSERT_GE(dcsBIndex, 0) << "DCS B not found in constructionAssignmentSets";

    int spaceWithDCS = Util::FindItemInList(std::string("SPACEWITHDCS"), state->dataHeatBal->space);
    ASSERT_GT(spaceWithDCS, 0);
    EXPECT_EQ(dcsBIndex, state->dataHeatBal->space(spaceWithDCS).constructionAssignmentSetIndex);

    int spaceWithoutDCS = Util::FindItemInList(std::string("SPACEWITHOUTDCS"), state->dataHeatBal->space);
    ASSERT_GT(spaceWithoutDCS, 0);
    EXPECT_EQ(-1, state->dataHeatBal->space(spaceWithoutDCS).constructionAssignmentSetIndex);
}

TEST_F(EnergyPlusFixture, ConstructionAssignmentSet_SpaceBadDCSName)
{
    // A Space referencing a non-existent ConstructionAssignmentSet name should produce a severe error.
    std::string const idf_objects = std::string(idf_constructions) + R"(
  Zone,
    TestZone,                               !- Name
    ,                                       !- Direction of Relative North {deg}
    0,                                      !- X Origin {m}
    0,                                      !- Y Origin {m}
    0,                                      !- Z Origin {m}
    ,                                       !- Type
    1,                                      !- Multiplier
    ,                                       !- Ceiling Height {m}
    ,                                       !- Volume {m3}
    ,                                       !- Floor Area {m2}
    ,                                       !- Zone Inside Convection Algorithm
    ,                                       !- Zone Outside Convection Algorithm
    Yes;                                    !- Part of Total Floor Area

  Space,
    BadSpace,                               !- Name
    TestZone,                               !- Zone Name
    ,                                       !- Ceiling Height {m}
    ,                                       !- Volume {m3}
    ,                                       !- Floor Area {m2}
    Space Type 1,                           !- Space Type
    NONEXISTENT DCS;                        !- Construction Assignment Set Name
)";

    ASSERT_TRUE(process_idf(idf_objects));
    state->init_state(*state);
    loadConstructions(*state);

    bool ErrorsFound = false;
    ConstructionAssignments::GetConstructionAssignmentSetData(*state, ErrorsFound);
    ASSERT_FALSE(ErrorsFound);

    HeatBalanceManager::GetZoneData(*state, ErrorsFound);
    EXPECT_TRUE(ErrorsFound);
    EXPECT_TRUE(compare_err_stream_substring(R"(invalid construction_assignment_set_name="NONEXISTENT DCS" not found.)", false));
}

TEST_F(EnergyPlusFixture, ConstructionResolution_BuildingLevelOnly)
{
    // End-to-end: SurfaceGeometry::GetSurfaceData resolves blank Construction Name fields via the
    // Building-level ConstructionAssignmentSet when there is no Space-level override.
    std::string const idf_objects = std::string(idf_dcs_all_types) + R"(
  Building,
    Building1,                              !- Name
    ,                                       !- North Axis {deg}
    ,                                       !- Terrain
    ,                                       !- Loads Convergence Tolerance Value {W}
    ,                                       !- Temperature Convergence Tolerance Value {deltaC}
    ,                                       !- Solar Distribution
    ,                                       !- Maximum Number of Warmup Days
    ,                                       !- Minimum Number of Warmup Days
    Default Construction Set;               !- Construction Assignment Set Name

  Zone,
    TestZone,                               !- Name
    ,                                       !- Direction of Relative North {deg}
    0,                                      !- X Origin {m}
    0,                                      !- Y Origin {m}
    0,                                      !- Z Origin {m}
    ,                                       !- Type
    1,                                      !- Multiplier
    ,                                       !- Ceiling Height {m}
    ,                                       !- Volume {m3}
    ,                                       !- Floor Area {m2}
    ,                                       !- Zone Inside Convection Algorithm
    ,                                       !- Zone Outside Convection Algorithm
    Yes;                                    !- Part of Total Floor Area

  BuildingSurface:Detailed,
    Wall1,                                  !- Name
    Wall,                                   !- Surface Type
    ,                                       !- Construction Name
    TestZone,                               !- Zone Name
    ,                                       !- Space Name
    Outdoors,                               !- Outside Boundary Condition
    ,                                       !- Outside Boundary Condition Object
    SunExposed,                             !- Sun Exposure
    WindExposed,                            !- Wind Exposure
    ,                                       !- View Factor to Ground
    ,                                       !- Number of Vertices
    0, 0, 3,                                !- X,Y,Z Vertex 1 {m}
    0, 0, 0,                                !- X,Y,Z Vertex 2 {m}
    10, 0, 0,                               !- X,Y,Z Vertex 3 {m}
    10, 0, 3;                               !- X,Y,Z Vertex 4 {m}

  BuildingSurface:Detailed,
    Roof1,                                  !- Name
    Roof,                                   !- Surface Type
    ,                                       !- Construction Name
    TestZone,                               !- Zone Name
    ,                                       !- Space Name
    Outdoors,                               !- Outside Boundary Condition
    ,                                       !- Outside Boundary Condition Object
    SunExposed,                             !- Sun Exposure
    WindExposed,                            !- Wind Exposure
    ,                                       !- View Factor to Ground
    ,                                       !- Number of Vertices
    0, 10, 3,                               !- X,Y,Z Vertex 1 {m}
    0, 0, 3,                                !- X,Y,Z Vertex 2 {m}
    10, 0, 3,                               !- X,Y,Z Vertex 3 {m}
    10, 10, 3;                              !- X,Y,Z Vertex 4 {m}

  BuildingSurface:Detailed,
    Floor1,                                 !- Name
    Floor,                                  !- Surface Type
    ,                                       !- Construction Name
    TestZone,                               !- Zone Name
    ,                                       !- Space Name
    Outdoors,                               !- Outside Boundary Condition
    ,                                       !- Outside Boundary Condition Object
    NoSun,                                  !- Sun Exposure
    NoWind,                                 !- Wind Exposure
    ,                                       !- View Factor to Ground
    ,                                       !- Number of Vertices
    10, 10, 0,                              !- X,Y,Z Vertex 1 {m}
    10, 0, 0,                               !- X,Y,Z Vertex 2 {m}
    0, 0, 0,                                !- X,Y,Z Vertex 3 {m}
    0, 10, 0;                               !- X,Y,Z Vertex 4 {m}
)";

    ASSERT_TRUE(process_idf(idf_objects));
    state->init_state(*state);
    loadConstructions(*state);

    bool ErrorsFound = false;
    // Reads the Building object, including its Construction Assignment Set Name (A4), which
    // ConstructionAssignments::GetConstructionAssignmentSetData resolves against the parsed DCS below.
    HeatBalanceManager::GetProjectControlData(*state, ErrorsFound);
    ASSERT_FALSE(ErrorsFound);

    ConstructionAssignments::GetConstructionAssignmentSetData(*state, ErrorsFound);
    ASSERT_FALSE(ErrorsFound);

    HeatBalanceManager::GetZoneData(*state, ErrorsFound);
    ASSERT_FALSE(ErrorsFound);

    state->dataSurfaceGeometry->CosZoneRelNorth.allocate(1);
    state->dataSurfaceGeometry->SinZoneRelNorth.allocate(1);
    state->dataSurfaceGeometry->CosZoneRelNorth(1) = 1.0;
    state->dataSurfaceGeometry->SinZoneRelNorth(1) = 0.0;
    state->dataSurfaceGeometry->CosBldgRelNorth = 1.0;
    state->dataSurfaceGeometry->SinBldgRelNorth = 0.0;

    EXPECT_NO_THROW(SurfaceGeometry::GetSurfaceData(*state, ErrorsFound));
    compare_err_stream("");
    ASSERT_FALSE(ErrorsFound);

    int wallNum = Util::FindItemInList("WALL1", state->dataSurface->Surface);
    int roofNum = Util::FindItemInList("ROOF1", state->dataSurface->Surface);
    int floorNum = Util::FindItemInList("FLOOR1", state->dataSurface->Surface);
    ASSERT_GT(wallNum, 0);
    ASSERT_GT(roofNum, 0);
    ASSERT_GT(floorNum, 0);

    auto const &wall = state->dataSurface->Surface(wallNum);
    auto const &roof = state->dataSurface->Surface(roofNum);
    auto const &floor = state->dataSurface->Surface(floorNum);

    ASSERT_GT(wall.Construction, 0);
    ASSERT_GT(roof.Construction, 0);
    ASSERT_GT(floor.Construction, 0);

    EXPECT_EQ("EXTERIOR WALL CONSTRUCTION", state->dataConstruction->Construct(wall.Construction).Name);
    EXPECT_EQ("EXTERIOR ROOF CONSTRUCTION", state->dataConstruction->Construct(roof.Construction).Name);
    EXPECT_EQ("EXTERIOR FLOOR CONSTRUCTION", state->dataConstruction->Construct(floor.Construction).Name);

    EXPECT_EQ(static_cast<int>(ConstructionAssignments::SearchDistanceType::Building), wall.ConstructionAssignmentSource);
    EXPECT_EQ(static_cast<int>(ConstructionAssignments::SearchDistanceType::Building), roof.ConstructionAssignmentSource);
    EXPECT_EQ(static_cast<int>(ConstructionAssignments::SearchDistanceType::Building), floor.ConstructionAssignmentSource);
}

TEST_F(EnergyPlusFixture, ConstructionResolution_SpaceOverridesBuildingWithFallback)
{
    // A Space-level ConstructionAssignmentSet that only fills in the Exterior Wall slot overrides
    // the Building-level set for that slot, while its unset Roof slot falls through to the
    // Building-level set - mirroring the "Space3" pattern in the NFP's own example file.
    std::string const idf_objects = std::string(idf_dcs_all_types) + R"(
  SurfaceConstructionAssignments,
    Space3 Exterior Surface Constructions,  !- Name
    ,                                       !- Floor Construction Name
    Exterior Wall Construction - Space3;    !- Wall Construction Name

  ConstructionAssignmentSet,
    Space3 Construction Set,                !- Name
    Space3 Exterior Surface Constructions;  !- Exterior Surface Construction Assignments Name

  Building,
    Building1,                              !- Name
    ,                                       !- North Axis {deg}
    ,                                       !- Terrain
    ,                                       !- Loads Convergence Tolerance Value {W}
    ,                                       !- Temperature Convergence Tolerance Value {deltaC}
    ,                                       !- Solar Distribution
    ,                                       !- Maximum Number of Warmup Days
    ,                                       !- Minimum Number of Warmup Days
    Default Construction Set;               !- Construction Assignment Set Name

  Zone,
    TestZone,                               !- Name
    ,                                       !- Direction of Relative North {deg}
    0,                                      !- X Origin {m}
    0,                                      !- Y Origin {m}
    0,                                      !- Z Origin {m}
    ,                                       !- Type
    1,                                      !- Multiplier
    ,                                       !- Ceiling Height {m}
    ,                                       !- Volume {m3}
    ,                                       !- Floor Area {m2}
    ,                                       !- Zone Inside Convection Algorithm
    ,                                       !- Zone Outside Convection Algorithm
    Yes;                                    !- Part of Total Floor Area

  Space,
    Space3,                                 !- Name
    TestZone,                               !- Zone Name
    ,                                       !- Ceiling Height {m}
    ,                                       !- Volume {m3}
    ,                                       !- Floor Area {m2}
    ,                                       !- Space Type
    Space3 Construction Set;                !- Construction Assignment Set Name

  BuildingSurface:Detailed,
    Space3 Wall,                            !- Name
    Wall,                                   !- Surface Type
    ,                                       !- Construction Name
    TestZone,                               !- Zone Name
    Space3,                                 !- Space Name
    Outdoors,                               !- Outside Boundary Condition
    ,                                       !- Outside Boundary Condition Object
    SunExposed,                             !- Sun Exposure
    WindExposed,                            !- Wind Exposure
    ,                                       !- View Factor to Ground
    ,                                       !- Number of Vertices
    0, 0, 3,                                !- X,Y,Z Vertex 1 {m}
    0, 0, 0,                                !- X,Y,Z Vertex 2 {m}
    10, 0, 0,                               !- X,Y,Z Vertex 3 {m}
    10, 0, 3;                               !- X,Y,Z Vertex 4 {m}

  BuildingSurface:Detailed,
    Space3 Roof,                            !- Name
    Roof,                                   !- Surface Type
    ,                                       !- Construction Name
    TestZone,                               !- Zone Name
    Space3,                                 !- Space Name
    Outdoors,                               !- Outside Boundary Condition
    ,                                       !- Outside Boundary Condition Object
    SunExposed,                             !- Sun Exposure
    WindExposed,                            !- Wind Exposure
    ,                                       !- View Factor to Ground
    ,                                       !- Number of Vertices
    0, 10, 3,                               !- X,Y,Z Vertex 1 {m}
    0, 0, 3,                                !- X,Y,Z Vertex 2 {m}
    10, 0, 3,                               !- X,Y,Z Vertex 3 {m}
    10, 10, 3;                              !- X,Y,Z Vertex 4 {m}
)";

    ASSERT_TRUE(process_idf(idf_objects));
    state->init_state(*state);
    loadConstructions(*state);

    bool ErrorsFound = false;
    HeatBalanceManager::GetProjectControlData(*state, ErrorsFound);
    ASSERT_FALSE(ErrorsFound);

    ConstructionAssignments::GetConstructionAssignmentSetData(*state, ErrorsFound);
    ASSERT_FALSE(ErrorsFound);

    HeatBalanceManager::GetZoneData(*state, ErrorsFound);
    ASSERT_FALSE(ErrorsFound);

    state->dataSurfaceGeometry->CosZoneRelNorth.allocate(1);
    state->dataSurfaceGeometry->SinZoneRelNorth.allocate(1);
    state->dataSurfaceGeometry->CosZoneRelNorth(1) = 1.0;
    state->dataSurfaceGeometry->SinZoneRelNorth(1) = 0.0;
    state->dataSurfaceGeometry->CosBldgRelNorth = 1.0;
    state->dataSurfaceGeometry->SinBldgRelNorth = 0.0;

    EXPECT_NO_THROW(SurfaceGeometry::GetSurfaceData(*state, ErrorsFound));
    compare_err_stream("");
    ASSERT_FALSE(ErrorsFound);

    int wallNum = Util::FindItemInList("SPACE3 WALL", state->dataSurface->Surface);
    int roofNum = Util::FindItemInList("SPACE3 ROOF", state->dataSurface->Surface);
    ASSERT_GT(wallNum, 0);
    ASSERT_GT(roofNum, 0);

    auto const &wall = state->dataSurface->Surface(wallNum);
    auto const &roof = state->dataSurface->Surface(roofNum);

    ASSERT_GT(wall.Construction, 0);
    ASSERT_GT(roof.Construction, 0);

    // Space-level set has a Wall entry: it wins over the Building-level "Exterior Wall Construction".
    EXPECT_EQ("EXTERIOR WALL CONSTRUCTION - SPACE3", state->dataConstruction->Construct(wall.Construction).Name);
    EXPECT_EQ(static_cast<int>(ConstructionAssignments::SearchDistanceType::Space), wall.ConstructionAssignmentSource);

    // Space-level set has no Roof entry: falls through to the Building-level "Exterior Roof Construction".
    EXPECT_EQ("EXTERIOR ROOF CONSTRUCTION", state->dataConstruction->Construct(roof.Construction).Name);
    EXPECT_EQ(static_cast<int>(ConstructionAssignments::SearchDistanceType::Building), roof.ConstructionAssignmentSource);
}

TEST_F(EnergyPlusFixture, ConstructionResolution_AdiabaticAndInteriorPartition)
{
    // Adiabatic surfaces and InternalMass objects don't go through the Wall/Floor/Roof
    // SurfaceConstructionAssignments dispatch - they resolve directly to the DCS's
    // Adiabatic Surface Construction Name / Interior Partition Construction Name fields.
    std::string const idf_objects = std::string(idf_dcs_all_types) + R"(
  Building,
    Building1,                              !- Name
    ,                                       !- North Axis {deg}
    ,                                       !- Terrain
    ,                                       !- Loads Convergence Tolerance Value {W}
    ,                                       !- Temperature Convergence Tolerance Value {deltaC}
    ,                                       !- Solar Distribution
    ,                                       !- Maximum Number of Warmup Days
    ,                                       !- Minimum Number of Warmup Days
    Default Construction Set;               !- Construction Assignment Set Name

  Zone,
    TestZone,                               !- Name
    ,                                       !- Direction of Relative North {deg}
    0,                                      !- X Origin {m}
    0,                                      !- Y Origin {m}
    0,                                      !- Z Origin {m}
    ,                                       !- Type
    1,                                      !- Multiplier
    ,                                       !- Ceiling Height {m}
    ,                                       !- Volume {m3}
    ,                                       !- Floor Area {m2}
    ,                                       !- Zone Inside Convection Algorithm
    ,                                       !- Zone Outside Convection Algorithm
    Yes;                                    !- Part of Total Floor Area

  BuildingSurface:Detailed,
    AdiabaticWall,                          !- Name
    Wall,                                   !- Surface Type
    ,                                       !- Construction Name
    TestZone,                               !- Zone Name
    ,                                       !- Space Name
    Adiabatic,                              !- Outside Boundary Condition
    ,                                       !- Outside Boundary Condition Object
    NoSun,                                  !- Sun Exposure
    NoWind,                                 !- Wind Exposure
    ,                                       !- View Factor to Ground
    ,                                       !- Number of Vertices
    0, 0, 3,                                !- X,Y,Z Vertex 1 {m}
    0, 0, 0,                                !- X,Y,Z Vertex 2 {m}
    10, 0, 0,                               !- X,Y,Z Vertex 3 {m}
    10, 0, 3;                               !- X,Y,Z Vertex 4 {m}

  InternalMass,
    IntMass1,                               !- Name
    ,                                       !- Construction Name
    TestZone,                               !- Zone or ZoneList Name
    ,                                       !- Space or SpaceList Name
    10;                                     !- Surface Area {m2}
)";

    ASSERT_TRUE(process_idf(idf_objects));
    state->init_state(*state);
    loadConstructions(*state);

    bool ErrorsFound = false;
    HeatBalanceManager::GetProjectControlData(*state, ErrorsFound);
    ASSERT_FALSE(ErrorsFound);

    ConstructionAssignments::GetConstructionAssignmentSetData(*state, ErrorsFound);
    ASSERT_FALSE(ErrorsFound);

    HeatBalanceManager::GetZoneData(*state, ErrorsFound);
    ASSERT_FALSE(ErrorsFound);

    state->dataSurfaceGeometry->CosZoneRelNorth.allocate(1);
    state->dataSurfaceGeometry->SinZoneRelNorth.allocate(1);
    state->dataSurfaceGeometry->CosZoneRelNorth(1) = 1.0;
    state->dataSurfaceGeometry->SinZoneRelNorth(1) = 0.0;
    state->dataSurfaceGeometry->CosBldgRelNorth = 1.0;
    state->dataSurfaceGeometry->SinBldgRelNorth = 0.0;

    EXPECT_NO_THROW(SurfaceGeometry::GetSurfaceData(*state, ErrorsFound));
    compare_err_stream("");
    ASSERT_FALSE(ErrorsFound);

    int adiabaticNum = Util::FindItemInList("ADIABATICWALL", state->dataSurface->Surface);
    int intMassNum = Util::FindItemInList("INTMASS1", state->dataSurface->Surface);
    ASSERT_GT(adiabaticNum, 0);
    ASSERT_GT(intMassNum, 0);

    auto const &adiabaticWall = state->dataSurface->Surface(adiabaticNum);
    auto const &intMass = state->dataSurface->Surface(intMassNum);

    ASSERT_GT(adiabaticWall.Construction, 0);
    ASSERT_GT(intMass.Construction, 0);

    EXPECT_EQ("ADIABATIC SURFACE CONSTRUCTION", state->dataConstruction->Construct(adiabaticWall.Construction).Name);
    EXPECT_EQ(static_cast<int>(ConstructionAssignments::SearchDistanceType::Building), adiabaticWall.ConstructionAssignmentSource);

    EXPECT_EQ("INTERIOR PARTITION CONSTRUCTION", state->dataConstruction->Construct(intMass.Construction).Name);
    EXPECT_EQ(static_cast<int>(ConstructionAssignments::SearchDistanceType::Building), intMass.ConstructionAssignmentSource);
}

TEST_F(EnergyPlusFixture, ConstructionResolution_ExplicitUnaffectedByConstructionAssignmentSet)
{
    // A surface with an explicit (non-blank) Construction Name is never routed through
    // ConstructionAssignmentSet resolution, even when a Building-level DCS exists that would
    // have picked a different construction for that surface type/BC.
    std::string const idf_objects = std::string(idf_dcs_all_types) + R"(
  Building,
    Building1,                              !- Name
    ,                                       !- North Axis {deg}
    ,                                       !- Terrain
    ,                                       !- Loads Convergence Tolerance Value {W}
    ,                                       !- Temperature Convergence Tolerance Value {deltaC}
    ,                                       !- Solar Distribution
    ,                                       !- Maximum Number of Warmup Days
    ,                                       !- Minimum Number of Warmup Days
    Default Construction Set;               !- Construction Assignment Set Name

  Zone,
    TestZone,                               !- Name
    ,                                       !- Direction of Relative North {deg}
    0,                                      !- X Origin {m}
    0,                                      !- Y Origin {m}
    0,                                      !- Z Origin {m}
    ,                                       !- Type
    1,                                      !- Multiplier
    ,                                       !- Ceiling Height {m}
    ,                                       !- Volume {m3}
    ,                                       !- Floor Area {m2}
    ,                                       !- Zone Inside Convection Algorithm
    ,                                       !- Zone Outside Convection Algorithm
    Yes;                                    !- Part of Total Floor Area

  BuildingSurface:Detailed,
    Wall1,                                  !- Name
    Wall,                                   !- Surface Type
    Interior Wall Construction,             !- Construction Name
    TestZone,                               !- Zone Name
    ,                                       !- Space Name
    Outdoors,                               !- Outside Boundary Condition
    ,                                       !- Outside Boundary Condition Object
    SunExposed,                             !- Sun Exposure
    WindExposed,                            !- Wind Exposure
    ,                                       !- View Factor to Ground
    ,                                       !- Number of Vertices
    0, 0, 3,                                !- X,Y,Z Vertex 1 {m}
    0, 0, 0,                                !- X,Y,Z Vertex 2 {m}
    10, 0, 0,                               !- X,Y,Z Vertex 3 {m}
    10, 0, 3;                               !- X,Y,Z Vertex 4 {m}
)";

    ASSERT_TRUE(process_idf(idf_objects));
    state->init_state(*state);
    loadConstructions(*state);

    bool ErrorsFound = false;
    HeatBalanceManager::GetProjectControlData(*state, ErrorsFound);
    ASSERT_FALSE(ErrorsFound);

    ConstructionAssignments::GetConstructionAssignmentSetData(*state, ErrorsFound);
    ASSERT_FALSE(ErrorsFound);

    HeatBalanceManager::GetZoneData(*state, ErrorsFound);
    ASSERT_FALSE(ErrorsFound);

    state->dataSurfaceGeometry->CosZoneRelNorth.allocate(1);
    state->dataSurfaceGeometry->SinZoneRelNorth.allocate(1);
    state->dataSurfaceGeometry->CosZoneRelNorth(1) = 1.0;
    state->dataSurfaceGeometry->SinZoneRelNorth(1) = 0.0;
    state->dataSurfaceGeometry->CosBldgRelNorth = 1.0;
    state->dataSurfaceGeometry->SinBldgRelNorth = 0.0;

    EXPECT_NO_THROW(SurfaceGeometry::GetSurfaceData(*state, ErrorsFound));
    compare_err_stream("");
    ASSERT_FALSE(ErrorsFound);

    int wallNum = Util::FindItemInList("WALL1", state->dataSurface->Surface);
    ASSERT_GT(wallNum, 0);

    auto const &wall = state->dataSurface->Surface(wallNum);
    ASSERT_GT(wall.Construction, 0);

    // Kept its own explicit construction, not the DCS's "Exterior Wall Construction" for this
    // Wall/Outdoors combination.
    EXPECT_EQ("INTERIOR WALL CONSTRUCTION", state->dataConstruction->Construct(wall.Construction).Name);
    EXPECT_EQ(static_cast<int>(ConstructionAssignments::SearchDistanceType::HardAssigned), wall.ConstructionAssignmentSource);
}

TEST_F(EnergyPlusFixture, ConstructionResolution_ExplicitPrecedenceOverInheritedInPair)
{
    // Two sides of an interzone wall: one hardcoded, one blank. Per the NFP, an explicit
    // assignment takes precedence over an inherited one for the pair, and EnergyPlus finds or
    // creates the reversed construction for the inherited side - it must NOT just copy the
    // hardcoded side's construction number verbatim (that would leave both sides with the same
    // outside-to-inside layer order, which is physically wrong for an asymmetric construction).
    //
    // WallA is explicit ("Exterior Wall Construction" = C5 concrete outside, R13LAYER inside).
    // WallB is blank; the Building-level DCS would give it "Interior Wall Construction" (a
    // different, gypsum-based construction) if inheritance won - it must not, since WallA's
    // explicit assignment has higher precedence.
    std::string const idf_objects = std::string(idf_dcs_all_types) + R"(
  Building,
    Building1,                              !- Name
    ,                                       !- North Axis {deg}
    ,                                       !- Terrain
    ,                                       !- Loads Convergence Tolerance Value {W}
    ,                                       !- Temperature Convergence Tolerance Value {deltaC}
    ,                                       !- Solar Distribution
    ,                                       !- Maximum Number of Warmup Days
    ,                                       !- Minimum Number of Warmup Days
    Default Construction Set;               !- Construction Assignment Set Name

  Zone,
    Zone1,                                  !- Name
    ,                                       !- Direction of Relative North {deg}
    0,                                      !- X Origin {m}
    0,                                      !- Y Origin {m}
    0,                                      !- Z Origin {m}
    ,                                       !- Type
    1,                                      !- Multiplier
    ,                                       !- Ceiling Height {m}
    ,                                       !- Volume {m3}
    ,                                       !- Floor Area {m2}
    ,                                       !- Zone Inside Convection Algorithm
    ,                                       !- Zone Outside Convection Algorithm
    Yes;                                    !- Part of Total Floor Area

  Zone,
    Zone2,                                  !- Name
    ,                                       !- Direction of Relative North {deg}
    10,                                     !- X Origin {m}
    0,                                      !- Y Origin {m}
    0,                                      !- Z Origin {m}
    ,                                       !- Type
    1,                                      !- Multiplier
    ,                                       !- Ceiling Height {m}
    ,                                       !- Volume {m3}
    ,                                       !- Floor Area {m2}
    ,                                       !- Zone Inside Convection Algorithm
    ,                                       !- Zone Outside Convection Algorithm
    Yes;                                    !- Part of Total Floor Area

  BuildingSurface:Detailed,
    WallA,                                  !- Name
    Wall,                                   !- Surface Type
    Exterior Wall Construction,             !- Construction Name
    Zone1,                                  !- Zone Name
    ,                                       !- Space Name
    Surface,                                !- Outside Boundary Condition
    WallB,                                  !- Outside Boundary Condition Object
    NoSun,                                  !- Sun Exposure
    NoWind,                                 !- Wind Exposure
    ,                                       !- View Factor to Ground
    ,                                       !- Number of Vertices
    10, 0, 3,                               !- X,Y,Z Vertex 1 {m}
    10, 10, 3,                              !- X,Y,Z Vertex 2 {m}
    10, 10, 0,                              !- X,Y,Z Vertex 3 {m}
    10, 0, 0;                               !- X,Y,Z Vertex 4 {m}

  BuildingSurface:Detailed,
    WallB,                                  !- Name
    Wall,                                   !- Surface Type
    ,                                       !- Construction Name
    Zone2,                                  !- Zone Name
    ,                                       !- Space Name
    Surface,                                !- Outside Boundary Condition
    WallA,                                  !- Outside Boundary Condition Object
    NoSun,                                  !- Sun Exposure
    NoWind,                                 !- Wind Exposure
    ,                                       !- View Factor to Ground
    ,                                       !- Number of Vertices
    10, 10, 3,                              !- X,Y,Z Vertex 1 {m}
    10, 0, 3,                               !- X,Y,Z Vertex 2 {m}
    10, 0, 0,                               !- X,Y,Z Vertex 3 {m}
    10, 10, 0;                              !- X,Y,Z Vertex 4 {m}
)";

    ASSERT_TRUE(process_idf(idf_objects));
    state->init_state(*state);
    loadConstructions(*state);

    bool ErrorsFound = false;
    HeatBalanceManager::GetProjectControlData(*state, ErrorsFound);
    ASSERT_FALSE(ErrorsFound);

    ConstructionAssignments::GetConstructionAssignmentSetData(*state, ErrorsFound);
    ASSERT_FALSE(ErrorsFound);

    HeatBalanceManager::GetZoneData(*state, ErrorsFound);
    ASSERT_FALSE(ErrorsFound);

    state->dataSurfaceGeometry->CosZoneRelNorth.allocate(2);
    state->dataSurfaceGeometry->SinZoneRelNorth.allocate(2);
    state->dataSurfaceGeometry->CosZoneRelNorth(1) = 1.0;
    state->dataSurfaceGeometry->CosZoneRelNorth(2) = 1.0;
    state->dataSurfaceGeometry->SinZoneRelNorth(1) = 0.0;
    state->dataSurfaceGeometry->SinZoneRelNorth(2) = 0.0;
    state->dataSurfaceGeometry->CosBldgRelNorth = 1.0;
    state->dataSurfaceGeometry->SinBldgRelNorth = 0.0;

    EXPECT_NO_THROW(SurfaceGeometry::GetSurfaceData(*state, ErrorsFound));
    compare_err_stream("");
    ASSERT_FALSE(ErrorsFound);

    int wallANum = Util::FindItemInList("WALLA", state->dataSurface->Surface);
    int wallBNum = Util::FindItemInList("WALLB", state->dataSurface->Surface);
    ASSERT_GT(wallANum, 0);
    ASSERT_GT(wallBNum, 0);

    auto const &wallA = state->dataSurface->Surface(wallANum);
    auto const &wallB = state->dataSurface->Surface(wallBNum);

    ASSERT_GT(wallA.Construction, 0);
    ASSERT_GT(wallB.Construction, 0);

    // WallA is untouched: still its own explicit construction.
    EXPECT_EQ("EXTERIOR WALL CONSTRUCTION", state->dataConstruction->Construct(wallA.Construction).Name);
    EXPECT_EQ(static_cast<int>(ConstructionAssignments::SearchDistanceType::HardAssigned), wallA.ConstructionAssignmentSource);

    // WallB must NOT have inherited "Interior Wall Construction" from the Building-level DCS -
    // WallA's explicit assignment has higher precedence and governs the pair.
    EXPECT_NE("INTERIOR WALL CONSTRUCTION", state->dataConstruction->Construct(wallB.Construction).Name);

    // WallB's construction must have the reverse layer order of WallA's "Exterior Wall
    // Construction" (Outside Layer=C5 concrete, Layer 2=R13LAYER) - not the identical,
    // unreversed construction number.
    auto const &constrA = state->dataConstruction->Construct(wallA.Construction);
    auto const &constrB = state->dataConstruction->Construct(wallB.Construction);
    ASSERT_EQ(2, constrA.TotLayers);
    ASSERT_EQ(2, constrB.TotLayers);
    EXPECT_EQ(state->dataMaterial->materials(constrA.LayerPoint(1))->Name, state->dataMaterial->materials(constrB.LayerPoint(2))->Name);
    EXPECT_EQ(state->dataMaterial->materials(constrA.LayerPoint(2))->Name, state->dataMaterial->materials(constrB.LayerPoint(1))->Name);
    EXPECT_NE(wallA.Construction, wallB.Construction) << "WallB must not share WallA's construction number unreversed";
}

TEST_F(EnergyPlusFixture, ConstructionResolution_ExplicitPrecedenceOverInheritedInPair_FloorRoof)
{
    // Unlike the Wall/Wall case, idf_dcs_all_types's "Interior Floor Construction" and "Interior Roof Construction"
    // are ALREADY defined as reverses of each other.
    // Here the RoofCeiling side is hardcoded, forcing the Floor side through the "defer to adjacent, reverse it" path
    // even though its own inherited answer "Interior Floor Construction" would already have been correct (it's reverse equal)
    //
    //
    // AssignReverseConstructionNumber searches for an existing construction with the matching reversed layer order before creating a new one,
    // so the reversed result will land on a construction that is reverse equal and existing (not a newly generated "iz-..." duplicate).
    // But it picks the FIRST construction that satisfies the constraint... In this case it happens to be the "Interior Floor Construction"
    std::string const idf_objects = std::string(idf_dcs_all_types) + R"(
  Building,
    Building1,                              !- Name
    ,                                       !- North Axis {deg}
    ,                                       !- Terrain
    ,                                       !- Loads Convergence Tolerance Value {W}
    ,                                       !- Temperature Convergence Tolerance Value {deltaC}
    ,                                       !- Solar Distribution
    ,                                       !- Maximum Number of Warmup Days
    ,                                       !- Minimum Number of Warmup Days
    Default Construction Set;               !- Construction Assignment Set Name

  Zone,
    Zone1,                                  !- Name
    ,                                       !- Direction of Relative North {deg}
    0,                                      !- X Origin {m}
    0,                                      !- Y Origin {m}
    0,                                      !- Z Origin {m}
    ,                                       !- Type
    1,                                      !- Multiplier
    ,                                       !- Ceiling Height {m}
    ,                                       !- Volume {m3}
    ,                                       !- Floor Area {m2}
    ,                                       !- Zone Inside Convection Algorithm
    ,                                       !- Zone Outside Convection Algorithm
    Yes;                                    !- Part of Total Floor Area

  Zone,
    Zone2,                                  !- Name
    ,                                       !- Direction of Relative North {deg}
    0,                                      !- X Origin {m}
    0,                                      !- Y Origin {m}
    3,                                      !- Z Origin {m}
    ,                                       !- Type
    1,                                      !- Multiplier
    ,                                       !- Ceiling Height {m}
    ,                                       !- Volume {m3}
    ,                                       !- Floor Area {m2}
    ,                                       !- Zone Inside Convection Algorithm
    ,                                       !- Zone Outside Convection Algorithm
    Yes;                                    !- Part of Total Floor Area

  BuildingSurface:Detailed,
    Zone1RoofCeiling,                       !- Name
    Roof,                                   !- Surface Type
    Interior Roof Construction,             !- Construction Name
    Zone1,                                  !- Zone Name
    ,                                       !- Space Name
    Surface,                                !- Outside Boundary Condition
    Zone2Floor,                             !- Outside Boundary Condition Object
    NoSun,                                  !- Sun Exposure
    NoWind,                                 !- Wind Exposure
    ,                                       !- View Factor to Ground
    ,                                       !- Number of Vertices
    0, 10, 3,                               !- X,Y,Z Vertex 1 {m}
    0, 0, 3,                                !- X,Y,Z Vertex 2 {m}
    10, 0, 3,                               !- X,Y,Z Vertex 3 {m}
    10, 10, 3;                              !- X,Y,Z Vertex 4 {m}

  BuildingSurface:Detailed,
    Zone2Floor,                             !- Name
    Floor,                                  !- Surface Type
    ,                                       !- Construction Name
    Zone2,                                  !- Zone Name
    ,                                       !- Space Name
    Surface,                                !- Outside Boundary Condition
    Zone1RoofCeiling,                       !- Outside Boundary Condition Object
    NoSun,                                  !- Sun Exposure
    NoWind,                                 !- Wind Exposure
    ,                                       !- View Factor to Ground
    ,                                       !- Number of Vertices
    10, 10, 0,                              !- X,Y,Z Vertex 1 {m}
    10, 0, 0,                               !- X,Y,Z Vertex 2 {m}
    0, 0, 0,                                !- X,Y,Z Vertex 3 {m}
    0, 10, 0;                               !- X,Y,Z Vertex 4 {m}
)";

    ASSERT_TRUE(process_idf(idf_objects));
    state->init_state(*state);
    loadConstructions(*state);

    bool ErrorsFound = false;
    HeatBalanceManager::GetProjectControlData(*state, ErrorsFound);
    ASSERT_FALSE(ErrorsFound);

    ConstructionAssignments::GetConstructionAssignmentSetData(*state, ErrorsFound);
    ASSERT_FALSE(ErrorsFound);

    HeatBalanceManager::GetZoneData(*state, ErrorsFound);
    ASSERT_FALSE(ErrorsFound);

    state->dataSurfaceGeometry->CosZoneRelNorth.allocate(2);
    state->dataSurfaceGeometry->SinZoneRelNorth.allocate(2);
    state->dataSurfaceGeometry->CosZoneRelNorth(1) = 1.0;
    state->dataSurfaceGeometry->CosZoneRelNorth(2) = 1.0;
    state->dataSurfaceGeometry->SinZoneRelNorth(1) = 0.0;
    state->dataSurfaceGeometry->SinZoneRelNorth(2) = 0.0;
    state->dataSurfaceGeometry->CosBldgRelNorth = 1.0;
    state->dataSurfaceGeometry->SinBldgRelNorth = 0.0;

    int const totConstructsBefore = state->dataHeatBal->TotConstructs;

    EXPECT_NO_THROW(SurfaceGeometry::GetSurfaceData(*state, ErrorsFound));
    compare_err_stream("");
    ASSERT_FALSE(ErrorsFound);

    int roofNum = Util::FindItemInList("ZONE1ROOFCEILING", state->dataSurface->Surface);
    int floorNum = Util::FindItemInList("ZONE2FLOOR", state->dataSurface->Surface);
    ASSERT_GT(roofNum, 0);
    ASSERT_GT(floorNum, 0);

    auto const &roof = state->dataSurface->Surface(roofNum);
    auto const &floor = state->dataSurface->Surface(floorNum);

    ASSERT_GT(roof.Construction, 0);
    ASSERT_GT(floor.Construction, 0);

    // RoofCeiling is untouched: still its own explicit construction.
    EXPECT_EQ("INTERIOR ROOF CONSTRUCTION", state->dataConstruction->Construct(roof.Construction).Name);
    EXPECT_EQ(static_cast<int>(ConstructionAssignments::SearchDistanceType::HardAssigned), roof.ConstructionAssignmentSource);

    // The Floor side's reversed-of-the-winner construction should land on the pre-existing
    // "Interior Floor Construction" (an exact layer-order match), not a newly generated one.
    EXPECT_EQ("INTERIOR FLOOR CONSTRUCTION", state->dataConstruction->Construct(floor.Construction).Name);
    EXPECT_EQ(totConstructsBefore, state->dataHeatBal->TotConstructs) << "No new construction should have been created; the reverse of \"Interior "
                                                                         "Roof Construction\" already exists as \"Interior Floor Construction\"";
}

TEST_F(EnergyPlusFixture, ConstructionResolution_ExplicitPrecedenceOverInheritedInPair_RoofFloor)
{
    // Unlike the Wall/Wall case, idf_dcs_all_types's "Interior Floor Construction" and "Interior Roof Construction"
    // are ALREADY defined as reverses of each other.
    // Here the Floor side is hardcoded, forcing the RoofCeiling side through the "defer to adjacent, reverse it" path
    // even though its own inherited answer "Interior Roof Construction" would already have been correct (it's reverse equal)
    //
    //
    // AssignReverseConstructionNumber searches for an existing construction with the matching reversed layer order before creating a new one,
    // so the reversed result will land on a construction that is reverse equal and existing (not a newly generated "iz-..." duplicate).
    // But it picks the FIRST construction that satisfies the constraint... In this case it happens to NOT be the "Interior Roof Construction"
    std::string const idf_objects = std::string(idf_dcs_all_types) + R"(
  Building,
    Building1,                              !- Name
    ,                                       !- North Axis {deg}
    ,                                       !- Terrain
    ,                                       !- Loads Convergence Tolerance Value {W}
    ,                                       !- Temperature Convergence Tolerance Value {deltaC}
    ,                                       !- Solar Distribution
    ,                                       !- Maximum Number of Warmup Days
    ,                                       !- Minimum Number of Warmup Days
    Default Construction Set;               !- Construction Assignment Set Name

  Zone,
    Zone1,                                  !- Name
    ,                                       !- Direction of Relative North {deg}
    0,                                      !- X Origin {m}
    0,                                      !- Y Origin {m}
    0,                                      !- Z Origin {m}
    ,                                       !- Type
    1,                                      !- Multiplier
    ,                                       !- Ceiling Height {m}
    ,                                       !- Volume {m3}
    ,                                       !- Floor Area {m2}
    ,                                       !- Zone Inside Convection Algorithm
    ,                                       !- Zone Outside Convection Algorithm
    Yes;                                    !- Part of Total Floor Area

  Zone,
    Zone2,                                  !- Name
    ,                                       !- Direction of Relative North {deg}
    0,                                      !- X Origin {m}
    0,                                      !- Y Origin {m}
    3,                                      !- Z Origin {m}
    ,                                       !- Type
    1,                                      !- Multiplier
    ,                                       !- Ceiling Height {m}
    ,                                       !- Volume {m3}
    ,                                       !- Floor Area {m2}
    ,                                       !- Zone Inside Convection Algorithm
    ,                                       !- Zone Outside Convection Algorithm
    Yes;                                    !- Part of Total Floor Area

  BuildingSurface:Detailed,
    Zone1RoofCeiling,                       !- Name
    Roof,                                   !- Surface Type
    ,                                       !- Construction Name
    Zone1,                                  !- Zone Name
    ,                                       !- Space Name
    Surface,                                !- Outside Boundary Condition
    Zone2Floor,                             !- Outside Boundary Condition Object
    NoSun,                                  !- Sun Exposure
    NoWind,                                 !- Wind Exposure
    ,                                       !- View Factor to Ground
    ,                                       !- Number of Vertices
    0, 10, 3,                               !- X,Y,Z Vertex 1 {m}
    0, 0, 3,                                !- X,Y,Z Vertex 2 {m}
    10, 0, 3,                               !- X,Y,Z Vertex 3 {m}
    10, 10, 3;                              !- X,Y,Z Vertex 4 {m}

  BuildingSurface:Detailed,
    Zone2Floor,                             !- Name
    Floor,                                  !- Surface Type
    Interior Floor Construction,            !- Construction Name
    Zone2,                                  !- Zone Name
    ,                                       !- Space Name
    Surface,                                !- Outside Boundary Condition
    Zone1RoofCeiling,                       !- Outside Boundary Condition Object
    NoSun,                                  !- Sun Exposure
    NoWind,                                 !- Wind Exposure
    ,                                       !- View Factor to Ground
    ,                                       !- Number of Vertices
    10, 10, 0,                              !- X,Y,Z Vertex 1 {m}
    10, 0, 0,                               !- X,Y,Z Vertex 2 {m}
    0, 0, 0,                                !- X,Y,Z Vertex 3 {m}
    0, 10, 0;                               !- X,Y,Z Vertex 4 {m}
)";

    ASSERT_TRUE(process_idf(idf_objects));
    state->init_state(*state);
    loadConstructions(*state);

    bool ErrorsFound = false;
    HeatBalanceManager::GetProjectControlData(*state, ErrorsFound);
    ASSERT_FALSE(ErrorsFound);

    ConstructionAssignments::GetConstructionAssignmentSetData(*state, ErrorsFound);
    ASSERT_FALSE(ErrorsFound);

    HeatBalanceManager::GetZoneData(*state, ErrorsFound);
    ASSERT_FALSE(ErrorsFound);

    state->dataSurfaceGeometry->CosZoneRelNorth.allocate(2);
    state->dataSurfaceGeometry->SinZoneRelNorth.allocate(2);
    state->dataSurfaceGeometry->CosZoneRelNorth(1) = 1.0;
    state->dataSurfaceGeometry->CosZoneRelNorth(2) = 1.0;
    state->dataSurfaceGeometry->SinZoneRelNorth(1) = 0.0;
    state->dataSurfaceGeometry->SinZoneRelNorth(2) = 0.0;
    state->dataSurfaceGeometry->CosBldgRelNorth = 1.0;
    state->dataSurfaceGeometry->SinBldgRelNorth = 0.0;

    int const totConstructsBefore = state->dataHeatBal->TotConstructs;

    EXPECT_NO_THROW(SurfaceGeometry::GetSurfaceData(*state, ErrorsFound));
    compare_err_stream("");
    ASSERT_FALSE(ErrorsFound);

    int roofNum = Util::FindItemInList("ZONE1ROOFCEILING", state->dataSurface->Surface);
    int floorNum = Util::FindItemInList("ZONE2FLOOR", state->dataSurface->Surface);
    ASSERT_GT(roofNum, 0);
    ASSERT_GT(floorNum, 0);

    auto const &roof = state->dataSurface->Surface(roofNum);
    auto const &floor = state->dataSurface->Surface(floorNum);

    ASSERT_GT(roof.Construction, 0);
    ASSERT_GT(floor.Construction, 0);

    // Floor is untouched: still its own explicit construction.
    EXPECT_EQ("INTERIOR FLOOR CONSTRUCTION", state->dataConstruction->Construct(floor.Construction).Name);
    EXPECT_EQ(static_cast<int>(ConstructionAssignments::SearchDistanceType::HardAssigned), floor.ConstructionAssignmentSource);

    // The Roof side's reversed-of-the-winner construction should land on the pre-existing
    // "Interior Floor Construction" (an exact layer-order match), not a newly generated one.
    EXPECT_EQ("INTERIOR ROOF CONSTRUCTION", state->dataConstruction->Construct(roof.Construction).Name);
    EXPECT_EQ(totConstructsBefore, state->dataHeatBal->TotConstructs) << "No new construction should have been created; the reverse of \"Interior "
                                                                         "Roof Construction\" already exists as \"Interior Roof Construction\"";
}
