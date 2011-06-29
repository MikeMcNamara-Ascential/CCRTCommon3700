namespace MercedesBrakeForceEditor
{
    partial class ParameterEditorForm
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
            this.menuStrip1 = new System.Windows.Forms.MenuStrip();
            this.fileToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.newToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.openToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.saveToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.exitToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.setupToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.vehicleBuildFolderToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.m_truckFamilyLabel = new System.Windows.Forms.Label();
            this.m_truckFamilyTextBox = new System.Windows.Forms.TextBox();
            this.m_brkStopDistStartSpeedLabel = new System.Windows.Forms.Label();
            this.m_brkStopDistSpeedTextBox = new System.Windows.Forms.TextBox();
            this.m_brakeForceParamsLabel = new System.Windows.Forms.Label();
            this.m_brakeForceParamDataGridView = new System.Windows.Forms.DataGridView();
            this.Axle = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.MinBrakeForce = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.MaxBrakeForce = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.MaxDiffForce = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.MaxStopDist = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.menuStrip1.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.m_brakeForceParamDataGridView)).BeginInit();
            this.SuspendLayout();
            // 
            // menuStrip1
            // 
            this.menuStrip1.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.fileToolStripMenuItem,
            this.setupToolStripMenuItem});
            this.menuStrip1.Location = new System.Drawing.Point(0, 0);
            this.menuStrip1.Name = "menuStrip1";
            this.menuStrip1.Size = new System.Drawing.Size(584, 24);
            this.menuStrip1.TabIndex = 0;
            this.menuStrip1.Text = "menuStrip1";
            // 
            // fileToolStripMenuItem
            // 
            this.fileToolStripMenuItem.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.newToolStripMenuItem,
            this.openToolStripMenuItem,
            this.saveToolStripMenuItem,
            this.exitToolStripMenuItem});
            this.fileToolStripMenuItem.Name = "fileToolStripMenuItem";
            this.fileToolStripMenuItem.Size = new System.Drawing.Size(35, 20);
            this.fileToolStripMenuItem.Text = "File";
            // 
            // newToolStripMenuItem
            // 
            this.newToolStripMenuItem.Name = "newToolStripMenuItem";
            this.newToolStripMenuItem.Size = new System.Drawing.Size(100, 22);
            this.newToolStripMenuItem.Text = "New";
            this.newToolStripMenuItem.Click += new System.EventHandler(this.newToolStripMenuItem_Click);
            // 
            // openToolStripMenuItem
            // 
            this.openToolStripMenuItem.Name = "openToolStripMenuItem";
            this.openToolStripMenuItem.Size = new System.Drawing.Size(100, 22);
            this.openToolStripMenuItem.Text = "Open";
            this.openToolStripMenuItem.Click += new System.EventHandler(this.openToolStripMenuItem_Click);
            // 
            // saveToolStripMenuItem
            // 
            this.saveToolStripMenuItem.Name = "saveToolStripMenuItem";
            this.saveToolStripMenuItem.Size = new System.Drawing.Size(100, 22);
            this.saveToolStripMenuItem.Text = "Save";
            this.saveToolStripMenuItem.Click += new System.EventHandler(this.saveToolStripMenuItem_Click);
            // 
            // exitToolStripMenuItem
            // 
            this.exitToolStripMenuItem.Name = "exitToolStripMenuItem";
            this.exitToolStripMenuItem.Size = new System.Drawing.Size(100, 22);
            this.exitToolStripMenuItem.Text = "Exit";
            this.exitToolStripMenuItem.Click += new System.EventHandler(this.exitToolStripMenuItem_Click);
            // 
            // setupToolStripMenuItem
            // 
            this.setupToolStripMenuItem.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.vehicleBuildFolderToolStripMenuItem});
            this.setupToolStripMenuItem.Name = "setupToolStripMenuItem";
            this.setupToolStripMenuItem.Size = new System.Drawing.Size(47, 20);
            this.setupToolStripMenuItem.Text = "Setup";
            // 
            // vehicleBuildFolderToolStripMenuItem
            // 
            this.vehicleBuildFolderToolStripMenuItem.Name = "vehicleBuildFolderToolStripMenuItem";
            this.vehicleBuildFolderToolStripMenuItem.Size = new System.Drawing.Size(165, 22);
            this.vehicleBuildFolderToolStripMenuItem.Text = "Vehicle Build Folder";
            this.vehicleBuildFolderToolStripMenuItem.Click += new System.EventHandler(this.vehicleBuildFolderToolStripMenuItem_Click);
            // 
            // m_truckFamilyLabel
            // 
            this.m_truckFamilyLabel.AutoSize = true;
            this.m_truckFamilyLabel.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.m_truckFamilyLabel.Location = new System.Drawing.Point(20, 45);
            this.m_truckFamilyLabel.Name = "m_truckFamilyLabel";
            this.m_truckFamilyLabel.Size = new System.Drawing.Size(83, 13);
            this.m_truckFamilyLabel.TabIndex = 1;
            this.m_truckFamilyLabel.Text = "Truck Family:";
            // 
            // m_truckFamilyTextBox
            // 
            this.m_truckFamilyTextBox.Location = new System.Drawing.Point(109, 42);
            this.m_truckFamilyTextBox.MaxLength = 21;
            this.m_truckFamilyTextBox.Name = "m_truckFamilyTextBox";
            this.m_truckFamilyTextBox.Size = new System.Drawing.Size(169, 20);
            this.m_truckFamilyTextBox.TabIndex = 2;
            // 
            // m_brkStopDistStartSpeedLabel
            // 
            this.m_brkStopDistStartSpeedLabel.AutoSize = true;
            this.m_brkStopDistStartSpeedLabel.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.m_brkStopDistStartSpeedLabel.Location = new System.Drawing.Point(20, 81);
            this.m_brkStopDistStartSpeedLabel.Name = "m_brkStopDistStartSpeedLabel";
            this.m_brkStopDistStartSpeedLabel.Size = new System.Drawing.Size(190, 26);
            this.m_brkStopDistStartSpeedLabel.TabIndex = 3;
            this.m_brkStopDistStartSpeedLabel.Text = "Stopping Distance Measurement\r\n            Start Speed (kmh)";
            // 
            // m_brkStopDistSpeedTextBox
            // 
            this.m_brkStopDistSpeedTextBox.Location = new System.Drawing.Point(98, 110);
            this.m_brkStopDistSpeedTextBox.Name = "m_brkStopDistSpeedTextBox";
            this.m_brkStopDistSpeedTextBox.Size = new System.Drawing.Size(57, 20);
            this.m_brkStopDistSpeedTextBox.TabIndex = 4;
            this.m_brkStopDistSpeedTextBox.TextChanged += new System.EventHandler(this.m_brkStopDistSpeedTextBox_TextChanged);
            // 
            // m_brakeForceParamsLabel
            // 
            this.m_brakeForceParamsLabel.AutoSize = true;
            this.m_brakeForceParamsLabel.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.m_brakeForceParamsLabel.Location = new System.Drawing.Point(20, 154);
            this.m_brakeForceParamsLabel.Name = "m_brakeForceParamsLabel";
            this.m_brakeForceParamsLabel.Size = new System.Drawing.Size(147, 13);
            this.m_brakeForceParamsLabel.TabIndex = 5;
            this.m_brakeForceParamsLabel.Text = "Brake Force Parameters:";
            // 
            // m_brakeForceParamDataGridView
            // 
            this.m_brakeForceParamDataGridView.AllowUserToAddRows = false;
            this.m_brakeForceParamDataGridView.AllowUserToDeleteRows = false;
            this.m_brakeForceParamDataGridView.AutoSizeColumnsMode = System.Windows.Forms.DataGridViewAutoSizeColumnsMode.Fill;
            this.m_brakeForceParamDataGridView.BackgroundColor = System.Drawing.Color.LightSteelBlue;
            this.m_brakeForceParamDataGridView.BorderStyle = System.Windows.Forms.BorderStyle.None;
            this.m_brakeForceParamDataGridView.ColumnHeadersHeightSizeMode = System.Windows.Forms.DataGridViewColumnHeadersHeightSizeMode.AutoSize;
            this.m_brakeForceParamDataGridView.Columns.AddRange(new System.Windows.Forms.DataGridViewColumn[] {
            this.Axle,
            this.MinBrakeForce,
            this.MaxBrakeForce,
            this.MaxDiffForce,
            this.MaxStopDist});
            this.m_brakeForceParamDataGridView.Location = new System.Drawing.Point(23, 170);
            this.m_brakeForceParamDataGridView.Name = "m_brakeForceParamDataGridView";
            this.m_brakeForceParamDataGridView.RowHeadersVisible = false;
            this.m_brakeForceParamDataGridView.Size = new System.Drawing.Size(533, 130);
            this.m_brakeForceParamDataGridView.TabIndex = 6;
            this.m_brakeForceParamDataGridView.CellValueChanged += new System.Windows.Forms.DataGridViewCellEventHandler(this.m_brakeForceParamDataGridView_CellValueChanged);
            // 
            // Axle
            // 
            this.Axle.HeaderText = "Axle";
            this.Axle.Name = "Axle";
            this.Axle.ReadOnly = true;
            // 
            // MinBrakeForce
            // 
            this.MinBrakeForce.HeaderText = "Minimum Brake Force (kgf)";
            this.MinBrakeForce.Name = "MinBrakeForce";
            // 
            // MaxBrakeForce
            // 
            this.MaxBrakeForce.HeaderText = "Maximum Brake Force (kgf)";
            this.MaxBrakeForce.Name = "MaxBrakeForce";
            // 
            // MaxDiffForce
            // 
            this.MaxDiffForce.HeaderText = "Maximum Side to Side Force Difference (kgf)";
            this.MaxDiffForce.Name = "MaxDiffForce";
            // 
            // MaxStopDist
            // 
            this.MaxStopDist.HeaderText = "Maximum Stopping Distance (m)";
            this.MaxStopDist.Name = "MaxStopDist";
            // 
            // ParameterEditorForm
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.BackColor = System.Drawing.Color.LightSteelBlue;
            this.ClientSize = new System.Drawing.Size(584, 335);
            this.Controls.Add(this.m_brakeForceParamDataGridView);
            this.Controls.Add(this.m_brakeForceParamsLabel);
            this.Controls.Add(this.m_brkStopDistSpeedTextBox);
            this.Controls.Add(this.m_brkStopDistStartSpeedLabel);
            this.Controls.Add(this.m_truckFamilyTextBox);
            this.Controls.Add(this.m_truckFamilyLabel);
            this.Controls.Add(this.menuStrip1);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.Fixed3D;
            this.MainMenuStrip = this.menuStrip1;
            this.Name = "ParameterEditorForm";
            this.Text = "Brake Force Editor";
            this.menuStrip1.ResumeLayout(false);
            this.menuStrip1.PerformLayout();
            ((System.ComponentModel.ISupportInitialize)(this.m_brakeForceParamDataGridView)).EndInit();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.MenuStrip menuStrip1;
        private System.Windows.Forms.ToolStripMenuItem fileToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem newToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem openToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem saveToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem exitToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem setupToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem vehicleBuildFolderToolStripMenuItem;
        private System.Windows.Forms.Label m_truckFamilyLabel;
        private System.Windows.Forms.TextBox m_truckFamilyTextBox;
        private System.Windows.Forms.Label m_brkStopDistStartSpeedLabel;
        private System.Windows.Forms.TextBox m_brkStopDistSpeedTextBox;
        private System.Windows.Forms.Label m_brakeForceParamsLabel;
        private System.Windows.Forms.DataGridView m_brakeForceParamDataGridView;
        private System.Windows.Forms.DataGridViewTextBoxColumn Axle;
        private System.Windows.Forms.DataGridViewTextBoxColumn MinBrakeForce;
        private System.Windows.Forms.DataGridViewTextBoxColumn MaxBrakeForce;
        private System.Windows.Forms.DataGridViewTextBoxColumn MaxDiffForce;
        private System.Windows.Forms.DataGridViewTextBoxColumn MaxStopDist;
    }
}

