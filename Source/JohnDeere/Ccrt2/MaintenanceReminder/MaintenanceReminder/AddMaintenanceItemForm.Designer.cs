namespace MaintenanceReminder
{
    partial class AddMaintenanceItemForm
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
            this.m_maintenanceItemTextBox = new System.Windows.Forms.TextBox();
            this.m_maintenanceIntervalLabel = new System.Windows.Forms.Label();
            this.m_intervalTextBox = new System.Windows.Forms.TextBox();
            this.m_intervalUnitsComboBox = new System.Windows.Forms.ComboBox();
            this.m_faultMessageLabel = new System.Windows.Forms.Label();
            this.m_faultMessageTextBox = new System.Windows.Forms.TextBox();
            this.m_closeButton = new System.Windows.Forms.Button();
            this.m_addButton = new System.Windows.Forms.Button();
            this.m_workInstructionLabel = new System.Windows.Forms.Label();
            this.m_workInstructionTextBox = new System.Windows.Forms.TextBox();
            this.m_wiBrowseButton = new System.Windows.Forms.Button();
            this.SuspendLayout();
            // 
            // m_maintenanceItemLabel
            // 
            this.m_maintenanceItemLabel.AutoSize = true;
            this.m_maintenanceItemLabel.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.m_maintenanceItemLabel.Location = new System.Drawing.Point(12, 19);
            this.m_maintenanceItemLabel.Name = "m_maintenanceItemLabel";
            this.m_maintenanceItemLabel.Size = new System.Drawing.Size(112, 13);
            this.m_maintenanceItemLabel.TabIndex = 0;
            this.m_maintenanceItemLabel.Text = "Maintenance Item:";
            // 
            // m_maintenanceItemTextBox
            // 
            this.m_maintenanceItemTextBox.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.m_maintenanceItemTextBox.Location = new System.Drawing.Point(15, 35);
            this.m_maintenanceItemTextBox.MaxLength = 128;
            this.m_maintenanceItemTextBox.Name = "m_maintenanceItemTextBox";
            this.m_maintenanceItemTextBox.Size = new System.Drawing.Size(348, 20);
            this.m_maintenanceItemTextBox.TabIndex = 1;
            // 
            // m_maintenanceIntervalLabel
            // 
            this.m_maintenanceIntervalLabel.AutoSize = true;
            this.m_maintenanceIntervalLabel.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.m_maintenanceIntervalLabel.Location = new System.Drawing.Point(10, 78);
            this.m_maintenanceIntervalLabel.Name = "m_maintenanceIntervalLabel";
            this.m_maintenanceIntervalLabel.Size = new System.Drawing.Size(131, 13);
            this.m_maintenanceIntervalLabel.TabIndex = 2;
            this.m_maintenanceIntervalLabel.Text = "Maintenance Interval:";
            // 
            // m_intervalTextBox
            // 
            this.m_intervalTextBox.Location = new System.Drawing.Point(13, 94);
            this.m_intervalTextBox.Name = "m_intervalTextBox";
            this.m_intervalTextBox.Size = new System.Drawing.Size(83, 20);
            this.m_intervalTextBox.TabIndex = 3;
            // 
            // m_intervalUnitsComboBox
            // 
            this.m_intervalUnitsComboBox.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.m_intervalUnitsComboBox.FormattingEnabled = true;
            this.m_intervalUnitsComboBox.Items.AddRange(new object[] {
            "Hours",
            "Days",
            "Weeks",
            "Months",
            "Years"});
            this.m_intervalUnitsComboBox.Location = new System.Drawing.Point(102, 94);
            this.m_intervalUnitsComboBox.MaxDropDownItems = 10;
            this.m_intervalUnitsComboBox.Name = "m_intervalUnitsComboBox";
            this.m_intervalUnitsComboBox.Size = new System.Drawing.Size(85, 21);
            this.m_intervalUnitsComboBox.TabIndex = 4;
            // 
            // m_faultMessageLabel
            // 
            this.m_faultMessageLabel.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.m_faultMessageLabel.AutoSize = true;
            this.m_faultMessageLabel.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.m_faultMessageLabel.Location = new System.Drawing.Point(9, 200);
            this.m_faultMessageLabel.Name = "m_faultMessageLabel";
            this.m_faultMessageLabel.Size = new System.Drawing.Size(243, 13);
            this.m_faultMessageLabel.TabIndex = 5;
            this.m_faultMessageLabel.Text = "Message to display when interval expires:";
            // 
            // m_faultMessageTextBox
            // 
            this.m_faultMessageTextBox.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.m_faultMessageTextBox.Location = new System.Drawing.Point(12, 216);
            this.m_faultMessageTextBox.MaxLength = 512;
            this.m_faultMessageTextBox.Multiline = true;
            this.m_faultMessageTextBox.Name = "m_faultMessageTextBox";
            this.m_faultMessageTextBox.Size = new System.Drawing.Size(485, 132);
            this.m_faultMessageTextBox.TabIndex = 6;
            // 
            // m_closeButton
            // 
            this.m_closeButton.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.m_closeButton.DialogResult = System.Windows.Forms.DialogResult.Cancel;
            this.m_closeButton.Location = new System.Drawing.Point(422, 354);
            this.m_closeButton.Name = "m_closeButton";
            this.m_closeButton.Size = new System.Drawing.Size(75, 23);
            this.m_closeButton.TabIndex = 7;
            this.m_closeButton.Text = "Close";
            this.m_closeButton.UseVisualStyleBackColor = true;
            // 
            // m_addButton
            // 
            this.m_addButton.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.m_addButton.Location = new System.Drawing.Point(341, 354);
            this.m_addButton.Name = "m_addButton";
            this.m_addButton.Size = new System.Drawing.Size(75, 23);
            this.m_addButton.TabIndex = 8;
            this.m_addButton.Text = "Add";
            this.m_addButton.UseVisualStyleBackColor = true;
            this.m_addButton.Click += new System.EventHandler(this.m_addButton_Click);
            // 
            // m_workInstructionLabel
            // 
            this.m_workInstructionLabel.AutoSize = true;
            this.m_workInstructionLabel.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.m_workInstructionLabel.Location = new System.Drawing.Point(12, 134);
            this.m_workInstructionLabel.Name = "m_workInstructionLabel";
            this.m_workInstructionLabel.Size = new System.Drawing.Size(166, 13);
            this.m_workInstructionLabel.TabIndex = 9;
            this.m_workInstructionLabel.Text = "Work Instruction Document:";
            // 
            // m_workInstructionTextBox
            // 
            this.m_workInstructionTextBox.Location = new System.Drawing.Point(15, 150);
            this.m_workInstructionTextBox.Name = "m_workInstructionTextBox";
            this.m_workInstructionTextBox.Size = new System.Drawing.Size(401, 20);
            this.m_workInstructionTextBox.TabIndex = 10;
            // 
            // m_wiBrowseButton
            // 
            this.m_wiBrowseButton.Location = new System.Drawing.Point(422, 150);
            this.m_wiBrowseButton.Name = "m_wiBrowseButton";
            this.m_wiBrowseButton.Size = new System.Drawing.Size(75, 20);
            this.m_wiBrowseButton.TabIndex = 11;
            this.m_wiBrowseButton.Text = "Browse...";
            this.m_wiBrowseButton.UseVisualStyleBackColor = true;
            this.m_wiBrowseButton.Click += new System.EventHandler(this.m_wiBrowseButton_Click);
            // 
            // AddMaintenanceItemForm
            // 
            this.AcceptButton = this.m_addButton;
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.BackColor = System.Drawing.Color.LightSteelBlue;
            this.CancelButton = this.m_closeButton;
            this.ClientSize = new System.Drawing.Size(509, 389);
            this.Controls.Add(this.m_wiBrowseButton);
            this.Controls.Add(this.m_workInstructionTextBox);
            this.Controls.Add(this.m_workInstructionLabel);
            this.Controls.Add(this.m_addButton);
            this.Controls.Add(this.m_closeButton);
            this.Controls.Add(this.m_faultMessageTextBox);
            this.Controls.Add(this.m_faultMessageLabel);
            this.Controls.Add(this.m_intervalUnitsComboBox);
            this.Controls.Add(this.m_intervalTextBox);
            this.Controls.Add(this.m_maintenanceIntervalLabel);
            this.Controls.Add(this.m_maintenanceItemTextBox);
            this.Controls.Add(this.m_maintenanceItemLabel);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedToolWindow;
            this.Name = "AddMaintenanceItemForm";
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterParent;
            this.Text = "Add Maintenance Reminder";
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Label m_maintenanceItemLabel;
        private System.Windows.Forms.TextBox m_maintenanceItemTextBox;
        private System.Windows.Forms.Label m_maintenanceIntervalLabel;
        private System.Windows.Forms.TextBox m_intervalTextBox;
        private System.Windows.Forms.ComboBox m_intervalUnitsComboBox;
        private System.Windows.Forms.Label m_faultMessageLabel;
        private System.Windows.Forms.TextBox m_faultMessageTextBox;
        private System.Windows.Forms.Button m_closeButton;
        private System.Windows.Forms.Button m_addButton;
        private System.Windows.Forms.Label m_workInstructionLabel;
        private System.Windows.Forms.TextBox m_workInstructionTextBox;
        private System.Windows.Forms.Button m_wiBrowseButton;
    }
}