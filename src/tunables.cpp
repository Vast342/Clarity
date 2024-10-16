/*
    Clarity
    Copyright (C) 2024 Joseph Pasfield

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

Tunable aspBaseDelta("ASP_BaseDelta", 13, 1);
Tunable aspDeltaMultiplier("ASP_DeltaMultiplier", 1.22, 10);
Tunable aspDepthCondition("ASP_DepthCondition", 5, 1);

Tunable rfpDepthCondition("RFP_DepthCondition", 9, 1);
Tunable rfpMultiplier("RFP_Multiplier", 85, 1);

Tunable iirDepthCondition("IIR_DepthCondition", 2, 1);

Tunable fpDepthCondition("FP_DepthCondition", 0, 1);
Tunable fpBase("FP_Base", 347, 1);
Tunable fpMultiplier("FP_Multiplier", 44, 1);

Tunable lmpBase("LMP_Base", 3, 1);

Tunable sprDepthCondition("SPR_DepthCondition", 1, 1);
Tunable sprCaptureThreshold("SPR_CaptureThreshold", -97, -1);
Tunable sprQuietThreshold("SPR_QuietThreshold", -36, -1);

Tunable nmpDivisor("NMP_Divisor", 131, 1);
Tunable nmpSubtractor("NMP_Subtractor", 4, 1);
Tunable nmpDepthCondition("NMP_DepthCondition", 1, 1); // I need to fix this before next tune, it's locked at 0

Tunable hmrDivisor("HMR_Divisor", 8711, 1); 
Tunable cmrDivisor("CMR_Divisor", 4677, 1);

Tunable lmrBase("LMR_Base", 0.97, 100);
Tunable lmrMultiplier("LMR_Multiplier", 0.54, 100);

Tunable hstMaxBonus("HST_MaxBonus", 1632, 1);
Tunable hstAdder("HST_Adder", 276, 1);
Tunable hstSubtractor("HST_Subtractor", 119, 1);

Tunable sinDepthCondition("SIN_DepthCondition", 7, 1);
Tunable sinDepthMargin("SIN_DepthMargin", 3, 1);
Tunable sinDepthScale("SIN_DepthScale", 30, 1);

Tunable razDepthMultiplier("RAZ_DepthMultiplier", 436, 1);

Tunable ntmDepthCondition("NTM_DepthCondition", 9, 1);
Tunable ntmSubtractor("NTM_Subtractor", 1.51, 100);
Tunable ntmMultiplier("NTM_Multiplier", 1.44, 100);
Tunable ntmDefault("NTM_Default", 1.25, 100);

Tunable hipDepthCondition("HIP_DepthCondition", 9, 1);
Tunable hipDepthMultiplier("HIP_DepthMultiplier", -2335, -1);

Tunable qhsMaxBonus("QHS_MaxBonus", 1525, 1);
Tunable qhsMultiplier("QHS_Multiplier", 6, 1);
Tunable qhsAdder("QHS_Adder", 79, 1);
Tunable qhsSubtractor("QHS_Subtractor", 92, 1);

Tunable dexMargin("DEX_Margin", 33, 1);
Tunable dexLimit("DEX_Limit", 21, 1);

Tunable deiDepth("DEI_Depth", 14, 1);

Tunable lmrDepth("LMR_Depth", 1, 1);

Tunable mvvPawn("MVV_Pawn", 95, 1);
Tunable mvvKnight("MVV_Knight", 462, 1);
Tunable mvvBishop("MVV_Bishop", 498, 1);
Tunable mvvRook("MVV_Rook", 647, 1);
Tunable mvvQueen("MVV_Queen", 1057, 1);
Tunable blank("blank", 0, 1);

Tunable seePawn("SEE_Pawn", 117, 1);
Tunable seeKnight("SEE_Knight", 370, 1);
Tunable seeBishop("SEE_Bishop", 422, 1);
Tunable seeRook("SEE_Rook", 606, 1);
Tunable seeQueen("SEE_Queen", 1127, 1);

Tunable tmhDivisor("TMH_Divisor", 2, 1);
Tunable tmsNumerator("TMS_Numerator", 3, 1);
Tunable tmsDenominator("TMS_Denominator", 4, 1);
Tunable tmsMultiplier("TMS_Multiplier", 0.6, 10);

Tunable pvTTDepthMargin("PVTT_DepthMargin", 3, 1);

Tunable texMargin("TEX_Margin", 67, 1);

// Declaration of pointers to tunables
std::array<Tunable *, 7> MVV_values = {
    &mvvPawn,
    &mvvKnight,
    &mvvBishop,
    &mvvRook,
    &mvvQueen,
    &blank,
    &blank,
};

std::array<Tunable *, 7> SEE_values = {
    &seePawn,
    &seeKnight,
    &seeBishop,
    &seeRook,
    &seeQueen,
    &blank,
    &blank,
};

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
    &lmrDepth,
    &mvvPawn,
    &mvvKnight,
    &mvvBishop,
    &mvvRook,
    &mvvQueen,
    &seePawn,
    &seeKnight,
    &seeBishop,
    &seeRook,
    &seeQueen,
    &tmhDivisor,
    &tmsNumerator,
    &tmsDenominator,
    &tmsMultiplier,
    &pvTTDepthMargin
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
