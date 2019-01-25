using System;
using System.Collections.Generic;
using System.Text;
using System.Xml;
using System.IO;
using System.Windows.Forms;

namespace ToyotaParameterEditor
{
    public class ParameterFile
    {
        public ParameterFile()
        {
            m_vehicleParameters = new XmlDocument();
            m_parameterDisplayNames = new XmlDocument();
            LoadParameterDisplayNames();
            ParametersChanged = false;
        }

        /// <summary>
        /// Load the contents of the specified vehicle parameter file.
        /// </summary>
        /// <param name="fileName">Vehicle parameter file.</param>
        public void LoadParameterFile(String fileName)
        {   // Save the file name so any changes can be saved
            ParameterFileName = fileName;
            m_vehicleParameters.Load(ParameterFileName);
            ParameterFileLoaded = true;
            ParametersChanged = false;
        }

        /// <summary>
        /// Save the parameter back to file.
        /// </summary>
        public void SaveParameterFile()
        {
            if (ParametersChanged)
            {   // Check if this is the template file
                if (ParameterFileName == ToyotaParameterEditor.Properties.Resources.NewVehicleTemplateFileName)
                {   // Save to the vehicle file name
                    ParameterFileName = ToyotaParameterEditor.Properties.Resources.ParameterFilePath + "Vin" + GetVehicleParameter("BodyStyle") + ".xml";
                }
                if (Directory.Exists(ToyotaParameterEditor.Properties.Resources.ParameterFilePath))
                {
                    m_vehicleParameters.Save(ParameterFileName);
                }
                else
                {
                    MessageBox.Show("Unable to access " + ToyotaParameterEditor.Properties.Resources.ParameterFilePath +
                                    "\n\nConfiguration data not saved",
                                    "Folder Access Error",
                                    MessageBoxButtons.OK, MessageBoxIcon.Error);
                }
                ParametersChanged = false;
            }
        }

        /// <summary>
        /// Load the display text for each parameter.
        /// </summary>
        private void LoadParameterDisplayNames()
        {   // Load the parameter display name file
            m_parameterDisplayNames.Load(ToyotaParameterEditor.Properties.Resources.ParameterDisplayNameFile);
        }

        /// <summary>
        /// Get the value of the specified vehicle parameter.
        /// </summary>
        /// <param name="parameterName">Name of the parameter to retrieve.</param>
        /// <returns>Value of the specified parameter.</returns>
        public String GetVehicleParameter(String parameterName)
        {
            String value = " ";
            XmlNode parameterNode = VehicleParameterList.DocumentElement.SelectSingleNode("VehicleBuild/" + parameterName);
            if (parameterNode != null)
            {
                if (parameterNode.FirstChild != null)
                {
                    value = parameterNode.FirstChild.Value;
                }
            }
            return value;
        }

        /// <summary>
        /// Update the specified parameter to the value provided.
        /// </summary>
        /// <param name="parameterName">Name of parameter to update.</param>
        /// <param name="parameterValue">New parameter value.</param>
        public void UpdateVehicleParameter(String parameterName, String parameterValue)
        {   // Update the parameter node
            if (VehicleParameterList.DocumentElement.SelectSingleNode("VehicleBuild/" + parameterName).FirstChild == null)
            {   // Create a new node with a value and replace the existing node that has no value
                String tempName = parameterName.Substring(parameterName.LastIndexOf("/") + 1);
                XmlElement newValue = VehicleParameterList.CreateElement(tempName);
                newValue.InnerText = parameterValue;
                // If the original node has a units attribute, it must be inserted in the new node
                if (VehicleParameterList.DocumentElement.SelectSingleNode("VehicleBuild/" + parameterName).Attributes.Count > 0)
                {
                    newValue.SetAttribute("Units", VehicleParameterList.DocumentElement.SelectSingleNode("VehicleBuild/" + parameterName).Attributes.GetNamedItem("Units").FirstChild.Value);
                }
                // Find out where the child node lives
                if (parameterName.IndexOf("/") == -1)
                {
                    VehicleParameterList.DocumentElement.SelectSingleNode("VehicleBuild").ReplaceChild(newValue,
                                                                                                       VehicleParameterList.DocumentElement.SelectSingleNode("VehicleBuild/" + parameterName));
                }
                else
                {   // Find where the child node actually is
                    String nodePath = "VehicleBuild/" + parameterName.Substring(0, parameterName.LastIndexOf("/"));
                    VehicleParameterList.DocumentElement.SelectSingleNode(nodePath).ReplaceChild(newValue,
                                                                                                 VehicleParameterList.DocumentElement.SelectSingleNode(nodePath + "/" + tempName));
                }
            }
            else
            {
                VehicleParameterList.DocumentElement.SelectSingleNode("VehicleBuild/" + parameterName).FirstChild.Value = parameterValue;
            }
            ParametersChanged = true;
        }

