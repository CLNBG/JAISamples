#include "stdafx.h"
#include "ALC.h"
#define _USE_MATH_DEFINES
#include <cmath>

CAutomaticLevelControl::CAutomaticLevelControl(void)
{
    m_MinGain = 0;
    m_MaxGain = 0;
    m_CurrentGain = 0;
    m_MinExposure = 0;
    m_MaxExposure = 0;
    m_CurrentExposure = 0;
}

CAutomaticLevelControl::~CAutomaticLevelControl(void)
{
}

// Re-calculate based on last input value
bool CAutomaticLevelControl::ReCalculate()
{
    return Calculate(m_Average);
}

// This is the main closed-loop calculation function that calculates the new gain and exposure
bool CAutomaticLevelControl::Calculate(int input)
{
    m_Average = input;

    // We can now continue with the actual Closed-Loop calculations
    double relativeError = ((double)m_SetPoint - (double)input)/(double)m_SetPoint;

    bool changes = false;

    switch (m_ALCType)
    {
        case MinGainPriority:
            // We typically only want to adjust one of the camera settings at a time
            // and typically it makes sense to minimize the Gain in order to get as
            // little noise as possible in the image.

            // Do we need to increase Gain/Exposure time?
            if (relativeError > m_Threshold)
            {
                // As long as we can expose longer then we want to force the Gain to a minimum!
                if ((m_CurrentExposure < m_MaxExposure) && (m_CurrentGain > m_MinGain))
                {
                    changes = AdjustGain(-relativeError/2);
                    changes |= AdjustExposureTime(relativeError*2);
                }
                else 
                {
                    if (m_CurrentExposure == m_MaxExposure)
                        changes = AdjustGain(relativeError);
                    else
                        changes = AdjustExposureTime(relativeError);
                }
            }
            else if (relativeError < -m_Threshold)// We need to decrease Gain/Exposure time
            {
                // As long as we can expose shorter then we want to force the Gain to a minimum!
                if (m_CurrentGain > m_MinGain)
                {
                    changes = AdjustGain(relativeError);
                }
                else
                {
                    changes = AdjustExposureTime(relativeError);
                }
            }
            else // OK - we reached the set-point so we need to see if we can adjust gain even lower!??
            {
                if ((m_CurrentGain > m_MinGain) && (m_CurrentExposure < m_MaxExposure))
                {
                    if (m_CurrentGain > m_MinGain)                    
                        changes = AdjustGain(-0.1);

                    if (changes && (m_CurrentExposure < m_MaxExposure))                    
                        changes = AdjustExposureTime(0.1);
                }
            }
            break;

        case MinExposurePriority:
            // We typically only want to adjust one of the camera settings at a time
            // and typically it makes sense to minimize the Gain in order to get as
            // little noise as possible in the image.

            // Do we need to increase Gain/Exposure time?
            if (relativeError > m_Threshold)
            {
                // As long as we can increase Gain then we want to force the Exposure Time to a minimum!
                if ((m_CurrentGain < m_MaxGain) && (m_CurrentExposure > m_MinExposure))
                {
                    changes = AdjustExposureTime(-relativeError/2);
                    changes |= AdjustGain(relativeError*2);
                }
                else 
                {
                    if (m_CurrentGain == m_MaxGain)
                        changes = AdjustExposureTime(relativeError);
                    else
                        changes = AdjustGain(relativeError);
                }
            }
            else if (relativeError < -m_Threshold)// We need to decrease Gain/Exposure time
            {
                // As long as we can expose shorter then we want to force the Gain to a minimum!
                if (m_CurrentExposure > m_MinExposure)
                {
                    changes = AdjustExposureTime(relativeError);
                }
                else
                {
                    changes = AdjustGain(relativeError);
                }
            }
            else // OK - we reached the set-point so we need to see if we can adjust exposure time even lower!??
            {
                if ((m_CurrentExposure > m_MinExposure) && (m_CurrentGain < m_MaxGain))
                {
                    if (m_CurrentExposure > m_MinExposure)                    
                        changes = AdjustExposureTime(-0.1);

                    if (changes && (m_CurrentGain < m_MaxGain))                    
                        changes = AdjustGain(0.1);
                }
            }
            break;

        case GainPriority:
            // In this mode we try to adjust gain until it reaches one of the limits and then we adjust the exposure time.

            // Do we need to increase Gain/Exposure time?
            if (relativeError > m_Threshold)
            {
                // As long as we can increase Gain then we do it!
                if (m_CurrentGain < m_MaxGain)
                {
                    changes = AdjustGain(relativeError);
                }
                else 
                {
                    changes = AdjustExposureTime(relativeError);
                }
            }
            else if (relativeError < -m_Threshold) // We need to decrease Gain/Exposure time
            {
                // As long as we can decrease Gain then we do it!
                if (m_CurrentGain > m_MinGain)
                {
                    changes = AdjustGain(relativeError);
                }
                else
                {
                    changes = AdjustExposureTime(relativeError);
                }
            }
            break;

        case ExposurePriority:
            // In this mode we try to adjust exposure time until it reaches one of the limits and then we adjust the gain.

            // Do we need to increase Gain/Exposure time?
            if (relativeError > m_Threshold)
            {
                // As long as we can increase exposure time then we do it!
                if (m_CurrentExposure < m_MaxExposure)
                {
                    changes = AdjustExposureTime(relativeError);
                }
                else 
                {
                    changes = AdjustGain(relativeError);
                }
            }
            else if (relativeError < -m_Threshold) // We need to decrease Gain/Exposure time
            {
                // As long as we can decrease exposure time then we do it!
                if (m_CurrentExposure > m_MinExposure)
                {
                    changes = AdjustExposureTime(relativeError);
                }
                else
                {
                    changes = AdjustGain(relativeError);
                }
            }
            break;

        case GainOnly:
            // In this mode we only try to adjust the gain.

            // Do we need to increase Gain/Exposure time?
            if (relativeError > m_Threshold)
            {
                changes = AdjustGain(relativeError);
            }
            else if (relativeError < -m_Threshold) // We need to decrease Gain/Exposure time
            {
                changes = AdjustGain(relativeError);
            }
            break;

        case ExposureOnly:
            // In this mode we only try to adjust the exposure time.

            // Do we need to increase Gain/Exposure time?
            if (relativeError > m_Threshold)
            {
                changes = AdjustExposureTime(relativeError);
            }
            else if (relativeError < -m_Threshold) // We need to decrease Gain/Exposure time
            {
                changes = AdjustExposureTime(relativeError);
            }
            break;
    }

    return changes;
}

