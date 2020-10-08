namespace BomFileProcessor
{
    partial class AddModelSpecificOptionsForm
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
            this.m_cancelButton = new System.Windows.Forms.Button();
            this.m_okButton = new System.Windows.Forms.Button();
            this.m_upcLabel = new System.Windows.Forms.Label();
            this.m_modelCodeTextBox = new System.Windows.Forms.TextBox();
            this.m_itemNameLabel = new System.Windows.Forms.Label();
            this.m_writeESNCheckBox = new System.Windows.Forms.CheckBox();
            this.m_esnLeadingCharsTextBox = new System.Windows.Forms.TextBox();
            this.m_esnLeadingCharsLabel = new System.Windows.Forms.Label();
            this.SuspendLayout();
            // 
            // m_cancelButton
            // 
            this.m_cancelButton.DialogResult = System.Windows.Forms.DialogResult.Cancel;
            this.m_cancelButton.Location = new System.Drawing.Point(8, 114);
            this.m_cancelButton.Name = "m_cancelButton";
            this.m_cancelButton.Size = new System.Drawing.Size(75, 23);
            this.m_cancelButton.TabIndex = 15;
            this.m_cancelButton.Text = "Cancel";
            this.m_cancelButton.UseVisualStyleBackColor = true;
            this.m_cancelButton.Click += new System.EventHandler(this.m_cancelButton_Click);
            // 
            // m_okButton
            // 
            this.m_okButton.Location = new System.Drawing.Point(182, 114);
            this.m_okButton.Name = "m_okButton";
            this.m_okButton.Size = new System.Drawing.Size(75, 23);
            this.m_okButton.TabIndex = 14;
            this.m_okButton.Text = "OK";
            this.m_okButton.UseVisualStyleBackColor = true;
            this.m_okButton.Click += new System.EventHandler(this.m_okButton_Click);
            // 
            // m_upcLabel
            // 
            this.m_upcLabel.AutoSize = true;
            this.m_upcLabel.Location = new System.Drawing.Point(9, 38);
            this.m_upcLabel.Name = "m_upcLabel";
            this.m_upcLabel.Size = new System.Drawing.Size(60, 13);
            this.m_upcLabel.TabIndex = 10;
            this.m_upcLabel.Text = "Write ESN:";
            // 
            // m_modelCodeTextBox
            // 
            this.m_modelCodeTextBox.Location = new System.Drawing.Point(142, 8);
            this.m_modelCodeTextBox.Name = "m_modelCodeTextBox";
            this.m_modelCodeTextBox.Size = new System.Drawing.Size(115, 20);
            this.m_modelCodeTextBox.TabIndex = 9;
            // 
            // m_itemNameLabel
            // 
            this.m_itemNameLabel.AutoSize = true;
            this.m_itemNameLabel.Location = new System.Drawing.Point(8, 8);
            this.m_itemNameLabel.Name = "m_itemNameLabel";
            this.m_itemNameLabel.Size = new System.Drawing.Size(67, 13);
            this.m_itemNameLabel.TabIndex = 8;
            this.m_itemNameLabel.Text = "Model Code:";
            // 
            // m_writeESNCheckBox
            // 
            this.m_writeESNCheckBox.AutoSize = true;
            this.m_writeESNCheckBox.Location = new System.Drawing.Point(142, 37);
            this.m_writeESNCheckBox.Margin = new System.Windows.Forms.Padding(2);
            this.m_writeESNCheckBox.Name = "m_writeESNCheckBox";
            this.m_writeESNCheckBox.Size = new System.Drawing.Size(15, 14);
            this.m_writeESNCheckBox.TabIndex = 16;
            this.m_writeESNCheckBox.UseVisualStyleBackColor = true;
            // 
            // m_esnLeadingCharsTextBox
            // 
            this.m_esnLeadingCharsTextBox.Location = new System.Drawing.Point(142, 59);
            this.m_esnLeadingCharsTextBox.Name = "m_esnLeadingCharsTextBox";
            this.m_esnLeadingCharsTextBox.Size = new System.Drawing.Size(115, 20);
            this.m_esnLeadingCharsTextBox.TabIndex = 18;
            // 
            // m_esnLeadingCharsLabel
            // 
            this.m_esnLeadingCharsLabel.AutoSize = true;
            this.m_esnLeadingCharsLabel.Location = new System.Drawing.Point(9, 62);
            this.m_esnLeadingCharsLabel.Name = "m_esnLeadingCharsLabel";
            this.m_esnLeadingCharsLabel.Size = new System.Drawing.Size(96, 13);
            this.m_esnLeadingCharsLabel.TabIndex = 17;
            this.m_esnLeadingCharsLabel.Text = "ESN Engine Code:";
            // 
            // AddModelSpecificOptionsForm
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(262, 141);
            this.Controls.Add(this.m_esnLeadingCharsTextBox);
            this.Controls.Add(this.m_esnLeadingCharsLabel);
            this.Controls.Add(this.m_writeESNCheckBox);
            this.Controls.Add(this.m_cancelButton);
            this.Controls.Add(this.m_okButton);
            this.Controls.Add(this.m_upcLabel);
            this.Controls.Add(this.m_modelCodeTextBox);
            this.Controls.Add(this.m_itemNameLabel);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.SizableToolWindow;
            this.Margin = new System.Windows.Forms.Padding(2);
            this.Name = "AddModelSpecificOptionsForm";
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterParent;
            this.Text = "AddModelSpecificOptionsForm";
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Button m_cancelButton;
        private System.Windows.Forms.Button m_okButton;
        private System.Windows.Forms.Label m_upcLabel;
        private System.Windows.Forms.TextBox m_modelCodeTextBox;
        private System.Windows.Forms.Label m_itemNameLabel;
        private System.Windows.Forms.CheckBox m_writeESNCheckBox;
        private System.Windows.Forms.TextBox m_esnLeadingCharsTextBox;
        private System.Windows.Forms.Label m_esnLeadingCharsLabel;
    }
}