namespace MaintenanceReminder
{
    partial class CloseMaintenanceItemForm
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
            this.m_notesTextBox = new System.Windows.Forms.TextBox();
            this.m_notesLabel = new System.Windows.Forms.Label();
            this.m_saveButton = new System.Windows.Forms.Button();
            this.m_itemCompleteCheckBox = new System.Windows.Forms.CheckBox();
            this.m_itemLabel = new System.Windows.Forms.Label();
            this.m_itemName = new System.Windows.Forms.Label();
            this.m_itemTextLabel = new System.Windows.Forms.Label();
            this.m_itemText = new System.Windows.Forms.Label();
            this.m_nameLabel = new System.Windows.Forms.Label();
            this.m_nameTextBox = new System.Windows.Forms.TextBox();
            this.m_viewWiButton = new System.Windows.Forms.Button();
            this.SuspendLayout();
            // 
            // m_notesTextBox
            // 
            this.m_notesTextBox.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.m_notesTextBox.Location = new System.Drawing.Point(12, 161);
            this.m_notesTextBox.Multiline = true;
            this.m_notesTextBox.Name = "m_notesTextBox";
            this.m_notesTextBox.Size = new System.Drawing.Size(544, 184);
            this.m_notesTextBox.TabIndex = 0;
            // 
            // m_notesLabel
            // 
            this.m_notesLabel.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.m_notesLabel.AutoSize = true;
            this.m_notesLabel.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.m_notesLabel.Location = new System.Drawing.Point(9, 145);
            this.m_notesLabel.Name = "m_notesLabel";
            this.m_notesLabel.Size = new System.Drawing.Size(44, 13);
            this.m_notesLabel.TabIndex = 1;
            this.m_notesLabel.Text = "Notes:";
            // 
            // m_saveButton
            // 
            this.m_saveButton.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.m_saveButton.DialogResult = System.Windows.Forms.DialogResult.Cancel;
            this.m_saveButton.Location = new System.Drawing.Point(481, 351);
            this.m_saveButton.Name = "m_saveButton";
            this.m_saveButton.Size = new System.Drawing.Size(75, 23);
            this.m_saveButton.TabIndex = 2;
            this.m_saveButton.Text = "Save";
            this.m_saveButton.UseVisualStyleBackColor = true;
            this.m_saveButton.Click += new System.EventHandler(this.m_saveButton_Click);
            // 
            // m_itemCompleteCheckBox
            // 
            this.m_itemCompleteCheckBox.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.m_itemCompleteCheckBox.AutoSize = true;
            this.m_itemCompleteCheckBox.Location = new System.Drawing.Point(12, 355);
            this.m_itemCompleteCheckBox.Name = "m_itemCompleteCheckBox";
            this.m_itemCompleteCheckBox.Size = new System.Drawing.Size(158, 17);
            this.m_itemCompleteCheckBox.TabIndex = 3;
            this.m_itemCompleteCheckBox.Text = "Maintenance Item Complete";
            this.m_itemCompleteCheckBox.UseVisualStyleBackColor = true;
            // 
            // m_itemLabel
            // 
            this.m_itemLabel.AutoSize = true;
            this.m_itemLabel.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.m_itemLabel.Location = new System.Drawing.Point(9, 19);
            this.m_itemLabel.Name = "m_itemLabel";
            this.m_itemLabel.Size = new System.Drawing.Size(112, 13);
            this.m_itemLabel.TabIndex = 4;
            this.m_itemLabel.Text = "Maintenance Item:";
            // 
            // m_itemName
            // 
            this.m_itemName.AutoSize = true;
            this.m_itemName.Location = new System.Drawing.Point(12, 32);
            this.m_itemName.Name = "m_itemName";
            this.m_itemName.Size = new System.Drawing.Size(35, 13);
            this.m_itemName.TabIndex = 5;
            this.m_itemName.Text = "label1";
            // 
            // m_itemTextLabel
            // 
            this.m_itemTextLabel.AutoSize = true;
            this.m_itemTextLabel.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.m_itemTextLabel.Location = new System.Drawing.Point(9, 59);
            this.m_itemTextLabel.Name = "m_itemTextLabel";
            this.m_itemTextLabel.Size = new System.Drawing.Size(113, 13);
            this.m_itemTextLabel.TabIndex = 6;
            this.m_itemTextLabel.Text = "Maintenance Text:";
            // 
            // m_itemText
            // 
            this.m_itemText.AutoSize = true;
            this.m_itemText.Location = new System.Drawing.Point(9, 72);
            this.m_itemText.Name = "m_itemText";
            this.m_itemText.Size = new System.Drawing.Size(35, 13);
            this.m_itemText.TabIndex = 7;
            this.m_itemText.Text = "label1";
            // 
            // m_nameLabel
            // 
            this.m_nameLabel.AutoSize = true;
            this.m_nameLabel.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.m_nameLabel.Location = new System.Drawing.Point(9, 103);
            this.m_nameLabel.Name = "m_nameLabel";
            this.m_nameLabel.Size = new System.Drawing.Size(43, 13);
            this.m_nameLabel.TabIndex = 8;
            this.m_nameLabel.Text = "Name:";
            // 
            // m_nameTextBox
            // 
            this.m_nameTextBox.Location = new System.Drawing.Point(58, 100);
            this.m_nameTextBox.Name = "m_nameTextBox";
            this.m_nameTextBox.Size = new System.Drawing.Size(120, 20);
            this.m_nameTextBox.TabIndex = 9;
            // 
            // m_viewWiButton
            // 
            this.m_viewWiButton.Location = new System.Drawing.Point(422, 132);
            this.m_viewWiButton.Name = "m_viewWiButton";
            this.m_viewWiButton.Size = new System.Drawing.Size(134, 23);
            this.m_viewWiButton.TabIndex = 10;
            this.m_viewWiButton.Text = "View Work Instruction";
            this.m_viewWiButton.UseVisualStyleBackColor = true;
            this.m_viewWiButton.Click += new System.EventHandler(this.m_viewWiButton_Click);
            // 
            // CloseMaintenanceItemForm
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.BackColor = System.Drawing.Color.LightSteelBlue;
            this.ClientSize = new System.Drawing.Size(568, 384);
            this.ControlBox = false;
            this.Controls.Add(this.m_viewWiButton);
            this.Controls.Add(this.m_nameTextBox);
            this.Controls.Add(this.m_nameLabel);
            this.Controls.Add(this.m_itemText);
            this.Controls.Add(this.m_itemTextLabel);
            this.Controls.Add(this.m_itemName);
            this.Controls.Add(this.m_itemLabel);
            this.Controls.Add(this.m_itemCompleteCheckBox);
            this.Controls.Add(this.m_saveButton);
            this.Controls.Add(this.m_notesLabel);
            this.Controls.Add(this.m_notesTextBox);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedToolWindow;
            this.Name = "CloseMaintenanceItemForm";
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterParent;
            this.Text = "Close Maintenance Item";
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.TextBox m_notesTextBox;
        private System.Windows.Forms.Label m_notesLabel;
        private System.Windows.Forms.Button m_saveButton;
        private System.Windows.Forms.CheckBox m_itemCompleteCheckBox;
        private System.Windows.Forms.Label m_itemLabel;
        private System.Windows.Forms.Label m_itemName;
        private System.Windows.Forms.Label m_itemTextLabel;
        private System.Windows.Forms.Label m_itemText;
        private System.Windows.Forms.Label m_nameLabel;
        private System.Windows.Forms.TextBox m_nameTextBox;
        private System.Windows.Forms.Button m_viewWiButton;
    }
}