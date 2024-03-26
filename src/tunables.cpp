/*
    Clarity
    Copyright (C) 2023 Joseph Pasfield

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/
#include "globals.h"

Tunable aspBaseDelta("ASP_BaseDelta", 17, 1);
Tunable aspDeltaMultiplier("ASP_DeltaMultiplier", 1.67, 10);
Tunable aspDepthCondition("ASP_DepthCondition", 4, 1);

Tunable rfpDepthCondition("RFP_DepthCondition", 11, 1);
Tunable rfpMultiplier("RFP_Multiplier", 96, 1);

Tunable iirDepthCondition("IIR_DepthCondition", 4, 1);

Tunable fpDepthCondition("FP_DepthCondition", 1, 1);
Tunable fpBase("FP_Base", 268, 1);
Tunable fpMultiplier("FP_Multiplier", 64, 1);

Tunable lmpBase("LMP_Base", 2, 1);// No divisor adjustment

Tunable sprDepthCondition("SPR_DepthCondition", 4, 1);
Tunable sprCaptureThreshold("SPR_CaptureThreshold", -118, -1);
Tunable sprQuietThreshold("SPR_QuietThreshold", -38, -1);

Tunable nmpDivisor("NMP_Divisor", 157, 1);
Tunable nmpSubtractor("NMP_Subtractor", 5, 1);
Tunable nmpDepthCondition("NMP_DepthCondition", 1, 1);

Tunable hmrDivisor("HMR_Divisor", 8000, 1); 
Tunable cmrDivisor("CMR_Divisor", 2926, 1);

Tunable lmrBase("LMR_Base", 0.81, 100);// Adjusted divisor for lmrBase
Tunable lmrMultiplier("LMR_Multiplier", 0.58, 100);// Adjusted divisor for lmrMultiplier

Tunable hstMaxBonus("HST_MaxBonus", 1884, 1);
Tunable hstMultiplier("HST_Multiplier", 4, 1);
Tunable hstAdder("HST_Adder", 150, 1);
Tunable hstSubtractor("HST_Subtractor", 110, 1);

Tunable sinDepthCondition("SIN_DepthCondition", 8, 1);
Tunable sinDepthMargin("SIN_DepthMargin", 3, 1);
Tunable sinDepthScale("SIN_DepthScale", 48, 1);

Tunable razDepthMultiplier("RAZ_DepthMultiplier", 394, 1);

Tunable ntmDepthCondition("NTM_DepthCondition", 8, 1);
Tunable ntmSubtractor("NTM_Subtractor", 1.61, 100);
Tunable ntmMultiplier("NTM_Multiplier", 1.41, 100);
Tunable ntmDefault("NTM_Default", 0.83, 100);

Tunable hipDepthCondition("HIP_DepthCondition", 7, 1);
Tunable hipDepthMultiplier("HIP_DepthMultiplier", -2066, -1);

Tunable qhsMaxBonus("QHS_MaxBonus", 1830, 1);
Tunable qhsMultiplier("QHS_Multiplier", 5, 1);
Tunable qhsAdder("QHS_Adder", 126, 1);
Tunable qhsSubtractor("QHS_Subtractor", 107, 1);

Tunable dexMargin("DEX_Margin", 50, 1);
Tunable dexLimit("DEX_Limit", 20, 1);

Tunable deiDepth("DEI_Depth", 10, 1);

Tunable lmrDepth("LMR_Depth", 1, 1);

// Declaration of pointers to tunables

std::vector<Tunable *> tunables = {
    &aspBaseDelta,
    &aspDeltaMultiplier,
    &aspDepthCondition,
    &rfpDepthCondition,
    &rfpMultiplier,
    &iirDepthCondition,
    &fpDepthCondition,
    &fpBase,
    &fpMultiplier,
    &lmpBase,
    &sprDepthCondition,
    &sprCaptureThreshold,
    &sprQuietThreshold,
    &nmpDivisor,
    &nmpSubtractor,
    &nmpDepthCondition,
    &hmrDivisor,
    &cmrDivisor,
    &lmrBase,
    &lmrMultiplier,
    &hstMaxBonus,
    &hstMultiplier,
    &hstAdder,
    &hstSubtractor,
    &sinDepthCondition,
    &sinDepthMargin,
    &sinDepthScale,
    &razDepthMultiplier,
    &ntmDepthCondition,
    &ntmSubtractor,
    &ntmMultiplier,
    &ntmDefault,
    &hipDepthCondition,
    &hipDepthMultiplier,
    &qhsMaxBonus,
    &qhsMultiplier,
    &qhsAdder,
    &qhsSubtractor,
    &dexMargin,
    &dexLimit,
    &deiDepth,
    &lmrDepth
};


void outputTunables() {
    for(Tunable *tunable : tunables) {
        std::cout << "option name " << tunable->name << " type spin default " << std::to_string(static_cast<int>(tunable->value * tunable->divisor)) << " min " << "0" << " max " << tunable->max << std::endl;
    }
}
void outputTunableJSON() {
    std::cout << "{\n";
    for(Tunable *tunable : tunables) {
        std::cout << "   \"" << tunable->name << "\": {\n";
        std::cout << "      \"value\": " << std::to_string(static_cast<int>(tunable->value * tunable->divisor)) << "," << std::endl;
        std::cout << "      \"min_value\": " << "0" << "," << std::endl;
        std::cout << "      \"max_value\": " << tunable->max << "," << std::endl;
        std::cout << "      \"step\": " << tunable->step << std::endl;
        std::cout << "   },\n";
    }
    std::cout << "}\n";
}
void outputTunableOB() {
    for(Tunable *tunable : tunables) {
        std::cout << tunable->name << ", int, " << std::to_string(static_cast<int>(tunable->value * tunable->divisor)) << ", 0, " << tunable->max << ", " << tunable->step << ", 0.002" << std::endl;
    }
}
void adjustTunable(const std::string &name, const int &value) {
    for(Tunable *tunable : tunables) {
        if(tunable->name == name) {
            tunable->updateValue(value);
            if(name == "LMR_Base" || name == "LMR_Multiplier") {
                calculateReductions();
            }
            return;
        }
    }
    std::cout << "No Such Tunable\n";
}
void readTunable(const std::string &name) {
    for(Tunable *tunable : tunables) {
        if(tunable->name == name) {
            std::cout << "value: " << tunable->value << std::endl;
            return;
        }
    }
    std::cout << "No Such Tunable\n";
}
void readTunables() {
    for(Tunable *tunable : tunables) {
        std::cout << "name: " << tunable->name << std::endl;
        std::cout << "value: " << tunable->value << std::endl;
        std::cout << "min: " << "0" << std::endl;
        std::cout << "max: " << tunable->max << std::endl;
        std::cout << "divisor: " << tunable->divisor << std::endl;
        std::cout << "step: " << tunable->step << std::endl;
        //std::cout << "value at tuning scale: " << static_cast<int>(tunable->value * tunable->divisor) << std::endl;
    }
}
