using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using System.Xml;

namespace VcEditor
{
    public partial class VehicleTypeEditorForm : Form
    {
        public VehicleTypeEditorForm()
        {   // If the selected location for VC Files is the same as the default location, 
            // make the user select the correct location
            if (VcEditor.Properties.Settings.Default.VcFileLocation == VcEditor.Properties.Resources.DefaultVcFileLocation)
            {
                SelectVcFileLocation();
            }
            InitializeComponent();
            // Set the icon to use
            this.Icon = Icon.FromHandle(VcEditor.Properties.Resources.bep_best.GetHicon());
            VcFile = new XmlDocument();
            VcFile.Load("VehicleTemplate.xml");
            VcFileName = "";
            BringToFront();
            // Set combo box items
            m_driveAxleComboBox.SelectedIndex = 0;
        }

        //=========================================================================================
        // Member Functions
        //=========================================================================================
        /// <summary>
        /// Clear the form in preparation for a new vehicle type.
        /// </summary>
        void ClearForm()
        {   // Clear the vehicle info
            m_vcNumberTextBox.Clear();
            m_rearAxleWheelbaseTextBox.Clear();
            m_vinCharTextBox.Clear();
            // Clear the brake test parameters
            m_minPbForceTextBox.Clear();
            m_maxPbForceTextBox.Clear();
            m_frontDragTextBox.Clear();
            m_rearDragTextBox.Clear();
            m_minFrontBrakeForceTextBox.Clear();
            m_maxFrontBrakeForceTextBox.Clear();
            m_minRearBrakeForceTextBox.Clear();
            m_maxRearBrakeForceTextBox.Clear();
            // Reset drive axle to front
            m_driveAxleComboBox.SelectedIndex = 0;
            // Uncheck the 4WD option
            m_fourWdCheckBox.Checked = false;
        }

        /// <summary>
        /// Convert the provided force from LBF to KGF.
        /// </summary>
        /// <param name="force">Force to convert to KGF.</param>
        /// <returns>Force converted to KGF.</returns>
        String ConvertLbfToKgf(String force)
        {
            double frc = 0.0;
            if (force != "")
            {
                frc = Convert.ToDouble(force) * 0.4535924;
            }
            return (Convert.ToString(Convert.ToInt32(frc)));
        }

        /// <summary>
        /// Convert the provided force from KGF to LBF.
        /// </summary>
        /// <param name="force">Force in KGF.</param>
        /// <returns>Force converted to LBF</returns>
        String ConvertKgfToLbf(String force)
        {
            double frc = 0.0;
            if (force != "")
            {
                frc = Convert.ToDouble(force) / 0.4535924;
            }
            return Convert.ToString(frc);
        }

        /// <summary>
        /// Convert the provided speed from MPH to KPH.
        /// </summary>
        /// <param name="speed">Speed to convert to KPH.</param>
        /// <returns>Speed converted to KPH.</returns>
        String ConvertMphToKph(String speed)
        {
            double spd = 0.0;
            if (speed != "")
            {
                spd = Convert.ToDouble(speed) * 1.609344;
            }
            return (Convert.ToString(Convert.ToInt32(spd)));
        }

        /// <summary>
        /// Convert the provided speed back to MPH from KPH.
        /// </summary>
        /// <param name="speed">Speed in KPH.</param>
        /// <returns>Speed converted to MPH.</returns>
        String ConvertKphToMph(String speed)
        {
            double spd = 0.0;
            if (speed != "")
            {
                spd = Convert.ToDouble(speed) / 1.609344;
            }
            return (Convert.ToString(spd));
        }

        /// <summary>
        /// Get the value of the specified parameter in the config file.
        /// </summary>
        /// <param name="parameter">Parameter to retrieve.</param>
        /// <returns>Value of the requested parameter.</returns>
        String GetParameter(String parameter)
        {
            String value = "1";
            XmlNode paramNode = VcFile.DocumentElement.SelectSingleNode("VehicleBuild/" + parameter);
            if (paramNode != null)
            {
                if (paramNode.FirstChild != null)
                {
                    value = paramNode.FirstChild.Value;
                }
            }
            return value;
        }

