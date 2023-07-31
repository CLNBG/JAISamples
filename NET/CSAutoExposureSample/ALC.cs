using System;
using System.Collections.Generic;
using System.Text;

namespace CSAutoExposureSample
{
    class ALC
    {
        public enum EALCType
        {
            MinGainPriority = 0,
            MinExposurePriority = 1,
            GainPriority = 2,
            ExposurePriority = 3,
            GainOnly = 4,
            ExposureOnly = 5,
            Undefined
        };


        private EALCType m_ALCType = EALCType.MinGainPriority;      // Type of ALC control
        private int m_MinGain;                                      // Minimum Gain value
        private int m_MaxGain;                                      // Maximum Gain value
        private int m_CurrentGain;                                  // Current Gain value
        private int m_MinExposure;                                  // Minimum Exposure Time value
        private int m_MaxExposure;                                  // Maximum Exposure Time value
        private int m_CurrentExposure;                              // Current Exposure Time value

        private int m_SetPoint;                                     // This is the target average for the Closed-Loop control
        private int m_Average;                                      // This is the last known average input for the Closed-Loop control

        private double m_KGain;                                     // Gain for the Gain adjustments
        private double m_KExposure;                                 // Gain for the Exposure Time adjustments
        private double m_Threshold;                                 // Threshold value for the error

        private bool ReCalculate()
        {
            return Calculate(m_Average);
        }

        public bool Calculate(int input)
        {
            m_Average = input;

            // We can now continue with the actual Closed-Loop calculations
            double relativeError = ((double)m_SetPoint - (double)input) / (double)m_SetPoint;

            bool changes = false;

            switch (m_ALCType)
            {
                case EALCType.MinGainPriority:
                    // We typically only want to adjust one of the camera settings at a time
                    // and typically it makes sense to minimize the Gain in order to get as
                    // little noise as possible in the image.

                    // Do we need to increase Gain/Exposure time?
                    if (relativeError > m_Threshold)
                    {
                        // As long as we can expose longer then we want to force the Gain to a minimum!
                        if ((m_CurrentExposure < m_MaxExposure) && (m_CurrentGain > m_MinGain))
                        {
                            changes = AdjustGain(-relativeError / 2);
                            changes |= AdjustExposureTime(relativeError * 2);
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

                case EALCType.MinExposurePriority:
                    // We typically only want to adjust one of the camera settings at a time
                    // and typically it makes sense to minimize the Gain in order to get as
                    // little noise as possible in the image.

                    // Do we need to increase Gain/Exposure time?
                    if (relativeError > m_Threshold)
                    {
                        // As long as we can increase Gain then we want to force the Exposure Time to a minimum!
                        if ((m_CurrentGain < m_MaxGain) && (m_CurrentExposure > m_MinExposure))
                        {
                            changes = AdjustExposureTime(-relativeError / 2);
                            changes |= AdjustGain(relativeError * 2);
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

                case EALCType.GainPriority:
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

                case EALCType.ExposurePriority:
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

                case EALCType.GainOnly:
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

                case EALCType.ExposureOnly:
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
        public bool SetALCType(EALCType type)
        {
            m_ALCType = type;
            return ReCalculate();
        }

        // Set target for the closed-loop control
        public bool SetSetPoint(int setpoint)
        {
            m_SetPoint = setpoint;
            return ReCalculate();
        }

        // Set the min gain for the closed-loop control
        public bool SetMinGain(int min)
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
        public bool SetMaxGain(int max)
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
        public bool SetMinExposure(int min)
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
        public bool SetMaxExposure(int max)
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
        public bool SetCurrentGain(int gain)
        {
            m_CurrentGain = gain;
            return true;
        }

        // Initial exposure value
        public bool SetCurrentExposure(int exposure)
        {
            m_CurrentExposure = exposure;
            return true;
        }

        // Relaive threshold value around setpoint
        public bool SetThreshold(double threshold)
        {
            m_Threshold = threshold;
            return ReCalculate();
        }

        // Set closed-loop "Gain" for the Gain
        public bool SetKGain(double kGain)
        {
            m_KGain = kGain;
            return ReCalculate();
        }

        // Set closed-loop "Gain" for the Exposure
        public bool SetKExposure(double kExposure)
        {
            m_KExposure = kExposure;
            return ReCalculate();
        }

        // Get the current Gain value
        public int GetCurrentGain()
        {
            return m_CurrentGain;
        }

        // Get the current Exposure value
        public int GetCurrentExposure()
        {
            return m_CurrentExposure;
        }

        public EALCType GetALCType()
        {
            return m_ALCType;
        }

        public int GetSetPoint()
        {
            return m_SetPoint;
        }

        public int GetMinGain()
        {
            return m_MinGain;
        }

        public int GetMaxGain()
        {
            return m_MaxGain;
        }

        public int GetMinExposure()
        {
            return m_MinExposure;
        }

        public int GetMaxExposure()
        {
            return m_MaxExposure;
        }

        public double GetThreshold()
        {
            return m_Threshold;
        }

        public double GetKGain()
        {
            return m_KGain;
        }

        public double GetKExposure()
        {
            return m_KExposure;
        }

        private bool AdjustGain(double error)
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

        private bool AdjustExposureTime(double error)
        {
            bool changes = false;
            int newExposureTime = m_CurrentExposure + (int)(m_KExposure * error * (double)(Math.Log((double)m_CurrentExposure) / Math.Log(2.0)));

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
    }
}
