namespace CcrtDataBaseLibrary
{
    partial class DatabaseViewerForm
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
            this.m_portSettingsLabel = new System.Windows.Forms.Label();
            this.m_portSettingsDataGridView = new System.Windows.Forms.DataGridView();
            this.m_closeButton = new System.Windows.Forms.Button();
            this.m_dataItemsLabel = new System.Windows.Forms.Label();
            this.m_diDataGridView = new System.Windows.Forms.DataGridView();
            this.DataItem = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.MinLength = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.MaxLength = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.ServerNameCol = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.DeviceNameCol = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.DeviceType = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.BaudRateCol = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.ParityCol = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.DataBitsCol = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.StopBitsCol = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.SerialPortCol = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.RxtimeoutCol = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.TxTimeoutCol = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.MonitorDataCol = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.HeaderData = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.HeaderLength = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.TrailerData = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.TrailerLength = new System.Windows.Forms.DataGridViewTextBoxColumn();
            ((System.ComponentModel.ISupportInitialize)(this.m_portSettingsDataGridView)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.m_diDataGridView)).BeginInit();
            this.SuspendLayout();
            // 
            // m_portSettingsLabel
            // 
            this.m_portSettingsLabel.AutoSize = true;
            this.m_portSettingsLabel.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.m_portSettingsLabel.Location = new System.Drawing.Point(12, 9);
            this.m_portSettingsLabel.Name = "m_portSettingsLabel";
            this.m_portSettingsLabel.Size = new System.Drawing.Size(84, 13);
            this.m_portSettingsLabel.TabIndex = 0;
            this.m_portSettingsLabel.Text = "Port Settings:";
            // 
            // m_portSettingsDataGridView
            // 
            this.m_portSettingsDataGridView.AllowUserToAddRows = false;
            this.m_portSettingsDataGridView.AllowUserToDeleteRows = false;
            this.m_portSettingsDataGridView.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.m_portSettingsDataGridView.ColumnHeadersHeightSizeMode = System.Windows.Forms.DataGridViewColumnHeadersHeightSizeMode.AutoSize;
            this.m_portSettingsDataGridView.Columns.AddRange(new System.Windows.Forms.DataGridViewColumn[] {
            this.ServerNameCol,
            this.DeviceNameCol,
            this.DeviceType,
            this.BaudRateCol,
            this.ParityCol,
            this.DataBitsCol,
            this.StopBitsCol,
            this.SerialPortCol,
            this.RxtimeoutCol,
            this.TxTimeoutCol,
            this.MonitorDataCol,
            this.HeaderData,
            this.HeaderLength,
            this.TrailerData,
            this.TrailerLength});
            this.m_portSettingsDataGridView.Location = new System.Drawing.Point(15, 25);
            this.m_portSettingsDataGridView.Name = "m_portSettingsDataGridView";
            this.m_portSettingsDataGridView.ReadOnly = true;
            this.m_portSettingsDataGridView.RowHeadersVisible = false;
            this.m_portSettingsDataGridView.Size = new System.Drawing.Size(1403, 187);
            this.m_portSettingsDataGridView.TabIndex = 1;
            // 
            // m_closeButton
            // 
            this.m_closeButton.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this.m_closeButton.Location = new System.Drawing.Point(1343, 454);
            this.m_closeButton.Name = "m_closeButton";
            this.m_closeButton.Size = new System.Drawing.Size(75, 23);
            this.m_closeButton.TabIndex = 2;
            this.m_closeButton.Text = "Close";
            this.m_closeButton.UseVisualStyleBackColor = true;
            this.m_closeButton.Click += new System.EventHandler(this.m_closeButton_Click);
            // 
            // m_dataItemsLabel
            // 
            this.m_dataItemsLabel.AutoSize = true;
            this.m_dataItemsLabel.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.m_dataItemsLabel.Location = new System.Drawing.Point(12, 215);
            this.m_dataItemsLabel.Name = "m_dataItemsLabel";
            this.m_dataItemsLabel.Size = new System.Drawing.Size(72, 13);
            this.m_dataItemsLabel.TabIndex = 3;
            this.m_dataItemsLabel.Text = "Data Items:";
            // 
            // m_diDataGridView
            // 
            this.m_diDataGridView.AllowUserToAddRows = false;
            this.m_diDataGridView.AllowUserToDeleteRows = false;
            this.m_diDataGridView.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.m_diDataGridView.ColumnHeadersHeightSizeMode = System.Windows.Forms.DataGridViewColumnHeadersHeightSizeMode.AutoSize;
            this.m_diDataGridView.Columns.AddRange(new System.Windows.Forms.DataGridViewColumn[] {
            this.DataItem,
            this.MinLength,
            this.MaxLength});
            this.m_diDataGridView.Location = new System.Drawing.Point(12, 231);
            this.m_diDataGridView.Name = "m_diDataGridView";
            this.m_diDataGridView.ReadOnly = true;
            this.m_diDataGridView.RowHeadersVisible = false;
            this.m_diDataGridView.Size = new System.Drawing.Size(1203, 221);
            this.m_diDataGridView.TabIndex = 4;
            // 
            // DataItem
            // 
            this.DataItem.HeaderText = "Data Item";
            this.DataItem.Name = "DataItem";
            this.DataItem.ReadOnly = true;
            // 
            // MinLength
            // 
            this.MinLength.HeaderText = "Minimum Length";
            this.MinLength.Name = "MinLength";
            this.MinLength.ReadOnly = true;
            // 
            // MaxLength
            // 
            this.MaxLength.HeaderText = "MaximumLength";
            this.MaxLength.Name = "MaxLength";
            this.MaxLength.ReadOnly = true;
            // 
            // ServerNameCol
            // 
            this.ServerNameCol.HeaderText = "Server Name";
            this.ServerNameCol.Name = "ServerNameCol";
            this.ServerNameCol.ReadOnly = true;
            // 
            // DeviceNameCol
            // 
            this.DeviceNameCol.HeaderText = "Device Name";
            this.DeviceNameCol.Name = "DeviceNameCol";
            this.DeviceNameCol.ReadOnly = true;
            // 
            // DeviceType
            // 
            this.DeviceType.HeaderText = "Device Type";
            this.DeviceType.Name = "DeviceType";
            this.DeviceType.ReadOnly = true;
            // 
            // BaudRateCol
            // 
            this.BaudRateCol.HeaderText = "Baud Rate";
            this.BaudRateCol.Name = "BaudRateCol";
            this.BaudRateCol.ReadOnly = true;
            // 
            // ParityCol
            // 
            this.ParityCol.HeaderText = "Parity";
            this.ParityCol.Name = "ParityCol";
            this.ParityCol.ReadOnly = true;
            // 
            // DataBitsCol
            // 
            this.DataBitsCol.HeaderText = "Data Bits";
            this.DataBitsCol.Name = "DataBitsCol";
            this.DataBitsCol.ReadOnly = true;
            // 
            // StopBitsCol
            // 
            this.StopBitsCol.HeaderText = "Stop Bits";
            this.StopBitsCol.Name = "StopBitsCol";
            this.StopBitsCol.ReadOnly = true;
            // 
            // SerialPortCol
            // 
            this.SerialPortCol.HeaderText = "Serial Port";
            this.SerialPortCol.Name = "SerialPortCol";
            this.SerialPortCol.ReadOnly = true;
            // 
            // RxtimeoutCol
            // 
            this.RxtimeoutCol.HeaderText = "Rx Timeout";
            this.RxtimeoutCol.Name = "RxtimeoutCol";
            this.RxtimeoutCol.ReadOnly = true;
            // 
            // TxTimeoutCol
            // 
            this.TxTimeoutCol.HeaderText = "Tx Timeout";
            this.TxTimeoutCol.Name = "TxTimeoutCol";
            this.TxTimeoutCol.ReadOnly = true;
            // 
            // MonitorDataCol
            // 
            this.MonitorDataCol.HeaderText = "Monitor Data";
            this.MonitorDataCol.Name = "MonitorDataCol";
            this.MonitorDataCol.ReadOnly = true;
            this.MonitorDataCol.Resizable = System.Windows.Forms.DataGridViewTriState.True;
            this.MonitorDataCol.SortMode = System.Windows.Forms.DataGridViewColumnSortMode.NotSortable;
            // 
            // HeaderData
            // 
            this.HeaderData.HeaderText = "Header Data";
            this.HeaderData.Name = "HeaderData";
            this.HeaderData.ReadOnly = true;
            // 
            // HeaderLength
            // 
            this.HeaderLength.HeaderText = "HeaderLength";
            this.HeaderLength.Name = "HeaderLength";
            this.HeaderLength.ReadOnly = true;
            // 
            // TrailerData
            // 
            this.TrailerData.HeaderText = "Trailer Data";
            this.TrailerData.Name = "TrailerData";
            this.TrailerData.ReadOnly = true;
            // 
            // TrailerLength
            // 
            this.TrailerLength.HeaderText = "Trailer Length";
            this.TrailerLength.Name = "TrailerLength";
            this.TrailerLength.ReadOnly = true;
            // 
            // DatabaseViewerForm
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.BackColor = System.Drawing.Color.LightBlue;
            this.ClientSize = new System.Drawing.Size(1430, 487);
            this.ControlBox = false;
            this.Controls.Add(this.m_diDataGridView);
            this.Controls.Add(this.m_dataItemsLabel);
            this.Controls.Add(this.m_closeButton);
            this.Controls.Add(this.m_portSettingsDataGridView);
            this.Controls.Add(this.m_portSettingsLabel);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedToolWindow;
            this.Name = "DatabaseViewerForm";
            this.Text = "Database Viewer";
            ((System.ComponentModel.ISupportInitialize)(this.m_portSettingsDataGridView)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.m_diDataGridView)).EndInit();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Label m_portSettingsLabel;
        private System.Windows.Forms.DataGridView m_portSettingsDataGridView;
        private System.Windows.Forms.Button m_closeButton;
        private System.Windows.Forms.Label m_dataItemsLabel;
        private System.Windows.Forms.DataGridView m_diDataGridView;
        private System.Windows.Forms.DataGridViewTextBoxColumn DataItem;
        private System.Windows.Forms.DataGridViewTextBoxColumn MinLength;
        private System.Windows.Forms.DataGridViewTextBoxColumn MaxLength;
        private System.Windows.Forms.DataGridViewTextBoxColumn ServerNameCol;
        private System.Windows.Forms.DataGridViewTextBoxColumn DeviceNameCol;
        private System.Windows.Forms.DataGridViewTextBoxColumn DeviceType;
        private System.Windows.Forms.DataGridViewTextBoxColumn BaudRateCol;
        private System.Windows.Forms.DataGridViewTextBoxColumn ParityCol;
        private System.Windows.Forms.DataGridViewTextBoxColumn DataBitsCol;
        private System.Windows.Forms.DataGridViewTextBoxColumn StopBitsCol;
        private System.Windows.Forms.DataGridViewTextBoxColumn SerialPortCol;
        private System.Windows.Forms.DataGridViewTextBoxColumn RxtimeoutCol;
        private System.Windows.Forms.DataGridViewTextBoxColumn TxTimeoutCol;
        private System.Windows.Forms.DataGridViewTextBoxColumn MonitorDataCol;
        private System.Windows.Forms.DataGridViewTextBoxColumn HeaderData;
        private System.Windows.Forms.DataGridViewTextBoxColumn HeaderLength;
        private System.Windows.Forms.DataGridViewTextBoxColumn TrailerData;
        private System.Windows.Forms.DataGridViewTextBoxColumn TrailerLength;
    }
}