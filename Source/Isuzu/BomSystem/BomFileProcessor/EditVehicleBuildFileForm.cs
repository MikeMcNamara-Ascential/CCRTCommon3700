using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using System.Xml;

namespace BomFileProcessor
{
    public partial class EditVehicleBuildFileForm : Form
    {
        //-----------------------------------------------------------------------------------------
        // Member Functions
        //-----------------------------------------------------------------------------------------
        /// <summary>
        /// Create a new parameter editor form for the specified parameter file.
        /// </summary>
        /// <param name="fileName">Name of the parameter file to edit.</param>
        public EditVehicleBuildFileForm(String fileName, VehicleOptionCollection axleTypes, 
                                        BrakeForceCollection brakeForces, VehicleOptionCollection wheelbases)
        {   // Initialize the form
            InitializeComponent();
            // Store the name of the file so we can save the data to file later
            ParamFileName = fileName;
            m_fileNameLabel.Text = ParamFileName;
            // Store the configured items in case the user wants to set to configured values.
            m_axleTypes = axleTypes;
            m_brakeForces = brakeForces;
            m_wheelbases = wheelbases;
            // Get the model code from the file name so that we can look up defaults if needed
            ModelCode = fileName.Substring(fileName.LastIndexOf("\\") + 3, 5);
            // Load the file
            m_vehicleBuild = new XmlDocument();
            m_vehicleBuild.Load(ParamFileName);
            // Display the values
            PopulateBuildDataInForm();
        }

        /// <summary>
        /// Data will be loaded into the form so it can be edited.
        /// </summary>
        private void PopulateBuildDataInForm()
        {   // Populate all build options
            XmlNodeList buildNodes = m_vehicleBuild.SelectSingleNode("Vin/VehicleBuild").ChildNodes;
            m_buildOptionDataGridView.Rows.Clear();
            foreach (XmlNode node in buildNodes)
            {   // place the item in the build option table
                if (node.Name != "BrakeForces")
                {   
                    String[] row = { node.Name, node.InnerText };
                    m_buildOptionDataGridView.Rows.Add(row);
                }
            }
            // Populate the brake force parameters
            m_brakeParamDataGridView.Rows.Clear();
            // Add two rows to the table
            String[] front = { "Front", "", "", "", "" };
            String[] rear = { "Rear", "", "", "", "" };
            m_brakeParamDataGridView.Rows.Add(front);
            m_brakeParamDataGridView.Rows.Add(rear);
            try
            {
                XmlNodeList brakeNodes = m_vehicleBuild.SelectSingleNode("Vin/VehicleBuild/BrakeForces").ChildNodes;
                foreach (XmlNode node in brakeNodes)
                {   // Determine if this is the front or rear
                    Int32 rowIndex = node.Name.Contains("Front") ? 0 : 1;
                    String colName = (rowIndex == 0) ? node.Name.Substring("Front".Length) : node.Name.Substring("Rear".Length);
                    m_brakeParamDataGridView.Rows[rowIndex].Cells[colName].Value = node.InnerText;
                }
            }
            catch (NullReferenceException)
            {
            }
        }

        /// <summary>
        /// Set the value of the specified parameter.
        /// </summary>
        /// <param name="path">XPath to the parameter node.</param>
        /// <param name="paramName">Parameter to be changed.</param>
        /// <param name="paramValue">New value of the parameter.</param>
        private void SetVehicleParameter(String path, String paramName, String paramValue)
        {   // Select the parameter node and set the value
            XmlNode paramNode = m_vehicleBuild.SelectSingleNode(path + "/" + paramName);
            if (paramNode == null)
            {   // Node does not exist, create it
                XmlElement newItem = m_vehicleBuild.CreateElement(paramName);
                XmlNode temp = m_vehicleBuild.SelectSingleNode(path);
                if (temp == null)
                {
                    XmlElement newNode = m_vehicleBuild.CreateElement(path.Substring(path.LastIndexOf("/") + 1));
                    m_vehicleBuild.SelectSingleNode(path.Substring(0, path.LastIndexOf("/"))).AppendChild(newNode);
                    temp = m_vehicleBuild.SelectSingleNode(path);
                }
                temp.AppendChild(newItem);
                paramNode = m_vehicleBuild.SelectSingleNode(path + "/" + paramName);
            }
            paramNode.InnerText = paramValue;
        }



        //-----------------------------------------------------------------------------------------
        // Callback Functions
        //-----------------------------------------------------------------------------------------
        /// <summary>
        /// Editing has been completed, close the form.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void m_closeButton_Click(object sender, EventArgs e)
        {
            Close();
        }

