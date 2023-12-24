#pragma once

#include "globals.h"

extern Tunable LMR_Base;
extern Tunable LMR_Multiplier;

extern Tunable ASP_BaseDelta;
extern Tunable ASP_DeltaMultiplier;
extern Tunable ASP_DepthCondition;

extern Tunable RFP_DepthCondition;
extern Tunable RFP_Multiplier;

extern Tunable IIR_DepthCondition;

extern Tunable FP_DepthCondition;
extern Tunable FP_Base;
extern Tunable FP_Multiplier;

extern Tunable LMP_DepthCondition;
extern Tunable LMP_Base;

extern Tunable SP_DepthCondition;
extern Tunable SP_CaptureThreshold;
extern Tunable SP_QuietThreshold;

extern Tunable NMP_DepthCondition;
extern Tunable NMP_Divisor;
extern Tunable NMP_Subtractor;

extern Tunable HMR_Divisor;

extern Tunable HST_MaxBonus;
extern Tunable HST_Multiplier;
extern Tunable HST_Adder;
extern Tunable HST_Subtractor;

extern Tunable SIN_DepthCondition;
extern Tunable SIN_DepthMargin;
extern Tunable SIN_DepthScale;

extern Tunable RAZ_DepthMultiplier;
extern Tunable RAZ_DepthCondition;

extern std::vector<Tunable *> Tunables;


void outputTunables();
void outputTunableJSON();
void adjustTunable(const std::string& name, const int &value);
void readTunable(const std::string &name);
void readTunables();
