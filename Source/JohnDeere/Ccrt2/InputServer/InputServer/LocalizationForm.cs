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

namespace InputServer
{
    public partial class LocalizationForm : Form
    {
        // ----------------------------------------------------------------------------------------
        // Class Functions and Methods
        // ----------------------------------------------------------------------------------------

        public LocalizationForm(CultureInfo culture, ResourceManager resMgr)
        {
            InitializeComponent();
            // Store the items passed in
            DisplayCulture = culture;
            LanguageResMgr = resMgr;
            // Select the correct radio button to be checked
            if (InputServer.Properties.Settings.Default.CurrentLanguage == "en-US")
            {
                m_englishRadioButton.Checked = true;
                m_portugueseRadioButton.Checked = false;
            }
            else
            {
                m_englishRadioButton.Checked = false;
                m_portugueseRadioButton.Checked = true;
            }
            // Display the correct text in the form
            UpdateDisplayLanguage();
        }

        /// <summary>
        /// Look up the localized text for the given tag.
        /// </summary>
        /// <param name="tag">Tag to localize.</param>
        /// <returns>Localized string to display on the form.</returns>
        private String GetLocalizedString(String tag)
        {
            return LanguageResMgr.GetString(tag, DisplayCulture);
        }

        /// <summary>
        /// Update the form with the correct localized text.
        /// </summary>
        private void UpdateDisplayLanguage()
        {
            Text = GetLocalizedString("LocalizationTitle");
            m_languageLabel.Text = GetLocalizedString("LanguageLabel");
            m_englishRadioButton.Text = GetLocalizedString("EnglishOption");
            m_portugueseRadioButton.Text = GetLocalizedString("PortugueseOption");
            m_closeButton.Text = GetLocalizedString("CloseLabel");
            m_saveButton.Text = GetLocalizedString("SaveLabel");
        }


        // ----------------------------------------------------------------------------------------
        // Member Variables and Properties
        // ----------------------------------------------------------------------------------------

        /// <summary>
        /// Get/Set the current culture information being used for displaying data.
        /// </summary>
        private CultureInfo DisplayCulture { get; set; }

        /// <summary>
        /// Get/Set the resource manager to use for getting localized text.
        /// </summary>
        private ResourceManager LanguageResMgr { get; set; }


        // ----------------------------------------------------------------------------------------
        // Callbacks
        // ----------------------------------------------------------------------------------------

        /// <summary>
        /// Save the current language settings.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void m_saveButton_Click(object sender, EventArgs e)
        {   // Store the selected language option
            if (m_englishRadioButton.Checked)
            {
                InputServer.Properties.Settings.Default.CurrentLanguage = "en-US";
            }
            else
            {
                InputServer.Properties.Settings.Default.CurrentLanguage = "pt-BR";
            }
            InputServer.Properties.Settings.Default.Save();
            // Create new objects for correctly displaying the form
            DisplayCulture = new CultureInfo(InputServer.Properties.Settings.Default.CurrentLanguage, true);
            LanguageResMgr = new ResourceManager("InputServer.InputServerLanguageResource", typeof(InputServerForm).Assembly);
            // Redisplay the data in the form
            UpdateDisplayLanguage();
        }

    }
}