        /// <summary>
        /// Display the brake force parameter in the form.
        /// </summary>
        void PopulateBrakeForceParameters()
        {   // Display the brake force parameters
            m_frontDragTextBox.Text = ConvertLbfToKgf(GetParameter("BrakeForces/FrontMaxDragForce"));
            m_rearDragTextBox.Text = ConvertLbfToKgf(GetParameter("BrakeForces/RearMaxDragForce"));
            m_minFrontBrakeForceTextBox.Text = ConvertLbfToKgf(GetParameter("BrakeForces/FrontMinBrakeForce"));
            m_maxFrontBrakeForceTextBox.Text = ConvertLbfToKgf(GetParameter("BrakeForces/FrontMaxBrakeForce"));
            m_minRearBrakeForceTextBox.Text = ConvertLbfToKgf(GetParameter("BrakeForces/RearMinBrakeForce"));
            m_maxRearBrakeForceTextBox.Text = ConvertLbfToKgf(GetParameter("BrakeForces/RearMaxBrakeForce"));
            m_minPbForceTextBox.Text = ConvertLbfToKgf(GetParameter("BrakeForces/ParkBrakeMinForce"));
            m_maxPbForceTextBox.Text = ConvertLbfToKgf(GetParameter("BrakeForces/ParkBrakeMaxForce"));
        }

        /// <summary>
        /// Populate the form with all the data
        /// </summary>
        void PopulateForm()
        {   // Set the vehicle information
            m_vcNumberTextBox.Text = GetParameter("VehicleType");
            m_vinCharTextBox.Text = VcFileName.Substring(VcFileName.IndexOf("Vin") + 3, 5);
            m_rearAxleWheelbaseTextBox.Text = GetParameter("WheelbasePositionInchesX10");
            m_fourWdCheckBox.Checked = (GetParameter("FourWD") == "True");
            m_driveAxleComboBox.SelectedIndex = m_driveAxleComboBox.FindString(GetParameter("DriveAxle"));
            // Set the brake force information
            PopulateBrakeForceParameters();
        }

        /// <summary>
        /// Select the location of the VC files.
        /// </summary>
        void SelectVcFileLocation()
        {   // Have the user select the real VC file location
            FolderBrowserDialog dlg = new FolderBrowserDialog();
            dlg.Description = "Select location of Vehicle files";
            dlg.ShowNewFolderButton = true;
            dlg.SelectedPath = VcEditor.Properties.Settings.Default.VcFileLocation;
            if (dlg.ShowDialog() == DialogResult.OK)
            {   // Store  the ne location
                VcEditor.Properties.Settings.Default.VcFileLocation = dlg.SelectedPath;
                VcEditor.Properties.Settings.Default.Save();
            }
        }

        /// <summary>
        /// Set the new parameter value in the config file.
        /// </summary>
        /// <param name="parameterName">Name of the parameter that has been modified.</param>
        /// <param name="parameterValue">New value of the parameter.</param>
        void SetParameter(String parameterName, String parameterValue)
        {
            XmlNode paramNode = VcFile.DocumentElement.SelectSingleNode("VehicleBuild/" + parameterName);
            if (paramNode != null)
            {   // Only need to save the value if it has been changed
                if (paramNode.FirstChild != null)
                {
                    if (paramNode.FirstChild.Value != parameterValue)
                    {
                        paramNode.FirstChild.Value = parameterValue;
                    }
                }
                else
                {
                    paramNode.InnerText = parameterValue;
                }
            }
        }

        //=========================================================================================
        // Callbacks
        //=========================================================================================

        /// <summary>
        /// Exit the application.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void exitToolStripMenuItem_Click(object sender, EventArgs e)
        {
            Environment.Exit(0);
        }

        /// <summary>
        /// Set up the form for a new vehicle type.  This involves clearing the form.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void newToolStripMenuItem_Click(object sender, EventArgs e)
        {
            ClearForm();
            VcFileName = "";
            m_driveAxleComboBox.SelectedIndex = 0;
        }

        /// <summary>
        /// Open an existing VC file and populate the window.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void openToolStripMenuItem_Click(object sender, EventArgs e)
        {
            OpenFileDialog openDlg = new OpenFileDialog();
            openDlg.DefaultExt = "xml";
            openDlg.Filter = "Vehicle Files (Vin*.xml) | Vin*.xml";
            openDlg.Multiselect = false;
            openDlg.Title = "Select Vehicle File";
            openDlg.InitialDirectory = VcEditor.Properties.Settings.Default.VcFileLocation;
            if (openDlg.ShowDialog() == DialogResult.OK)
            {
                ClearForm();
                VcFileName = openDlg.FileName;
                m_vcFile.Load(openDlg.FileName);
                PopulateForm();
            }
        }

        /// <summary>
        /// Save the updates to file.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void saveToolStripMenuItem_Click(object sender, EventArgs e)
        {
            if (VcFileName == "")
            {
                VcFileName = VcEditor.Properties.Settings.Default.VcFileLocation + "\\" + "Vin" + m_vinCharTextBox.Text + ".xml";
            }
            m_vcFile.Save(VcFileName);
        }

        /// <summary>
        /// Allow the use to choose a default directory for the VC files.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void vCFileLocationToolStripMenuItem_Click(object sender, EventArgs e)
        {
            SelectVcFileLocation();
        }

