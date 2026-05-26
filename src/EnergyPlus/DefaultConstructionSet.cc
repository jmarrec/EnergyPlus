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

// C++ Headers
#include <algorithm>
#include <format>

// EnergyPlus Headers
#include <EnergyPlus/Construction.hh>
#include <EnergyPlus/Data/EnergyPlusData.hh>
#include <EnergyPlus/DataHeatBalance.hh>
#include <EnergyPlus/DataSurfaces.hh>
#include <EnergyPlus/DefaultConstructionSet.hh>
#include <EnergyPlus/InputProcessing/InputProcessor.hh>
#include <EnergyPlus/UtilityRoutines.hh>

namespace EnergyPlus {

namespace DefaultConstructions {

    int DefaultConstructionSetData::getDefaultConstruction(DataSurfaces::SurfaceData const &surface) const
    {
        int extBoundCond = surface.ExtBoundCond;

        // Base opaque surfaces
        if (surface.Class == DataSurfaces::SurfaceClass::Wall || surface.Class == DataSurfaces::SurfaceClass::Floor ||
            surface.Class == DataSurfaces::SurfaceClass::Roof) {

            DefaultSurfaceConstructionsData const *surfConstrs = nullptr;
            if (extBoundCond == DataSurfaces::ExternalEnvironment || extBoundCond == DataSurfaces::OtherSideCoefNoCalcExt ||
                extBoundCond == DataSurfaces::OtherSideCoefCalcExt) {
                surfConstrs = exteriorSurfConstructions.get();
            } else if (extBoundCond == DataSurfaces::Ground || extBoundCond == DataSurfaces::GroundFCfactorMethod ||
                       extBoundCond == DataSurfaces::KivaFoundation) {
                surfConstrs = groundContactSurfConstructions.get();
            } else if (surface.ExtBoundCondName == surface.Name) {
                // Adiabatic: surface points to itself.
                // ExtBoundCondName == Name is preserved through BC reconciliation.
                return adiabaticSurfConstrNum;
            } else {
                surfConstrs = interiorSurfConstructions.get();
            }

            if (surfConstrs == nullptr) {
                return 0;
            }
            if (surface.Class == DataSurfaces::SurfaceClass::Floor) {
                return surfConstrs->floorConstrNum;
            }
            if (surface.Class == DataSurfaces::SurfaceClass::Roof) {
                return surfConstrs->roofCeilingConstrNum;
            }
            return surfConstrs->wallConstrNum;
        }

        // Interior mass
        if (surface.Class == DataSurfaces::SurfaceClass::IntMass) {
            return interiorPartitionConstrNum;
        }

        // Subsurfaces — extBoundCond is inherited from base surface (see GetHTSubSurfaceData)
        DefaultSubSurfaceConstructionsData const *subSurfConstrs = nullptr;
        if (extBoundCond == DataSurfaces::ExternalEnvironment || extBoundCond == DataSurfaces::OtherSideCoefNoCalcExt ||
            extBoundCond == DataSurfaces::OtherSideCoefCalcExt) {
            subSurfConstrs = exteriorSubSurfConstructions.get();
        } else {
            subSurfConstrs = interiorSubSurfConstructions.get();
        }

        if (subSurfConstrs == nullptr) {
            return 0;
        }
        switch (surface.OriginalClass) {
        case DataSurfaces::SurfaceClass::Window:
        case DataSurfaces::SurfaceClass::FixedWindow:
            return subSurfConstrs->fixedWindowConstrNum;
        case DataSurfaces::SurfaceClass::OperableWindow:
            return subSurfConstrs->operableWindowConstrNum;
        case DataSurfaces::SurfaceClass::Skylight:
            return subSurfConstrs->skylightConstrNum;
        case DataSurfaces::SurfaceClass::Door:
            return subSurfConstrs->doorConstrNum;
        case DataSurfaces::SurfaceClass::GlassDoor:
            return subSurfConstrs->glassDoorConstrNum;
        case DataSurfaces::SurfaceClass::OverheadDoor:
            return subSurfConstrs->overheadDoorConstrNum;
        case DataSurfaces::SurfaceClass::TDD_Dome:
            return subSurfConstrs->tddDomeConstrNum;
        case DataSurfaces::SurfaceClass::TDD_Diffuser:
            return subSurfConstrs->tddDiffuserConstrNum;
        default:
            return 0;
        }
    }

