// EnergyPlus, Copyright (c) 1996-2022, The Board of Trustees of the University of Illinois,
// The Regents of the University of California, through Lawrence Berkeley National Laboratory
// (subject to receipt of any required approvals from the U.S. Dept. of Energy), Oak Ridge
// National Laboratory, managed by UT-Battelle, Alliance for Sustainable Energy, LLC, and other
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
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <string>

// ObjexxFCL Headers
#include <ObjexxFCL/Fmath.hh>
// #include <ObjexxFCL/gio.hh>
#include <ObjexxFCL/string.functions.hh>

// EnergyPlus Headers
#include <EnergyPlus/CommandLineInterface.hh>
#include <EnergyPlus/Data/EnergyPlusData.hh>
#include <EnergyPlus/DataEnvironment.hh>
#include <EnergyPlus/DataGlobals.hh>
#include <EnergyPlus/DataHVACGlobals.hh>
#include <EnergyPlus/DataPrecisionGlobals.hh>
#include <EnergyPlus/DisplayRoutines.hh>
#include <EnergyPlus/ExtCtrl.hh>
#include <EnergyPlus/General.hh>
#include <EnergyPlus/UtilityRoutines.hh>

#include <array>
#include <string>

namespace EnergyPlus {

namespace ExtCtrl {
    // Module containing the external control

    // MODULE INFORMATION:
    //       AUTHOR         Takao Moriyama, IBM Corporation
    //       DATE WRITTEN   December 2017
    //       MODIFIED       na
    //       RE-ENGINEERED  na

    // PURPOSE OF THIS MODULE:
    // This module provides a repository for suporting external control

    // Object Data

    // Subroutine Specifications for the Module

    // Functions

    void InitializeExtCtrlRoutines(EnergyPlusData &state)
    {
        if (state.dataExtCtrl->AlreadyDidOnce) {
            return;
        }

        // DisplayString("InitializeExtCtrlRoutine(): First call");
        get_environment_variable(cActPipeFilename, state.dataExtCtrl->act_filename);
        if (state.dataExtCtrl->act_filename.empty()) {
            ShowFatalError(state, "InitializeExtCtrlActRoutines: Environment variable ACT_PIPE_FILENAME not specified");
        }
        get_environment_variable(cObsPipeFilename, state.dataExtCtrl->obs_filename);
        if (state.dataExtCtrl->obs_filename.empty()) {
            ShowFatalError(state, "InitializeExtCtrlActRoutines: Environment variable OBS_PIPE_FILENAME not specified");
        }

        state.dataExtCtrl->AlreadyDidOnce = true;
    }

    std::string ExtCtrlRead(EnergyPlusData &state)
    {
        auto &act_ifs = state.dataExtCtrl->act_ifs;
        if (!act_ifs.is_open()) {
            act_ifs.open(state.dataExtCtrl->act_filename);
            act_ifs.rdbuf()->pubsetbuf(nullptr, 0); // Making unbuffered
            if (!act_ifs.is_open()) {
                ShowFatalError(state, "ExtCtrlRead: ACT file could not open");
                return "";
            }
            DisplayString(state, "ExtCtrlRead: Opened ACT file: " + std::string(state.dataExtCtrl->act_filename));
        }

        // We just do an infinite loop until a value comes in
        std::string line;
        size_t idx = std::string::npos;
        while (idx == std::string::npos) {
            act_ifs >> line;
            idx = line.find(',');
            // TODO: include some kind of timeout?
        }

        std::string seq = line.substr(0, idx);
        std::string val = line.substr(idx + 1, std::string::npos);
        assert(state.dataExtCtrl->act_seq == std::stoi(seq));
        state.dataExtCtrl->act_seq++;
        return val;
    }