        /// <summary>
        /// Store the new drive axle.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void m_driveAxleComboBox_SelectedIndexChanged(object sender, EventArgs e)
        {
            SetParameter("DriveAxle", m_driveAxleComboBox.SelectedItem.ToString());
        }

        /// <summary>
        /// Store the new value of the 4wd option.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void m_fourWdCheckBox_CheckedChanged(object sender, EventArgs e)
        {
            SetParameter("FourWD", m_fourWdCheckBox.Checked ? "True" : "False");
        }

        /// <summary>
        /// Convert the front maximum drag force from kgf to lbf and store in the parameter list.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void m_frontDragTextBox_TextChanged(object sender, EventArgs e)
        {
            SetParameter("BrakeForces/FrontMaxDragForce", ConvertKgfToLbf(m_frontDragTextBox.Text));
        }

        /// <summary>
        /// Convert the front maximum brake force from kgf to lbf and store in the parameterlist.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void m_maxFrontBrakeForceTextBox_TextChanged(object sender, EventArgs e)
        {
            SetParameter("BrakeForces/FrontMaxBrakeForce", ConvertKgfToLbf(m_maxFrontBrakeForceTextBox.Text));
        }

        /// <summary>
        /// Convert he maximum park brake force from kgf to lbf and store in the parameter list.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void m_maxPbForceTextBox_TextChanged(object sender, EventArgs e)
        {
            SetParameter("BrakeForces/ParkBrakeMaxForce", ConvertKgfToLbf(m_maxPbForceTextBox.Text));
        }

        /// <summary>
        /// Convert the rear maximum brake force from kgf to lbf and store in the parameter list.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void m_maxRearBrakeForceTextBox_TextChanged(object sender, EventArgs e)
        {
            SetParameter("BrakeForces/RearMaxBrakeForce", ConvertKgfToLbf(m_maxRearBrakeForceTextBox.Text));
        }

        /// <summary>
        /// Convert the minimum front brake force from kgf to lbf and store in the parameter list.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void m_minFrontBrakeForceTextBox_TextChanged(object sender, EventArgs e)
        {
            SetParameter("BrakeForces/FrontMinBrakeForce", ConvertKgfToLbf(m_minFrontBrakeForceTextBox.Text));
        }

        /// <summary>
        /// Convert the minimum park brake force from kgf to lbf and store in the parameter list.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void m_minPbForceTextBox_TextChanged(object sender, EventArgs e)
        {
            SetParameter("BrakeForces/ParkBrakeMinForce", ConvertKgfToLbf(m_minPbForceTextBox.Text));
        }

        /// <summary>
        /// Convert the minimum rear brake force from kgf to lbf and store in the parameter list.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void m_minRearBrakeForceTextBox_TextChanged(object sender, EventArgs e)
        {
            SetParameter("BrakeForces/RearMinBrakeForce", ConvertKgfToLbf(m_minRearBrakeForceTextBox.Text));
        }

        /// <summary>
        /// Store the rear axle wheelbase value.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void m_rearAxleWheelbaseTextBox_TextChanged(object sender, EventArgs e)
        {
            SetParameter("WheelbasePositionInchesX10", m_rearAxleWheelbaseTextBox.Text);
        }

        /// <summary>
        /// Convert the rear maximum drag force from kgf to lbf and store in the parameter list.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void m_rearDragTextBox_TextChanged(object sender, EventArgs e)
        {
            SetParameter("BrakeForces/RearMaxDrageForce", ConvertKgfToLbf(m_rearDragTextBox.Text));
        }

        /// <summary>
        /// Store the new VC Number.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void m_vcNumberTextBox_TextChanged(object sender, EventArgs e)
        {
            SetParameter("VehicleType", m_vcNumberTextBox.Text);
        }

        /// <summary>
        /// Store the new VIN character information
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void m_vinCharTextBox_TextChanged(object sender, EventArgs e)
        {
            VcFileName = VcEditor.Properties.Settings.Default.VcFileLocation + "\\" + "Vin" + m_vinCharTextBox.Text + ".xml";
        }

        //=========================================================================================
        // Member Variables and Properties
        //=========================================================================================

        /// <summary>
        /// VC file being edited / created.
        /// </summary>
        XmlDocument m_vcFile;

        /// <summary>
        /// Get/Set the VC file being edited / created.
        /// </summary>
        public XmlDocument VcFile
        {
            get { return m_vcFile; }
            set { m_vcFile = value; }
        }

        /// <summary>
        /// Name of the VC file that was loaded.
        /// </summary>
        String m_vcFileName;

        /// <summary>
        /// Get/Set the name of the VC file that was loaded.
        /// </summary>
        public String VcFileName
        {
            get { return m_vcFileName; }
            set { m_vcFileName = value; }
        }
    }
}
