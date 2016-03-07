using System;
using System.Collections.Generic;
using System.Globalization;
using System.Linq;
using System.Resources;
using System.Text;
using Logger;

namespace MaintenanceReminder
{
    public class Localizer
    {
        // ----------------------------------------------------------------------------------------
        // Class Functions and Methods
        // ----------------------------------------------------------------------------------------

        public Localizer(String currentLanguage, CcrtLogger logFile)
        {
            LogFile = logFile;
            SetupLocalization(currentLanguage);
        }

        /// <summary>
        /// Look up the localized text for the given tag.
        /// </summary>
        /// <param name="tag">Tag to localize.</param>
        /// <returns>Localized string to display on the form.</returns>
        public String GetLocalizedString(String tag)
        {
            String localString = LanguageResMgr.GetString(tag, DisplayCulture);
            return localString;
        }

        /// <summary>
        /// Setup for localizing text on forms.
        /// </summary>
        /// <param name="currentLanguage">Local language to use.</param>
        private void SetupLocalization(String currentLanguage)
        {
            LogFile.Log("Changing local language to " + currentLanguage, "Localizer");
            DisplayCulture = new CultureInfo(currentLanguage, true);
            LanguageResMgr = new ResourceManager("MaintenanceReminder.MaintenanceReminderLanguageResource", 
                                                 typeof(MaintenanceReminderForm).Assembly);
            MaintenanceReminder.Properties.Settings.Default.CurrentLanguage = currentLanguage;
            MaintenanceReminder.Properties.Settings.Default.Save();
        }

        /// <summary>
        /// Allow the user to change the current locale.
        /// The localization will also be changed so the forms can be updated.
        /// </summary>
        public void SwitchLocale()
        {   // Store the current language
            String orgLanguage = MaintenanceReminder.Properties.Settings.Default.CurrentLanguage;
            // Display the form for the user
            LocalizationForm form = new LocalizationForm(DisplayCulture, LanguageResMgr);
            form.ShowDialog();
            // Check if the settings have changed
            if (orgLanguage != MaintenanceReminder.Properties.Settings.Default.CurrentLanguage)
            {
                SetupLocalization(MaintenanceReminder.Properties.Settings.Default.CurrentLanguage);
            }
        }

        // ----------------------------------------------------------------------------------------
        // Class Variables and Properties
        // ----------------------------------------------------------------------------------------

        /// <summary>
        /// Get/Set the display culture to be used for all forms.
        /// </summary>
        private CultureInfo DisplayCulture { get; set; }

        /// <summary>
        /// Get/Set the resource manager to use for getting localized strings.
        /// </summary>
        private ResourceManager LanguageResMgr { get; set; }

        /// <summary>
        /// Get/Set the logger instance to use for logging information.
        /// </summary>
        private CcrtLogger LogFile { get; set; }
    }
}
