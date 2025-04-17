#ifndef ENERGYPLUS_PCMTHERMALSTORAGE_HH
#define ENERGYPLUS_PCMTHERMALSTORAGE_HH

#include <EnergyPlus/Data/EnergyPlusData.hh>
#include <EnergyPlus/PhaseChangeModeling/HysteresisModel.hh>
#include <EnergyPlus/Plant/PlantLocation.hh>
#include <EnergyPlus/PlantComponent.hh>
#include <string>

namespace EnergyPlus {
namespace PCMStorage {

    struct PCMStorageData : public PlantComponent
    {
        // Initialization flags
        bool Initialized = false;
        bool MyPlantScanFlag = true;
        bool MyEnvrnFlag = true;

        // Inputs
        std::string Name;
        std::string AvailabilityScheduleName;
        int PCMMaterialNum = 0;                          // Index to PCM material
        Material::MaterialPhaseChange *PCMmat = nullptr; // Pointer to PCM material
        Real64 TankCapacity = 0.0;                       // kg
        Real64 HeatLossRate = 0.0;                       // W
        Real64 MeltingTemp = 0.0;                        // C (gets overwritten by PCMmat->peakTempMelting)
        Real64 FreezingTemp = 0.0;                       // C
        Real64 LatentHeat = 0.0;                         // J/kg (typically from PCMmat)
        Real64 SpecificHeat = 0.0;                       // J/kg-K (backup if needed)
        Real64 Effectiveness = 0.9;                      // HX effectiveness (optional input)

        // Plant loop connection info
        EnergyPlus::PlantLocation plantLoc;
        int PlantSideInletNode = 0;
        int PlantSideOutletNode = 0;
        int UseSideInletNode = 0;
        int UseSideOutletNode = 0;

        // Schedule reference
        int AvailabilityScheduleIndex = 0;

        // Dynamic State Variables
        Real64 PCM_TankTemp = 0.0;       // C, estimated from enthalpy
        Real64 EnergyStored = 0.0;       // J
        Real64 PercentCapacity = 0.0;    // %
        Real64 HeatLossRate_W = 0.0;     // W
        Real64 useheatTransfer = 0.0;    // W
        Real64 plantheatTransfer = 0.0;  // W
        Real64 DesignMassFlowRate = 0.0; // kg/s

        // Required functions
        void Init(EnergyPlusData &state);
        void Calculate(EnergyPlusData &state, PlantLocation const &plantLoc);

        // Singleton instance and factory
        static PCMStorageData &instance();
        static PlantComponent *factory(EnergyPlusData &state, std::string const &objectName);

        // Overrides from PlantComponent
        void
        simulate(EnergyPlusData &state, const PlantLocation &calledFromLocation, bool FirstHVACIteration, Real64 &CurLoad, bool RunFlag) override;

        void oneTimeInit(EnergyPlusData &state) override;
    };

    // Global functions
    void SimulatePCMStorage(EnergyPlusData &state, PlantLocation const &plantLoc, bool FirstHVACIteration, Real64 &CurLoad, bool RunFlag);

    void GetPCMStorageInput(EnergyPlusData &state);
    void RegisterPCMStorageOutputVariables(EnergyPlusData &state);

} // namespace PCMStorage
} // namespace EnergyPlus

#endif // ENERGYPLUS_PCMTHERMALSTORAGE_HH
