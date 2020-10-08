namespace BomFileProcessor
{
    partial class RemoveOptionForm
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
            this.m_modelCodeComboBox = new System.Windows.Forms.ComboBox();
            this.m_modelCodeLabel = new System.Windows.Forms.Label();
            this.m_cancelButton = new System.Windows.Forms.Button();
            this.m_removeButton = new System.Windows.Forms.Button();
            this.SuspendLayout();
            // 
            // m_modelCodeComboBox
            // 
            this.m_modelCodeComboBox.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.m_modelCodeComboBox.FormattingEnabled = true;
            this.m_modelCodeComboBox.Location = new System.Drawing.Point(85, 12);
            this.m_modelCodeComboBox.Name = "m_modelCodeComboBox";
            this.m_modelCodeComboBox.Size = new System.Drawing.Size(121, 21);
            this.m_modelCodeComboBox.TabIndex = 0;
            // 
            // m_modelCodeLabel
            // 
            this.m_modelCodeLabel.AutoSize = true;
            this.m_modelCodeLabel.Location = new System.Drawing.Point(12, 15);
            this.m_modelCodeLabel.Name = "m_modelCodeLabel";
            this.m_modelCodeLabel.Size = new System.Drawing.Size(67, 13);
            this.m_modelCodeLabel.TabIndex = 1;
            this.m_modelCodeLabel.Text = "Model Code:";
            // 
            // m_cancelButton
            // 
            this.m_cancelButton.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)));
            this.m_cancelButton.DialogResult = System.Windows.Forms.DialogResult.Cancel;
            this.m_cancelButton.Location = new System.Drawing.Point(15, 55);
            this.m_cancelButton.Name = "m_cancelButton";
            this.m_cancelButton.Size = new System.Drawing.Size(75, 23);
            this.m_cancelButton.TabIndex = 2;
            this.m_cancelButton.Text = "Cancel";
            this.m_cancelButton.UseVisualStyleBackColor = true;
            // 
            // m_removeButton
            // 
            this.m_removeButton.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this.m_removeButton.Location = new System.Drawing.Point(131, 55);
            this.m_removeButton.Name = "m_removeButton";
            this.m_removeButton.Size = new System.Drawing.Size(75, 23);
            this.m_removeButton.TabIndex = 3;
            this.m_removeButton.Text = "Remove";
            this.m_removeButton.UseVisualStyleBackColor = true;
            this.m_removeButton.Click += new System.EventHandler(this.m_removeButton_Click);
            // 
            // RemoveOptionForm
            // 
            this.AcceptButton = this.m_removeButton;
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.BackColor = System.Drawing.Color.LightSteelBlue;
            this.CancelButton = this.m_cancelButton;
            this.ClientSize = new System.Drawing.Size(220, 90);
            this.ControlBox = false;
            this.Controls.Add(this.m_removeButton);
            this.Controls.Add(this.m_cancelButton);
            this.Controls.Add(this.m_modelCodeLabel);
            this.Controls.Add(this.m_modelCodeComboBox);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.SizableToolWindow;
            this.Name = "RemoveOptionForm";
            this.ShowIcon = false;
            this.ShowInTaskbar = false;
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterParent;
            this.Text = "RemoveOptionForm";
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.ComboBox m_modelCodeComboBox;
        private System.Windows.Forms.Label m_modelCodeLabel;
        private System.Windows.Forms.Button m_cancelButton;
        private System.Windows.Forms.Button m_removeButton;
    }
}