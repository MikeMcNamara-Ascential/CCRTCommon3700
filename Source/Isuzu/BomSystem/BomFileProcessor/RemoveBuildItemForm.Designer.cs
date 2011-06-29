namespace BomFileProcessor
{
    partial class RemoveBuildItemForm
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
            this.m_itemComboBox = new System.Windows.Forms.ComboBox();
            this.m_buildItemLabel = new System.Windows.Forms.Label();
            this.m_removeButton = new System.Windows.Forms.Button();
            this.m_cancelButton = new System.Windows.Forms.Button();
            this.SuspendLayout();
            // 
            // m_itemComboBox
            // 
            this.m_itemComboBox.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.m_itemComboBox.FormattingEnabled = true;
            this.m_itemComboBox.Location = new System.Drawing.Point(74, 12);
            this.m_itemComboBox.Name = "m_itemComboBox";
            this.m_itemComboBox.Size = new System.Drawing.Size(121, 21);
            this.m_itemComboBox.TabIndex = 0;
            // 
            // m_buildItemLabel
            // 
            this.m_buildItemLabel.AutoSize = true;
            this.m_buildItemLabel.Location = new System.Drawing.Point(12, 15);
            this.m_buildItemLabel.Name = "m_buildItemLabel";
            this.m_buildItemLabel.Size = new System.Drawing.Size(56, 13);
            this.m_buildItemLabel.TabIndex = 1;
            this.m_buildItemLabel.Text = "Build Item:";
            // 
            // m_removeButton
            // 
            this.m_removeButton.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this.m_removeButton.Location = new System.Drawing.Point(120, 52);
            this.m_removeButton.Name = "m_removeButton";
            this.m_removeButton.Size = new System.Drawing.Size(75, 23);
            this.m_removeButton.TabIndex = 2;
            this.m_removeButton.Text = "Remove";
            this.m_removeButton.UseVisualStyleBackColor = true;
            this.m_removeButton.Click += new System.EventHandler(this.m_removeButton_Click);
            // 
            // m_cancelButton
            // 
            this.m_cancelButton.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)));
            this.m_cancelButton.Location = new System.Drawing.Point(12, 52);
            this.m_cancelButton.Name = "m_cancelButton";
            this.m_cancelButton.Size = new System.Drawing.Size(75, 23);
            this.m_cancelButton.TabIndex = 3;
            this.m_cancelButton.Text = "Cancel";
            this.m_cancelButton.UseVisualStyleBackColor = true;
            // 
            // RemoveBuildItemForm
            // 
            this.AcceptButton = this.m_removeButton;
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.BackColor = System.Drawing.Color.LightSteelBlue;
            this.CancelButton = this.m_cancelButton;
            this.ClientSize = new System.Drawing.Size(211, 87);
            this.ControlBox = false;
            this.Controls.Add(this.m_cancelButton);
            this.Controls.Add(this.m_removeButton);
            this.Controls.Add(this.m_buildItemLabel);
            this.Controls.Add(this.m_itemComboBox);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedToolWindow;
            this.Name = "RemoveBuildItemForm";
            this.Text = "Remove Build Item";
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.ComboBox m_itemComboBox;
        private System.Windows.Forms.Label m_buildItemLabel;
        private System.Windows.Forms.Button m_removeButton;
        private System.Windows.Forms.Button m_cancelButton;
    }
}