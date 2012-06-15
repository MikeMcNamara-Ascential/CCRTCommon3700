namespace ModuleCommServer
{
    partial class AddBrakeModuleForm
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
            this.m_moduleTypeLabel = new System.Windows.Forms.Label();
            this.m_absTypeTextBox = new System.Windows.Forms.TextBox();
            this.m_commProtocolLabel = new System.Windows.Forms.Label();
            this.m_commBusLabel = new System.Windows.Forms.Label();
            this.m_protocolComboBox = new System.Windows.Forms.ComboBox();
            this.m_busComboBox = new System.Windows.Forms.ComboBox();
            this.m_configFileLabel = new System.Windows.Forms.Label();
            this.m_configFileTextBox = new System.Windows.Forms.TextBox();
            this.m_browseButton = new System.Windows.Forms.Button();
            this.m_cancelButton = new System.Windows.Forms.Button();
            this.m_addButton = new System.Windows.Forms.Button();
            this.SuspendLayout();
            // 
            // m_moduleTypeLabel
            // 
            this.m_moduleTypeLabel.AutoSize = true;
            this.m_moduleTypeLabel.Font = new System.Drawing.Font("Microsoft Sans Serif", 10F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.m_moduleTypeLabel.Location = new System.Drawing.Point(12, 18);
            this.m_moduleTypeLabel.Name = "m_moduleTypeLabel";
            this.m_moduleTypeLabel.Size = new System.Drawing.Size(75, 17);
            this.m_moduleTypeLabel.TabIndex = 0;
            this.m_moduleTypeLabel.Text = "ABS Type:";
            // 
            // m_absTypeTextBox
            // 
            this.m_absTypeTextBox.Location = new System.Drawing.Point(93, 17);
            this.m_absTypeTextBox.Name = "m_absTypeTextBox";
            this.m_absTypeTextBox.Size = new System.Drawing.Size(148, 20);
            this.m_absTypeTextBox.TabIndex = 1;
            // 
            // m_commProtocolLabel
            // 
            this.m_commProtocolLabel.AutoSize = true;
            this.m_commProtocolLabel.Font = new System.Drawing.Font("Microsoft Sans Serif", 10F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.m_commProtocolLabel.Location = new System.Drawing.Point(12, 62);
            this.m_commProtocolLabel.Name = "m_commProtocolLabel";
            this.m_commProtocolLabel.Size = new System.Drawing.Size(164, 17);
            this.m_commProtocolLabel.TabIndex = 2;
            this.m_commProtocolLabel.Text = "Communication Protocol:";
            // 
            // m_commBusLabel
            // 
            this.m_commBusLabel.AutoSize = true;
            this.m_commBusLabel.Font = new System.Drawing.Font("Microsoft Sans Serif", 10F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.m_commBusLabel.Location = new System.Drawing.Point(12, 106);
            this.m_commBusLabel.Name = "m_commBusLabel";
            this.m_commBusLabel.Size = new System.Drawing.Size(136, 17);
            this.m_commBusLabel.TabIndex = 3;
            this.m_commBusLabel.Text = "Communication Bus:";
            // 
            // m_protocolComboBox
            // 
            this.m_protocolComboBox.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.m_protocolComboBox.FormattingEnabled = true;
            this.m_protocolComboBox.Location = new System.Drawing.Point(182, 61);
            this.m_protocolComboBox.Name = "m_protocolComboBox";
            this.m_protocolComboBox.Size = new System.Drawing.Size(170, 21);
            this.m_protocolComboBox.TabIndex = 4;
            // 
            // m_busComboBox
            // 
            this.m_busComboBox.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.m_busComboBox.FormattingEnabled = true;
            this.m_busComboBox.Location = new System.Drawing.Point(182, 105);
            this.m_busComboBox.Name = "m_busComboBox";
            this.m_busComboBox.Size = new System.Drawing.Size(170, 21);
            this.m_busComboBox.TabIndex = 5;
            // 
            // m_configFileLabel
            // 
            this.m_configFileLabel.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.m_configFileLabel.AutoSize = true;
            this.m_configFileLabel.Font = new System.Drawing.Font("Microsoft Sans Serif", 10F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.m_configFileLabel.Location = new System.Drawing.Point(12, 149);
            this.m_configFileLabel.Name = "m_configFileLabel";
            this.m_configFileLabel.Size = new System.Drawing.Size(122, 17);
            this.m_configFileLabel.TabIndex = 6;
            this.m_configFileLabel.Text = "Configuration File:";
            // 
            // m_configFileTextBox
            // 
            this.m_configFileTextBox.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.m_configFileTextBox.Location = new System.Drawing.Point(15, 169);
            this.m_configFileTextBox.Name = "m_configFileTextBox";
            this.m_configFileTextBox.Size = new System.Drawing.Size(306, 20);
            this.m_configFileTextBox.TabIndex = 7;
            // 
            // m_browseButton
            // 
            this.m_browseButton.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.m_browseButton.Location = new System.Drawing.Point(327, 166);
            this.m_browseButton.Name = "m_browseButton";
            this.m_browseButton.Size = new System.Drawing.Size(75, 23);
            this.m_browseButton.TabIndex = 8;
            this.m_browseButton.Text = "Browse...";
            this.m_browseButton.UseVisualStyleBackColor = true;
            this.m_browseButton.Click += new System.EventHandler(this.m_browseButton_Click);
            // 
            // m_cancelButton
            // 
            this.m_cancelButton.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.m_cancelButton.DialogResult = System.Windows.Forms.DialogResult.Cancel;
            this.m_cancelButton.Location = new System.Drawing.Point(327, 207);
            this.m_cancelButton.Name = "m_cancelButton";
            this.m_cancelButton.Size = new System.Drawing.Size(75, 23);
            this.m_cancelButton.TabIndex = 9;
            this.m_cancelButton.Text = "Cancel";
            this.m_cancelButton.UseVisualStyleBackColor = true;
            // 
            // m_addButton
            // 
            this.m_addButton.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.m_addButton.Location = new System.Drawing.Point(246, 207);
            this.m_addButton.Name = "m_addButton";
            this.m_addButton.Size = new System.Drawing.Size(75, 23);
            this.m_addButton.TabIndex = 10;
            this.m_addButton.Text = "Add";
            this.m_addButton.UseVisualStyleBackColor = true;
            this.m_addButton.Click += new System.EventHandler(this.m_addButton_Click);
            // 
            // AddBrakeModuleForm
            // 
            this.AcceptButton = this.m_addButton;
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.BackColor = System.Drawing.Color.White;
            this.CancelButton = this.m_cancelButton;
            this.ClientSize = new System.Drawing.Size(410, 245);
            this.ControlBox = false;
            this.Controls.Add(this.m_addButton);
            this.Controls.Add(this.m_cancelButton);
            this.Controls.Add(this.m_browseButton);
            this.Controls.Add(this.m_configFileTextBox);
            this.Controls.Add(this.m_configFileLabel);
            this.Controls.Add(this.m_busComboBox);
            this.Controls.Add(this.m_protocolComboBox);
            this.Controls.Add(this.m_commBusLabel);
            this.Controls.Add(this.m_commProtocolLabel);
            this.Controls.Add(this.m_absTypeTextBox);
            this.Controls.Add(this.m_moduleTypeLabel);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.Fixed3D;
            this.Name = "AddBrakeModuleForm";
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterParent;
            this.Text = "Add Brake Module";
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Label m_moduleTypeLabel;
        private System.Windows.Forms.TextBox m_absTypeTextBox;
        private System.Windows.Forms.Label m_commProtocolLabel;
        private System.Windows.Forms.Label m_commBusLabel;
        private System.Windows.Forms.ComboBox m_protocolComboBox;
        private System.Windows.Forms.ComboBox m_busComboBox;
        private System.Windows.Forms.Label m_configFileLabel;
        private System.Windows.Forms.TextBox m_configFileTextBox;
        private System.Windows.Forms.Button m_browseButton;
        private System.Windows.Forms.Button m_cancelButton;
        private System.Windows.Forms.Button m_addButton;
    }
}