        /// <summary>
        /// Save all the parameters back to file.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void m_saveButton_Click(object sender, EventArgs e)
        {   // Save all the Vehicle options
            foreach (DataGridViewRow row in m_buildOptionDataGridView.Rows)
            {
                SetVehicleParameter("Vin/VehicleBuild", row.Cells["BuildOption"].Value.ToString(), row.Cells["Value"].Value.ToString());
            }
            // Save all the brake force parameters
            foreach (DataGridViewRow row in m_brakeParamDataGridView.Rows)
            {
                String axle = row.Cells[0].Value.ToString();
                if (row.Cells["MaxDragForce"].Value != null)
                    SetVehicleParameter("Vin/VehicleBuild/BrakeForces", axle + "MaxDragForce", row.Cells["MaxDragForce"].Value.ToString());
                if (row.Cells["MinBrakeForce"].Value != null)
                    SetVehicleParameter("Vin/VehicleBuild/BrakeForces", axle + "MinBrakeForce", row.Cells["MinBrakeForce"].Value.ToString());
                if (row.Cells["MaxBrakeForce"].Value != null)
                    SetVehicleParameter("Vin/VehicleBuild/BrakeForces", axle + "MaxBrakeForce", row.Cells["MaxBrakeForce"].Value.ToString());
                if (row.Cells["SideToSideBalance"].Value != null)
                    SetVehicleParameter("Vin/VehicleBuild/BrakeForces", axle + "SideToSideBalance", row.Cells["SideToSideBalance"].Value.ToString());
            }
            // Write the data back to file
            m_vehicleBuild.Save(ParamFileName);
        }

        /// <summary>
        /// Update the items in the checked box to configured values.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void m_updateToConfigValButton_Click(object sender, EventArgs e)
        {   // Update the Build Options if the check box is selected
            if (m_buildoptionsCheckBox.Checked || m_brkForceCheckBox.Checked)
            {
                if (m_buildoptionsCheckBox.Checked)
                {   // Update the wheelbase and axle type to configured values
                    VehicleOption wheelbase = m_wheelbases.Find(ModelCode);
                    if (wheelbase != null)
                    {
                        SetVehicleParameter("Vin/VehicleBuild", "WheelbasePositionInchesX10", wheelbase.OptionValue);
                    }
                    VehicleOption axle = m_axleTypes.Find(ModelCode);
                    if (axle != null)
                    {
                        SetVehicleParameter("Vin/VehicleBuild", "Axle", axle.OptionValue);
                    }
                }
                if (m_brkForceCheckBox.Checked)
                {
                    BrakeForce defForce = m_brakeForces.Find(ModelCode);
                    if (defForce != null)
                    {
                        SetVehicleParameter("Vin/VehicleBuild/BrakeForces", "FrontMaxDragForce", defForce.FrontMaxDragForce.ToString());
                        SetVehicleParameter("Vin/VehicleBuild/BrakeForces", "FrontMinBrakeForce", defForce.FrontMinBrakeForce.ToString());
                        SetVehicleParameter("Vin/VehicleBuild/BrakeForces", "FrontMaxBrakeForce", defForce.FrontMaxBrakeForce.ToString());
                        SetVehicleParameter("Vin/VehicleBuild/BrakeForces", "FrontSideToSideBalance", defForce.FrontBalanceOffset.ToString());
                        SetVehicleParameter("Vin/VehicleBuild/BrakeForces", "RearMaxDragForce", defForce.RearMaxDragForce.ToString());
                        SetVehicleParameter("Vin/VehicleBuild/BrakeForces", "RearMinBrakeForce", defForce.RearMinBrakeForce.ToString());
                        SetVehicleParameter("Vin/VehicleBuild/BrakeForces", "RearMaxBrakeForce", defForce.RearMaxBrakeForce.ToString());
                        SetVehicleParameter("Vin/VehicleBuild/BrakeForces", "RearSideToSideBalance", defForce.RearBalanceOffset.ToString());
                        SetVehicleParameter("Vin/VehicleBuild/BrakeForces", "BalanceFrontToRearMin", defForce.FrontToRearBalanceMin.ToString());
                        SetVehicleParameter("Vin/VehicleBuild/BrakeForces", "BalanceFrontToRearMax", defForce.FrontToRearBalanceMax.ToString());
                    }
                }
                // Redisplay the parameter in the display
                PopulateBuildDataInForm();
            }
        }



        //-----------------------------------------------------------------------------------------
        // Member Variables and Properties
        //-----------------------------------------------------------------------------------------
        /// <summary>
        /// Name of the parameter file being edited.
        /// </summary>
        private String m_paramFileName;
        /// <summary>
        /// Get/Set the name of the parameter file being edited.
        /// </summary>
        private String ParamFileName
        {
            get { return m_paramFileName; }
            set { m_paramFileName = value; }
        }

        /// <summary>
        /// Model code of the selected parameter file.
        /// </summary>
        private String m_modelCode;
        /// <summary>
        /// Get/Set the model code for the selected parameter set.
        /// </summary>
        private String ModelCode
        {
            get { return m_modelCode; }
            set { m_modelCode = value; }
        }

        /// <summary>
        /// Vehicle build data from the file.
        /// </summary>
        private XmlDocument m_vehicleBuild;

        /// <summary>
        /// Configured axle types.
        /// </summary>
        private VehicleOptionCollection m_axleTypes;

        /// <summary>
        /// Configured wheelbase positions.
        /// </summary>
        private VehicleOptionCollection m_wheelbases;

        /// <summary>
        /// Configured brake force sets.
        /// </summary>
        private BrakeForceCollection m_brakeForces;
    }
}
