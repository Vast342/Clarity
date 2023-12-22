#include "globals.h"

Tunable LMR_Base("LMR_Base", 78, 0, 100, 100, 10);
Tunable LMR_Multiplier("LMR_Multiplier", 55, 0, 100, 100, 10);

Tunable ASP_BaseDelta("ASP_BaseDelta", 20, 0, 40, 1, 7);
Tunable ASP_DeltaMultiplier("ASP_DeltaMultiplier", 18, 0, 30, 10, 7);
Tunable ASP_DepthCondition("ASP_DepthCondition", 4, 0, 20, 1, 5);

Tunable RFP_DepthCondition("RFP_DepthCondition", 11, 0, 20, 1, 5);
Tunable RFP_Multiplier("RFP_Multiplier", 82, 0, 100, 1, 10);

Tunable IIR_DepthCondition("IIR_DepthCondition", 5, 0, 20, 1, 5);

Tunable FP_DepthCondition("FP_DepthCondition", 3, 0, 20, 1, 5);
Tunable FP_Base("FP_Base", 273, 0, 400, 1, 100);
Tunable FP_Multiplier("FP_Multiplier", 65, 0, 100, 1, 25);

Tunable LMP_DepthCondition("LMP_DepthCondition", 8, 0, 20, 1, 5);
Tunable LMP_Base("LMP_Base", 0, 0, 20, 1, 5);

Tunable SP_DepthCondition("SP_DepthCondition", 3, 0, 20, 1, 5);
Tunable SP_CaptureThreshold("SP_CaptureThreshold", 110, 0, 200, -1, 25);
Tunable SP_QuietThreshold("SP_QuietThreshold", 32, 0, 100, -1, 25);

Tunable NMP_DepthCondition("NMP_DepthCondition", 2, 0, 20, 1, 5);
Tunable NMP_Divisor("NMP_Divisor", 196, 150, 250, 1, 25);
Tunable NMP_Subtractor("NMP_Subtractor", 3, 0, 10, 1, 3);

Tunable HMR_Divisor("HMR_Divisor", 8192, 0, 10000, 1, 1500);

Tunable HST_MaxBonus("HST_MaxBonus", 1892, 1500, 2500, 1, 250);
Tunable HST_Multiplier("HST_Multiplier", 4, 0, 10, 1, 2);
Tunable HST_Adder("HST_Adder", 120, 100, 200, 1, 20);
Tunable HST_Subtractor("HST_Subtractor", 120, 100, 200, 1, 20);

Tunable SIN_DepthCondition("SIN_DepthCondition", 8, 0, 20, 1, 5);
Tunable SIN_DepthMargin("Sin_DepthMargin", 3, 0, 5, 1, 1);
Tunable SIN_DepthScale("SIN_DepthScale", 32, 0, 64, 1, 8);

std::vector<Tunable *> tunables = {
    &LMR_Base,
    &LMR_Multiplier,
    &ASP_BaseDelta,
    &ASP_DeltaMultiplier,
    &ASP_DepthCondition,
    &RFP_DepthCondition,
    &RFP_Multiplier,
    &IIR_DepthCondition,
    &FP_DepthCondition,
    &FP_Base,
    &FP_Multiplier, 
    &LMP_DepthCondition,
    &LMP_Base,
    &SP_DepthCondition,
    &SP_CaptureThreshold,
    &SP_QuietThreshold,
    &NMP_DepthCondition,
    &NMP_Divisor,
    &NMP_Subtractor,
    &HMR_Divisor,
    &HST_MaxBonus,
    &HST_Multiplier,
    &HST_Adder,
    &HST_Subtractor,
    &SIN_DepthCondition,
    &SIN_DepthMargin,
    &SIN_DepthScale
};


void outputTunables() {
    for(Tunable *tunable : tunables) {
        std::cout << "option name " << tunable->name << " type spin default " << std::to_string(static_cast<int>(tunable->value * tunable->divisor)) << " min " << tunable->min << " max " << tunable->max << std::endl;
    }
}
void outputTunableJSON() {
    std::cout << "{\n";
    for(Tunable *tunable : tunables) {
        std::cout << "   \"" << tunable->name << "\": {\n";
        std::cout << "      \"value\": " << std::to_string(static_cast<int>(tunable->value * tunable->divisor)) << "," << std::endl;
        std::cout << "      \"min_value\": " << tunable->min << "," << std::endl;
        std::cout << "      \"max_value\": " << tunable->max << "," << std::endl;
        std::cout << "      \"step\": " << tunable->step << std::endl;
        std::cout << "   },\n";
    }
    std::cout << "}\n";
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
