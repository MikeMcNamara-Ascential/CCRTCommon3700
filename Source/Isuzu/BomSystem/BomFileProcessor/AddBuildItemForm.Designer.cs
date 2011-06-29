namespace BomFileProcessor
{
    partial class AddBuildItemForm
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
            this.m_itemNameLabel = new System.Windows.Forms.Label();
            this.m_itemNameTextBox = new System.Windows.Forms.TextBox();
            this.m_upcLabel = new System.Windows.Forms.Label();
            this.m_upcTextBox = new System.Windows.Forms.TextBox();
            this.m_fnaLlabel = new System.Windows.Forms.Label();
            this.m_fnaTextBox = new System.Windows.Forms.TextBox();
            this.m_okButton = new System.Windows.Forms.Button();
            this.m_cancelButton = new System.Windows.Forms.Button();
            this.SuspendLayout();
            // 
            // m_itemNameLabel
            // 
            this.m_itemNameLabel.AutoSize = true;
            this.m_itemNameLabel.Location = new System.Drawing.Point(12, 15);
            this.m_itemNameLabel.Name = "m_itemNameLabel";
            this.m_itemNameLabel.Size = new System.Drawing.Size(61, 13);
            this.m_itemNameLabel.TabIndex = 0;
            this.m_itemNameLabel.Text = "Item Name:";
            // 
            // m_itemNameTextBox
            // 
            this.m_itemNameTextBox.Location = new System.Drawing.Point(79, 12);
            this.m_itemNameTextBox.Name = "m_itemNameTextBox";
            this.m_itemNameTextBox.Size = new System.Drawing.Size(115, 20);
            this.m_itemNameTextBox.TabIndex = 1;
            // 
            // m_upcLabel
            // 
            this.m_upcLabel.AutoSize = true;
            this.m_upcLabel.Location = new System.Drawing.Point(41, 51);
            this.m_upcLabel.Name = "m_upcLabel";
            this.m_upcLabel.Size = new System.Drawing.Size(32, 13);
            this.m_upcLabel.TabIndex = 2;
            this.m_upcLabel.Text = "UPC:";
            // 
            // m_upcTextBox
            // 
            this.m_upcTextBox.Location = new System.Drawing.Point(79, 48);
            this.m_upcTextBox.Name = "m_upcTextBox";
            this.m_upcTextBox.Size = new System.Drawing.Size(115, 20);
            this.m_upcTextBox.TabIndex = 3;
            // 
            // m_fnaLlabel
            // 
            this.m_fnaLlabel.AutoSize = true;
            this.m_fnaLlabel.Location = new System.Drawing.Point(42, 86);
            this.m_fnaLlabel.Name = "m_fnaLlabel";
            this.m_fnaLlabel.Size = new System.Drawing.Size(31, 13);
            this.m_fnaLlabel.TabIndex = 4;
            this.m_fnaLlabel.Text = "FNA:";
            // 
            // m_fnaTextBox
            // 
            this.m_fnaTextBox.Location = new System.Drawing.Point(79, 83);
            this.m_fnaTextBox.Name = "m_fnaTextBox";
            this.m_fnaTextBox.Size = new System.Drawing.Size(115, 20);
            this.m_fnaTextBox.TabIndex = 5;
            // 
            // m_okButton
            // 
            this.m_okButton.Location = new System.Drawing.Point(120, 120);
            this.m_okButton.Name = "m_okButton";
            this.m_okButton.Size = new System.Drawing.Size(75, 23);
            this.m_okButton.TabIndex = 6;
            this.m_okButton.Text = "OK";
            this.m_okButton.UseVisualStyleBackColor = true;
            this.m_okButton.Click += new System.EventHandler(this.m_okButton_Click);
            // 
            // m_cancelButton
            // 
            this.m_cancelButton.DialogResult = System.Windows.Forms.DialogResult.Cancel;
            this.m_cancelButton.Location = new System.Drawing.Point(12, 120);
            this.m_cancelButton.Name = "m_cancelButton";
            this.m_cancelButton.Size = new System.Drawing.Size(75, 23);
            this.m_cancelButton.TabIndex = 7;
            this.m_cancelButton.Text = "Cancel";
            this.m_cancelButton.UseVisualStyleBackColor = true;
            // 
            // AddBuildItemForm
            // 
            this.AcceptButton = this.m_okButton;
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.BackColor = System.Drawing.Color.LightSteelBlue;
            this.CancelButton = this.m_cancelButton;
            this.ClientSize = new System.Drawing.Size(207, 155);
            this.ControlBox = false;
            this.Controls.Add(this.m_cancelButton);
            this.Controls.Add(this.m_okButton);
            this.Controls.Add(this.m_fnaTextBox);
            this.Controls.Add(this.m_fnaLlabel);
            this.Controls.Add(this.m_upcTextBox);
            this.Controls.Add(this.m_upcLabel);
            this.Controls.Add(this.m_itemNameTextBox);
            this.Controls.Add(this.m_itemNameLabel);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedToolWindow;
            this.Name = "AddBuildItemForm";
            this.ShowIcon = false;
            this.ShowInTaskbar = false;
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterParent;
            this.Text = "Add Build Item";
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Label m_itemNameLabel;
        private System.Windows.Forms.TextBox m_itemNameTextBox;
        private System.Windows.Forms.Label m_upcLabel;
        private System.Windows.Forms.TextBox m_upcTextBox;
        private System.Windows.Forms.Label m_fnaLlabel;
        private System.Windows.Forms.TextBox m_fnaTextBox;
        private System.Windows.Forms.Button m_okButton;
        private System.Windows.Forms.Button m_cancelButton;
    }
}