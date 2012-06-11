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
    public partial class BrakeModuleConfigForm : Form
    {
        //-----------------------------------------------------------------------------------------
        // Member Functions
        //-----------------------------------------------------------------------------------------

        /// <summary>
        /// Create the brake module configuration form and display the configured modules.
        /// </summary>
        /// <param name="modules">Configured brake modules.</param>
        public BrakeModuleConfigForm(List<BrakeModule> modules)
        {
            InitializeComponent();
            BrakeModules = modules;
            LoadAbsModuleComboBox();
            if (BrakeModules.Count > 0)
            {
                m_absTypeComboBox.SelectedIndex = 0;
            }
        }

        /// <summary>
        /// Display the configured info for the specified brake module.
        /// </summary>
        /// <param name="moduleIndex">Index of the brake module to display.</param>
        private void DisplayBrakeModule(int moduleIndex = 0)
        {
            if (BrakeModules.Count > 0)
            {
                m_bus.Text = BrakeModules[moduleIndex].CommBus;
                m_protocol.Text = BrakeModules[moduleIndex].CommProtocol;
                m_reqId.Text = "$";
                foreach(Byte reqIdByte in BrakeModules[moduleIndex].MessageTable["EnterDiagnosticMode"].m_messageFilter.requestID)
                {
                    m_reqId.Text += String.Format("{0,02:X2}", reqIdByte);
                }
                m_respId.Text = "$";
                foreach (Byte respIdByte in BrakeModules[moduleIndex].MessageTable["EnterDiagnosticMode"].m_messageFilter.responseID)
                {
                    m_respId.Text += String.Format("{0,02:X2}", respIdByte);
                }
                // Populate the message table
                m_msgDataGridView.Rows.Clear();
                foreach(KeyValuePair<String, CcrtJ2534Defs.ECUMessage> msg in BrakeModules[moduleIndex].MessageTable)
                {
                    String dispMsg = "";
                    foreach (Byte msgByte in msg.Value.m_txMessage)
                    {
                        dispMsg += String.Format("${0,02:X2}  ", msgByte);
                    }
                    String[] row = { msg.Key, dispMsg };
                    m_msgDataGridView.Rows.Add(row);
                }
            }
            else
            {
                m_bus.Text = "";
                m_protocol.Text = "";
                m_reqId.Text = "";
                m_respId.Text = "";
            }
        }

        /// <summary>
        /// Load the combo box with all configured ABS modules.
        /// </summary>
        private void LoadAbsModuleComboBox()
        {
            m_absTypeComboBox.Items.Clear();
            foreach (BrakeModule module in BrakeModules)
            {
                m_absTypeComboBox.Items.Add(module.ModuleName);
            }
        }




        //-----------------------------------------------------------------------------------------
        // Member Variables and Properties
        //-----------------------------------------------------------------------------------------

        /// <summary>
        /// List of configured ABS modules.
        /// </summary>
        private List<BrakeModule> BrakeModules { get; set; }




        //-----------------------------------------------------------------------------------------
        // Callbacks, Delegates and Events
        //-----------------------------------------------------------------------------------------

        /// <summary>
        /// Change the display to match the selected ABS module.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void m_absTypeComboBox_SelectedIndexChanged(object sender, EventArgs e)
        {
            DisplayBrakeModule(m_absTypeComboBox.SelectedIndex);
        }

        /// <summary>
        /// Display the form to allow the user to add a new ABS module.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void m_addButton_Click(object sender, EventArgs e)
        {
            AddBrakeModuleForm frm = new AddBrakeModuleForm(BrakeModules);
            frm.ShowDialog();
            LoadAbsModuleComboBox();
            m_absTypeComboBox.SelectedIndex = 0;
        }

    }
}