// Set the type of ALC control
bool CAutomaticLevelControl::SetALCType(EALCType type)                     
{
    m_ALCType = type;
    return ReCalculate();
}

// Set target for the closed-loop control
bool CAutomaticLevelControl::SetSetpoint(int setpoint)
{
    m_SetPoint = setpoint;
    return ReCalculate();
}

// Set the min gain for the closed-loop control
bool CAutomaticLevelControl::SetMinGain(int min)
{
    bool changes = false;

    m_MinGain = min;
    
    // When we adjust the Min gain then we should adjust the current gain accordingly in case the
    // current gain is outside the new boundries
    if (m_CurrentGain < m_MinGain)
    {
        m_CurrentGain = m_MinGain;
        changes = true;
    }

    return changes;
}

// Set the max gain for the closed-loop control
bool CAutomaticLevelControl::SetMaxGain(int max)
{
    bool changes = false;

    m_MaxGain = max;
    
    // When we adjust the Min gain then we should adjust the current gain accordingly in case the
    // current gain is outside the new boundries
    if (m_CurrentGain > m_MaxGain)
    {
        m_CurrentGain = m_MaxGain;
        changes = true;
    }

    return changes;
}

// Set the min exposure for the closed-loop control
bool CAutomaticLevelControl::SetMinExposure(int min)
{
    bool changes = false;

    m_MinExposure = min;
    
    // When we adjust the Min gain then we should adjust the current gain accordingly in case the
    // current gain is outside the new boundries
    if (m_CurrentExposure < m_MinExposure)
    {
        m_CurrentExposure = m_MinExposure;
        changes = true;
    }

    return changes;
}

