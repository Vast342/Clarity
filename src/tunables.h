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
// header guards woooooooo
#pragma once

#include "globals.h"


struct Tunable {
    explicit Tunable(std::string _name, double _value, int _divisor)
        : name(std::move(_name)),
          value(_value),
          max(value * _divisor * 2),
          divisor(_divisor),
          step(max / 20 == 0 ? 1 : max / 20) {}

    void updateValue(double newValue) {
        value = newValue / divisor;
    }

    std::string name;
    double value;
    int max;
    int divisor;
    int step;
};

inline Tunable aspBaseDelta("ASP_BaseDelta", 13, 1);
inline Tunable aspDeltaMultiplier("ASP_DeltaMultiplier", 1.22, 10);
inline Tunable aspDepthCondition("ASP_DepthCondition", 5, 1);

inline Tunable rfpDepthCondition("RFP_DepthCondition", 9, 1);
inline Tunable rfpMultiplier("RFP_Multiplier", 85, 1);

inline Tunable iirDepthCondition("IIR_DepthCondition", 2, 1);

inline Tunable fpDepthCondition("FP_DepthCondition", 0, 1);
inline Tunable fpBase("FP_Base", 347, 1);
inline Tunable fpMultiplier("FP_Multiplier", 44, 1);

inline Tunable lmpBase("LMP_Base", 3, 1);

inline Tunable sprDepthCondition("SPR_DepthCondition", 1, 1);
inline Tunable sprCaptureThreshold("SPR_CaptureThreshold", -97, -1);
inline Tunable sprQuietThreshold("SPR_QuietThreshold", -36, -1);

inline Tunable nmpDivisor("NMP_Divisor", 131, 1);
inline Tunable nmpSubtractor("NMP_Subtractor", 4, 1);
inline Tunable nmpDepthCondition("NMP_DepthCondition", 1, 1); // I need to fix this before next tune, it's locked at 0

inline Tunable hmrDivisor("HMR_Divisor", 8711, 1); 
inline Tunable cmrDivisor("CMR_Divisor", 4677, 1);

inline Tunable lmrBase("LMR_Base", 0.97, 100);
inline Tunable lmrMultiplier("LMR_Multiplier", 0.54, 100);

inline Tunable hstMaxBonus("HST_MaxBonus", 1632, 1);
inline Tunable hstAdder("HST_Adder", 276, 1);
inline Tunable hstSubtractor("HST_Subtractor", 119, 1);

inline Tunable sinDepthCondition("SIN_DepthCondition", 7, 1);
inline Tunable sinDepthMargin("SIN_DepthMargin", 3, 1);
inline Tunable sinDepthScale("SIN_DepthScale", 30, 1);

inline Tunable razDepthMultiplier("RAZ_DepthMultiplier", 436, 1);

inline Tunable ntmDepthCondition("NTM_DepthCondition", 9, 1);
inline Tunable ntmSubtractor("NTM_Subtractor", 1.51, 100);
inline Tunable ntmMultiplier("NTM_Multiplier", 1.44, 100);
inline Tunable ntmDefault("NTM_Default", 1.25, 100);

inline Tunable hipDepthCondition("HIP_DepthCondition", 9, 1);
inline Tunable hipDepthMultiplier("HIP_DepthMultiplier", -2335, -1);

inline Tunable qhsMaxBonus("QHS_MaxBonus", 1525, 1);
inline Tunable qhsMultiplier("QHS_Multiplier", 6, 1);
inline Tunable qhsAdder("QHS_Adder", 79, 1);
inline Tunable qhsSubtractor("QHS_Subtractor", 92, 1);

inline Tunable dexMargin("DEX_Margin", 33, 1);
inline Tunable dexLimit("DEX_Limit", 21, 1);

inline Tunable deiDepth("DEI_Depth", 14, 1);

inline Tunable lmrDepth("LMR_Depth", 1, 1);

inline Tunable mvvPawn("MVV_Pawn", 95, 1);
inline Tunable mvvKnight("MVV_Knight", 462, 1);
inline Tunable mvvBishop("MVV_Bishop", 498, 1);
inline Tunable mvvRook("MVV_Rook", 647, 1);
inline Tunable mvvQueen("MVV_Queen", 1057, 1);
inline Tunable blank("blank", 0, 1);

inline Tunable seePawn("SEE_Pawn", 117, 1);
inline Tunable seeKnight("SEE_Knight", 370, 1);
inline Tunable seeBishop("SEE_Bishop", 422, 1);
inline Tunable seeRook("SEE_Rook", 606, 1);
inline Tunable seeQueen("SEE_Queen", 1127, 1);

inline Tunable tmhDivisor("TMH_Divisor", 2, 1);
inline Tunable tmsNumerator("TMS_Numerator", 3, 1);
inline Tunable tmsDenominator("TMS_Denominator", 4, 1);
inline Tunable tmsMultiplier("TMS_Multiplier", 0.6, 10);

inline Tunable texMargin("TEX_Margin", 67, 1);

// corrhist tunables
inline Tunable chUncertaintyMargin("CH_Uncertainty_Margin", 128, 1);

inline Tunable chScale("CH_Scale", 256, 1);
inline Tunable chMin("CH_Min", -64, -1);
inline Tunable chMax("CH_Max", 64, 1);

inline Tunable pawnChWeight("Pawn_CH_Weight", 512, 1);
inline Tunable nonpawnChWeight("NonPawn_CH_Weight", 512, 1);
inline Tunable majorChWeight("Major_CH_Weight", 512, 1);
inline Tunable minorChWeight("Minor_CH_Weight", 512, 1);

// Declaration of pointers to tunables
inline std::array<Tunable *, 7> MVV_values = {
    &mvvPawn,
    &mvvKnight,
    &mvvBishop,
    &mvvRook,
    &mvvQueen,
    &blank,
    &blank,
};

inline std::array<Tunable *, 7> SEE_values = {
    &seePawn,
    &seeKnight,
    &seeBishop,
    &seeRook,
    &seeQueen,
    &blank,
    &blank,
};

inline std::vector<Tunable *> tunables = {
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
    &texMargin,
    &chUncertaintyMargin,
    &chScale,
    &chMin,
    &chMax,
    &pawnChWeight,
    &nonpawnChWeight,
    &majorChWeight,
    &minorChWeight,
};


inline void outputTunables() {
    for(Tunable *tunable : tunables) {
        std::cout << "option name " << tunable->name << " type spin default " << std::to_string(static_cast<int>(tunable->value * tunable->divisor)) << " min " << "0" << " max " << tunable->max << std::endl;
    }
}
inline void outputTunableJSON() {
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
inline void outputTunableOB() {
    for(Tunable *tunable : tunables) {
        std::cout << tunable->name << ", int, " << std::to_string(static_cast<int>(tunable->value * tunable->divisor)) << ", 0, " << tunable->max << ", " << tunable->step << ", 0.002" << std::endl;
    }
}
inline void adjustTunable(const std::string &name, const int &value) {
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
inline void readTunable(const std::string &name) {
    for(Tunable *tunable : tunables) {
        if(tunable->name == name) {
            std::cout << "value: " << tunable->value << std::endl;
            return;
        }
    }
    std::cout << "No Such Tunable\n";
}
inline void readTunables() {
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
