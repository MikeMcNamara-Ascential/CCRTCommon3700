namespace MaintenanceReminder
{
    partial class LocalizationForm
    {
        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows Form Designer generated code

        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            this.m_languageLabel = new System.Windows.Forms.Label();
            this.m_englishRadioButton = new System.Windows.Forms.RadioButton();
            this.m_spanishRadioButton = new System.Windows.Forms.RadioButton();
            this.m_saveButton = new System.Windows.Forms.Button();
            this.m_closeButton = new System.Windows.Forms.Button();
            this.SuspendLayout();
            // 
            // m_languageLabel
            // 
            this.m_languageLabel.AutoSize = true;
            this.m_languageLabel.Font = new System.Drawing.Font("Microsoft Sans Serif", 10F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.m_languageLabel.Location = new System.Drawing.Point(12, 27);
            this.m_languageLabel.Name = "m_languageLabel";
            this.m_languageLabel.Size = new System.Drawing.Size(85, 17);
            this.m_languageLabel.TabIndex = 0;
            this.m_languageLabel.Text = "Language:";
            // 
            // m_englishRadioButton
            // 
            this.m_englishRadioButton.AutoSize = true;
            this.m_englishRadioButton.Location = new System.Drawing.Point(62, 64);
            this.m_englishRadioButton.Name = "m_englishRadioButton";
            this.m_englishRadioButton.Size = new System.Drawing.Size(59, 17);
            this.m_englishRadioButton.TabIndex = 1;
            this.m_englishRadioButton.TabStop = true;
            this.m_englishRadioButton.Text = "English";
            this.m_englishRadioButton.UseVisualStyleBackColor = true;
            // 
            // m_spanishRadioButton
            // 
            this.m_spanishRadioButton.AutoSize = true;
            this.m_spanishRadioButton.Location = new System.Drawing.Point(62, 97);
            this.m_spanishRadioButton.Name = "m_spanishRadioButton";
            this.m_spanishRadioButton.Size = new System.Drawing.Size(63, 17);
            this.m_spanishRadioButton.TabIndex = 2;
            this.m_spanishRadioButton.TabStop = true;
            this.m_spanishRadioButton.Text = "Spanish";
            this.m_spanishRadioButton.UseVisualStyleBackColor = true;
            // 
            // m_saveButton
            // 
            this.m_saveButton.Location = new System.Drawing.Point(123, 155);
            this.m_saveButton.Name = "m_saveButton";
            this.m_saveButton.Size = new System.Drawing.Size(75, 23);
            this.m_saveButton.TabIndex = 3;
            this.m_saveButton.Text = "Save";
            this.m_saveButton.UseVisualStyleBackColor = true;
            this.m_saveButton.Click += new System.EventHandler(this.m_saveButton_Click);
            // 
            // m_closeButton
            // 
            this.m_closeButton.Location = new System.Drawing.Point(12, 155);
            this.m_closeButton.Name = "m_closeButton";
            this.m_closeButton.Size = new System.Drawing.Size(75, 23);
            this.m_closeButton.TabIndex = 4;
            this.m_closeButton.Text = "Close";
            this.m_closeButton.UseVisualStyleBackColor = true;
            // 
            // LocalizationForm
            // 
            this.AcceptButton = this.m_saveButton;
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.CancelButton = this.m_closeButton;
            this.ClientSize = new System.Drawing.Size(210, 190);
            this.Controls.Add(this.m_closeButton);
            this.Controls.Add(this.m_saveButton);
            this.Controls.Add(this.m_spanishRadioButton);
            this.Controls.Add(this.m_englishRadioButton);
            this.Controls.Add(this.m_languageLabel);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedToolWindow;
            this.Name = "LocalizationForm";
            this.Text = "Localization";
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Label m_languageLabel;
        private System.Windows.Forms.RadioButton m_englishRadioButton;
        private System.Windows.Forms.RadioButton m_spanishRadioButton;
        private System.Windows.Forms.Button m_saveButton;
        private System.Windows.Forms.Button m_closeButton;
    }
}