// Set the max gain for the closed-loop control
bool CAutomaticLevelControl::SetMaxExposure(int max)
{
    bool changes = false;

    m_MaxExposure = max;
    
    // When we adjust the Min gain then we should adjust the current gain accordingly in case the
    // current gain is outside the new boundries
    if (m_CurrentExposure > m_MaxExposure)
    {
        m_CurrentExposure = m_MaxExposure;
        changes = true;
    }

    return changes;
}

// Initial gain value
bool CAutomaticLevelControl::SetCurrentGain(int gain)
{
    m_CurrentGain = gain;
    return true;
}

// Initial exposure value
bool CAutomaticLevelControl::SetCurrentExposure(int exposure)
{
    m_CurrentExposure = exposure;
    return true;
}

// Relaive threshold value around setpoint
bool CAutomaticLevelControl::SetThreshold(double threshold)
{
    m_Threshold = threshold;
    return ReCalculate();
}

// Set closed-loop "Gain" for the Gain
bool CAutomaticLevelControl::SetKGain(double kGain)
{
    m_KGain = kGain;
    return ReCalculate();
}

// Set closed-loop "Gain" for the Exposure
bool CAutomaticLevelControl::SetKExposure(double kExposure)
{
    m_KExposure = kExposure;
    return ReCalculate();
}

// Get the current Gain value
int CAutomaticLevelControl::GetCurrentGain()
{
    return m_CurrentGain;
}

// Get the current Exposure value
int CAutomaticLevelControl::GetCurrentExposure()
{
    return m_CurrentExposure;
}

EALCType CAutomaticLevelControl::GetALCType()
{
    return m_ALCType;
}

int CAutomaticLevelControl::GetSetPoint()
{
    return m_SetPoint;
}

int CAutomaticLevelControl::GetMinGain()
{
    return m_MinGain;
}

int CAutomaticLevelControl::GetMaxGain()
{
    return m_MaxGain;
}

int CAutomaticLevelControl::GetMinExposure()
{
    return m_MinExposure;
}

int CAutomaticLevelControl::GetMaxExposure()
{
    return m_MaxExposure;
}

double CAutomaticLevelControl::GetThreshold()
{
    return m_Threshold;
}

double CAutomaticLevelControl::GetKGain()
{
    return m_KGain;
}

double CAutomaticLevelControl::GetKExposure()
{
    return m_KExposure;
}

bool CAutomaticLevelControl::AdjustGain(double error)
{
    bool changes = false;
    int newGain = m_CurrentGain + (int)(m_KGain * error);

    // Limit the range of the Gain value
    if (newGain > m_MaxGain)
        newGain = m_MaxGain;

    if (newGain < m_MinGain)
        newGain = m_MinGain;

    // Did we change gain?
    if (newGain != m_CurrentGain)
    {
        m_CurrentGain = newGain;
        changes = true;
    }
    return changes;
}

bool CAutomaticLevelControl::AdjustExposureTime(double error)
{
    bool changes = false;
    int newExposureTime = m_CurrentExposure + (int)(m_KExposure * error * (double)(log((double)m_CurrentExposure)/log(2.0)));

    // Check to see if the values are close to minimum

    // Limit the range of the Exposure value
    if (newExposureTime > m_MaxExposure)
        newExposureTime = m_MaxExposure;

    if (newExposureTime < m_MinExposure)
        newExposureTime = m_MinExposure;

    // Did we change Exposure?
    if (newExposureTime != m_CurrentExposure)
    {
        m_CurrentExposure = newExposureTime;
        changes = true;
    }
    return changes;
}