        /// <summary>
        /// Get the units for the specified vehicle parameter.
        /// </summary>
        /// <param name="parameterName">Vehicle parameter to get the units for.</param>
        /// <returns>Units for the specified vehicle parameter.</returns>
        public String GetParameterUnits(String parameterName)
        {
            String value = " ";
            XmlNode parameterNode = VehicleParameterList.DocumentElement.SelectSingleNode("VehicleBuild/" + parameterName);
            if (parameterNode != null) value = parameterNode.Attributes.GetNamedItem("Units").Value;
            return value;
        }

        /// <summary>
        /// Get the text to display for the specified parameter.
        /// </summary>
        /// <param name="parameterName">Parameter to get the display text for.</param>
        /// <returns>Display text for the specified parameter.</returns>
        public String GetParameterDisplayText(String parameterName)
        {
            String text = " ";
            XmlNode displayNode = ParameterDiaplyNames.DocumentElement.SelectSingleNode(parameterName);
            if (displayNode != null) text = displayNode.FirstChild.Value;
            return text;
        }

        /// <summary>
        /// Get the parameter name for the specified display text.
        /// </summary>
        /// <param name="displayText">Display text for the desired parameter.</param>
        /// <returns>Name of the parameter that has the specified display text.</returns>
        public String GetParameterName(String displayText)
        {
            String name = " ";
            foreach (XmlNode paramNode in ParameterDiaplyNames.DocumentElement.ChildNodes)
            {
                if (paramNode.NodeType == XmlNodeType.Element)
                {
                    if (paramNode.FirstChild.Value == displayText) name = paramNode.Name;
                }
            }
            return name;
        }

        /// <summary>
        /// Get/Set the name of the parameter file.
        /// </summary>
        private String m_paramFileName;
        public String ParameterFileName
        {
            get { return m_paramFileName; }
            set { m_paramFileName = value; }
        }

        /// <summary>
        /// Get/Set the flag idicating if a parameter file is currently loaded.
        /// </summary>
        private Boolean m_fileLoaded = false;
        private Boolean ParameterFileLoaded
        {
            get { return m_fileLoaded; }
            set { m_fileLoaded = value; }
        }

        /// <summary>
        /// Get/Set the flag indicating if any parameters have been updated since the last save
        /// </summary>
        private Boolean m_parametersChanged;
        public Boolean ParametersChanged
        {
            get { return m_parametersChanged; }
            set { m_parametersChanged = value; }
        }

        /// <summary>
        /// Get/Set the vehicle parameters
        /// </summary>
        private XmlDocument m_vehicleParameters;
        private XmlDocument VehicleParameterList
        {
            get { return m_vehicleParameters; }
            set { m_vehicleParameters = value; }
        }

        /// <summary>
        /// Get/Set the parameter display name translation
        /// </summary>
        private XmlDocument m_parameterDisplayNames;
        private XmlDocument ParameterDiaplyNames
        {
            get { return m_parameterDisplayNames; }
            set { m_parameterDisplayNames = value; }
        }
    }
}
