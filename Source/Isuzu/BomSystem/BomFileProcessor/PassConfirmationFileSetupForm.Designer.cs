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
            this.m_spartanPassConfirmLabelFlash = new System.Windows.Forms.Label();
            this.m_spartanFlashPassFileLocationTextBox = new System.Windows.Forms.TextBox();
            this.m_SpartanFlashSelectButton = new System.Windows.Forms.Button();
            this.m_widowsESNSelectButton = new System.Windows.Forms.Button();
            this.m_widowsESNFileLocationTextBox = new System.Windows.Forms.TextBox();
            this.label2 = new System.Windows.Forms.Label();
            this.m_realTimeESNSelectButton = new System.Windows.Forms.Button();
            this.m_realTimeESNFileLocationTextBox = new System.Windows.Forms.TextBox();
            this.label3 = new System.Windows.Forms.Label();
            ((System.ComponentModel.ISupportInitialize)(this.m_fileCheckDelayNumericUpDown)).BeginInit();
            this.SuspendLayout();
            // 
            // m_passConfirmCcrtLocationLabel
            // 
            this.m_passConfirmCcrtLocationLabel.AutoSize = true;
            this.m_passConfirmCcrtLocationLabel.Location = new System.Drawing.Point(12, 22);
            this.m_passConfirmCcrtLocationLabel.Name = "m_passConfirmCcrtLocationLabel";
            this.m_passConfirmCcrtLocationLabel.Size = new System.Drawing.Size(219, 13);
            this.m_passConfirmCcrtLocationLabel.TabIndex = 0;
            this.m_passConfirmCcrtLocationLabel.Text = "Real time Pc Pass Confirmation Ftp Location:";
            // 
            // m_spartanPassConfirmLabel
            // 
            this.m_spartanPassConfirmLabel.AutoSize = true;
            this.m_spartanPassConfirmLabel.Location = new System.Drawing.Point(9, 60);
            this.m_spartanPassConfirmLabel.Name = "m_spartanPassConfirmLabel";
            this.m_spartanPassConfirmLabel.Size = new System.Drawing.Size(270, 13);
            this.m_spartanPassConfirmLabel.TabIndex = 1;
            this.m_spartanPassConfirmLabel.Text = "Spartan Expected DVT Pass Confirmation File Location:";
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
            this.m_spartanPassFileLocationTextBox.Location = new System.Drawing.Point(284, 57);
            this.m_spartanPassFileLocationTextBox.Name = "m_spartanPassFileLocationTextBox";
            this.m_spartanPassFileLocationTextBox.Size = new System.Drawing.Size(236, 20);
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
            this.m_monitorPassConfirmFilesCheckBox.Location = new System.Drawing.Point(413, 202);
            this.m_monitorPassConfirmFilesCheckBox.Name = "m_monitorPassConfirmFilesCheckBox";
            this.m_monitorPassConfirmFilesCheckBox.Size = new System.Drawing.Size(187, 17);
            this.m_monitorPassConfirmFilesCheckBox.TabIndex = 6;
            this.m_monitorPassConfirmFilesCheckBox.Text = "Monitor for Pass Confirmation Files";
            this.m_monitorPassConfirmFilesCheckBox.UseVisualStyleBackColor = true;
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(52, 202);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(199, 13);
            this.label1.TabIndex = 7;
            this.label1.Text = "Pass Confirmation File Monitor Time (ms):";
            // 
            // m_fileCheckDelayNumericUpDown
            // 
            this.m_fileCheckDelayNumericUpDown.Location = new System.Drawing.Point(260, 202);
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
            this.m_saveButton.Location = new System.Drawing.Point(525, 230);
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
            this.m_closeButton.Location = new System.Drawing.Point(444, 230);
            this.m_closeButton.Name = "m_closeButton";
            this.m_closeButton.Size = new System.Drawing.Size(75, 23);
            this.m_closeButton.TabIndex = 10;
            this.m_closeButton.Text = "Close";
            this.m_closeButton.UseVisualStyleBackColor = true;
            // 
            // m_spartanPassConfirmLabelFlash
            // 
            this.m_spartanPassConfirmLabelFlash.AutoSize = true;
            this.m_spartanPassConfirmLabelFlash.Location = new System.Drawing.Point(9, 94);
            this.m_spartanPassConfirmLabelFlash.Name = "m_spartanPassConfirmLabelFlash";
            this.m_spartanPassConfirmLabelFlash.Size = new System.Drawing.Size(273, 13);
            this.m_spartanPassConfirmLabelFlash.TabIndex = 11;
            this.m_spartanPassConfirmLabelFlash.Text = "Spartan Expected Flash Pass Confirmation File Location:";
            // 
            // m_spartanFlashPassFileLocationTextBox
            // 
            this.m_spartanFlashPassFileLocationTextBox.Location = new System.Drawing.Point(284, 94);
            this.m_spartanFlashPassFileLocationTextBox.Name = "m_spartanFlashPassFileLocationTextBox";
            this.m_spartanFlashPassFileLocationTextBox.Size = new System.Drawing.Size(236, 20);
            this.m_spartanFlashPassFileLocationTextBox.TabIndex = 12;
            // 
            // m_SpartanFlashSelectButton
            // 
            this.m_SpartanFlashSelectButton.Location = new System.Drawing.Point(525, 94);
            this.m_SpartanFlashSelectButton.Name = "m_SpartanFlashSelectButton";
            this.m_SpartanFlashSelectButton.Size = new System.Drawing.Size(75, 23);
            this.m_SpartanFlashSelectButton.TabIndex = 13;
            this.m_SpartanFlashSelectButton.Text = "Select";
            this.m_SpartanFlashSelectButton.UseVisualStyleBackColor = true;
            this.m_SpartanFlashSelectButton.Click += new System.EventHandler(this.m_SpartanFlashSelectButton_Click);
            // 
            // m_widowsESNSelectButton
            // 
            this.m_widowsESNSelectButton.Location = new System.Drawing.Point(526, 124);
            this.m_widowsESNSelectButton.Name = "m_widowsESNSelectButton";
            this.m_widowsESNSelectButton.Size = new System.Drawing.Size(75, 23);
            this.m_widowsESNSelectButton.TabIndex = 16;
            this.m_widowsESNSelectButton.Text = "Select";
            this.m_widowsESNSelectButton.UseVisualStyleBackColor = true;
            this.m_widowsESNSelectButton.Click += new System.EventHandler(this.m_widowsESNSelectButton_Click);
            // 
            // m_widowsESNFileLocationTextBox
            // 
            this.m_widowsESNFileLocationTextBox.Location = new System.Drawing.Point(284, 124);
            this.m_widowsESNFileLocationTextBox.Name = "m_widowsESNFileLocationTextBox";
            this.m_widowsESNFileLocationTextBox.Size = new System.Drawing.Size(236, 20);
            this.m_widowsESNFileLocationTextBox.TabIndex = 15;
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Location = new System.Drawing.Point(10, 124);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(159, 13);
            this.label2.TabIndex = 14;
            this.label2.Text = "Windows PC ESN File Location:";
            // 
            // m_realTimeESNSelectButton
            // 
            this.m_realTimeESNSelectButton.Location = new System.Drawing.Point(525, 156);
            this.m_realTimeESNSelectButton.Name = "m_realTimeESNSelectButton";
            this.m_realTimeESNSelectButton.Size = new System.Drawing.Size(75, 23);
            this.m_realTimeESNSelectButton.TabIndex = 19;
            this.m_realTimeESNSelectButton.Text = "Select";
            this.m_realTimeESNSelectButton.UseVisualStyleBackColor = true;
            this.m_realTimeESNSelectButton.Click += new System.EventHandler(this.m_realTimeESNSelectButton_Click);
            // 
            // m_realTimeESNFileLocationTextBox
            // 
            this.m_realTimeESNFileLocationTextBox.Location = new System.Drawing.Point(284, 156);
            this.m_realTimeESNFileLocationTextBox.Name = "m_realTimeESNFileLocationTextBox";
            this.m_realTimeESNFileLocationTextBox.Size = new System.Drawing.Size(236, 20);
            this.m_realTimeESNFileLocationTextBox.TabIndex = 18;
            this.m_realTimeESNFileLocationTextBox.Text = "m";
            // 
            // label3
            // 
            this.label3.AutoSize = true;
            this.label3.Location = new System.Drawing.Point(9, 156);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(159, 13);
            this.label3.TabIndex = 17;
            this.label3.Text = "RealTime PC ESN Ftp Location:";
            // 
            // PassConfirmationFileSetupForm
            // 
            this.AcceptButton = this.m_saveButton;
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.BackColor = System.Drawing.Color.LightSteelBlue;
            this.CancelButton = this.m_closeButton;
            this.ClientSize = new System.Drawing.Size(622, 265);
            this.ControlBox = false;
            this.Controls.Add(this.m_realTimeESNSelectButton);
            this.Controls.Add(this.m_realTimeESNFileLocationTextBox);
            this.Controls.Add(this.label3);
            this.Controls.Add(this.m_widowsESNSelectButton);
            this.Controls.Add(this.m_widowsESNFileLocationTextBox);
            this.Controls.Add(this.label2);
            this.Controls.Add(this.m_SpartanFlashSelectButton);
            this.Controls.Add(this.m_spartanFlashPassFileLocationTextBox);
            this.Controls.Add(this.m_spartanPassConfirmLabelFlash);
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
        private System.Windows.Forms.Label m_spartanPassConfirmLabelFlash;
        private System.Windows.Forms.TextBox m_spartanFlashPassFileLocationTextBox;
        private System.Windows.Forms.Button m_SpartanFlashSelectButton;
        private System.Windows.Forms.Button m_widowsESNSelectButton;
        private System.Windows.Forms.TextBox m_widowsESNFileLocationTextBox;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.Button m_realTimeESNSelectButton;
        private System.Windows.Forms.TextBox m_realTimeESNFileLocationTextBox;
        private System.Windows.Forms.Label label3;
    }
}