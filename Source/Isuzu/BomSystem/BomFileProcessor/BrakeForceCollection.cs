using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Xml.Serialization;

namespace BomFileProcessor
{
    public class BrakeForceCollection
    {
        //-----------------------------------------------------------------------------------------
        // Member Functions
        //-----------------------------------------------------------------------------------------
        /// <summary>
        /// Create a new brake force collection.
        /// </summary>
        /// <param name="logger">Logger object so activites can be logged.</param>
        public BrakeForceCollection(Logger logger)
        {   // Save the logger
            m_logger = logger;
            m_brakeParameters = new List<BrakeForce>();
            Load();
        }

        /// <summary>
        /// Add a new brake force parameter set.
        /// </summary>
        /// <param name="modelCode">Model code of the vehicle this set belongs to.</param>
        /// <param name="frontMaxDrag">Maximum drag force for each front wheel.</param>
        /// <param name="rearMaxDrag">Maximum drag force for each rear wheel.</param>
        /// <param name="frontMinBrake">Minimum brake force required for each front wheel.</param>
        /// <param name="frontMaxBrake">Maximum brake force allowed for each front wheel.</param>
        /// <param name="rearMinBrake">Minimum brake force required for each rear wheel.</param>
        /// <param name="rearMaxBrake">Maximum brake force allowed for each rear wheel.</param>
        /// <param name="frontBalanceOffset">Front side to side brake balance offset from 50%.</param>
        /// <param name="rearBalanceOffset">Rear side to sode brake balance offset from 50%.</param>
        /// <param name="frontToRearBalMin">Front to rear balance minimum.</param>
        /// <param name="frontToRearBalMax">Front to rear balance maximum.</param>
        public void Add(String modelCode, Double frontMaxDrag, Double rearMaxDrag,
                        Double frontMinBrake, Double frontMaxBrake, Double rearMinBrake, Double rearMaxBrake,
                        Double frontBalanceOffset, Double rearBalanceOffset, Double frontToRearBalMin,
                        Double frontToRearBalMax)
        {   // Remove the existing item so it can be re-added as a new item
            RemoveBrakeForceSet(modelCode);
            // Create a new brake force set and add it to the list
            BrakeForce brakeForceSet = new BrakeForce(modelCode, frontMaxDrag, rearMaxDrag,
                                                      frontMinBrake, frontMaxBrake, rearMinBrake, rearMaxBrake,
                                                      frontBalanceOffset, rearBalanceOffset, frontToRearBalMin,
                                                      frontToRearBalMax);
            m_brakeParameters.Add(brakeForceSet);
            m_logger.Log("INFO:  Added brake force set for " + modelCode);
            // Save the updated list
            Save();
        }

        /// <summary>
        /// Find the brake force set for the specified model code.
        /// </summary>
        /// <param name="modelCode">Model code to find the brake force set of.</param>
        /// <returns>Brake force set for the specified model code.</returns>
        public BrakeForce Find(String modelCode)
        {   // Search the list for the specified model code
            foreach (BrakeForce brakeForceSet in m_brakeParameters)
            {   // Return the brake force set if it is for the model code we are looking for
                if (brakeForceSet.ModelCode == modelCode) return brakeForceSet;
            }
            // No matching brake force set
            return null;
        }

        /// <summary>
        /// Load the option associations for this vehicle option
        /// </summary>
        public void Load()
        {   // Create a new list of options
            m_brakeParameters = new List<BrakeForce>();
            try
            {   // Load the option association file
                if (File.Exists("BrakeForces.xml"))
                {   // Read the options from the file
                    XmlSerializer mySerializer = new XmlSerializer(typeof(List<BrakeForce>));
                    using (StreamReader reader = new StreamReader("BrakeForces.xml"))
                    {
                        m_brakeParameters = (List<BrakeForce>)mySerializer.Deserialize(reader);
                    }
                }
                else
                {
                    m_logger.Log("WARNING:  " + "BrakeForces.xml does not exist, please add brake force parameters");
                    File.Create("BrakeForces.xml");
                }
            }
            catch
            {
                m_logger.Log("ERROR:  Exception loading brake forces, no brake force sets have been loaded");
            }
        }

        /// <summary>
        /// Get the list of model codes that have brake force parameters configured.
        /// </summary>
        /// <returns>List of model codes that have brake force parameters configured.</returns>
        public List<String> ConfiguredModelCodes()
        {
            List<String> modelCodes = new List<String>();
            // Add each of the model codes to the list
            foreach (BrakeForce paramSet in m_brakeParameters)
            {
                modelCodes.Add(paramSet.ModelCode);
            }
            return modelCodes;
        }

        /// <summary>
        /// Remove the brake force set for the specified model code.
        /// </summary>
        /// <param name="modelCode">Model code to remove the brake force set from.</param>
        public void RemoveBrakeForceSet(String modelCode)
        {   // Find the brake parameter set
            BrakeForce paramSet = Find(modelCode);
            if (paramSet != null)
            {   // Remove the set from the list
                m_brakeParameters.Remove(paramSet);
                m_logger.Log("INFO:  Removed brake parameter set from model code: " + modelCode);
                // Save the updated list
                Save();
            }
        }

        /// <summary>
        /// Save the updated brake force parameter sets.
        /// </summary>
        public void Save()
        {
            try
            {   // Save the brake parameter sets to xml
                XmlSerializer mySerializer = new XmlSerializer(typeof(List<BrakeForce>));
                using (StreamWriter writer = new StreamWriter("BrakeForces.xml"))
                {
                    mySerializer.Serialize(writer, m_brakeParameters);
                }
            }
            catch
            {
                m_logger.Log("ERROR:  Exception saving brake parameter sets to BrakeForces.xml");
            }
        }


        //-----------------------------------------------------------------------------------------
        // Member Variables and Properties
        //-----------------------------------------------------------------------------------------
        /// <summary>
        /// local copy of the logger so data can be logged.
        /// </summary>
        Logger m_logger;

        /// <summary>
        /// List of configured brake force parameter sets.
        /// </summary>
        List<BrakeForce> m_brakeParameters;
    }
}
