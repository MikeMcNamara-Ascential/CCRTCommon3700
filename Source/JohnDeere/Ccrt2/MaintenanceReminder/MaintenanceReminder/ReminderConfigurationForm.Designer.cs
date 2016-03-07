namespace MaintenanceReminder
{
    partial class ReminderConfigurationForm
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
            this.m_maintenanceItemLabel = new System.Windows.Forms.Label();
            this.m_maintenanceItemsComboBox = new System.Windows.Forms.ComboBox();
            this.m_maintenanceIntervalLabel = new System.Windows.Forms.Label();
            this.m_intervalText = new System.Windows.Forms.Label();
            this.m_intervalUnitsText = new System.Windows.Forms.Label();
            this.m_currentElapsedTimeLabel = new System.Windows.Forms.Label();
            this.m_elapsedTimeText = new System.Windows.Forms.Label();
            this.m_elapsedTimeUnitsText = new System.Windows.Forms.Label();
            this.m_faultMessageLabel = new System.Windows.Forms.Label();
            this.m_faultMessageTextBox = new System.Windows.Forms.TextBox();
            this.m_maintenanceNotesLabel = new System.Windows.Forms.Label();
            this.m_maintenanceNotesTextBox = new System.Windows.Forms.TextBox();
            this.m_addNoteButton = new System.Windows.Forms.Button();
            this.m_addButton = new System.Windows.Forms.Button();
            this.m_editButton = new System.Windows.Forms.Button();
            this.m_deleteButton = new System.Windows.Forms.Button();
            this.m_closeButton = new System.Windows.Forms.Button();
            this.SuspendLayout();
            // 
            // m_maintenanceItemLabel
            // 
            this.m_maintenanceItemLabel.AutoSize = true;
            this.m_maintenanceItemLabel.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.m_maintenanceItemLabel.Location = new System.Drawing.Point(31, 20);
            this.m_maintenanceItemLabel.Name = "m_maintenanceItemLabel";
            this.m_maintenanceItemLabel.Size = new System.Drawing.Size(112, 13);
            this.m_maintenanceItemLabel.TabIndex = 0;
            this.m_maintenanceItemLabel.Text = "Maintenance Item:";
            // 
            // m_maintenanceItemsComboBox
            // 
            this.m_maintenanceItemsComboBox.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.m_maintenanceItemsComboBox.FormattingEnabled = true;
            this.m_maintenanceItemsComboBox.Location = new System.Drawing.Point(149, 17);
            this.m_maintenanceItemsComboBox.Name = "m_maintenanceItemsComboBox";
            this.m_maintenanceItemsComboBox.Size = new System.Drawing.Size(282, 21);
            this.m_maintenanceItemsComboBox.TabIndex = 1;
            this.m_maintenanceItemsComboBox.SelectedIndexChanged += new System.EventHandler(this.m_maintenanceItemsComboBox_SelectedIndexChanged);
            // 
            // m_maintenanceIntervalLabel
            // 
            this.m_maintenanceIntervalLabel.AutoSize = true;
            this.m_maintenanceIntervalLabel.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.m_maintenanceIntervalLabel.Location = new System.Drawing.Point(12, 56);
            this.m_maintenanceIntervalLabel.Name = "m_maintenanceIntervalLabel";
            this.m_maintenanceIntervalLabel.Size = new System.Drawing.Size(131, 13);
            this.m_maintenanceIntervalLabel.TabIndex = 2;
            this.m_maintenanceIntervalLabel.Text = "Maintenance Interval:";
            // 
            // m_intervalText
            // 
            this.m_intervalText.AutoSize = true;
            this.m_intervalText.ForeColor = System.Drawing.SystemColors.ControlText;
            this.m_intervalText.Location = new System.Drawing.Point(149, 56);
            this.m_intervalText.Name = "m_intervalText";
            this.m_intervalText.Size = new System.Drawing.Size(35, 13);
            this.m_intervalText.TabIndex = 3;
            this.m_intervalText.Text = "label1";
            // 
            // m_intervalUnitsText
            // 
            this.m_intervalUnitsText.AutoSize = true;
            this.m_intervalUnitsText.Location = new System.Drawing.Point(208, 56);
            this.m_intervalUnitsText.Name = "m_intervalUnitsText";
            this.m_intervalUnitsText.Size = new System.Drawing.Size(35, 13);
            this.m_intervalUnitsText.TabIndex = 4;
            this.m_intervalUnitsText.Text = "label1";
            // 
            // m_currentElapsedTimeLabel
            // 
            this.m_currentElapsedTimeLabel.AutoSize = true;
            this.m_currentElapsedTimeLabel.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.m_currentElapsedTimeLabel.Location = new System.Drawing.Point(11, 82);
            this.m_currentElapsedTimeLabel.Name = "m_currentElapsedTimeLabel";
            this.m_currentElapsedTimeLabel.Size = new System.Drawing.Size(132, 13);
            this.m_currentElapsedTimeLabel.TabIndex = 5;
            this.m_currentElapsedTimeLabel.Text = "Current Elapsed Time:";
            // 
            // m_elapsedTimeText
            // 
            this.m_elapsedTimeText.AutoSize = true;
            this.m_elapsedTimeText.Location = new System.Drawing.Point(149, 82);
            this.m_elapsedTimeText.Name = "m_elapsedTimeText";
            this.m_elapsedTimeText.Size = new System.Drawing.Size(35, 13);
            this.m_elapsedTimeText.TabIndex = 6;
            this.m_elapsedTimeText.Text = "label1";
            // 
            // m_elapsedTimeUnitsText
            // 
            this.m_elapsedTimeUnitsText.AutoSize = true;
            this.m_elapsedTimeUnitsText.Location = new System.Drawing.Point(208, 82);
            this.m_elapsedTimeUnitsText.Name = "m_elapsedTimeUnitsText";
            this.m_elapsedTimeUnitsText.Size = new System.Drawing.Size(35, 13);
            this.m_elapsedTimeUnitsText.TabIndex = 7;
            this.m_elapsedTimeUnitsText.Text = "label1";
            // 
            // m_faultMessageLabel
            // 
            this.m_faultMessageLabel.AutoSize = true;
            this.m_faultMessageLabel.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.m_faultMessageLabel.Location = new System.Drawing.Point(12, 132);
            this.m_faultMessageLabel.Name = "m_faultMessageLabel";
            this.m_faultMessageLabel.Size = new System.Drawing.Size(243, 13);
            this.m_faultMessageLabel.TabIndex = 8;
            this.m_faultMessageLabel.Text = "Message to display when interval expires:";
            // 
            // m_faultMessageTextBox
            // 
            this.m_faultMessageTextBox.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.m_faultMessageTextBox.Location = new System.Drawing.Point(14, 148);
            this.m_faultMessageTextBox.Multiline = true;
            this.m_faultMessageTextBox.Name = "m_faultMessageTextBox";
            this.m_faultMessageTextBox.ReadOnly = true;
            this.m_faultMessageTextBox.Size = new System.Drawing.Size(602, 114);
            this.m_faultMessageTextBox.TabIndex = 9;
            // 
            // m_maintenanceNotesLabel
            // 
            this.m_maintenanceNotesLabel.AutoSize = true;
            this.m_maintenanceNotesLabel.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.m_maintenanceNotesLabel.Location = new System.Drawing.Point(12, 295);
            this.m_maintenanceNotesLabel.Name = "m_maintenanceNotesLabel";
            this.m_maintenanceNotesLabel.Size = new System.Drawing.Size(121, 13);
            this.m_maintenanceNotesLabel.TabIndex = 10;
            this.m_maintenanceNotesLabel.Text = "Maintenance Notes:";
            // 
            // m_maintenanceNotesTextBox
            // 
            this.m_maintenanceNotesTextBox.Location = new System.Drawing.Point(15, 311);
            this.m_maintenanceNotesTextBox.Multiline = true;
            this.m_maintenanceNotesTextBox.Name = "m_maintenanceNotesTextBox";
            this.m_maintenanceNotesTextBox.ReadOnly = true;
            this.m_maintenanceNotesTextBox.Size = new System.Drawing.Size(601, 208);
            this.m_maintenanceNotesTextBox.TabIndex = 11;
            // 
            // m_addNoteButton
            // 
            this.m_addNoteButton.Location = new System.Drawing.Point(521, 287);
            this.m_addNoteButton.Name = "m_addNoteButton";
            this.m_addNoteButton.Size = new System.Drawing.Size(95, 21);
            this.m_addNoteButton.TabIndex = 12;
            this.m_addNoteButton.Text = "Add Note";
            this.m_addNoteButton.UseVisualStyleBackColor = true;
            this.m_addNoteButton.Visible = false;
            this.m_addNoteButton.Click += new System.EventHandler(this.m_addNoteButton_Click);
            // 
            // m_addButton
            // 
            this.m_addButton.Location = new System.Drawing.Point(15, 531);
            this.m_addButton.Name = "m_addButton";
            this.m_addButton.Size = new System.Drawing.Size(90, 23);
            this.m_addButton.TabIndex = 13;
            this.m_addButton.Text = "Add Reminder";
            this.m_addButton.UseVisualStyleBackColor = true;
            this.m_addButton.Click += new System.EventHandler(this.m_addButton_Click);
            // 
            // m_editButton
            // 
            this.m_editButton.Location = new System.Drawing.Point(111, 531);
            this.m_editButton.Name = "m_editButton";
            this.m_editButton.Size = new System.Drawing.Size(85, 23);
            this.m_editButton.TabIndex = 14;
            this.m_editButton.Text = "Edit Reminder";
            this.m_editButton.UseVisualStyleBackColor = true;
            this.m_editButton.Click += new System.EventHandler(this.m_editButton_Click);
            // 
            // m_deleteButton
            // 
            this.m_deleteButton.Location = new System.Drawing.Point(202, 531);
            this.m_deleteButton.Name = "m_deleteButton";
            this.m_deleteButton.Size = new System.Drawing.Size(96, 23);
            this.m_deleteButton.TabIndex = 15;
            this.m_deleteButton.Text = "Delete Reminder";
            this.m_deleteButton.UseVisualStyleBackColor = true;
            this.m_deleteButton.Click += new System.EventHandler(this.m_deleteButton_Click);
            // 
            // m_closeButton
            // 
            this.m_closeButton.DialogResult = System.Windows.Forms.DialogResult.Cancel;
            this.m_closeButton.Location = new System.Drawing.Point(541, 531);
            this.m_closeButton.Name = "m_closeButton";
            this.m_closeButton.Size = new System.Drawing.Size(75, 23);
            this.m_closeButton.TabIndex = 16;
            this.m_closeButton.Text = "Close";
            this.m_closeButton.UseVisualStyleBackColor = true;
            // 
            // ReminderConfigurationForm
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.BackColor = System.Drawing.Color.LightSteelBlue;
            this.CancelButton = this.m_closeButton;
            this.ClientSize = new System.Drawing.Size(628, 566);
            this.Controls.Add(this.m_closeButton);
            this.Controls.Add(this.m_deleteButton);
            this.Controls.Add(this.m_editButton);
            this.Controls.Add(this.m_addButton);
            this.Controls.Add(this.m_addNoteButton);
            this.Controls.Add(this.m_maintenanceNotesTextBox);
            this.Controls.Add(this.m_maintenanceNotesLabel);
            this.Controls.Add(this.m_faultMessageTextBox);
            this.Controls.Add(this.m_faultMessageLabel);
            this.Controls.Add(this.m_elapsedTimeUnitsText);
            this.Controls.Add(this.m_elapsedTimeText);
            this.Controls.Add(this.m_currentElapsedTimeLabel);
            this.Controls.Add(this.m_intervalUnitsText);
            this.Controls.Add(this.m_intervalText);
            this.Controls.Add(this.m_maintenanceIntervalLabel);
            this.Controls.Add(this.m_maintenanceItemsComboBox);
            this.Controls.Add(this.m_maintenanceItemLabel);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedToolWindow;
            this.Name = "ReminderConfigurationForm";
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterScreen;
            this.Text = "Reminder Configuration";
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Label m_maintenanceItemLabel;
        private System.Windows.Forms.ComboBox m_maintenanceItemsComboBox;
        private System.Windows.Forms.Label m_maintenanceIntervalLabel;
        private System.Windows.Forms.Label m_intervalText;
        private System.Windows.Forms.Label m_intervalUnitsText;
        private System.Windows.Forms.Label m_currentElapsedTimeLabel;
        private System.Windows.Forms.Label m_elapsedTimeText;
        private System.Windows.Forms.Label m_elapsedTimeUnitsText;
        private System.Windows.Forms.Label m_faultMessageLabel;
        private System.Windows.Forms.TextBox m_faultMessageTextBox;
        private System.Windows.Forms.Label m_maintenanceNotesLabel;
        private System.Windows.Forms.TextBox m_maintenanceNotesTextBox;
        private System.Windows.Forms.Button m_addNoteButton;
        private System.Windows.Forms.Button m_addButton;
        private System.Windows.Forms.Button m_editButton;
        private System.Windows.Forms.Button m_deleteButton;
        private System.Windows.Forms.Button m_closeButton;
    }
}