    void ExtCtrlWrite(EnergyPlusData &state, const std::string &str)
    {
        if (!state.dataExtCtrl->obs_ofs.is_open()) {
            state.dataExtCtrl->obs_ofs.open(state.dataExtCtrl->obs_filename);
            if (!state.dataExtCtrl->obs_ofs.is_open()) {
                ShowFatalError(state, "ExtCtrlWrite: InitializeExtCtrlRoutine: OBS file could not open");
                return;
            }
            DisplayString(state, "ExtCtrlWrite: Opened OBS file: " + std::string(state.dataExtCtrl->obs_filename));
        }
        state.dataExtCtrl->obs_ofs << state.dataExtCtrl->obs_seq << "," << str << std::endl;
        state.dataExtCtrl->obs_seq++;
    }

    void ExtCtrlFlush(EnergyPlusData &state)
    {
        state.dataExtCtrl->obs_ofs << "DELIMITER" << std::endl;
        state.dataExtCtrl->obs_ofs.flush();
    }

    bool ExtCtrlObs(EnergyPlusData &state,
                    int const index,   // command code
                    Real64 const value // command value
    )
    {
        InitializeExtCtrlRoutines(state);

        if (index >= CMD_OBS_INDEX_LOW && index <= CMD_OBS_INDEX_HIGH) {
            // DisplayString(format("ExtCtrlObs: set obs[{}] = {}", cmdInt, arg));
            state.dataExtCtrl->obss[index - 1] = value;
            return true;
        } else if (index == CMD_OBS_INIT) {
            // DisplayString("ExtCtrlObs: INIT");
            //  If not connected to the server, try to connect.
            //  TODO:
            // ShowFatalError("Failed to connect to external service");
            return true;
        }
        // TODO: Show error code
        ShowWarningMessage(state, format("ExtCtrlObs: Obs index {} is out of range [{}...{}]", index, CMD_OBS_INDEX_LOW, CMD_OBS_INDEX_HIGH));
        return false;
    }

    bool ExtCtrlAct(EnergyPlusData &state,
                    int const cmd, // command code
                    int const arg, // command value
                    Real64 &readValue)
    {
        InitializeExtCtrlRoutines(state);

        if (cmd >= CMD_ACT_INDEX_LOW && cmd <= CMD_ACT_INDEX_HIGH) {
            // DisplayString("ExtCtrlAct: get acts[" + std::to_string(cmd) + "] = " +
            // std::to_string(state.dataExtCtrl->acts[cmd - 1]));
            readValue = state.dataExtCtrl->acts[cmd - 1];
            return true;
        } else if (cmd == CMD_ACT_REQ) {
            if (!(arg >= 0 && arg <= CMD_ACT_INDEX_HIGH)) {
                ShowWarningMessage(state, format("ExtCtrlAct:  Number of obss {} it out of range [0...{}]", arg, CMD_ACT_INDEX_HIGH));
                return false;
            }
            // TODO: really want that?
            // skip system timestep
            if (state.dataHVACGlobal->TimeStepSys < state.dataGlobal->TimeStepZone) {
                return true;
            }

            // Send observation data to the server, and receive next action.
            ExtCtrlWrite(state, std::to_string(arg));
            for (int i = CMD_ACT_INDEX_LOW; i <= arg; i++) {
                ExtCtrlWrite(state, std::to_string(state.dataExtCtrl->obss[i - 1]));
            }
            ExtCtrlFlush(state);

            // Get action data
            std::string line = ExtCtrlRead(state);
            int NumActsReceived = std::stoi(line);
            assert(NumActsReceived >= 0 && NumActsReceived <= CMD_ACT_INDEX_HIGH);
            for (int i = 1; i <= NumActsReceived; i++) {
                line = ExtCtrlRead(state);
                double val = std::stod(line);
                if (i <= CMD_ACT_INDEX_HIGH) {
                    state.dataExtCtrl->acts[i - 1] = val;
                }
            }

            return true;
        }

        ShowWarningMessage(state, format("ExtCtrlAct: Act index {} is out of range [{}...{}]", cmd, CMD_ACT_INDEX_LOW, CMD_ACT_INDEX_HIGH));
        return false;
    }

} // namespace ExtCtrl

} // namespace EnergyPlus
