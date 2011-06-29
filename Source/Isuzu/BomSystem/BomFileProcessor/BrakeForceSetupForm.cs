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
    public partial class BrakeForceSetupForm : Form
    {
        //-----------------------------------------------------------------------------------------
        // Member Functions
        //-----------------------------------------------------------------------------------------
        /// <summary>
        /// Setup the form to allow user to enter brake force parameters
        /// </summary>
        public BrakeForceSetupForm(BrakeForceCollection brakeForceSets)
        {
            InitializeComponent();
            // Save the brake force sets so they can be updated
            m_brakeForceSets = brakeForceSets;
            // Initialize the brake force parameter grid
            InitializeBrakeForceForm();
        }

        /// <summary>
        /// Initialize the brake force form.
        /// </summary>
        private void InitializeBrakeForceForm()
        {   // Setup the parameter grid
            String[] front = { "Front", "", "", "", "", "", "" };
            String[] rear = { "Rear", "", "", "", "", "", "" };
            DataGridViewRow frontRow = new DataGridViewRow();
            DataGridViewRow rearRow = new DataGridViewRow();

            m_brkFrcParamDataGridView.Rows.Clear();
            m_brkFrcParamDataGridView.Rows.Add(front);
            m_brkFrcParamDataGridView.Rows.Add(rear);
            foreach (DataGridViewRow row in m_brkFrcParamDataGridView.Rows)
            {
                if (row.Cells["Axle"].Value.ToString() == "Front")
                {
                    frontRow = row;
                }
                else
                {
                    rearRow = row;
                }
            }
            rearRow.Cells["FrontToRearBalMin"].ReadOnly = true;
            rearRow.Cells["FrontToRearBalMin"].Style.BackColor = Color.LightGray;
            rearRow.Cells["FrontToRearBalMin"].Style.ForeColor = Color.DarkGray;
            rearRow.Cells["FrontToRearBalMax"].ReadOnly = true;
            rearRow.Cells["FrontToRearBalMax"].Style.BackColor = Color.LightGray;
            rearRow.Cells["FrontToRearBalMax"].Style.ForeColor = Color.DarkGray;
            // Populate the drop down list
            m_modelCodeListComboBox.Items.AddRange(m_brakeForceSets.ConfiguredModelCodes().ToArray());
        }

        /// <summary>
        /// Populate the form with the data for the selected model code.
        /// </summary>
        /// <param name="modelCode"></param>
        private void PopulateBrakeForceParameters(String modelCode)
        {   // Find the brake force parameter set
            BrakeForce paramSet = m_brakeForceSets.Find(modelCode);
            if (paramSet != null)
            {   // Clear the data grid before adding any items
                m_brkFrcParamDataGridView.Rows.Clear();
                String[] front = {"Front", paramSet.FrontMaxDragForce.ToString(), paramSet.FrontMinBrakeForce.ToString(),
                                    paramSet.FrontMaxBrakeForce.ToString(), paramSet.FrontBalanceOffset.ToString(),
                                    paramSet.FrontToRearBalanceMin.ToString(), paramSet.FrontToRearBalanceMax.ToString()};
                String[] rear = {"Rear", paramSet.RearMaxDragForce.ToString(), paramSet.RearMinBrakeForce.ToString(),
                                    paramSet.RearMaxBrakeForce.ToString(), paramSet.RearBalanceOffset.ToString(), "", ""};
                m_brkFrcParamDataGridView.Rows.Add(front);
                m_brkFrcParamDataGridView.Rows.Add(rear);

                DataGridViewRow frontRow = new DataGridViewRow();
                DataGridViewRow rearRow = new DataGridViewRow();
                foreach (DataGridViewRow row in m_brkFrcParamDataGridView.Rows)
                {
                    if (row.Cells["Axle"].Value.ToString() == "Front")
                    {
                        frontRow = row;
                    }
                    else
                    {
                        rearRow = row;
                    }
                }
                rearRow.Cells["FrontToRearBalMin"].ReadOnly = true;
                rearRow.Cells["FrontToRearBalMin"].Style.BackColor = Color.LightGray;
                rearRow.Cells["FrontToRearBalMin"].Style.ForeColor = Color.DarkGray;
                rearRow.Cells["FrontToRearBalMax"].ReadOnly = true;
                rearRow.Cells["FrontToRearBalMax"].Style.BackColor = Color.LightGray;
                rearRow.Cells["FrontToRearBalMax"].Style.ForeColor = Color.DarkGray;
            }
        }



        //-----------------------------------------------------------------------------------------
        // Callback Functions
        //-----------------------------------------------------------------------------------------
        /// <summary>
        /// Save the brake parameters for the specified model code.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void m_saveButton_Click(object sender, EventArgs e)
        {   // Create some rows so we can easily retrieve the parameters and place them accurately
            DataGridViewRow front = new DataGridViewRow();
            DataGridViewRow rear = new DataGridViewRow();
            foreach(DataGridViewRow row in m_brkFrcParamDataGridView.Rows)
            {
                if(row.Cells["Axle"].Value.ToString() == "Front")
                {
                    front = row;
                }
                else
                {
                    rear = row;
                }
            }
            // Add the brake force set for this model code
            m_brakeForceSets.Add(m_modelCodeListComboBox.Text.ToString(),
                                 Convert.ToDouble(front.Cells["MaxDragForce"].Value), Convert.ToDouble(rear.Cells["MaxDragForce"].Value),
                                 Convert.ToDouble(front.Cells["MinBrakeForce"].Value), Convert.ToDouble(front.Cells["MaxBrakeForce"].Value),
                                 Convert.ToDouble(rear.Cells["MinBrakeForce"].Value), Convert.ToDouble(rear.Cells["MaxBrakeForce"].Value),
                                 Convert.ToDouble(front.Cells["SideToSideBalance"].Value), Convert.ToDouble(rear.Cells["SideToSideBalance"].Value),
                                 Convert.ToDouble(front.Cells["FrontToRearBalMin"].Value), Convert.ToDouble(front.Cells["FrontToRearBalMax"].Value));
        }

        /// <summary>
        /// The user has selected a different set of parameters, update the form with the data.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void m_modelCodeListComboBox_SelectedIndexChanged(object sender, EventArgs e)
        {
            PopulateBrakeForceParameters(m_modelCodeListComboBox.SelectedItem.ToString());
        }
        


        //-----------------------------------------------------------------------------------------
        // Member Variables and Properties
        //-----------------------------------------------------------------------------------------
        /// <summary>
        /// Brake force sets that have been configured.
        /// </summary>
        BrakeForceCollection m_brakeForceSets;

        private void m_brkFrcParamDataGridView_CellContentClick(object sender, DataGridViewCellEventArgs e)
        {

        }

    }
}
