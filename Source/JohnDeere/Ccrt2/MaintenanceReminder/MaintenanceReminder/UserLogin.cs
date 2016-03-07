using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using System.IO;

namespace MaintenanceReminder
{
    public partial class UserLogin : Form
    {
        // ----------------------------------------------------------------------------------------
        // Member Functions and Methods
        // ----------------------------------------------------------------------------------------

        public UserLogin(Localizer localizer)
        {
            InitializeComponent();
            CurrentPassword = "bep";
            DisplayLocalizer = localizer;
            // Store the current runtime directory
            RuntimeDirectory = Directory.GetCurrentDirectory();
            // Load the password
            LoadPassword();
        }

        /// <summary>
        /// Setup the window to handle entering a new password.
        /// </summary>
        public void ChangePasswordSetup()
        {   // Change the default title and text
            Text = DisplayLocalizer.GetLocalizedString("EnterNewPasswordTitle");
            m_passwordLabel.Text = DisplayLocalizer.GetLocalizedString("EnterNewPasswordLabel");
            m_loginButton.Text = DisplayLocalizer.GetLocalizedString("OkLabel");
        }

        /// <summary>
        /// Retrieve the current password from the password file if it exists.
        /// </summary>
        private void LoadPassword()
        {
            String passwdFile = Path.Combine(RuntimeDirectory, PasswordFileName);
            if (File.Exists(passwdFile))
            {   // Load the password from the file
                using (StreamReader file = new StreamReader(passwdFile))
                {
                    byte[] passwdBytes = new byte[10];
                    int index = 0;
                    string passChar;
                    while ((passChar = file.ReadLine()) != null)
                    {
                        passwdBytes[index] = Convert.ToByte(passChar);
                        index++;
                    }
                    // Convert the password to a string
                    char[] passwdChars = Encoding.Default.GetChars(passwdBytes, 0, index);
                    CurrentPassword = new string(passwdChars);
                }
            }
        }

        /// <summary>
        /// Save the new password to a file.
        /// </summary>
        public void SaveNewPassword()
        {   // Open the password data file
            using (StreamWriter file = new StreamWriter(Path.Combine(RuntimeDirectory, PasswordFileName), false))
            {
                Encoding utf8 = Encoding.UTF8;
                byte[] passwd = utf8.GetBytes(m_passwordTextBox.Text);
                foreach (byte passwdByte in passwd)
                {   // Write the byte to the file
                    file.WriteLine(passwdByte);
                }
            }
        }

        /// <summary>
        /// Update the language on the display to the selected language.
        /// </summary>
        private void UpdateDisplayLanguage()
        {
            m_cancelButton.Text = DisplayLocalizer.GetLocalizedString("CancelLabel");
            m_loginButton.Text = DisplayLocalizer.GetLocalizedString("LoginLabel");
            m_passwordLabel.Text = DisplayLocalizer.GetLocalizedString("PasswordLabel");
        }

        /// <summary>
        /// Verify the password entered by the user is valid.
        /// </summary>
        /// <returns>Flag indicating if the password was correct.</returns>
        public bool VerifyPassword()
        {   // Verify the password matches the configured password
            bool goodPasswd = CurrentPassword == m_passwordTextBox.Text;
            // Clear the text box
            m_passwordTextBox.Clear();
            return goodPasswd;
        }



        // ----------------------------------------------------------------------------------------
        // Member Variables and Properties
        // ----------------------------------------------------------------------------------------

        /// <summary>
        /// Localizer to use for display text in the selected language
        /// </summary>
        private Localizer DisplayLocalizer { get; set; }

        /// <summary>
        /// Get/Set the runtime directory.
        /// </summary>
        private String RuntimeDirectory { get; set; }

        /// <summary>
        /// Get the name of the password file.
        /// </summary>
        private String PasswordFileName { get { return "passwd.dat"; } }

        /// <summary>
        /// Get/Set the current password.
        /// </summary>
        private String CurrentPassword { get; set; }




        // ----------------------------------------------------------------------------------------
        // Delegates, Events and Callbacks
        // ----------------------------------------------------------------------------------------

    }
}