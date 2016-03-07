using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Globalization;
using System.Linq;
using System.Resources;
using System.Text;
using System.Windows.Forms;
using CcrtDataBaseLibrary;
using InputDataItemLibrary;

namespace InputServer
{
    public partial class InputDataItemForm : Form
    {
        //-----------------------------------------------------------------------------------------
        // Class Functions
        //-----------------------------------------------------------------------------------------

        /// <summary>
        /// Class Constructor
        /// <param name="culture">Culture information to use for display purposes.</param>
        /// </summary>
        public InputDataItemForm(CcrtDataBaseInterface dbInterface, Localizer localizer)
        {
            InitializeComponent();
            // Save the interface to the CCRT database
            DbInterface = dbInterface;
            // Update the language on the form
            UpdateDisplayLanguage(localizer);
            // Load the configured data items
            LoadInputDataItems();
        }

        /// <summary>
        /// Display the data item settings for the data item at the given index.
        /// </summary>
        /// <param name="itemIndex">Index of the selected data item.</param>
        private void DisplayDataItem(Int32 itemIndex)
        {   // Make sure there are items in the list
            if ((m_dataItemComboBox.Items.Count >= itemIndex) && (m_dataItemComboBox.Items.Count > 0))
            {   // Display the device name at the selected index
                m_dataItemComboBox.SelectedIndex = itemIndex;
                // Get the data item settings from the database
                InputDataItem item = DbInterface.ReadDataItemSettings(m_dataItemComboBox.SelectedItem.ToString());
                m_minLengthTextBox.Text = item.ItemSettings.MinDataLength.ToString();
                m_maxLengthTextBox.Text = item.ItemSettings.MaxDataLength.ToString();
            }
        }

        /// <summary>
        /// Get the data item settings set by the user.
        /// </summary>
        /// <returns>InputDataItem settings configured by the user.</returns>
        private InputDataItem GetUserSelectedDataSettings()
        {
            InputDataItem item = new InputDataItem(m_dataItemComboBox.Text);
            item.ItemSettings.DataName = m_dataItemComboBox.Text;
            item.ItemSettings.MaxDataLength = Convert.ToInt32(m_maxLengthTextBox.Text);
            item.ItemSettings.MinDataLength = Convert.ToInt32(m_minLengthTextBox.Text);
            return item;
        }

        /// <summary>
        /// Load the input data items from the database.
        /// The name of each data item will be added to the drop down list for selection.
        /// </summary>
        private void LoadInputDataItems()
        {   // Clear the combo box of data items
            m_dataItemComboBox.Items.Clear();
            // Get a list of Input Data Items and add them to the combo box
            foreach (InputDataItem item in DbInterface.ReadDataItemSettings())
            {
                m_dataItemComboBox.Items.Add(item.ItemSettings.DataName);
            }
            // Display the first item in the list
            DisplayDataItem(0);
        }

        /// <summary>
        /// Update the for to show the label in the selected local language.
        /// </summary>
        /// <param name="localizer">Localizer to use for getting the correct text to display.</param>
        private void UpdateDisplayLanguage(Localizer localizer)
        {
            Text = localizer.GetLocalizedString("DataItemTitle");
            m_inputDataItemLabel.Text = localizer.GetLocalizedString("DataItemColumn");
            m_dataSettingsLabel.Text = localizer.GetLocalizedString("DataSettingsLabel");
            m_minLengthLabel.Text = localizer.GetLocalizedString("MinLengthLabel");
            m_maxLengthLabel.Text = localizer.GetLocalizedString("MaxLengthLabel");
            m_closeButton.Text = localizer.GetLocalizedString("CloseLabel");
            m_removeButton.Text = localizer.GetLocalizedString("RemoveItemLabel");
            m_saveButton.Text = localizer.GetLocalizedString("SaveSettingsLabel");
        }

        //-----------------------------------------------------------------------------------------
        // Class Variables/Properties
        //-----------------------------------------------------------------------------------------

        /// <summary>
        /// Interface to the CCRT Database.
        /// </summary>
        private CcrtDataBaseInterface m_dbInterface;

        /// <summary>
        /// Get/Set the object to interface with the CCRT Database.
        /// </summary>
        private CcrtDataBaseInterface DbInterface
        {
            get { return m_dbInterface; }
            set { m_dbInterface = value; }
        }


        //-----------------------------------------------------------------------------------------
        // Class Callbacks
        //-----------------------------------------------------------------------------------------

        /// <summary>
        /// Update the form with the settings for the selected data item.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void m_dataItemComboBox_SelectedIndexChanged(object sender, EventArgs e)
        {
            DisplayDataItem(m_dataItemComboBox.SelectedIndex);
        }

        /// <summary>
        /// Remove a configured data item from the database.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void m_removeButton_Click(object sender, EventArgs e)
        {   // Remove the selected item from the database
            DbInterface.RemoveDataItemSetting(m_dataItemComboBox.SelectedItem.ToString());
            m_dataItemComboBox.Items.Remove(m_dataItemComboBox.SelectedItem);
            // Display the remaining items
            DisplayDataItem(0);
        }

        /// <summary>
        /// SAve the data item settings to the database.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void m_saveButton_Click(object sender, EventArgs e)
        {   // Get all the data item setting from the form
            InputDataItem item = GetUserSelectedDataSettings();
            if (m_dataItemComboBox.FindString(item.ItemSettings.DataName) != -1)
            {   // Data item already exists, just update the settings in the database
                DbInterface.UpdateDataItemSettings(item);
            }
            else
            {   // This is a new item, so add the item to the database
                DbInterface.AddDataItemSettings(item);
                LoadInputDataItems();
                DisplayDataItem(m_dataItemComboBox.FindString(item.ItemSettings.DataName));
            }
        }

    }
}
