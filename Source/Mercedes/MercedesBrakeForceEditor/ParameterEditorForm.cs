using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using System.Xml;

namespace MercedesBrakeForceEditor
{
    public partial class ParameterEditorForm : Form
    {
        //-----------------------------------------------------------------------------------------
        // Member Functions
        //-----------------------------------------------------------------------------------------
        /// <summary>
        /// 
        /// </summary>
        public ParameterEditorForm()
        {
            InitializeComponent();
            // Set the icon for the form
            Icon = Icon.FromHandle(MercedesBrakeForceEditor.Properties.Resources.bep_best.GetHicon());
            // Make sure the vehicle build directory is set to a non-default value
            if (MercedesBrakeForceEditor.Properties.Settings.Default.VehicleBuildFolder ==
                MercedesBrakeForceEditor.Properties.Resources.DefaultVehicleBuildFolder)
            {   // Make the user select the vehicle build directory
                SetupVehicleBuildDirectory();
            }
            // Initialize the form
            InitializeForm();
            // Load the vehicle parameter template
            m_vehicleParameters = new XmlDocument();
            m_vehicleParameters.Load(MercedesBrakeForceEditor.Properties.Settings.Default.VehicleBuildFolder + "\\VehicleParameterTemplate.xml");
        }

        /// <summary>
        /// Convert the force difference to a percentage of the brake force.
        /// </summary>
        /// <param name="forceDiff">Force difference specified by the user.</param>
        /// <param name="axle">Axle this balance will be appied to.</param>
        /// <returns>Percent balance.</returns>
        private Double ConvertForceDiffToPercent(Double forceDiff, String axle)
        {
            Double percent = 50.0;
            // Get the min/max parameters from the existing parameters
            Double minParam = Convert.ToDouble(GetParameterValue(axle + "MinBrakeForce"));
            Double maxParam = Convert.ToDouble(GetParameterValue(axle + "MaxBrakeForce"));
            // Get the average brake force as this should be our target
            Double avgBrakeForce = (minParam + maxParam) / 2.0;
            if (avgBrakeForce > 0)
            {
                percent = ((ConvertParameterValue(ConversionTypes.KgfLbf, forceDiff) / avgBrakeForce) * 100.0) / 2.0;
            }
            else
            {
                percent = 0.0;
            }
            return percent;
        }

        /// <summary>
        /// Convert the brake balance percent to a force difference.
        /// </summary>
        /// <param name="percent">Brake balance percent.</param>
        /// <param name="axle">Axle this percent is applied to.</param>
        /// <returns>Force difference corresponding to the balance percentage.</returns>
        private Double ConvertBalancePercentToForceDiff(Double percent, String axle)
        {
            Double forceDiff = 0.0;
            // Get the min/max parameters from the existing parameters
            Double minParam = Convert.ToDouble(GetParameterValue(axle + "MinBrakeForce"));
            Double maxParam = Convert.ToDouble(GetParameterValue(axle + "MaxBrakeForce"));
            // Get the average brake force as this should be our target
            Double avgBrakeForce = (minParam + maxParam) / 2.0;
            if (avgBrakeForce > 0)
            {
                forceDiff = ConvertParameterValue(ConversionTypes.LbfKgf, (percent / 100.0) * avgBrakeForce * 2.0);
            }
            else
            {
                forceDiff = 0.0;
            }
            return forceDiff;
        }

        /// <summary>
        /// Convert a parameter from metric to english units and return the converted value.
        /// </summary>
        /// <param name="columnIndex">Column from the data grid so we know what type of data we are converting.</param>
        /// <param name="value">Metric value of the parameter.</param>
        /// <returns>Parameter valus converted to english units.</returns>
        private Double ConvertParameterValue(ConversionTypes type, Double value)
        {
            Double conversionFactor = 1.0;
            switch (type)
            {
                case ConversionTypes.KgfLbf:
                    conversionFactor = 2.204623;  // KGF to LBF
                    break;

                case ConversionTypes.LbfKgf:
                    conversionFactor = 0.4535924;
                    break;

                case ConversionTypes.MeterFeet:
                    conversionFactor = 3.28083;  // meter to feet
                    break;

                case ConversionTypes.FeetMeter:
                    conversionFactor = 0.3048;
                    break;

                case ConversionTypes.KmhMph:
                    conversionFactor = 0.6213712;
                    break;

                case ConversionTypes.MphKmh:
                    conversionFactor = 1.609344;
                    break;

                default:
                    conversionFactor = 1.0;
                    break;
            }
            return value * conversionFactor;
        }

