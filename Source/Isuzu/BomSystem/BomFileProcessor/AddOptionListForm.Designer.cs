namespace BomFileProcessor
{
    partial class AddOptionListForm
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
            this.m_modelCodeLabel = new System.Windows.Forms.Label();
            this.m_modelCodeTextBox = new System.Windows.Forms.TextBox();
            this.m_optionListComboBox = new System.Windows.Forms.ComboBox();
            this.m_optionTypeLabel = new System.Windows.Forms.Label();
            this.m_saveButton = new System.Windows.Forms.Button();
            this.m_cancelButton = new System.Windows.Forms.Button();
            this.SuspendLayout();
            // 
            // m_modelCodeLabel
            // 
            this.m_modelCodeLabel.AutoSize = true;
            this.m_modelCodeLabel.Location = new System.Drawing.Point(12, 15);
            this.m_modelCodeLabel.Name = "m_modelCodeLabel";
            this.m_modelCodeLabel.Size = new System.Drawing.Size(67, 13);
            this.m_modelCodeLabel.TabIndex = 0;
            this.m_modelCodeLabel.Text = "Model Code:";
            // 
            // m_modelCodeTextBox
            // 
            this.m_modelCodeTextBox.Location = new System.Drawing.Point(85, 12);
            this.m_modelCodeTextBox.Name = "m_modelCodeTextBox";
            this.m_modelCodeTextBox.Size = new System.Drawing.Size(100, 20);
            this.m_modelCodeTextBox.TabIndex = 1;
            // 
            // m_optionListComboBox
            // 
            this.m_optionListComboBox.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.m_optionListComboBox.FormattingEnabled = true;
            this.m_optionListComboBox.Location = new System.Drawing.Point(85, 38);
            this.m_optionListComboBox.Name = "m_optionListComboBox";
            this.m_optionListComboBox.Size = new System.Drawing.Size(175, 21);
            this.m_optionListComboBox.TabIndex = 2;
            // 
            // m_optionTypeLabel
            // 
            this.m_optionTypeLabel.AutoSize = true;
            this.m_optionTypeLabel.Location = new System.Drawing.Point(45, 41);
            this.m_optionTypeLabel.Name = "m_optionTypeLabel";
            this.m_optionTypeLabel.Size = new System.Drawing.Size(34, 13);
            this.m_optionTypeLabel.TabIndex = 3;
            this.m_optionTypeLabel.Text = "Type:";
            // 
            // m_saveButton
            // 
            this.m_saveButton.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this.m_saveButton.Location = new System.Drawing.Point(187, 119);
            this.m_saveButton.Name = "m_saveButton";
            this.m_saveButton.Size = new System.Drawing.Size(75, 23);
            this.m_saveButton.TabIndex = 4;
            this.m_saveButton.Text = "OK";
            this.m_saveButton.UseVisualStyleBackColor = true;
            this.m_saveButton.Click += new System.EventHandler(this.m_saveButton_Click);
            // 
            // m_cancelButton
            // 
            this.m_cancelButton.DialogResult = System.Windows.Forms.DialogResult.Cancel;
            this.m_cancelButton.Location = new System.Drawing.Point(12, 119);
            this.m_cancelButton.Name = "m_cancelButton";
            this.m_cancelButton.Size = new System.Drawing.Size(75, 23);
            this.m_cancelButton.TabIndex = 5;
            this.m_cancelButton.Text = "Cancel";
            this.m_cancelButton.UseVisualStyleBackColor = true;
            // 
            // AddOptionListForm
            // 
            this.AcceptButton = this.m_saveButton;
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.BackColor = System.Drawing.Color.LightSteelBlue;
            this.CancelButton = this.m_cancelButton;
            this.ClientSize = new System.Drawing.Size(274, 154);
            this.ControlBox = false;
            this.Controls.Add(this.m_cancelButton);
            this.Controls.Add(this.m_saveButton);
            this.Controls.Add(this.m_optionTypeLabel);
            this.Controls.Add(this.m_optionListComboBox);
            this.Controls.Add(this.m_modelCodeTextBox);
            this.Controls.Add(this.m_modelCodeLabel);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedToolWindow;
            this.Name = "AddOptionListForm";
            this.ShowIcon = false;
            this.ShowInTaskbar = false;
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterParent;
            this.Text = "AddOptionListForm";
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Label m_modelCodeLabel;
        private System.Windows.Forms.TextBox m_modelCodeTextBox;
        private System.Windows.Forms.ComboBox m_optionListComboBox;
        private System.Windows.Forms.Label m_optionTypeLabel;
        private System.Windows.Forms.Button m_saveButton;
        private System.Windows.Forms.Button m_cancelButton;
    }
}