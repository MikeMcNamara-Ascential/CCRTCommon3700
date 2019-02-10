namespace ToyotaParameterEditor
{
    partial class RemoveVehicleForm
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
            this.m_vehicleNameLabel = new System.Windows.Forms.Label();
            this.m_availableVehiclesComboBox = new System.Windows.Forms.ComboBox();
            this.m_cancelButton = new System.Windows.Forms.Button();
            this.m_okButton = new System.Windows.Forms.Button();
            this.label1 = new System.Windows.Forms.Label();
            this.m_removeSelTableCheckBox = new System.Windows.Forms.CheckBox();
            this.m_removeSelecNumCheckBox = new System.Windows.Forms.CheckBox();
            this.m_removeParamFileCheckBox = new System.Windows.Forms.CheckBox();
            this.m_removeImgFilesCheckBox = new System.Windows.Forms.CheckBox();
            this.label2 = new System.Windows.Forms.Label();
            this.SuspendLayout();
            // 
            // m_vehicleNameLabel
            // 
            this.m_vehicleNameLabel.AutoSize = true;
            this.m_vehicleNameLabel.Font = new System.Drawing.Font("Microsoft Sans Serif", 10F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.m_vehicleNameLabel.Location = new System.Drawing.Point(13, 10);
            this.m_vehicleNameLabel.Margin = new System.Windows.Forms.Padding(4, 0, 4, 0);
            this.m_vehicleNameLabel.Name = "m_vehicleNameLabel";
            this.m_vehicleNameLabel.Size = new System.Drawing.Size(111, 20);
            this.m_vehicleNameLabel.TabIndex = 2;
            this.m_vehicleNameLabel.Text = "Vehicle Name:";
            // 
            // m_availableVehiclesComboBox
            // 
            this.m_availableVehiclesComboBox.FormattingEnabled = true;
            this.m_availableVehiclesComboBox.Location = new System.Drawing.Point(131, 10);
            this.m_availableVehiclesComboBox.Name = "m_availableVehiclesComboBox";
            this.m_availableVehiclesComboBox.Size = new System.Drawing.Size(219, 24);
            this.m_availableVehiclesComboBox.TabIndex = 3;
            this.m_availableVehiclesComboBox.MouseDown += new System.Windows.Forms.MouseEventHandler(this.UpdateAvailableVehicleList);
            // 
            // m_cancelButton
            // 
            this.m_cancelButton.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this.m_cancelButton.Location = new System.Drawing.Point(171, 239);
            this.m_cancelButton.Margin = new System.Windows.Forms.Padding(4);
            this.m_cancelButton.Name = "m_cancelButton";
            this.m_cancelButton.Size = new System.Drawing.Size(100, 28);
            this.m_cancelButton.TabIndex = 9;
            this.m_cancelButton.Text = "Cancel";
            this.m_cancelButton.UseVisualStyleBackColor = true;
            this.m_cancelButton.Click += new System.EventHandler(this.m_cancelButton_Click);
            // 
            // m_okButton
            // 
            this.m_okButton.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this.m_okButton.Location = new System.Drawing.Point(279, 239);
            this.m_okButton.Margin = new System.Windows.Forms.Padding(4);
            this.m_okButton.Name = "m_okButton";
            this.m_okButton.Size = new System.Drawing.Size(100, 28);
            this.m_okButton.TabIndex = 10;
            this.m_okButton.Text = "OK";
            this.m_okButton.UseVisualStyleBackColor = true;
            this.m_okButton.Click += new System.EventHandler(this.m_okButton_Click);
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Font = new System.Drawing.Font("Microsoft Sans Serif", 10F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.label1.Location = new System.Drawing.Point(52, 47);
            this.label1.Margin = new System.Windows.Forms.Padding(4, 0, 4, 0);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(72, 20);
            this.label1.TabIndex = 11;
            this.label1.Text = "Remove:";
            // 
            // m_removeSelTableCheckBox
            // 
            this.m_removeSelTableCheckBox.AutoSize = true;
            this.m_removeSelTableCheckBox.Checked = true;
            this.m_removeSelTableCheckBox.CheckState = System.Windows.Forms.CheckState.Checked;
            this.m_removeSelTableCheckBox.Enabled = false;
            this.m_removeSelTableCheckBox.Location = new System.Drawing.Point(131, 49);
            this.m_removeSelTableCheckBox.Name = "m_removeSelTableCheckBox";
            this.m_removeSelTableCheckBox.Size = new System.Drawing.Size(162, 21);
            this.m_removeSelTableCheckBox.TabIndex = 12;
            this.m_removeSelTableCheckBox.Text = "Selection Table Entry";
            this.m_removeSelTableCheckBox.UseVisualStyleBackColor = true;
            // 
            // m_removeSelecNumCheckBox
            // 
            this.m_removeSelecNumCheckBox.AutoSize = true;
            this.m_removeSelecNumCheckBox.Checked = true;
            this.m_removeSelecNumCheckBox.CheckState = System.Windows.Forms.CheckState.Checked;
            this.m_removeSelecNumCheckBox.Enabled = false;
            this.m_removeSelecNumCheckBox.Location = new System.Drawing.Point(131, 76);
            this.m_removeSelecNumCheckBox.Name = "m_removeSelecNumCheckBox";
            this.m_removeSelecNumCheckBox.Size = new System.Drawing.Size(189, 21);
            this.m_removeSelecNumCheckBox.TabIndex = 13;
            this.m_removeSelecNumCheckBox.Text = "Vehicle Selection Number";
            this.m_removeSelecNumCheckBox.UseVisualStyleBackColor = true;
            // 
            // m_removeParamFileCheckBox
            // 
            this.m_removeParamFileCheckBox.AutoSize = true;
            this.m_removeParamFileCheckBox.Checked = true;
            this.m_removeParamFileCheckBox.CheckState = System.Windows.Forms.CheckState.Checked;
            this.m_removeParamFileCheckBox.Location = new System.Drawing.Point(131, 103);
            this.m_removeParamFileCheckBox.Name = "m_removeParamFileCheckBox";
            this.m_removeParamFileCheckBox.Size = new System.Drawing.Size(119, 21);
            this.m_removeParamFileCheckBox.TabIndex = 14;
            this.m_removeParamFileCheckBox.Text = "Parameter File";
            this.m_removeParamFileCheckBox.UseVisualStyleBackColor = true;
            // 
            // m_removeImgFilesCheckBox
            // 
            this.m_removeImgFilesCheckBox.AutoSize = true;
            this.m_removeImgFilesCheckBox.Checked = true;
            this.m_removeImgFilesCheckBox.CheckState = System.Windows.Forms.CheckState.Checked;
            this.m_removeImgFilesCheckBox.Location = new System.Drawing.Point(131, 130);
            this.m_removeImgFilesCheckBox.Name = "m_removeImgFilesCheckBox";
            this.m_removeImgFilesCheckBox.Size = new System.Drawing.Size(98, 21);
            this.m_removeImgFilesCheckBox.TabIndex = 15;
            this.m_removeImgFilesCheckBox.Text = "Image Files";
            this.m_removeImgFilesCheckBox.UseVisualStyleBackColor = true;
            // 
            // label2
            // 
            this.label2.Location = new System.Drawing.Point(12, 195);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(297, 40);
            this.label2.TabIndex = 16;
            this.label2.Text = "* All Vehicle info will be moved to an archive.     No data will be permanently d" +
    "eleted.";
            // 
            // RemoveVehicleForm
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(8F, 16F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(387, 271);
            this.Controls.Add(this.label2);
            this.Controls.Add(this.m_removeImgFilesCheckBox);
            this.Controls.Add(this.m_removeParamFileCheckBox);
            this.Controls.Add(this.m_removeSelecNumCheckBox);
            this.Controls.Add(this.m_removeSelTableCheckBox);
            this.Controls.Add(this.label1);
            this.Controls.Add(this.m_cancelButton);
            this.Controls.Add(this.m_okButton);
            this.Controls.Add(this.m_availableVehiclesComboBox);
            this.Controls.Add(this.m_vehicleNameLabel);
            this.MaximumSize = new System.Drawing.Size(405, 316);
            this.MinimumSize = new System.Drawing.Size(405, 316);
            this.Name = "RemoveVehicleForm";
            this.ShowIcon = false;
            this.Text = "Remove Vehicle";
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Label m_vehicleNameLabel;
        private System.Windows.Forms.ComboBox m_availableVehiclesComboBox;
        private System.Windows.Forms.Button m_cancelButton;
        private System.Windows.Forms.Button m_okButton;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.CheckBox m_removeSelTableCheckBox;
        private System.Windows.Forms.CheckBox m_removeSelecNumCheckBox;
        private System.Windows.Forms.CheckBox m_removeParamFileCheckBox;
        private System.Windows.Forms.CheckBox m_removeImgFilesCheckBox;
        private System.Windows.Forms.Label label2;
    }
}