        /// <summary>
        /// Get the value of the specified parameter.
        /// </summary>
        /// <param name="parameterName">Name of the parameter.</param>
        /// <param name="path">Path where to find the specified parameter.</param>
        /// <returns>Value of the specified parameter.</returns>
        private String GetParameterValue(String parameterName, String path = "VehicleBuild/BrakeForces/")
        {
            String value = "0";
            XmlNode paramNode = m_vehicleParameters.DocumentElement.SelectSingleNode(path + parameterName);
            // Make sure the parameter exists
            if (paramNode != null)
            {   // Make sure there is a value to be had
                if (paramNode.FirstChild != null)
                {
                    value = paramNode.FirstChild.Value;
                }
            }
            return value;
        }

        /// <summary>
        ///  Initialize the form.
        ///  The form will be cleared of any data and then set to an initial state with default values.
        /// </summary>
        private void InitializeForm()
        {   // Clear the brake force parameter grid and add blank rows
            m_brakeForceParamDataGridView.Rows.Clear();
            String[] frontRow = { "Front", "", "", "", "", "" };
            String[] rear1Row = { "Rear 1", "", "", "", "", "" };
            String[] rear2Row = { "Rear 2", "", "", "", "", "" };
            m_brakeForceParamDataGridView.Rows.Add(frontRow);
            m_brakeForceParamDataGridView.Rows.Add(rear1Row);
            m_brakeForceParamDataGridView.Rows.Add(rear2Row);
            // Clear the truck family text box
            m_truckFamilyTextBox.Clear();
            m_parkBrakeForceTextBox.Clear();
            m_brkStopDistSpeedTextBox.Clear();
            // Reset the form data changed flag
            FormDataChanged = false;
        }

        /// <summary>
        /// Create the parameter name for the cell that was just changed.
        /// </summary>
        /// <param name="rowIndex">Row index of  the parameter.</param>
        /// <param name="columnIndex">Column index of the parameter.</param>
        /// <returns></returns>
        private String MakeParameterName(Int32 rowIndex, Int32 columnIndex)
        {
            String paramName;
            // Translate the row name to a portion of the parameter name so we know which axle
            switch (rowIndex)
            {
                case 0:
                    paramName = "Front";
                    break;

                case 1:
                    paramName = "Rear";
                    break;

                case 2:
                    paramName = "Tandem";
                    break;

                default:
                    paramName = "UnknownAxle";
                    break;
            }
            // Translate the column name
            switch (columnIndex)
            {
                case 1:
                    paramName += "MinBrakeForce";
                    break;

                case 2:
                    paramName += "MaxBrakeForce";
                    break;

                case 3:
                    paramName += "SideToSideBalance";
                    break;

                case 4:
                    paramName += "MaximumStoppingDistance";
                    break;

                default:
                    paramName += "UnknownParameter";
                    break;
            }
            return paramName;
        }

