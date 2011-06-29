namespace BomFileProcessor
{
    partial class EditVehicleBuildFileForm
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
            this.m_saveButton = new System.Windows.Forms.Button();
            this.m_closeButton = new System.Windows.Forms.Button();
            this.m_paramFileLabel = new System.Windows.Forms.Label();
            this.m_fileNameLabel = new System.Windows.Forms.Label();
            this.m_buildOptionDataGridView = new System.Windows.Forms.DataGridView();
            this.BuildOption = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.Value = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.m_buildOptionLabel = new System.Windows.Forms.Label();
            this.m_brakeParamLabel = new System.Windows.Forms.Label();
            this.m_brakeParamDataGridView = new System.Windows.Forms.DataGridView();
            this.Axle = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.MaxDragForce = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.MinBrakeForce = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.MaxBrakeForce = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.SideToSideBalance = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.m_updateToConfigValButton = new System.Windows.Forms.Button();
            this.m_setToConfiguredValuesLabel = new System.Windows.Forms.Label();
            this.m_buildoptionsCheckBox = new System.Windows.Forms.CheckBox();
            this.m_brkForceCheckBox = new System.Windows.Forms.CheckBox();
            ((System.ComponentModel.ISupportInitialize)(this.m_buildOptionDataGridView)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.m_brakeParamDataGridView)).BeginInit();
            this.SuspendLayout();
            // 
            // m_saveButton
            // 
            this.m_saveButton.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this.m_saveButton.Location = new System.Drawing.Point(517, 399);
            this.m_saveButton.Name = "m_saveButton";
            this.m_saveButton.Size = new System.Drawing.Size(75, 23);
            this.m_saveButton.TabIndex = 0;
            this.m_saveButton.Text = "Save";
            this.m_saveButton.UseVisualStyleBackColor = true;
            this.m_saveButton.Click += new System.EventHandler(this.m_saveButton_Click);
            // 
            // m_closeButton
            // 
            this.m_closeButton.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)));
            this.m_closeButton.DialogResult = System.Windows.Forms.DialogResult.Cancel;
            this.m_closeButton.Location = new System.Drawing.Point(12, 399);
            this.m_closeButton.Name = "m_closeButton";
            this.m_closeButton.Size = new System.Drawing.Size(75, 23);
            this.m_closeButton.TabIndex = 1;
            this.m_closeButton.Text = "Close";
            this.m_closeButton.UseVisualStyleBackColor = true;
            this.m_closeButton.Click += new System.EventHandler(this.m_closeButton_Click);
            // 
            // m_paramFileLabel
            // 
            this.m_paramFileLabel.AutoSize = true;
            this.m_paramFileLabel.Location = new System.Drawing.Point(12, 9);
            this.m_paramFileLabel.Name = "m_paramFileLabel";
            this.m_paramFileLabel.Size = new System.Drawing.Size(77, 13);
            this.m_paramFileLabel.TabIndex = 2;
            this.m_paramFileLabel.Text = "Parameter File:";
            // 
            // m_fileNameLabel
            // 
            this.m_fileNameLabel.AutoSize = true;
            this.m_fileNameLabel.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.m_fileNameLabel.ForeColor = System.Drawing.Color.Red;
            this.m_fileNameLabel.Location = new System.Drawing.Point(95, 9);
            this.m_fileNameLabel.Name = "m_fileNameLabel";
            this.m_fileNameLabel.Size = new System.Drawing.Size(41, 13);
            this.m_fileNameLabel.TabIndex = 3;
            this.m_fileNameLabel.Text = "label1";
            // 
            // m_buildOptionDataGridView
            // 
            this.m_buildOptionDataGridView.AllowUserToAddRows = false;
            this.m_buildOptionDataGridView.AllowUserToDeleteRows = false;
            this.m_buildOptionDataGridView.AutoSizeColumnsMode = System.Windows.Forms.DataGridViewAutoSizeColumnsMode.Fill;
            this.m_buildOptionDataGridView.BackgroundColor = System.Drawing.Color.LightSteelBlue;
            this.m_buildOptionDataGridView.BorderStyle = System.Windows.Forms.BorderStyle.None;
            this.m_buildOptionDataGridView.ColumnHeadersHeightSizeMode = System.Windows.Forms.DataGridViewColumnHeadersHeightSizeMode.AutoSize;
            this.m_buildOptionDataGridView.Columns.AddRange(new System.Windows.Forms.DataGridViewColumn[] {
            this.BuildOption,
            this.Value});
            this.m_buildOptionDataGridView.Location = new System.Drawing.Point(12, 57);
            this.m_buildOptionDataGridView.Name = "m_buildOptionDataGridView";
            this.m_buildOptionDataGridView.RowHeadersVisible = false;
            this.m_buildOptionDataGridView.Size = new System.Drawing.Size(279, 150);
            this.m_buildOptionDataGridView.TabIndex = 4;
            // 
            // BuildOption
            // 
            this.BuildOption.HeaderText = "Build Option";
            this.BuildOption.Name = "BuildOption";
            this.BuildOption.ReadOnly = true;
            // 
            // Value
            // 
            this.Value.HeaderText = "Value";
            this.Value.Name = "Value";
            // 
            // m_buildOptionLabel
            // 
            this.m_buildOptionLabel.AutoSize = true;
            this.m_buildOptionLabel.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.m_buildOptionLabel.Location = new System.Drawing.Point(12, 41);
            this.m_buildOptionLabel.Name = "m_buildOptionLabel";
            this.m_buildOptionLabel.Size = new System.Drawing.Size(86, 13);
            this.m_buildOptionLabel.TabIndex = 5;
            this.m_buildOptionLabel.Text = "Build Options:";
            // 
            // m_brakeParamLabel
            // 
            this.m_brakeParamLabel.AutoSize = true;
            this.m_brakeParamLabel.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.m_brakeParamLabel.Location = new System.Drawing.Point(12, 239);
            this.m_brakeParamLabel.Name = "m_brakeParamLabel";
            this.m_brakeParamLabel.Size = new System.Drawing.Size(111, 13);
            this.m_brakeParamLabel.TabIndex = 6;
            this.m_brakeParamLabel.Text = "Brake Parameters:";
            // 
            // m_brakeParamDataGridView
            // 
            this.m_brakeParamDataGridView.AllowUserToAddRows = false;
            this.m_brakeParamDataGridView.AllowUserToDeleteRows = false;
            this.m_brakeParamDataGridView.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom)
                        | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.m_brakeParamDataGridView.AutoSizeColumnsMode = System.Windows.Forms.DataGridViewAutoSizeColumnsMode.Fill;
            this.m_brakeParamDataGridView.BackgroundColor = System.Drawing.Color.LightSteelBlue;
            this.m_brakeParamDataGridView.BorderStyle = System.Windows.Forms.BorderStyle.None;
            this.m_brakeParamDataGridView.ColumnHeadersHeightSizeMode = System.Windows.Forms.DataGridViewColumnHeadersHeightSizeMode.AutoSize;
            this.m_brakeParamDataGridView.Columns.AddRange(new System.Windows.Forms.DataGridViewColumn[] {
            this.Axle,
            this.MaxDragForce,
            this.MinBrakeForce,
            this.MaxBrakeForce,
            this.SideToSideBalance});
            this.m_brakeParamDataGridView.Location = new System.Drawing.Point(15, 259);
            this.m_brakeParamDataGridView.Name = "m_brakeParamDataGridView";
            this.m_brakeParamDataGridView.RowHeadersVisible = false;
            this.m_brakeParamDataGridView.Size = new System.Drawing.Size(577, 134);
            this.m_brakeParamDataGridView.TabIndex = 7;
            // 
            // Axle
            // 
            this.Axle.HeaderText = "Axle";
            this.Axle.Name = "Axle";
            this.Axle.ReadOnly = true;
            // 
            // MaxDragForce
            // 
            this.MaxDragForce.HeaderText = "Maximum Drag Force (lbf)";
            this.MaxDragForce.Name = "MaxDragForce";
            // 
            // MinBrakeForce
            // 
            this.MinBrakeForce.HeaderText = "Minimum Brake Force (lbf)";
            this.MinBrakeForce.Name = "MinBrakeForce";
            // 
            // MaxBrakeForce
            // 
            this.MaxBrakeForce.HeaderText = "Maximum Brake Force (lbf)";
            this.MaxBrakeForce.Name = "MaxBrakeForce";
            // 
            // SideToSideBalance
            // 
            this.SideToSideBalance.HeaderText = "Side to Side Brake Balance (offset from 50%)";
            this.SideToSideBalance.Name = "SideToSideBalance";
            // 
            // m_updateToConfigValButton
            // 
            this.m_updateToConfigValButton.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this.m_updateToConfigValButton.Location = new System.Drawing.Point(390, 137);
            this.m_updateToConfigValButton.Name = "m_updateToConfigValButton";
            this.m_updateToConfigValButton.Size = new System.Drawing.Size(167, 23);
            this.m_updateToConfigValButton.TabIndex = 8;
            this.m_updateToConfigValButton.Text = "Update To Configured Values";
            this.m_updateToConfigValButton.UseVisualStyleBackColor = true;
            this.m_updateToConfigValButton.Click += new System.EventHandler(this.m_updateToConfigValButton_Click);
            // 
            // m_setToConfiguredValuesLabel
            // 
            this.m_setToConfiguredValuesLabel.AutoSize = true;
            this.m_setToConfiguredValuesLabel.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.m_setToConfiguredValuesLabel.Location = new System.Drawing.Point(332, 62);
            this.m_setToConfiguredValuesLabel.Name = "m_setToConfiguredValuesLabel";
            this.m_setToConfiguredValuesLabel.Size = new System.Drawing.Size(223, 13);
            this.m_setToConfiguredValuesLabel.TabIndex = 9;
            this.m_setToConfiguredValuesLabel.Text = "Set Parameters To Configured Values:";
            // 
            // m_buildoptionsCheckBox
            // 
            this.m_buildoptionsCheckBox.AutoSize = true;
            this.m_buildoptionsCheckBox.Location = new System.Drawing.Point(390, 82);
            this.m_buildoptionsCheckBox.Name = "m_buildoptionsCheckBox";
            this.m_buildoptionsCheckBox.Size = new System.Drawing.Size(86, 17);
            this.m_buildoptionsCheckBox.TabIndex = 10;
            this.m_buildoptionsCheckBox.Text = "Build options";
            this.m_buildoptionsCheckBox.UseVisualStyleBackColor = true;
            // 
            // m_brkForceCheckBox
            // 
            this.m_brkForceCheckBox.AutoSize = true;
            this.m_brkForceCheckBox.Location = new System.Drawing.Point(390, 105);
            this.m_brkForceCheckBox.Name = "m_brkForceCheckBox";
            this.m_brkForceCheckBox.Size = new System.Drawing.Size(140, 17);
            this.m_brkForceCheckBox.TabIndex = 11;
            this.m_brkForceCheckBox.Text = "Brake Force Parameters";
            this.m_brkForceCheckBox.UseVisualStyleBackColor = true;
            // 
            // EditVehicleBuildFileForm
            // 
            this.AcceptButton = this.m_saveButton;
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.BackColor = System.Drawing.Color.LightSteelBlue;
            this.CancelButton = this.m_closeButton;
            this.ClientSize = new System.Drawing.Size(604, 434);
            this.ControlBox = false;
            this.Controls.Add(this.m_brkForceCheckBox);
            this.Controls.Add(this.m_buildoptionsCheckBox);
            this.Controls.Add(this.m_setToConfiguredValuesLabel);
            this.Controls.Add(this.m_updateToConfigValButton);
            this.Controls.Add(this.m_brakeParamDataGridView);
            this.Controls.Add(this.m_brakeParamLabel);
            this.Controls.Add(this.m_buildOptionLabel);
            this.Controls.Add(this.m_buildOptionDataGridView);
            this.Controls.Add(this.m_fileNameLabel);
            this.Controls.Add(this.m_paramFileLabel);
            this.Controls.Add(this.m_closeButton);
            this.Controls.Add(this.m_saveButton);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.Fixed3D;
            this.Name = "EditVehicleBuildFileForm";
            this.Text = "Edit Vehicle Parameters";
            ((System.ComponentModel.ISupportInitialize)(this.m_buildOptionDataGridView)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.m_brakeParamDataGridView)).EndInit();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Button m_saveButton;
        private System.Windows.Forms.Button m_closeButton;
        private System.Windows.Forms.Label m_paramFileLabel;
        private System.Windows.Forms.Label m_fileNameLabel;
        private System.Windows.Forms.DataGridView m_buildOptionDataGridView;
        private System.Windows.Forms.DataGridViewTextBoxColumn BuildOption;
        private System.Windows.Forms.DataGridViewTextBoxColumn Value;
        private System.Windows.Forms.Label m_buildOptionLabel;
        private System.Windows.Forms.Label m_brakeParamLabel;
        private System.Windows.Forms.DataGridView m_brakeParamDataGridView;
        private System.Windows.Forms.DataGridViewTextBoxColumn Axle;
        private System.Windows.Forms.DataGridViewTextBoxColumn MaxDragForce;
        private System.Windows.Forms.DataGridViewTextBoxColumn MinBrakeForce;
        private System.Windows.Forms.DataGridViewTextBoxColumn MaxBrakeForce;
        private System.Windows.Forms.DataGridViewTextBoxColumn SideToSideBalance;
        private System.Windows.Forms.Button m_updateToConfigValButton;
        private System.Windows.Forms.Label m_setToConfiguredValuesLabel;
        private System.Windows.Forms.CheckBox m_buildoptionsCheckBox;
        private System.Windows.Forms.CheckBox m_brkForceCheckBox;
    }
}