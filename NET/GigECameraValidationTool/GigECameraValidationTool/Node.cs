using System;
using System.Collections.Generic;
using System.Text;

namespace GigECameraValidationTool
{
    enum ValidationReturnCode { Passed, NotPassed, NullValue, Unknown};
    
    class Node
    {
        private string parentDisplayName;
        private string displayName;
        private string genIcamName;

        public Node(string _parentDisplayName, string _displayName, string _genIcamName)
        {
            parentDisplayName = _parentDisplayName;
            displayName = _displayName;
            genIcamName = _genIcamName;
        }

        public string GenIcamName
        {
            get { return genIcamName; }
        }
        public string ParentDisplayName
        {
            get { return parentDisplayName; }
        }
        public string DisplayName
        {
            get { return displayName; }
        }
    }

    class IntegerTypeNode : Node
    {
        private Int64 minValue;
        private Int64 maxValue;
        private Int64 actualValue;

        public IntegerTypeNode(string _parentDisplayName, string _displayName, string _genIcamName, 
            Int64 _minValue, Int64 _maxValue, Int64 _actualValue) : base(_parentDisplayName, _displayName, _genIcamName)
        {
            minValue = _minValue;
            maxValue = _maxValue;
            actualValue = _actualValue;
        }

        public Int64 MinValue
        {
            get { return minValue; }
        }
        public Int64 MaxValue
        {
            get { return maxValue; }
        }
        public Int64 ActualValue
        {
            get { return actualValue; }
        }
    }

    class FloatTypeNode : Node
    {
        private double minValue;
        private double maxValue;
        private double actualValue;

        public FloatTypeNode(string _parentDisplayName, string _displayName, string _genIcamName, 
            double _minValue, double _maxValue, double _actualValue)
            : base(_parentDisplayName, _displayName, _genIcamName)
        {
            minValue = _minValue;
            maxValue = _maxValue;
            actualValue = _actualValue;
        }

        public double MinValue
        {
            get { return minValue; }
        }
        public double MaxValue
        {
            get { return maxValue; }
        }
        public double ActualValue
        {
            get { return actualValue; }
        }
    }

    class EnumTypeNode : Node
    {
        private string actualValue;

        public EnumTypeNode(string _parentDisplayName, string _displayName, string _genIcamName, string _actualValue) 
            : base(_parentDisplayName, _displayName, _genIcamName)
        {
            actualValue = _actualValue;
        }

        public string ActualValue
        {
            get { return this.actualValue; }
        }
    }
}