        /// <summary>
        /// Display the parameters in the form.
        /// </summary>
        /// <param name="baumuster">Baumuster number to display in the form</param>
        private void PopulateParameters(String baumuster)
        {   // Populate the form with the baumuster
            m_truckFamilyTextBox.Text = baumuster;
            // Set the min/max brake force parameters
            m_brakeForceParamDataGridView.Rows[0].Cells["MinBrakeForce"].Value = String.Format("{0:F0}", ConvertParameterValue(ConversionTypes.LbfKgf, Convert.ToDouble(GetParameterValue("FrontMinBrakeForce"))));
            m_brakeForceParamDataGridView.Rows[0].Cells["MaxBrakeForce"].Value = String.Format("{0:F0}", ConvertParameterValue(ConversionTypes.LbfKgf, Convert.ToDouble(GetParameterValue("FrontMaxBrakeForce"))));
            m_brakeForceParamDataGridView.Rows[1].Cells["MinBrakeForce"].Value = String.Format("{0:F0}", ConvertParameterValue(ConversionTypes.LbfKgf, Convert.ToDouble(GetParameterValue("RearMinBrakeForce"))));
            m_brakeForceParamDataGridView.Rows[1].Cells["MaxBrakeForce"].Value = String.Format("{0:F0}", ConvertParameterValue(ConversionTypes.LbfKgf, Convert.ToDouble(GetParameterValue("RearMaxBrakeForce"))));
            m_brakeForceParamDataGridView.Rows[2].Cells["MinBrakeForce"].Value = String.Format("{0:F0}", ConvertParameterValue(ConversionTypes.LbfKgf, Convert.ToDouble(GetParameterValue("TandemMinBrakeForce"))));
            m_brakeForceParamDataGridView.Rows[2].Cells["MaxBrakeForce"].Value = String.Format("{0:F0}", ConvertParameterValue(ConversionTypes.LbfKgf, Convert.ToDouble(GetParameterValue("TandemMaxBrakeForce"))));
            // Set the brake stop distance
            m_brakeForceParamDataGridView.Rows[0].Cells["MaxStopDist"].Value = String.Format("{0:F0}", ConvertParameterValue(ConversionTypes.FeetMeter, Convert.ToDouble(GetParameterValue("FrontMaximumStoppingDistance"))));
            m_brakeForceParamDataGridView.Rows[1].Cells["MaxStopDist"].Value = String.Format("{0:F0}", ConvertParameterValue(ConversionTypes.FeetMeter, Convert.ToDouble(GetParameterValue("RearMaximumStoppingDistance"))));
            m_brakeForceParamDataGridView.Rows[2].Cells["MaxStopDist"].Value = String.Format("{0:F0}", ConvertParameterValue(ConversionTypes.FeetMeter, Convert.ToDouble(GetParameterValue("TandemMaximumStoppingDistance"))));
            // Set the brake force difference
            m_brakeForceParamDataGridView.Rows[0].Cells["MaxDiffForce"].Value = String.Format("{0:F0}", ConvertBalancePercentToForceDiff(Convert.ToDouble(GetParameterValue("FrontSideToSideBalance")), "Front"));
            m_brakeForceParamDataGridView.Rows[1].Cells["MaxDiffForce"].Value = String.Format("{0:F0}", ConvertBalancePercentToForceDiff(Convert.ToDouble(GetParameterValue("RearSideToSideBalance")), "Rear"));
            m_brakeForceParamDataGridView.Rows[2].Cells["MaxDiffForce"].Value = String.Format("{0:F0}", ConvertBalancePercentToForceDiff(Convert.ToDouble(GetParameterValue("TandemSideToSideBalance")), "Tandem"));
            // Display the stop distance start speed
            Double convertedSpeed = ConvertParameterValue(ConversionTypes.MphKmh,
                                                          Convert.ToDouble(GetParameterValue("BrakeStopdistanceStartSpeed")));
            String speedDisp = String.Format("{0:F0}", convertedSpeed);
            m_brkStopDistSpeedTextBox.Text = speedDisp;
            // Display the park brake force
            Double convertedForce = ConvertParameterValue(ConversionTypes.LbfKgf,
                                                          Convert.ToDouble(GetParameterValue("ParkBrakeApplyForce", "VehicleBuild/ParkBrakeParameters/")));
            String forceDisp = String.Format("{0:F0}", convertedForce);
            m_parkBrakeForceTextBox.Text = forceDisp;
            FormDataChanged = false;
        }

        /// <summary>
        /// The user will be prompted to save changes.
        /// If the user selects 'Yes', then the changes will be saved to the data file.
        /// </summary>
        /// <returns></returns>
        private DialogResult SaveFormChanges(Boolean promptForSave)
        {
            DialogResult result = DialogResult.Yes;
            // Determine if the user should be prompted to save the changes
            if (promptForSave)
            {
                result = MessageBox.Show("Save changes to data?", "Save Changes",
                                                  MessageBoxButtons.YesNoCancel, MessageBoxIcon.Question);
            }
            // Check the result to see if the changes should be saved
            if (DialogResult.Yes == result)
            {   // Save the data
                if (m_truckFamilyTextBox.Text.Length > 0)
                {   // Update the stopping distance start speed
                    SetParameterValue("BrakeStopdistanceStartSpeed",
                                      ConvertParameterValue(ConversionTypes.KmhMph, Convert.ToDouble(m_brkStopDistSpeedTextBox.Text)).ToString());
                    // Save the park brake apply force
                    SetParameterValue("ParkBrakeApplyForce",
                                      ConvertParameterValue(ConversionTypes.KgfLbf, Convert.ToDouble(m_parkBrakeForceTextBox.Text)).ToString(),
                                      "VehicleBuild/ParkBrakeParameters");
                    // Save the data to file.
                    String baumuster = m_truckFamilyTextBox.Text;
                    while (baumuster.Length < Convert.ToInt32(MercedesBrakeForceEditor.Properties.Resources.BaumusterLength))
                    {
                        baumuster += "_";
                    }
                    m_vehicleParameters.Save(MercedesBrakeForceEditor.Properties.Settings.Default.VehicleBuildFolder + "\\" +
                                             baumuster + ".xml");
                }
                FormDataChanged = false;
            }
            return result;
        }

