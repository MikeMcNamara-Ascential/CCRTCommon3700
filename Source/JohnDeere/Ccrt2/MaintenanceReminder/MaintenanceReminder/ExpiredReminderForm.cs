using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Diagnostics;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading;
using System.Windows.Forms;
using Logger;
using MaintenanceReminderLibrary;

namespace MaintenanceReminder
{
    public partial class ExpiredReminderForm : Form
    {
        //=========================================================================================
        // Member Functions
        //=========================================================================================

        /// <summary>
        /// Create the form to display the expired maintenance items.
        /// </summary>
        /// <param name="reminders">Configured maintenance reminders.</param>
        /// <param name="logger">CCRT Logge robject for logging messages.</param>
        /// <param name="displayLocalizer">Localizer to use for displaying the selected language in the form.</param>
        public ExpiredReminderForm(List<MaintenanceReminderItem> reminders, 
                                   CcrtLogger logger,
                                   Localizer displayLocalizer)
        {
            InitializeComponent();
            Reminders = reminders;
            Logger = logger;
            DisplayLocalizer = displayLocalizer;
            ExpiredReminders = new List<String>();
            UpdateDisplayLanguage();
        }

        /// <summary>
        /// Display the expired maintenance items.
        /// </summary>
        /// <param name="expiredItems">Expired maintenance items.</param>
        public void DisplayExpiredMaintenanceItems(List<String> expiredItems)
        {   // Only add the item if it is not already in the Expired items list
            foreach (String item in expiredItems)
            {
                if (ExpiredReminders.Find(exItem => exItem.Equals(item)) == null)
                {
                    ExpiredReminders.Add(item);
                }
            }
            DisplayExpiredMaintenanceItems();
        }

        /// <summary>
        /// Display the expired Maintenance items and determine if the form should be displayed 
        /// or hidden.
        /// </summary>
        private void DisplayExpiredMaintenanceItems()
        {   // Update the data grid view with the expired Items
            m_expiredReminderDataGridView.Rows.Clear();
            foreach (String item in ExpiredReminders)
            {   // Get the reminder item
                MaintenanceReminderItem reminder = Reminders.Find(rem => rem.ItemSettings.Name.Equals(item));
                if (reminder != null)
                {
                    DataGridViewRow row = new DataGridViewRow();
                    row.CreateCells(m_expiredReminderDataGridView);
                    row.Cells[m_expiredReminderDataGridView.Columns["ReminderItem"].Index].Value = reminder.ItemSettings.Name.Replace("_", " ");
                    row.Cells[m_expiredReminderDataGridView.Columns["MaintenanceText"].Index].Value = reminder.ItemSettings.FaultMessage;
                    m_expiredReminderDataGridView.Rows.Add(row);
                }
            }
            // Make sure to display or remove the dialog as needed
            if (!Visible && (ExpiredReminders.Count > 0))
            {
                Visible = true;
            }
            else if (Visible && (ExpiredReminders.Count == 0))
            {
                Visible = false;
            }
        }

        /// <summary>
        /// Have the user login in order to make selections and updates.
        /// </summary>
        /// <returns>Flag indicating if the login was successful.</returns>
        private bool Login()
        {   // Create a new login dialog
            bool loginSuccessful = false;
            UserLogin loginDialog = new UserLogin(DisplayLocalizer);
            // Allow 3 attempts to enter the password before closing down.
            for (int attempt = 0; (attempt < 3) && !loginSuccessful; attempt++)
            {
                if (loginDialog.ShowDialog() == DialogResult.OK)
                {   // Verify the password was correct
                    loginSuccessful = loginDialog.VerifyPassword();
                }
                else
                {
                    break;
                }
            }
            return loginSuccessful;
        }
        
        /// <summary>
        /// Update the displayed language on the form.
        /// </summary>
        public void UpdateDisplayLanguage()
        {
            Text = DisplayLocalizer.GetLocalizedString("MaintenanceItemDueTitle");
            m_maintenanceItemsDueLabel.Text = DisplayLocalizer.GetLocalizedString("MaintenanceDueLabel");
            m_expiredReminderDataGridView.Columns["ReminderItem"].HeaderText = DisplayLocalizer.GetLocalizedString("ReminderNameLabel");
            m_expiredReminderDataGridView.Columns["MaintenanceText"].HeaderText = DisplayLocalizer.GetLocalizedString("MaintenanceTextLabel");
            m_expiredReminderDataGridView.Columns["MaintenanceComplete"].HeaderText = DisplayLocalizer.GetLocalizedString("MaintenanceCompleteLabel");
        }




        //=========================================================================================
        // Member Variables and Properties
        //=========================================================================================

        /// <summary>
        /// Localizer to use for setting the displayed language on the form.
        /// </summary>
        private Localizer DisplayLocalizer { get; set; }

        /// <summary>
        /// Logger object for logging information.
        /// </summary>
        private CcrtLogger Logger { get; set; }

        /// <summary>
        /// List of configured reminders.
        /// </summary>
        private List<MaintenanceReminderItem> Reminders { get; set; }

        /// <summary>
        /// List of expired maintenance items.
        /// </summary>
        public List<String> ExpiredReminders { get; private set; }





        //=========================================================================================
        // Callbacks, Delegates and Events
        //=========================================================================================

        /// <summary>
        /// The user clicked on a cell.  Handle the event.
        /// Only need to do something if the button was clicked.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void m_expiredReminderDataGridView_CellClick(object sender, DataGridViewCellEventArgs e)
        {
            if ((e.RowIndex >= 0) &&
                (e.ColumnIndex == m_expiredReminderDataGridView.Columns["MaintenanceComplete"].Index))
            {   // The user clicked the Close Maintenance Item button
                TopMost = false;
                if (Login())
                {
                    String maintenanceName = m_expiredReminderDataGridView.Rows[e.RowIndex].Cells["ReminderItem"].Value.ToString().Replace(" ", "_");
                    CloseMaintenanceItemForm closeDlg = new CloseMaintenanceItemForm(Reminders.Find(rem => rem.ItemSettings.Name.Equals(maintenanceName)),
                                                                                     Logger, DisplayLocalizer);
                    closeDlg.TopMost = true;
                    closeDlg.ShowDialog();
                    MaintenanceReminderFile.SaveReminders(Reminders, Logger);
                    // NOTE:  This assumes the user closed the maintenance item.  If the item was not closed,
                    //        it will show up again on the next scan of elapsed times.
                    ExpiredReminders.Remove(maintenanceName);
                    DisplayExpiredMaintenanceItems();
                }
                TopMost = true;
            }
            if ((e.RowIndex >= 0) && (e.ColumnIndex == m_expiredReminderDataGridView.Columns["ViewWorkInstruction"].Index))
            {
                String reminderItem = m_expiredReminderDataGridView.Rows[e.RowIndex].Cells["ReminderItem"].Value.ToString().Replace(" ", "_");
                MaintenanceReminderItem item = Reminders.Find(reminder => reminder.ItemSettings.Name.Equals(reminderItem));
                if (item != null)
                {   // Display the work instruction
                    item.ViewWorkInstruction();
                }
            }                
        }

    }
}
