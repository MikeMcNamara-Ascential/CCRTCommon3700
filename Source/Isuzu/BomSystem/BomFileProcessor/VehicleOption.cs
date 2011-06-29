using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace BomFileProcessor
{
    public class VehicleOption
    {
        /// <summary>
        /// Create a new vehicle option association.
        /// </summary>
        public VehicleOption()
        {
        }

        /// <summary>
        /// Create a new vehicle option for the specified model code.
        /// </summary>
        /// <param name="modelCode">Model code of the vehicle.</param>
        /// <param name="optionValue">Option value to be stored.</param>
        public VehicleOption(String modelCode, String optionValue)
        {
            OptionValue = optionValue;
            ModelCode = modelCode;
        }

        /// <summary>
        /// Model code for the vehicle.
        /// </summary>
        private String m_modelCode;
        /// <summary>
        /// Get/Set the model code for this vehicle.
        /// </summary>
        public String ModelCode
        {
            get { return m_modelCode; }
            set { m_modelCode = value; }
        }

        /// <summary>
        /// Vehicle option.
        /// </summary>
        private String m_optionValue;
        /// <summary>
        /// Get/Set the Option Name for this vehicle type.
        /// </summary>
        public String OptionValue
        {
            get { return m_optionValue; }
            set { m_optionValue = value; }
        }
    }
}
