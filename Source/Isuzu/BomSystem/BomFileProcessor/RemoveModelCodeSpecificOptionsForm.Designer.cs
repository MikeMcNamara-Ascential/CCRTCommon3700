namespace BomFileProcessor
{
    partial class RemoveModelCodeSpecificOptionsForm
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
            this.m_removeButton = new System.Windows.Forms.Button();
            this.m_modelCodeLabel = new System.Windows.Forms.Label();
            this.m_itemComboBox = new System.Windows.Forms.ComboBox();
            this.SuspendLayout();
            // 
            // m_cancelButton
            // 
            this.m_cancelButton.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)));
            this.m_cancelButton.DialogResult = System.Windows.Forms.DialogResult.Cancel;
            this.m_cancelButton.Location = new System.Drawing.Point(9, 46);
            this.m_cancelButton.Name = "m_cancelButton";
            this.m_cancelButton.Size = new System.Drawing.Size(75, 23);
            this.m_cancelButton.TabIndex = 7;
            this.m_cancelButton.Text = "Cancel";
            this.m_cancelButton.UseVisualStyleBackColor = true;
            this.m_cancelButton.Click += new System.EventHandler(this.m_cancelButton_Click);
            // 
            // m_removeButton
            // 
            this.m_removeButton.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this.m_removeButton.Location = new System.Drawing.Point(139, 46);
            this.m_removeButton.Name = "m_removeButton";
            this.m_removeButton.Size = new System.Drawing.Size(75, 23);
            this.m_removeButton.TabIndex = 6;
            this.m_removeButton.Text = "Remove";
            this.m_removeButton.UseVisualStyleBackColor = true;
            this.m_removeButton.Click += new System.EventHandler(this.m_removeButton_Click);
            // 
            // m_modelCodeLabel
            // 
            this.m_modelCodeLabel.AutoSize = true;
            this.m_modelCodeLabel.Location = new System.Drawing.Point(7, 7);
            this.m_modelCodeLabel.Name = "m_modelCodeLabel";
            this.m_modelCodeLabel.Size = new System.Drawing.Size(67, 13);
            this.m_modelCodeLabel.TabIndex = 5;
            this.m_modelCodeLabel.Text = "Model Code:";
            // 
            // m_itemComboBox
            // 
            this.m_itemComboBox.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.m_itemComboBox.FormattingEnabled = true;
            this.m_itemComboBox.Location = new System.Drawing.Point(93, 4);
            this.m_itemComboBox.Name = "m_itemComboBox";
            this.m_itemComboBox.Size = new System.Drawing.Size(121, 21);
            this.m_itemComboBox.TabIndex = 4;
            this.m_itemComboBox.SelectedIndexChanged += new System.EventHandler(this.m_itemComboBox_SelectedIndexChanged);
            // 
            // RemoveModelCodeSpecificOptionsForm
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(224, 73);
            this.Controls.Add(this.m_cancelButton);
            this.Controls.Add(this.m_removeButton);
            this.Controls.Add(this.m_modelCodeLabel);
            this.Controls.Add(this.m_itemComboBox);
            this.Margin = new System.Windows.Forms.Padding(2, 2, 2, 2);
            this.Name = "RemoveModelCodeSpecificOptionsForm";
            this.Text = "RemoveModelCodeSpecificOptionsForm";
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Button m_cancelButton;
        private System.Windows.Forms.Button m_removeButton;
        private System.Windows.Forms.Label m_modelCodeLabel;
        private System.Windows.Forms.ComboBox m_itemComboBox;
    }
}