namespace BomFileProcessor
{
    partial class PassConfirmationFileSetupForm
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
            this.m_passConfirmCcrtLocationLabel = new System.Windows.Forms.Label();
            this.m_spartanPassConfirmLabel = new System.Windows.Forms.Label();
            this.m_ccrtPassConfirmFileLocationTextBox = new System.Windows.Forms.TextBox();
            this.m_spartanPassFileLocationTextBox = new System.Windows.Forms.TextBox();
            this.m_ccrtSelectButton = new System.Windows.Forms.Button();
            this.m_SpartanSelectButton = new System.Windows.Forms.Button();
            this.m_monitorPassConfirmFilesCheckBox = new System.Windows.Forms.CheckBox();
            this.label1 = new System.Windows.Forms.Label();
            this.m_fileCheckDelayNumericUpDown = new System.Windows.Forms.NumericUpDown();
            this.m_saveButton = new System.Windows.Forms.Button();
            this.m_closeButton = new System.Windows.Forms.Button();
            ((System.ComponentModel.ISupportInitialize)(this.m_fileCheckDelayNumericUpDown)).BeginInit();
            this.SuspendLayout();
            // 
            // m_passConfirmCcrtLocationLabel
            // 
            this.m_passConfirmCcrtLocationLabel.AutoSize = true;
            this.m_passConfirmCcrtLocationLabel.Location = new System.Drawing.Point(12, 22);
            this.m_passConfirmCcrtLocationLabel.Name = "m_passConfirmCcrtLocationLabel";
            this.m_passConfirmCcrtLocationLabel.Size = new System.Drawing.Size(242, 13);
            this.m_passConfirmCcrtLocationLabel.TabIndex = 0;
            this.m_passConfirmCcrtLocationLabel.Text = "CCRT Generated Pass Confirmation File Location:";
            // 
            // m_spartanPassConfirmLabel
            // 
            this.m_spartanPassConfirmLabel.AutoSize = true;
            this.m_spartanPassConfirmLabel.Location = new System.Drawing.Point(9, 60);
            this.m_spartanPassConfirmLabel.Name = "m_spartanPassConfirmLabel";
            this.m_spartanPassConfirmLabel.Size = new System.Drawing.Size(245, 13);
            this.m_spartanPassConfirmLabel.TabIndex = 1;
            this.m_spartanPassConfirmLabel.Text = "Spartan Expected Pass Confirmation File Location:";
            // 
            // m_ccrtPassConfirmFileLocationTextBox
            // 
            this.m_ccrtPassConfirmFileLocationTextBox.Location = new System.Drawing.Point(263, 19);
            this.m_ccrtPassConfirmFileLocationTextBox.Name = "m_ccrtPassConfirmFileLocationTextBox";
            this.m_ccrtPassConfirmFileLocationTextBox.Size = new System.Drawing.Size(256, 20);
            this.m_ccrtPassConfirmFileLocationTextBox.TabIndex = 2;
            // 
            // m_spartanPassFileLocationTextBox
            // 
            this.m_spartanPassFileLocationTextBox.Location = new System.Drawing.Point(263, 57);
            this.m_spartanPassFileLocationTextBox.Name = "m_spartanPassFileLocationTextBox";
            this.m_spartanPassFileLocationTextBox.Size = new System.Drawing.Size(256, 20);
            this.m_spartanPassFileLocationTextBox.TabIndex = 3;
            // 
            // m_ccrtSelectButton
            // 
            this.m_ccrtSelectButton.Location = new System.Drawing.Point(525, 19);
            this.m_ccrtSelectButton.Name = "m_ccrtSelectButton";
            this.m_ccrtSelectButton.Size = new System.Drawing.Size(75, 23);
            this.m_ccrtSelectButton.TabIndex = 4;
            this.m_ccrtSelectButton.Text = "Select";
            this.m_ccrtSelectButton.UseVisualStyleBackColor = true;
            this.m_ccrtSelectButton.Click += new System.EventHandler(this.m_ccrtSelectButton_Click);
            // 
            // m_SpartanSelectButton
            // 
            this.m_SpartanSelectButton.Location = new System.Drawing.Point(525, 57);
            this.m_SpartanSelectButton.Name = "m_SpartanSelectButton";
            this.m_SpartanSelectButton.Size = new System.Drawing.Size(75, 23);
            this.m_SpartanSelectButton.TabIndex = 5;
            this.m_SpartanSelectButton.Text = "Select";
            this.m_SpartanSelectButton.UseVisualStyleBackColor = true;
            this.m_SpartanSelectButton.Click += new System.EventHandler(this.m_SpartanSelectButton_Click);
            // 
            // m_monitorPassConfirmFilesCheckBox
            // 
            this.m_monitorPassConfirmFilesCheckBox.AutoSize = true;
            this.m_monitorPassConfirmFilesCheckBox.CheckAlign = System.Drawing.ContentAlignment.MiddleRight;
            this.m_monitorPassConfirmFilesCheckBox.Location = new System.Drawing.Point(413, 95);
            this.m_monitorPassConfirmFilesCheckBox.Name = "m_monitorPassConfirmFilesCheckBox";
            this.m_monitorPassConfirmFilesCheckBox.Size = new System.Drawing.Size(187, 17);
            this.m_monitorPassConfirmFilesCheckBox.TabIndex = 6;
            this.m_monitorPassConfirmFilesCheckBox.Text = "Monitor for Pass Confirmation Files";
            this.m_monitorPassConfirmFilesCheckBox.UseVisualStyleBackColor = true;
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(55, 94);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(199, 13);
            this.label1.TabIndex = 7;
            this.label1.Text = "Pass Confirmation File Monitor Time (ms):";
            // 
            // m_fileCheckDelayNumericUpDown
            // 
            this.m_fileCheckDelayNumericUpDown.Location = new System.Drawing.Point(263, 92);
            this.m_fileCheckDelayNumericUpDown.Maximum = new decimal(new int[] {
            10000,
            0,
            0,
            0});
            this.m_fileCheckDelayNumericUpDown.Minimum = new decimal(new int[] {
            100,
            0,
            0,
            0});
            this.m_fileCheckDelayNumericUpDown.Name = "m_fileCheckDelayNumericUpDown";
            this.m_fileCheckDelayNumericUpDown.Size = new System.Drawing.Size(56, 20);
            this.m_fileCheckDelayNumericUpDown.TabIndex = 8;
            this.m_fileCheckDelayNumericUpDown.Value = new decimal(new int[] {
            100,
            0,
            0,
            0});
            // 
            // m_saveButton
            // 
            this.m_saveButton.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this.m_saveButton.Location = new System.Drawing.Point(525, 136);
            this.m_saveButton.Name = "m_saveButton";
            this.m_saveButton.Size = new System.Drawing.Size(75, 23);
            this.m_saveButton.TabIndex = 9;
            this.m_saveButton.Text = "Save";
            this.m_saveButton.UseVisualStyleBackColor = true;
            this.m_saveButton.Click += new System.EventHandler(this.m_saveButton_Click);
            // 
            // m_closeButton
            // 
            this.m_closeButton.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this.m_closeButton.DialogResult = System.Windows.Forms.DialogResult.Cancel;
            this.m_closeButton.Location = new System.Drawing.Point(444, 136);
            this.m_closeButton.Name = "m_closeButton";
            this.m_closeButton.Size = new System.Drawing.Size(75, 23);
            this.m_closeButton.TabIndex = 10;
            this.m_closeButton.Text = "Close";
            this.m_closeButton.UseVisualStyleBackColor = true;
            // 
            // PassConfirmationFileSetupForm
            // 
            this.AcceptButton = this.m_saveButton;
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.BackColor = System.Drawing.Color.LightSteelBlue;
            this.CancelButton = this.m_closeButton;
            this.ClientSize = new System.Drawing.Size(622, 171);
            this.ControlBox = false;
            this.Controls.Add(this.m_closeButton);
            this.Controls.Add(this.m_saveButton);
            this.Controls.Add(this.m_fileCheckDelayNumericUpDown);
            this.Controls.Add(this.label1);
            this.Controls.Add(this.m_monitorPassConfirmFilesCheckBox);
            this.Controls.Add(this.m_SpartanSelectButton);
            this.Controls.Add(this.m_ccrtSelectButton);
            this.Controls.Add(this.m_spartanPassFileLocationTextBox);
            this.Controls.Add(this.m_ccrtPassConfirmFileLocationTextBox);
            this.Controls.Add(this.m_spartanPassConfirmLabel);
            this.Controls.Add(this.m_passConfirmCcrtLocationLabel);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.Fixed3D;
            this.Name = "PassConfirmationFileSetupForm";
            this.Text = "Pass Confirmation File Setup";
            ((System.ComponentModel.ISupportInitialize)(this.m_fileCheckDelayNumericUpDown)).EndInit();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Label m_passConfirmCcrtLocationLabel;
        private System.Windows.Forms.Label m_spartanPassConfirmLabel;
        private System.Windows.Forms.TextBox m_ccrtPassConfirmFileLocationTextBox;
        private System.Windows.Forms.TextBox m_spartanPassFileLocationTextBox;
        private System.Windows.Forms.Button m_ccrtSelectButton;
        private System.Windows.Forms.Button m_SpartanSelectButton;
        private System.Windows.Forms.CheckBox m_monitorPassConfirmFilesCheckBox;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.NumericUpDown m_fileCheckDelayNumericUpDown;
        private System.Windows.Forms.Button m_saveButton;
        private System.Windows.Forms.Button m_closeButton;
    }
}