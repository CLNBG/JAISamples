#ifndef C_ALC_DEF_H_
#define C_ALC_DEF_H_

enum EALCType
{
    MinGainPriority = 0,
    MinExposurePriority = 1,
    GainPriority = 2,
    ExposurePriority = 3,
    GainOnly = 4,
    ExposureOnly = 5,
    Undefined
};

class CAutomaticLevelControl
{
public:
    CAutomaticLevelControl(void);
    virtual ~CAutomaticLevelControl(void);

    bool Calculate(int input);                          // This is the main closed-loop calculation function that calculates the new gain and exposure

    bool SetALCType(EALCType type);                     // Set the type of ALC control

    bool SetSetpoint(int setpoint);                     // Set target for the closed-loop control
    bool SetMinGain(int min);                           // Set the minimum gain for the closed-loop control
    bool SetMaxGain(int max);                           // Set the maximum gain for the closed-loop control
    bool SetMinExposure(int min);                       // Set the minimum exposure for the closed-loop control
    bool SetMaxExposure(int max);                       // Set the maximum exposure for the closed-loop control
    bool SetCurrentGain(int gain);                      // Initial gain value
    bool SetCurrentExposure(int exposure);              // Initial exposure value
    bool SetThreshold(double threshold);                // Relaive threshold value around setpoint
    bool SetKGain(double kGain);                        // Set closed-loop "Gain" for the Gain
    bool SetKExposure(double kExposure);                // Set closed-loop "Gain" for the Exposure

    EALCType GetALCType();
    int GetSetPoint();
    int GetMinGain();
    int GetMaxGain();
    int GetMinExposure();
    int GetMaxExposure();
    int GetCurrentGain();                               // Get the current Gain value
    int GetCurrentExposure();                           // Get the current Exposure value
    double GetThreshold();
    double GetKGain();
    double GetKExposure();

private:
    bool AdjustExposureTime(double error);
    bool AdjustGain(double error);
    bool ReCalculate();                                 // Re-calculate based on last input value

    EALCType m_ALCType;     // Type of ALC control
    int m_MinGain;          // Minimum Gain value
    int m_MaxGain;          // Maximum Gain value
    int m_CurrentGain;      // Current Gain value
    int m_MinExposure;      // Minimum Exposure Time value
    int m_MaxExposure;      // Maximum Exposure Time value
    int m_CurrentExposure;  // Current Exposure Time value

    int m_SetPoint;         // This is the target average for the Closed-Loop control
    int m_Average;          // This is the last known average input for the Closed-Loop control

    double m_KGain;         // Gain for the Gain adjustments
    double m_KExposure;     // Gain for the Exposure Time adjustments
    double m_Threshold;     // Threshold value for the error
};

#endif // C_ALC_DEF_H_
  