    void GetDefaultConstructionSetData(EnergyPlusData &state, bool &ErrorsFound)
    {
        static constexpr std::string_view routineName = "GetDefaultConstructionSetData: ";
        auto &ip = state.dataInputProcessing->inputProcessor;
        auto &s_dc = state.dataDefaultConstructions;

        // Returns the construction index for a named construction, or 0 if blank.
        // Errors if the name is non-blank but not found.
        auto getConstrNum = [&state, &ErrorsFound, &ip](std::string_view objType,
                                                        std::string_view objName,
                                                        nlohmann::json const &fields,
                                                        nlohmann::json const &objectSchemaProps,
                                                        const char *fieldName) -> int {
            std::string constrName = Util::makeUPPER(ip->getAlphaFieldValue(fields, objectSchemaProps, fieldName));
            if (constrName.empty()) {
                return 0;
            }

            int num = Util::FindItemInList(constrName, state.dataConstruction->Construct, state.dataHeatBal->TotConstructs);
            if (num == 0) {
                ShowSevereError(state, std::format(R"({}{}="{}", invalid {}="{}" not found.)", routineName, objType, objName, fieldName, constrName));
                ErrorsFound = true;
            }
            return num;
        };

        // 1. Parse DefaultSubSurfaceConstructions
        {
            const std::string cCurrentModuleObject = "DefaultSubSurfaceConstructions";
            auto const instances = ip->epJSON.find(cCurrentModuleObject);
            if (instances != ip->epJSON.end()) {

                s_dc->defaultSubSurfaceConstructions.reserve(instances.value().size());

                auto const &objectSchemaProps = ip->getObjectSchemaProps(state, cCurrentModuleObject);
                for (auto instance = instances.value().begin(); instance != instances.value().end(); ++instance) {
                    ip->markObjectAsUsed(cCurrentModuleObject, instance.key());
                    auto const &fields = instance.value();
                    DefaultSubSurfaceConstructionsData data;
                    data.Name = Util::makeUPPER(instance.key());

                    auto lookUpConstrNum = [&](const char *fieldName) {
                        return getConstrNum(cCurrentModuleObject, data.Name, fields, objectSchemaProps, fieldName);
                    };
                    data.fixedWindowConstrNum = lookUpConstrNum("fixed_window_construction_name");
                    data.operableWindowConstrNum = lookUpConstrNum("operable_window_construction_name");
                    data.doorConstrNum = lookUpConstrNum("door_construction_name");
                    data.glassDoorConstrNum = lookUpConstrNum("glass_door_construction_name");
                    data.overheadDoorConstrNum = lookUpConstrNum("overhead_door_construction_name");
                    data.skylightConstrNum = lookUpConstrNum("skylight_construction_name");
                    data.tddDomeConstrNum = lookUpConstrNum("tubular_daylight_dome_construction_name");
                    data.tddDiffuserConstrNum = lookUpConstrNum("tubular_daylight_diffuser_construction_name");

                    s_dc->defaultSubSurfaceConstructions.push_back(std::move(data));
                }
            }
        }

        // 2. Parse DefaultSurfaceConstructions
        {
            const std::string cCurrentModuleObject = "DefaultSurfaceConstructions";
            auto const instances = ip->epJSON.find(cCurrentModuleObject);
            if (instances != ip->epJSON.end()) {

                s_dc->defaultSurfaceConstructions.reserve(instances.value().size());

                auto const &objectSchemaProps = ip->getObjectSchemaProps(state, cCurrentModuleObject);
                for (auto instance = instances.value().begin(); instance != instances.value().end(); ++instance) {
                    ip->markObjectAsUsed(cCurrentModuleObject, instance.key());
                    auto const &fields = instance.value();
                    DefaultSurfaceConstructionsData data;
                    data.Name = Util::makeUPPER(instance.key());

                    auto lookUpConstrNum = [&](const char *fieldName) {
                        return getConstrNum(cCurrentModuleObject, data.Name, fields, objectSchemaProps, fieldName);
                    };
                    data.floorConstrNum = lookUpConstrNum("floor_construction_name");
                    data.wallConstrNum = lookUpConstrNum("wall_construction_name");
                    data.roofCeilingConstrNum = lookUpConstrNum("roof_ceiling_construction_name");
                    s_dc->defaultSurfaceConstructions.push_back(std::move(data));
                }
            }
        }

        // 3. Parse DefaultConstructionSet (references the above, so must come last)
        {
            const std::string cCurrentModuleObject = "DefaultConstructionSet";
            auto const instances = ip->epJSON.find(cCurrentModuleObject);
            if (instances != ip->epJSON.end()) {

                s_dc->defaultConstructionSets.reserve(instances.value().size());

                auto const &objectSchemaProps = ip->getObjectSchemaProps(state, cCurrentModuleObject);
                for (auto instance = instances.value().begin(); instance != instances.value().end(); ++instance) {
                    ip->markObjectAsUsed(cCurrentModuleObject, instance.key());
                    auto const &fields = instance.value();
                    DefaultConstructionSetData dcs;
                    dcs.Name = Util::makeUPPER(instance.key());

                    auto getUC = [&](const char *key) { return Util::makeUPPER(ip->getAlphaFieldValue(fields, objectSchemaProps, key)); };

                    auto findSurfConstr = [&](const char *key) -> std::shared_ptr<DefaultSurfaceConstructionsData> {
                        std::string name = getUC(key);
                        if (name.empty()) {
                            return nullptr;
                        }
                        auto it = std::find_if(s_dc->defaultSurfaceConstructions.begin(),
                                               s_dc->defaultSurfaceConstructions.end(),
                                               [&name](const DefaultSurfaceConstructionsData &d) { return d.Name == name; });
                        if (it == s_dc->defaultSurfaceConstructions.end()) {
                            ShowSevereError(
                                state,
                                std::format(R"({}{}="{}", invalid {}="{}" not found.)", routineName, cCurrentModuleObject, dcs.Name, key, name));
                            ErrorsFound = true;
                            return nullptr;
                        }
                        return std::make_shared<DefaultSurfaceConstructionsData>(*it);
                    };

                    auto findSubSurfConstr = [&](const char *key) -> std::shared_ptr<DefaultSubSurfaceConstructionsData> {
                        std::string name = getUC(key);
                        if (name.empty()) {
                            return nullptr;
                        }
                        auto it = std::find_if(s_dc->defaultSubSurfaceConstructions.begin(),
                                               s_dc->defaultSubSurfaceConstructions.end(),
                                               [&name](const DefaultSubSurfaceConstructionsData &d) { return d.Name == name; });
                        if (it == s_dc->defaultSubSurfaceConstructions.end()) {
                            ShowSevereError(
                                state,
                                std::format(R"({}{}="{}", invalid {}="{}" not found.)", routineName, cCurrentModuleObject, dcs.Name, key, name));
                            ErrorsFound = true;
                            return nullptr;
                        }
                        return std::make_shared<DefaultSubSurfaceConstructionsData>(*it);
                    };

                    dcs.exteriorSurfConstructions = findSurfConstr("default_exterior_surface_constructions_name");
                    dcs.interiorSurfConstructions = findSurfConstr("default_interior_surface_constructions_name");
                    dcs.groundContactSurfConstructions = findSurfConstr("default_ground_contact_surface_constructions_name");
                    dcs.exteriorSubSurfConstructions = findSubSurfConstr("default_exterior_subsurface_constructions_name");
                    dcs.interiorSubSurfConstructions = findSubSurfConstr("default_interior_subsurface_constructions_name");

                    auto lookUpConstrNum = [&](const char *fieldName) {
                        return getConstrNum(cCurrentModuleObject, dcs.Name, fields, objectSchemaProps, fieldName);
                    };
                    dcs.interiorPartitionConstrNum = lookUpConstrNum("interior_partition_construction_name");
                    dcs.adiabaticSurfConstrNum = lookUpConstrNum("adiabatic_surface_construction_name");
                    s_dc->defaultConstructionSets.push_back(std::move(dcs));
                }
            }
        }
    }

} // namespace DefaultConstructions
} // namespace EnergyPlus
