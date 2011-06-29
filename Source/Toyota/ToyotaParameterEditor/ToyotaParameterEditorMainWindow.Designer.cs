namespace ToyotaParameterEditor
{
    partial class ToyotaParameterEditorMainWindow
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
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(ToyotaParameterEditorMainWindow));
            this.m_mainMenuStrip = new System.Windows.Forms.MenuStrip();
            this.fileToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.openVehicleToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.newVehicleToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.saveUpdatesToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.exportUpdatedFilesToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.importUpdatedFilesToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.exitToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.m_vehicleTypePictureBox = new System.Windows.Forms.PictureBox();
            this.m_vehicleDataLabel = new System.Windows.Forms.Label();
            this.m_vehicleDataGridView = new System.Windows.Forms.DataGridView();
            this.m_parameterNameColumn = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.m_paramValueColumn = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.m_paramUnitsColumn = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.m_brakeTestingParamsLabel = new System.Windows.Forms.Label();
            this.m_brakeDataGridView = new System.Windows.Forms.DataGridView();
            this.m_brakeParamNameColumn = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.m_brakeParamValueColumn = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.m_brakeParamUnitsColumn = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.m_transShiftpointsLabel = new System.Windows.Forms.Label();
            this.m_transmissionDataGridView = new System.Windows.Forms.DataGridView();
            this.m_transParamNameColumn = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.m_transEngineTypeColumn = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.m_transParamValueMph = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.m_transParamValueKph = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.m_speedTargetLabel = new System.Windows.Forms.Label();
            this.m_speedTargetDataGridView = new System.Windows.Forms.DataGridView();
            this.m_speedParamNameColumn = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.m_speedParamValueColumn = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.m_speedParamUnitsColumn = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.m_vehicleLogoPictureBox = new System.Windows.Forms.PictureBox();
            this.m_mainMenuStrip.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.m_vehicleTypePictureBox)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.m_vehicleDataGridView)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.m_brakeDataGridView)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.m_transmissionDataGridView)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.m_speedTargetDataGridView)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.m_vehicleLogoPictureBox)).BeginInit();
            this.SuspendLayout();
            // 
            // m_mainMenuStrip
            // 
            this.m_mainMenuStrip.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.fileToolStripMenuItem});
            this.m_mainMenuStrip.Location = new System.Drawing.Point(0, 0);
            this.m_mainMenuStrip.Name = "m_mainMenuStrip";
            this.m_mainMenuStrip.Size = new System.Drawing.Size(732, 24);
            this.m_mainMenuStrip.TabIndex = 0;
            this.m_mainMenuStrip.Text = "menuStrip1";
            // 
            // fileToolStripMenuItem
            // 
            this.fileToolStripMenuItem.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.openVehicleToolStripMenuItem,
            this.newVehicleToolStripMenuItem,
            this.saveUpdatesToolStripMenuItem,
            this.exportUpdatedFilesToolStripMenuItem,
            this.importUpdatedFilesToolStripMenuItem,
            this.exitToolStripMenuItem});
            this.fileToolStripMenuItem.Name = "fileToolStripMenuItem";
            this.fileToolStripMenuItem.Size = new System.Drawing.Size(35, 20);
            this.fileToolStripMenuItem.Text = "File";
            // 
            // openVehicleToolStripMenuItem
            // 
            this.openVehicleToolStripMenuItem.Name = "openVehicleToolStripMenuItem";
            this.openVehicleToolStripMenuItem.Size = new System.Drawing.Size(174, 22);
            this.openVehicleToolStripMenuItem.Text = "Open Vehicle";
            this.openVehicleToolStripMenuItem.Click += new System.EventHandler(this.openVehicleToolStripMenuItem_Click);
            // 
            // newVehicleToolStripMenuItem
            // 
            this.newVehicleToolStripMenuItem.Name = "newVehicleToolStripMenuItem";
            this.newVehicleToolStripMenuItem.Size = new System.Drawing.Size(174, 22);
            this.newVehicleToolStripMenuItem.Text = "New Vehicle";
            this.newVehicleToolStripMenuItem.Click += new System.EventHandler(this.newVehicleToolStripMenuItem_Click);
            // 
            // saveUpdatesToolStripMenuItem
            // 
            this.saveUpdatesToolStripMenuItem.Name = "saveUpdatesToolStripMenuItem";
            this.saveUpdatesToolStripMenuItem.Size = new System.Drawing.Size(174, 22);
            this.saveUpdatesToolStripMenuItem.Text = "Save Updates";
            this.saveUpdatesToolStripMenuItem.Click += new System.EventHandler(this.saveUpdatesToolStripMenuItem_Click);
            // 
            // exportUpdatedFilesToolStripMenuItem
            // 
            this.exportUpdatedFilesToolStripMenuItem.Name = "exportUpdatedFilesToolStripMenuItem";
            this.exportUpdatedFilesToolStripMenuItem.Size = new System.Drawing.Size(174, 22);
            this.exportUpdatedFilesToolStripMenuItem.Text = "Export Updated Files";
            this.exportUpdatedFilesToolStripMenuItem.Click += new System.EventHandler(this.exportUpdatedFilesToolStripMenuItem_Click);
            // 
            // importUpdatedFilesToolStripMenuItem
            // 
            this.importUpdatedFilesToolStripMenuItem.Name = "importUpdatedFilesToolStripMenuItem";
            this.importUpdatedFilesToolStripMenuItem.Size = new System.Drawing.Size(174, 22);
            this.importUpdatedFilesToolStripMenuItem.Text = "Import Updated Files";
            this.importUpdatedFilesToolStripMenuItem.Click += new System.EventHandler(this.importUpdatedFilesToolStripMenuItem_Click);
            // 
            // exitToolStripMenuItem
            // 
            this.exitToolStripMenuItem.Name = "exitToolStripMenuItem";
            this.exitToolStripMenuItem.Size = new System.Drawing.Size(174, 22);
            this.exitToolStripMenuItem.Text = "Exit";
            this.exitToolStripMenuItem.Click += new System.EventHandler(this.exitToolStripMenuItem_Click);
            // 
            // m_vehicleTypePictureBox
            // 
            this.m_vehicleTypePictureBox.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.m_vehicleTypePictureBox.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D;
            this.m_vehicleTypePictureBox.Image = ((System.Drawing.Image)(resources.GetObject("m_vehicleTypePictureBox.Image")));
            this.m_vehicleTypePictureBox.InitialImage = ((System.Drawing.Image)(resources.GetObject("m_vehicleTypePictureBox.InitialImage")));
            this.m_vehicleTypePictureBox.Location = new System.Drawing.Point(569, 27);
            this.m_vehicleTypePictureBox.Name = "m_vehicleTypePictureBox";
            this.m_vehicleTypePictureBox.Size = new System.Drawing.Size(151, 44);
            this.m_vehicleTypePictureBox.SizeMode = System.Windows.Forms.PictureBoxSizeMode.StretchImage;
            this.m_vehicleTypePictureBox.TabIndex = 1;
            this.m_vehicleTypePictureBox.TabStop = false;
            // 
            // m_vehicleDataLabel
            // 
            this.m_vehicleDataLabel.AutoSize = true;
            this.m_vehicleDataLabel.Font = new System.Drawing.Font("Microsoft Sans Serif", 16F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.m_vehicleDataLabel.Location = new System.Drawing.Point(12, 75);
            this.m_vehicleDataLabel.Name = "m_vehicleDataLabel";
            this.m_vehicleDataLabel.Size = new System.Drawing.Size(142, 26);
            this.m_vehicleDataLabel.TabIndex = 2;
            this.m_vehicleDataLabel.Text = "Vehicle Data:";
            // 
            // m_vehicleDataGridView
            // 
            this.m_vehicleDataGridView.AllowUserToAddRows = false;
            this.m_vehicleDataGridView.AllowUserToDeleteRows = false;
            this.m_vehicleDataGridView.AllowUserToResizeColumns = false;
            this.m_vehicleDataGridView.AllowUserToResizeRows = false;
            this.m_vehicleDataGridView.AutoSizeColumnsMode = System.Windows.Forms.DataGridViewAutoSizeColumnsMode.DisplayedCells;
            this.m_vehicleDataGridView.AutoSizeRowsMode = System.Windows.Forms.DataGridViewAutoSizeRowsMode.DisplayedCells;
            this.m_vehicleDataGridView.BackgroundColor = System.Drawing.SystemColors.Control;
            this.m_vehicleDataGridView.BorderStyle = System.Windows.Forms.BorderStyle.None;
            this.m_vehicleDataGridView.ColumnHeadersHeightSizeMode = System.Windows.Forms.DataGridViewColumnHeadersHeightSizeMode.AutoSize;
            this.m_vehicleDataGridView.Columns.AddRange(new System.Windows.Forms.DataGridViewColumn[] {
            this.m_parameterNameColumn,
            this.m_paramValueColumn,
            this.m_paramUnitsColumn});
            this.m_vehicleDataGridView.Location = new System.Drawing.Point(17, 104);
            this.m_vehicleDataGridView.MaximumSize = new System.Drawing.Size(430, 150);
            this.m_vehicleDataGridView.Name = "m_vehicleDataGridView";
            this.m_vehicleDataGridView.RowHeadersBorderStyle = System.Windows.Forms.DataGridViewHeaderBorderStyle.None;
            this.m_vehicleDataGridView.RowHeadersVisible = false;
            this.m_vehicleDataGridView.Size = new System.Drawing.Size(316, 110);
            this.m_vehicleDataGridView.TabIndex = 3;
            this.m_vehicleDataGridView.CellEndEdit += new System.Windows.Forms.DataGridViewCellEventHandler(this.m_vehicleDataGridView_CellEndEdit);
            // 
            // m_parameterNameColumn
            // 
            this.m_parameterNameColumn.AutoSizeMode = System.Windows.Forms.DataGridViewAutoSizeColumnMode.AllCells;
            this.m_parameterNameColumn.HeaderText = "Parameter Name:";
            this.m_parameterNameColumn.Name = "m_parameterNameColumn";
            this.m_parameterNameColumn.ReadOnly = true;
            this.m_parameterNameColumn.Width = 105;
            // 
            // m_paramValueColumn
            // 
            this.m_paramValueColumn.HeaderText = "Value";
            this.m_paramValueColumn.Name = "m_paramValueColumn";
            this.m_paramValueColumn.Width = 59;
            // 
            // m_paramUnitsColumn
            // 
            this.m_paramUnitsColumn.HeaderText = "Units";
            this.m_paramUnitsColumn.Name = "m_paramUnitsColumn";
            this.m_paramUnitsColumn.ReadOnly = true;
            this.m_paramUnitsColumn.Width = 56;
            // 
            // m_brakeTestingParamsLabel
            // 
            this.m_brakeTestingParamsLabel.AutoSize = true;
            this.m_brakeTestingParamsLabel.Font = new System.Drawing.Font("Microsoft Sans Serif", 16F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.m_brakeTestingParamsLabel.Location = new System.Drawing.Point(8, 252);
            this.m_brakeTestingParamsLabel.Name = "m_brakeTestingParamsLabel";
            this.m_brakeTestingParamsLabel.Size = new System.Drawing.Size(194, 26);
            this.m_brakeTestingParamsLabel.TabIndex = 4;
            this.m_brakeTestingParamsLabel.Text = "Brake Parameters:";
            // 
            // m_brakeDataGridView
            // 
            this.m_brakeDataGridView.AllowUserToAddRows = false;
            this.m_brakeDataGridView.AllowUserToDeleteRows = false;
            this.m_brakeDataGridView.AllowUserToResizeColumns = false;
            this.m_brakeDataGridView.AllowUserToResizeRows = false;
            this.m_brakeDataGridView.AutoSizeColumnsMode = System.Windows.Forms.DataGridViewAutoSizeColumnsMode.DisplayedCells;
            this.m_brakeDataGridView.BackgroundColor = System.Drawing.SystemColors.Control;
            this.m_brakeDataGridView.BorderStyle = System.Windows.Forms.BorderStyle.None;
            this.m_brakeDataGridView.ColumnHeadersHeightSizeMode = System.Windows.Forms.DataGridViewColumnHeadersHeightSizeMode.AutoSize;
            this.m_brakeDataGridView.Columns.AddRange(new System.Windows.Forms.DataGridViewColumn[] {
            this.m_brakeParamNameColumn,
            this.m_brakeParamValueColumn,
            this.m_brakeParamUnitsColumn});
            this.m_brakeDataGridView.Location = new System.Drawing.Point(13, 281);
            this.m_brakeDataGridView.Name = "m_brakeDataGridView";
            this.m_brakeDataGridView.RowHeadersBorderStyle = System.Windows.Forms.DataGridViewHeaderBorderStyle.None;
            this.m_brakeDataGridView.RowHeadersVisible = false;
            this.m_brakeDataGridView.Size = new System.Drawing.Size(316, 286);
            this.m_brakeDataGridView.TabIndex = 5;
            this.m_brakeDataGridView.CellEndEdit += new System.Windows.Forms.DataGridViewCellEventHandler(this.m_brakeDataGridView_CellEndEdit);
            // 
            // m_brakeParamNameColumn
            // 
            this.m_brakeParamNameColumn.HeaderText = "Parameter Name";
            this.m_brakeParamNameColumn.Name = "m_brakeParamNameColumn";
            this.m_brakeParamNameColumn.ReadOnly = true;
            this.m_brakeParamNameColumn.Width = 102;
            // 
            // m_brakeParamValueColumn
            // 
            this.m_brakeParamValueColumn.HeaderText = "Value";
            this.m_brakeParamValueColumn.Name = "m_brakeParamValueColumn";
            this.m_brakeParamValueColumn.Width = 59;
            // 
            // m_brakeParamUnitsColumn
            // 
            this.m_brakeParamUnitsColumn.HeaderText = "Units";
            this.m_brakeParamUnitsColumn.Name = "m_brakeParamUnitsColumn";
            this.m_brakeParamUnitsColumn.ReadOnly = true;
            this.m_brakeParamUnitsColumn.Width = 56;
            // 
            // m_transShiftpointsLabel
            // 
            this.m_transShiftpointsLabel.AutoSize = true;
            this.m_transShiftpointsLabel.Font = new System.Drawing.Font("Microsoft Sans Serif", 16F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.m_transShiftpointsLabel.Location = new System.Drawing.Point(362, 252);
            this.m_transShiftpointsLabel.Name = "m_transShiftpointsLabel";
            this.m_transShiftpointsLabel.Size = new System.Drawing.Size(264, 26);
            this.m_transShiftpointsLabel.TabIndex = 6;
            this.m_transShiftpointsLabel.Text = "Transmission Shift Points:";
            // 
            // m_transmissionDataGridView
            // 
            this.m_transmissionDataGridView.AllowUserToAddRows = false;
            this.m_transmissionDataGridView.AllowUserToDeleteRows = false;
            this.m_transmissionDataGridView.AllowUserToResizeColumns = false;
            this.m_transmissionDataGridView.AllowUserToResizeRows = false;
            this.m_transmissionDataGridView.AutoSizeColumnsMode = System.Windows.Forms.DataGridViewAutoSizeColumnsMode.DisplayedCells;
            this.m_transmissionDataGridView.BackgroundColor = System.Drawing.SystemColors.Control;
            this.m_transmissionDataGridView.BorderStyle = System.Windows.Forms.BorderStyle.None;
            this.m_transmissionDataGridView.ColumnHeadersHeightSizeMode = System.Windows.Forms.DataGridViewColumnHeadersHeightSizeMode.AutoSize;
            this.m_transmissionDataGridView.Columns.AddRange(new System.Windows.Forms.DataGridViewColumn[] {
            this.m_transParamNameColumn,
            this.m_transEngineTypeColumn,
            this.m_transParamValueMph,
            this.m_transParamValueKph});
            this.m_transmissionDataGridView.Location = new System.Drawing.Point(367, 281);
            this.m_transmissionDataGridView.Name = "m_transmissionDataGridView";
            this.m_transmissionDataGridView.RowHeadersVisible = false;
            this.m_transmissionDataGridView.Size = new System.Drawing.Size(365, 236);
            this.m_transmissionDataGridView.TabIndex = 7;
            this.m_transmissionDataGridView.CellEndEdit += new System.Windows.Forms.DataGridViewCellEventHandler(this.m_transmissionDataGridView_CellEndEdit);
            // 
            // m_transParamNameColumn
            // 
            this.m_transParamNameColumn.HeaderText = "Shift Point";
            this.m_transParamNameColumn.Name = "m_transParamNameColumn";
            this.m_transParamNameColumn.ReadOnly = true;
            this.m_transParamNameColumn.Width = 80;
            // 
            // m_transEngineTypeColumn
            // 
            this.m_transEngineTypeColumn.HeaderText = "Engine Type:";
            this.m_transEngineTypeColumn.Name = "m_transEngineTypeColumn";
            this.m_transEngineTypeColumn.ReadOnly = true;
            this.m_transEngineTypeColumn.Width = 95;
            // 
            // m_transParamValueMph
            // 
            this.m_transParamValueMph.HeaderText = "MPH";
            this.m_transParamValueMph.Name = "m_transParamValueMph";
            this.m_transParamValueMph.Width = 56;
            // 
            // m_transParamValueKph
            // 
            this.m_transParamValueKph.HeaderText = "KPH";
            this.m_transParamValueKph.Name = "m_transParamValueKph";
            this.m_transParamValueKph.Width = 54;
            // 
            // m_speedTargetLabel
            // 
            this.m_speedTargetLabel.AutoSize = true;
            this.m_speedTargetLabel.Font = new System.Drawing.Font("Microsoft Sans Serif", 16F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.m_speedTargetLabel.Location = new System.Drawing.Point(366, 75);
            this.m_speedTargetLabel.Name = "m_speedTargetLabel";
            this.m_speedTargetLabel.Size = new System.Drawing.Size(159, 26);
            this.m_speedTargetLabel.TabIndex = 8;
            this.m_speedTargetLabel.Text = "Speed Targets:";
            // 
            // m_speedTargetDataGridView
            // 
            this.m_speedTargetDataGridView.AllowUserToAddRows = false;
            this.m_speedTargetDataGridView.AllowUserToDeleteRows = false;
            this.m_speedTargetDataGridView.AllowUserToResizeColumns = false;
            this.m_speedTargetDataGridView.AllowUserToResizeRows = false;
            this.m_speedTargetDataGridView.AutoSizeColumnsMode = System.Windows.Forms.DataGridViewAutoSizeColumnsMode.DisplayedCells;
            this.m_speedTargetDataGridView.BackgroundColor = System.Drawing.SystemColors.Control;
            this.m_speedTargetDataGridView.BorderStyle = System.Windows.Forms.BorderStyle.None;
            this.m_speedTargetDataGridView.ColumnHeadersHeightSizeMode = System.Windows.Forms.DataGridViewColumnHeadersHeightSizeMode.AutoSize;
            this.m_speedTargetDataGridView.Columns.AddRange(new System.Windows.Forms.DataGridViewColumn[] {
            this.m_speedParamNameColumn,
            this.m_speedParamValueColumn,
            this.m_speedParamUnitsColumn});
            this.m_speedTargetDataGridView.Location = new System.Drawing.Point(371, 104);
            this.m_speedTargetDataGridView.Name = "m_speedTargetDataGridView";
            this.m_speedTargetDataGridView.RowHeadersVisible = false;
            this.m_speedTargetDataGridView.Size = new System.Drawing.Size(329, 145);
            this.m_speedTargetDataGridView.TabIndex = 9;
            this.m_speedTargetDataGridView.CellEndEdit += new System.Windows.Forms.DataGridViewCellEventHandler(this.m_speedTargetDataGridView_CellEndEdit);
            // 
            // m_speedParamNameColumn
            // 
            this.m_speedParamNameColumn.HeaderText = "Parameter Name:";
            this.m_speedParamNameColumn.Name = "m_speedParamNameColumn";
            this.m_speedParamNameColumn.ReadOnly = true;
            this.m_speedParamNameColumn.Width = 105;
            // 
            // m_speedParamValueColumn
            // 
            this.m_speedParamValueColumn.HeaderText = "Value";
            this.m_speedParamValueColumn.Name = "m_speedParamValueColumn";
            this.m_speedParamValueColumn.Width = 59;
            // 
            // m_speedParamUnitsColumn
            // 
            this.m_speedParamUnitsColumn.HeaderText = "Units";
            this.m_speedParamUnitsColumn.Name = "m_speedParamUnitsColumn";
            this.m_speedParamUnitsColumn.ReadOnly = true;
            this.m_speedParamUnitsColumn.Width = 56;
            // 
            // m_vehicleLogoPictureBox
            // 
            this.m_vehicleLogoPictureBox.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.m_vehicleLogoPictureBox.InitialImage = ((System.Drawing.Image)(resources.GetObject("m_vehicleLogoPictureBox.InitialImage")));
            this.m_vehicleLogoPictureBox.Location = new System.Drawing.Point(13, 27);
            this.m_vehicleLogoPictureBox.Name = "m_vehicleLogoPictureBox";
            this.m_vehicleLogoPictureBox.Size = new System.Drawing.Size(151, 44);
            this.m_vehicleLogoPictureBox.SizeMode = System.Windows.Forms.PictureBoxSizeMode.StretchImage;
            this.m_vehicleLogoPictureBox.TabIndex = 10;
            this.m_vehicleLogoPictureBox.TabStop = false;
            // 
            // ToyotaParameterEditorMainWindow
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(732, 574);
            this.Controls.Add(this.m_vehicleLogoPictureBox);
            this.Controls.Add(this.m_speedTargetDataGridView);
            this.Controls.Add(this.m_speedTargetLabel);
            this.Controls.Add(this.m_transmissionDataGridView);
            this.Controls.Add(this.m_transShiftpointsLabel);
            this.Controls.Add(this.m_brakeDataGridView);
            this.Controls.Add(this.m_brakeTestingParamsLabel);
            this.Controls.Add(this.m_vehicleDataGridView);
            this.Controls.Add(this.m_vehicleDataLabel);
            this.Controls.Add(this.m_vehicleTypePictureBox);
            this.Controls.Add(this.m_mainMenuStrip);
            this.MainMenuStrip = this.m_mainMenuStrip;
            this.Name = "ToyotaParameterEditorMainWindow";
            this.Text = "Toyota Parameter Editor";
            this.m_mainMenuStrip.ResumeLayout(false);
            this.m_mainMenuStrip.PerformLayout();
            ((System.ComponentModel.ISupportInitialize)(this.m_vehicleTypePictureBox)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.m_vehicleDataGridView)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.m_brakeDataGridView)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.m_transmissionDataGridView)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.m_speedTargetDataGridView)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.m_vehicleLogoPictureBox)).EndInit();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.MenuStrip m_mainMenuStrip;
        private System.Windows.Forms.ToolStripMenuItem fileToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem openVehicleToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem newVehicleToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem exitToolStripMenuItem;
        private System.Windows.Forms.PictureBox m_vehicleTypePictureBox;
        private System.Windows.Forms.Label m_vehicleDataLabel;
        private System.Windows.Forms.DataGridView m_vehicleDataGridView;
        private System.Windows.Forms.DataGridViewTextBoxColumn m_parameterNameColumn;
        private System.Windows.Forms.DataGridViewTextBoxColumn m_paramValueColumn;
        private System.Windows.Forms.DataGridViewTextBoxColumn m_paramUnitsColumn;
        private System.Windows.Forms.Label m_brakeTestingParamsLabel;
        private System.Windows.Forms.DataGridView m_brakeDataGridView;
        private System.Windows.Forms.Label m_transShiftpointsLabel;
        private System.Windows.Forms.DataGridView m_transmissionDataGridView;
        private System.Windows.Forms.DataGridViewTextBoxColumn m_transParamNameColumn;
        private System.Windows.Forms.DataGridViewTextBoxColumn m_transEngineTypeColumn;
        private System.Windows.Forms.DataGridViewTextBoxColumn m_transParamValueMph;
        private System.Windows.Forms.DataGridViewTextBoxColumn m_transParamValueKph;
        private System.Windows.Forms.DataGridViewTextBoxColumn m_brakeParamNameColumn;
        private System.Windows.Forms.DataGridViewTextBoxColumn m_brakeParamValueColumn;
        private System.Windows.Forms.DataGridViewTextBoxColumn m_brakeParamUnitsColumn;
        private System.Windows.Forms.Label m_speedTargetLabel;
        private System.Windows.Forms.DataGridView m_speedTargetDataGridView;
        private System.Windows.Forms.DataGridViewTextBoxColumn m_speedParamNameColumn;
        private System.Windows.Forms.DataGridViewTextBoxColumn m_speedParamValueColumn;
        private System.Windows.Forms.DataGridViewTextBoxColumn m_speedParamUnitsColumn;
        private System.Windows.Forms.PictureBox m_vehicleLogoPictureBox;
        private System.Windows.Forms.ToolStripMenuItem saveUpdatesToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem exportUpdatedFilesToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem importUpdatedFilesToolStripMenuItem;
    }
}

