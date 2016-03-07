namespace InputServer
{
    partial class InputDataItemForm
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
            this.m_inputDataItemLabel = new System.Windows.Forms.Label();
            this.m_dataItemComboBox = new System.Windows.Forms.ComboBox();
            this.m_dataSettingsLabel = new System.Windows.Forms.Label();
            this.m_minLengthLabel = new System.Windows.Forms.Label();
            this.m_minLengthTextBox = new System.Windows.Forms.TextBox();
            this.m_maxLengthLabel = new System.Windows.Forms.Label();
            this.m_maxLengthTextBox = new System.Windows.Forms.TextBox();
            this.m_closeButton = new System.Windows.Forms.Button();
            this.m_saveButton = new System.Windows.Forms.Button();
            this.m_removeButton = new System.Windows.Forms.Button();
            this.SuspendLayout();
            // 
            // m_inputDataItemLabel
            // 
            this.m_inputDataItemLabel.AutoSize = true;
            this.m_inputDataItemLabel.Font = new System.Drawing.Font("Microsoft Sans Serif", 9.75F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.m_inputDataItemLabel.Location = new System.Drawing.Point(12, 9);
            this.m_inputDataItemLabel.Name = "m_inputDataItemLabel";
            this.m_inputDataItemLabel.Size = new System.Drawing.Size(78, 16);
            this.m_inputDataItemLabel.TabIndex = 0;
            this.m_inputDataItemLabel.Text = "Data Item:";
            // 
            // m_dataItemComboBox
            // 
            this.m_dataItemComboBox.FormattingEnabled = true;
            this.m_dataItemComboBox.Location = new System.Drawing.Point(129, 8);
            this.m_dataItemComboBox.Name = "m_dataItemComboBox";
            this.m_dataItemComboBox.Size = new System.Drawing.Size(164, 21);
            this.m_dataItemComboBox.TabIndex = 1;
            this.m_dataItemComboBox.SelectedIndexChanged += new System.EventHandler(this.m_dataItemComboBox_SelectedIndexChanged);
            // 
            // m_dataSettingsLabel
            // 
            this.m_dataSettingsLabel.AutoSize = true;
            this.m_dataSettingsLabel.Font = new System.Drawing.Font("Microsoft Sans Serif", 10F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.m_dataSettingsLabel.Location = new System.Drawing.Point(12, 77);
            this.m_dataSettingsLabel.Name = "m_dataSettingsLabel";
            this.m_dataSettingsLabel.Size = new System.Drawing.Size(111, 17);
            this.m_dataSettingsLabel.TabIndex = 2;
            this.m_dataSettingsLabel.Text = "Data Settings:";
            // 
            // m_minLengthLabel
            // 
            this.m_minLengthLabel.AutoSize = true;
            this.m_minLengthLabel.Location = new System.Drawing.Point(36, 104);
            this.m_minLengthLabel.Name = "m_minLengthLabel";
            this.m_minLengthLabel.Size = new System.Drawing.Size(87, 13);
            this.m_minLengthLabel.TabIndex = 3;
            this.m_minLengthLabel.Text = "Minimum Length:";
            // 
            // m_minLengthTextBox
            // 
            this.m_minLengthTextBox.Location = new System.Drawing.Point(160, 101);
            this.m_minLengthTextBox.Name = "m_minLengthTextBox";
            this.m_minLengthTextBox.Size = new System.Drawing.Size(59, 20);
            this.m_minLengthTextBox.TabIndex = 4;
            // 
            // m_maxLengthLabel
            // 
            this.m_maxLengthLabel.AutoSize = true;
            this.m_maxLengthLabel.Location = new System.Drawing.Point(33, 137);
            this.m_maxLengthLabel.Name = "m_maxLengthLabel";
            this.m_maxLengthLabel.Size = new System.Drawing.Size(90, 13);
            this.m_maxLengthLabel.TabIndex = 5;
            this.m_maxLengthLabel.Text = "Maximum Length:";
            // 
            // m_maxLengthTextBox
            // 
            this.m_maxLengthTextBox.Location = new System.Drawing.Point(160, 134);
            this.m_maxLengthTextBox.Name = "m_maxLengthTextBox";
            this.m_maxLengthTextBox.Size = new System.Drawing.Size(59, 20);
            this.m_maxLengthTextBox.TabIndex = 6;
            // 
            // m_closeButton
            // 
            this.m_closeButton.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)));
            this.m_closeButton.DialogResult = System.Windows.Forms.DialogResult.Cancel;
            this.m_closeButton.Location = new System.Drawing.Point(12, 203);
            this.m_closeButton.Name = "m_closeButton";
            this.m_closeButton.Size = new System.Drawing.Size(75, 23);
            this.m_closeButton.TabIndex = 15;
            this.m_closeButton.Text = "Close";
            this.m_closeButton.UseVisualStyleBackColor = true;
            // 
            // m_saveButton
            // 
            this.m_saveButton.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this.m_saveButton.Location = new System.Drawing.Point(160, 203);
            this.m_saveButton.Name = "m_saveButton";
            this.m_saveButton.Size = new System.Drawing.Size(149, 23);
            this.m_saveButton.TabIndex = 16;
            this.m_saveButton.Text = "Save Settings";
            this.m_saveButton.UseVisualStyleBackColor = true;
            this.m_saveButton.Click += new System.EventHandler(this.m_saveButton_Click);
            // 
            // m_removeButton
            // 
            this.m_removeButton.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this.m_removeButton.Location = new System.Drawing.Point(160, 174);
            this.m_removeButton.Name = "m_removeButton";
            this.m_removeButton.Size = new System.Drawing.Size(149, 23);
            this.m_removeButton.TabIndex = 17;
            this.m_removeButton.Text = "Remove Item";
            this.m_removeButton.UseVisualStyleBackColor = true;
            this.m_removeButton.Click += new System.EventHandler(this.m_removeButton_Click);
            // 
            // InputDataItemForm
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.CancelButton = this.m_closeButton;
            this.ClientSize = new System.Drawing.Size(321, 238);
            this.Controls.Add(this.m_removeButton);
            this.Controls.Add(this.m_saveButton);
            this.Controls.Add(this.m_closeButton);
            this.Controls.Add(this.m_maxLengthTextBox);
            this.Controls.Add(this.m_maxLengthLabel);
            this.Controls.Add(this.m_minLengthTextBox);
            this.Controls.Add(this.m_minLengthLabel);
            this.Controls.Add(this.m_dataSettingsLabel);
            this.Controls.Add(this.m_dataItemComboBox);
            this.Controls.Add(this.m_inputDataItemLabel);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedToolWindow;
            this.Name = "InputDataItemForm";
            this.Text = "Data Item Form";
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Label m_inputDataItemLabel;
        private System.Windows.Forms.ComboBox m_dataItemComboBox;
        private System.Windows.Forms.Label m_dataSettingsLabel;
        private System.Windows.Forms.Label m_minLengthLabel;
        private System.Windows.Forms.TextBox m_minLengthTextBox;
        private System.Windows.Forms.Label m_maxLengthLabel;
        private System.Windows.Forms.TextBox m_maxLengthTextBox;
        private System.Windows.Forms.Button m_closeButton;
        private System.Windows.Forms.Button m_saveButton;
        private System.Windows.Forms.Button m_removeButton;
    }
}