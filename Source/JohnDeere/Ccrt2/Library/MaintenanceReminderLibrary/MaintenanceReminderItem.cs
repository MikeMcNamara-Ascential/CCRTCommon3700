using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
using System.Text;

namespace MaintenanceReminderLibrary
{
    public class MaintenanceReminderItem
    {
        //=========================================================================================
        // Member Functions
        //=========================================================================================
        
        /// <summary>
        /// Class Constructor.
        /// A new reminder item will be created with the provided settings.
        /// </summary>
        /// <param name="itemSettings">Settings for this maintenance reminder.</param>
        public MaintenanceReminderItem(ReminderSettings itemSettings)
        {
            ItemSettings = itemSettings;
        }

        /// <summary>
        /// Add a maintenance note from the user.
        /// </summary>
        /// <param name="note">Note to be added.</param>
        public void AddMaintenanceNote(String note)
        {   // Add the current date and time stamp to the note
            ItemSettings.MaintenanceNotes += DateTime.Now + " -- " + note + Environment.NewLine;
        }

        /// <summary>
        /// Update the current elapsed time.
        /// The specified elapsed time will be added to the current elapsed time.
        /// </summary>
        /// <param name="elapsedTime">Time to add to the total elapsed time.</param>
        public void UpdateElapsedTime(Double elapsedTime)
        {
            ItemSettings.CurrentElapsedTime += elapsedTime;
        }

        /// <summary>
        /// Update the settings for the current reminder.
        /// </summary>
        /// <param name="settings">New reminder settings.</param>
        public void UpdateReminderSettings(ReminderSettings settings)
        {
            ItemSettings = settings;
        }

        /// <summary>
        /// Display the work instruction document for the user.
        /// </summary>
        public void ViewWorkInstruction()
        {
            if (ItemSettings.WorkInstructionDocument.Length > 0)
            {
                Process.Start(ItemSettings.WorkInstructionDocument);
            }
        }


        //=========================================================================================
        // Member Variables and Properties
        //=========================================================================================

        /// <summary>
        /// Get/Set the settings for this reminder.
        /// </summary>
        public ReminderSettings ItemSettings { get; private set; }




        //=========================================================================================
        // Callbacks, Delegates and Events
        //=========================================================================================


    }
}