        /// <summary>
        /// Store the value of the specified parameter.
        /// </summary>
        /// <param name="name">Name of the parameter.</param>
        /// <param name="value">Value of the specified parameter.</param>
        /// <param name="path">Path to save parameters to.</param>
        private void SetParameterValue(String name, String value, String path="VehicleBuild/BrakeForces")
        {
            XmlNode paramNode = m_vehicleParameters.DocumentElement.SelectSingleNode(path + "/" + name);
            // Make sure the node was found, if not, create it
            if (paramNode == null)
            {   // Create a new node to be added to the list
                XmlElement newParam = m_vehicleParameters.CreateElement(name);
                m_vehicleParameters.DocumentElement.SelectSingleNode(path).AppendChild(newParam);
                paramNode = m_vehicleParameters.DocumentElement.SelectSingleNode(path + "/" + name);
            }
            // Update the value of the node if it has been found or created
            if (paramNode.FirstChild != null)
            {
                if (paramNode.FirstChild.Value != value)
                {
                    paramNode.FirstChild.Value = value;
                }
            }
            else
            {
                paramNode.InnerText = value;
            }
        }

        /// <summary>
        /// Display a form so the user can select the location of the vehicle build files.
        /// </summary>
        private void SetupVehicleBuildDirectory()
        {
            FolderBrowserDialog dlg = new FolderBrowserDialog();
            dlg.Description = "Select folder for Vehicle Parameter Files";
            dlg.SelectedPath = MercedesBrakeForceEditor.Properties.Settings.Default.VehicleBuildFolder;
            dlg.ShowNewFolderButton = true;
            if (dlg.ShowDialog() == DialogResult.OK)
            {   // Update the build file location
                MercedesBrakeForceEditor.Properties.Settings.Default.VehicleBuildFolder = dlg.SelectedPath;
                MercedesBrakeForceEditor.Properties.Settings.Default.Save();
            }
        }

        //-----------------------------------------------------------------------------------------
        // Callback Functions
        //-----------------------------------------------------------------------------------------
        /// <summary>
        /// Exit the application.
        /// If changes have not been saved, the user will be prompted to save changes before exiting.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void exitToolStripMenuItem_Click(object sender, EventArgs e)
        {   // Prompt user to save any changes
            DialogResult result = System.Windows.Forms.DialogResult.OK;
            if (FormDataChanged)
            {   // Check if the user would like to save changes
                result = SaveFormChanges(true);
            }
            // Exit the program unless the user canceled the Save request
            if (DialogResult.Cancel != result)
            {
                Environment.Exit(0);
            }
        }

        /// <summary>
        /// Update the brake parameter for the item that was changed.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void m_brakeForceParamDataGridView_CellValueChanged(object sender, DataGridViewCellEventArgs e)
        {   // Set the flag that the form data has been changed
            FormDataChanged = true;
            // Store the updated parameter
            if ((e.RowIndex > -1) && (e.ColumnIndex > -1))
            {
                ConversionTypes type = ConversionTypes.Unknnown;
                if ((e.ColumnIndex >= 1) && (e.ColumnIndex <= 3)) type = ConversionTypes.KgfLbf;
                else if (e.ColumnIndex == 4) type = ConversionTypes.MeterFeet;
                Double value = ConvertParameterValue(type, Convert.ToDouble(m_brakeForceParamDataGridView.Rows[e.RowIndex].Cells[e.ColumnIndex].Value));
                // If this is the force difference cell, need to convert to an offset balance percent
                if (e.ColumnIndex == 3)
                {
                    String axle = "UnknownAxle";
                    switch(e.RowIndex)
                    {
                        case 0:
                            axle = "Front";
                            break;

                        case 1:
                            axle = "Rear";
                            break;

                        case 2:
                            axle = "Tandem";
                            break;
                    }
                    value = ConvertForceDiffToPercent(Convert.ToDouble(m_brakeForceParamDataGridView.Rows[e.RowIndex].Cells[e.ColumnIndex].Value), axle);
                }
                SetParameterValue(MakeParameterName(e.RowIndex, e.ColumnIndex), value.ToString());
            }
        }

