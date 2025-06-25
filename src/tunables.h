/*
    Clarity
    Copyright (C) 2025 Joseph Pasfield

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

// leaving only these tunables because funnies

inline Tunable tmhDivisor("TMH_Divisor", 2, 1);
inline Tunable tmsNumerator("TMS_Numerator", 3, 1);
inline Tunable tmsDenominator("TMS_Denominator", 4, 1);
inline Tunable tmsMultiplier("TMS_Multiplier", 0.6, 10);

inline Tunable hstMaxBonus("HST_MaxBonus", 1632, 1);
inline Tunable hstAdder("HST_Adder", 276, 1);
inline Tunable hstSubtractor("HST_Subtractor", 119, 1);

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
    &tmhDivisor,
    &tmsNumerator,
    &tmsDenominator,
    &tmsMultiplier,
    &hstMaxBonus,
    &hstAdder,
    &hstSubtractor,
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
