using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using J2534ChannelLibrary;
using J2534DotNet;


namespace ModuleCommServer
{
    public partial class AddBrakeModuleForm : Form
    {
        //-----------------------------------------------------------------------------------------
        // Member Functions
        //-----------------------------------------------------------------------------------------

        /// <summary>
        /// Create a new brake module addition form and add the newly configured brake module.
        /// </summary>
        /// <param name="modules"></param>
        public AddBrakeModuleForm(List<BrakeModule> modules, List<String> commChannelNames)
        {
            InitializeComponent();
            BrakeModules = modules;
            CommChannels = commChannelNames;
            LoadComboBoxes();
        }

        /// <summary>
        /// Clear the form to allow a new module entry.
        /// </summary>
        private void InitializeForm()
        {
            m_absTypeTextBox.Clear();
            m_configFileTextBox.Clear();
        }

        /// <summary>
        /// Load the combo boxes with the defaults.
        /// </summary>
        private void LoadComboBoxes()
        {   // Load the bus combo box
            m_protocolComboBox.Items.Clear();
            m_protocolComboBox.Items.Add("Keyword Protocol 2000");
            m_busComboBox.Items.Clear();
            foreach (String name in CommChannels)
            {
                m_busComboBox.Items.Add(name);
            }
            try
            {
                // Set the default item
                m_protocolComboBox.SelectedIndex = 0;
                m_busComboBox.SelectedIndex = 0;
            }
            catch (ArgumentOutOfRangeException e)
            {
                MessageBox.Show("Default index not valid. Please create and then select brake module" + Environment.NewLine + e.ToString());
            }
        }




        //-----------------------------------------------------------------------------------------
        // Member Variables and Properties
        //-----------------------------------------------------------------------------------------

        /// <summary>
        /// List of configured ABS modules.
        /// </summary>
        private List<BrakeModule> BrakeModules { get; set; }

        /// <summary>
        /// List of configured comm channels.
        /// </summary>
        private List<String> CommChannels { get; set; }




        //-----------------------------------------------------------------------------------------
        // Callbacks, Delegates and Events
        //-----------------------------------------------------------------------------------------

        /// <summary>
        /// Add the new module to the list of configured modules.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void m_addButton_Click(object sender, EventArgs e)
        {   // Make sure a name was provided
            if (!String.IsNullOrEmpty(m_absTypeTextBox.Text) &&
                !String.IsNullOrWhiteSpace(m_absTypeTextBox.Text))
            {   // Make sure a configuration file was selected
                if (!String.IsNullOrEmpty(m_configFileTextBox.Text) &&
                    !String.IsNullOrWhiteSpace(m_configFileTextBox.Text))
                {
                    BrakeModule module = new BrakeModule();
                    module.ModuleName = m_absTypeTextBox.Text;
                    module.CommBus = m_busComboBox.SelectedItem.ToString();
                    module.CommProtocol = m_protocolComboBox.SelectedItem.ToString();
                    module.ConfigFile = m_configFileTextBox.Text;
                    module.LoadMessageTable();
                    BrakeModules.Add(module);
                    MessageBox.Show("Added ABS Module: " + module.ModuleName, "Added new ABS",
                                    MessageBoxButtons.OK, MessageBoxIcon.Information);
                    InitializeForm();
                }
                else
                {
                    MessageBox.Show("Configuration File cannot be blank!" + Environment.NewLine +
                                    "Please select the module configuration file for this ABS module.", 
                                    "Missing Information",
                                    MessageBoxButtons.OK, MessageBoxIcon.Exclamation);
                }
            }
            else
            {
                MessageBox.Show("ABS Type cannot be blank!" + Environment.NewLine +
                                "Please provide the type of ABS module.", "Missing Information",
                                MessageBoxButtons.OK, MessageBoxIcon.Exclamation);
            }
        }

        /// <summary>
        /// Allow the user to browse for the configuration file to be used.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void m_browseButton_Click(object sender, EventArgs e)
        {
            OpenFileDialog dlg = new OpenFileDialog();
            dlg.DefaultExt = "xml";
            dlg.Filter = "XML Files (*.xml)|*.xml|" + "All Files (*.*)|*.*";
            dlg.FilterIndex = 0;
            dlg.Multiselect = false;
            dlg.RestoreDirectory = true;
            dlg.Title = "Select ABS Module Configuration File";
            if (dlg.ShowDialog() == DialogResult.OK)
            {
                m_configFileTextBox.Text = dlg.FileName;
            }
        }

    }
}