        /// <summary>
        /// A new value has been added for the brake stop distance.  Set the flag that the data has changed.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void m_brkStopDistSpeedTextBox_TextChanged(object sender, EventArgs e)
        {
            FormDataChanged = true;
        }

        /// <summary>
        /// A new value has been set for the park brake apply force.  Set the flag that the data has changed.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void m_parkBrakeForceTextBox_TextChanged(object sender, EventArgs e)
        {
            FormDataChanged = true;
        }

        /// <summary>
        /// Setup the form for a new parameter file.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void newToolStripMenuItem_Click(object sender, EventArgs e)
        {   // See if any data changes need to be saved
            DialogResult result = DialogResult.OK;
            if (FormDataChanged)
            {
                result = SaveFormChanges(true);
            }
            // Make sure it is OK to clear the form - user did not select Cancel
            if (DialogResult.Cancel != result)
            {
                InitializeForm();
                m_vehicleParameters.Load("VehicleParameterTemplate.xml");
            }
        }

        /// <summary>
        /// Attempt to open an existing parameter file.
        /// If the current parameters have not been saved the operator will be prompted to save changes.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void openToolStripMenuItem_Click(object sender, EventArgs e)
        {   // Make sure the current data is saved if needed
            if (FormDataChanged)
            {
                SaveFormChanges(true);
            }
            // Create a dialog for the user to select the file to load
            OpenFileDialog dlg = new OpenFileDialog();
            dlg.DefaultExt = "xml";
            dlg.Filter = "xml | *.xml";
            dlg.InitialDirectory = MercedesBrakeForceEditor.Properties.Settings.Default.VehicleBuildFolder;
            dlg.Multiselect = false;
            dlg.Title = "Select Vehicle Parameter File";
            if (dlg.ShowDialog() == DialogResult.OK)
            {   // Load the parameter file
                m_vehicleParameters.Load(dlg.FileName);
                // Get the baumuster number from the file name
                Int32 startIndex = dlg.FileName.LastIndexOf("\\") + 1;
                Int32 length = dlg.FileName.LastIndexOf(".");
                String baumuster = dlg.FileName.Substring(startIndex, length - startIndex); 
                PopulateParameters(baumuster);
            }
        }

        /// <summary>
        /// Save the changes to the parameters.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void saveToolStripMenuItem_Click(object sender, EventArgs e)
        {
            SaveFormChanges(false);
        }

        /// <summary>
        /// Allow the user to select the location of the vehicle parameter files.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void vehicleBuildFolderToolStripMenuItem_Click(object sender, EventArgs e)
        {
            SetupVehicleBuildDirectory();
        }


        //-----------------------------------------------------------------------------------------
        // Member Variables and Properties
        //-----------------------------------------------------------------------------------------
        /// <summary>
        /// Flag to indicate if any of the data in the from has been changed.
        /// </summary>
        Boolean m_dataChanged;

        /// <summary>
        /// Get/Set the flag indicating form data has been changed.
        /// </summary>
        private Boolean FormDataChanged
        {
            get { return m_dataChanged; }
            set { m_dataChanged = value; }
        }

        /// <summary>
        /// Vehicle parameters to be used.
        /// </summary>
        private XmlDocument m_vehicleParameters;

        /// <summary>
        /// List of valid conversion types for converting data to/from english/metric units.
        /// </summary>
        enum ConversionTypes
        {
            Unknnown = 0,
            KgfLbf,
            LbfKgf,
            KmhMph,
            MphKmh,
            MeterFeet,
            FeetMeter
        };

    }
}