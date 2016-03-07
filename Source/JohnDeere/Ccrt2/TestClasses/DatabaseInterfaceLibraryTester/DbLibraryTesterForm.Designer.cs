namespace DatabaseInterfaceLibraryTester
{
    partial class DbLibraryTesterForm
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
            this.m_deviceNameLabel = new System.Windows.Forms.Label();
            this.m_devNameTextBox = new System.Windows.Forms.TextBox();
            this.m_serialPortLabel = new System.Windows.Forms.Label();
            this.m_serialPortComboBox = new System.Windows.Forms.ComboBox();
            this.m_baudRateLabel = new System.Windows.Forms.Label();
            this.m_baudRateTextBox = new System.Windows.Forms.TextBox();
            this.m_parityLabel = new System.Windows.Forms.Label();
            this.m_parityComboBox = new System.Windows.Forms.ComboBox();
            this.m_dataBitsLabel = new System.Windows.Forms.Label();
            this.m_dataBitsTextBox = new System.Windows.Forms.TextBox();
            this.m_stopBitsLabel = new System.Windows.Forms.Label();
            this.m_stopBitsComboBox = new System.Windows.Forms.ComboBox();
            this.m_monitorDataCheckBox = new System.Windows.Forms.CheckBox();
            this.m_rxTimeoutLabel = new System.Windows.Forms.Label();
            this.m_rxTimeoutTextBox = new System.Windows.Forms.TextBox();
            this.m_txTimeoutLabel = new System.Windows.Forms.Label();
            this.m_txTimeoutTextBox = new System.Windows.Forms.TextBox();
            this.m_addEntryButton = new System.Windows.Forms.Button();
            this.m_currentPortSettingsLabel = new System.Windows.Forms.Label();
            this.m_portSettingsDataGridView = new System.Windows.Forms.DataGridView();
            this.DeviceName = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.PortName = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.BaudRate = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.ParitySetting = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.DataBits = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.StopBitSetting = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.RxTimeout = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.TxTimeout = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.MonitorData = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.m_updatePortSettingsButton = new System.Windows.Forms.Button();
            this.m_removePortButton = new System.Windows.Forms.Button();
            this.m_displayViewerButton = new System.Windows.Forms.Button();
            this.m_currentDataSettingsLabel = new System.Windows.Forms.Label();
            this.m_diDataGridView = new System.Windows.Forms.DataGridView();
            this.m_dataItemLabel = new System.Windows.Forms.Label();
            this.m_minLengthLabel = new System.Windows.Forms.Label();
            this.m_maxLengthLabel = new System.Windows.Forms.Label();
            this.m_dataItemTextBox = new System.Windows.Forms.TextBox();
            this.m_minLengthTextBox = new System.Windows.Forms.TextBox();
            this.m_maxLengthTextBox = new System.Windows.Forms.TextBox();
            this.m_headerDataLabel = new System.Windows.Forms.Label();
            this.m_headerLengthLabel = new System.Windows.Forms.Label();
            this.m_headerDataTextBox = new System.Windows.Forms.TextBox();
            this.m_headerLengthTextBox = new System.Windows.Forms.TextBox();
            this.m_trailerDataLabel = new System.Windows.Forms.Label();
            this.m_trailerLengthLabel = new System.Windows.Forms.Label();
            this.m_trailerDataTextBox = new System.Windows.Forms.TextBox();
            this.m_trailerDataLengthTextBox = new System.Windows.Forms.TextBox();
            this.m_addDataItemButton = new System.Windows.Forms.Button();
            this.m_updateDataItemButton = new System.Windows.Forms.Button();
            this.m_removeDataItemButton = new System.Windows.Forms.Button();
            this.DataItem = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.MinLength = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.MaxLength = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.HeaderData = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.HeaderLength = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.TrailerData = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.TrailerLength = new System.Windows.Forms.DataGridViewTextBoxColumn();
            ((System.ComponentModel.ISupportInitialize)(this.m_portSettingsDataGridView)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.m_diDataGridView)).BeginInit();
            this.SuspendLayout();
            // 
            // m_deviceNameLabel
            // 
            this.m_deviceNameLabel.AutoSize = true;
            this.m_deviceNameLabel.Location = new System.Drawing.Point(9, 15);
            this.m_deviceNameLabel.Name = "m_deviceNameLabel";
            this.m_deviceNameLabel.Size = new System.Drawing.Size(75, 13);
            this.m_deviceNameLabel.TabIndex = 2;
            this.m_deviceNameLabel.Text = "Device Name:";
            // 
            // m_devNameTextBox
            // 
            this.m_devNameTextBox.Location = new System.Drawing.Point(90, 12);
            this.m_devNameTextBox.Name = "m_devNameTextBox";
            this.m_devNameTextBox.Size = new System.Drawing.Size(201, 20);
            this.m_devNameTextBox.TabIndex = 3;
            // 
            // m_serialPortLabel
            // 
            this.m_serialPortLabel.AutoSize = true;
            this.m_serialPortLabel.Location = new System.Drawing.Point(26, 41);
            this.m_serialPortLabel.Name = "m_serialPortLabel";
            this.m_serialPortLabel.Size = new System.Drawing.Size(58, 13);
            this.m_serialPortLabel.TabIndex = 4;
            this.m_serialPortLabel.Text = "Serial Port:";
            // 
            // m_serialPortComboBox
            // 
            this.m_serialPortComboBox.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.m_serialPortComboBox.FormattingEnabled = true;
            this.m_serialPortComboBox.Location = new System.Drawing.Point(90, 38);
            this.m_serialPortComboBox.Name = "m_serialPortComboBox";
            this.m_serialPortComboBox.Size = new System.Drawing.Size(78, 21);
            this.m_serialPortComboBox.TabIndex = 5;
            // 
            // m_baudRateLabel
            // 
            this.m_baudRateLabel.AutoSize = true;
            this.m_baudRateLabel.Location = new System.Drawing.Point(23, 68);
            this.m_baudRateLabel.Name = "m_baudRateLabel";
            this.m_baudRateLabel.Size = new System.Drawing.Size(61, 13);
            this.m_baudRateLabel.TabIndex = 6;
            this.m_baudRateLabel.Text = "Baud Rate:";
            // 
            // m_baudRateTextBox
            // 
            this.m_baudRateTextBox.Location = new System.Drawing.Point(90, 65);
            this.m_baudRateTextBox.Name = "m_baudRateTextBox";
            this.m_baudRateTextBox.Size = new System.Drawing.Size(78, 20);
            this.m_baudRateTextBox.TabIndex = 7;
            // 
            // m_parityLabel
            // 
            this.m_parityLabel.AutoSize = true;
            this.m_parityLabel.Location = new System.Drawing.Point(215, 41);
            this.m_parityLabel.Name = "m_parityLabel";
            this.m_parityLabel.Size = new System.Drawing.Size(36, 13);
            this.m_parityLabel.TabIndex = 8;
            this.m_parityLabel.Text = "Parity:";
            // 
            // m_parityComboBox
            // 
            this.m_parityComboBox.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.m_parityComboBox.FormattingEnabled = true;
            this.m_parityComboBox.Items.AddRange(new object[] {
            "Even",
            "None",
            "Odd"});
            this.m_parityComboBox.Location = new System.Drawing.Point(257, 38);
            this.m_parityComboBox.Name = "m_parityComboBox";
            this.m_parityComboBox.Size = new System.Drawing.Size(76, 21);
            this.m_parityComboBox.TabIndex = 9;
            // 
            // m_dataBitsLabel
            // 
            this.m_dataBitsLabel.AutoSize = true;
            this.m_dataBitsLabel.Location = new System.Drawing.Point(198, 68);
            this.m_dataBitsLabel.Name = "m_dataBitsLabel";
            this.m_dataBitsLabel.Size = new System.Drawing.Size(53, 13);
            this.m_dataBitsLabel.TabIndex = 10;
            this.m_dataBitsLabel.Text = "Data Bits:";
            // 
            // m_dataBitsTextBox
            // 
            this.m_dataBitsTextBox.Location = new System.Drawing.Point(257, 65);
            this.m_dataBitsTextBox.Name = "m_dataBitsTextBox";
            this.m_dataBitsTextBox.Size = new System.Drawing.Size(76, 20);
            this.m_dataBitsTextBox.TabIndex = 11;
            // 
            // m_stopBitsLabel
            // 
            this.m_stopBitsLabel.AutoSize = true;
            this.m_stopBitsLabel.Location = new System.Drawing.Point(372, 41);
            this.m_stopBitsLabel.Name = "m_stopBitsLabel";
            this.m_stopBitsLabel.Size = new System.Drawing.Size(52, 13);
            this.m_stopBitsLabel.TabIndex = 12;
            this.m_stopBitsLabel.Text = "Stop Bits:";
            // 
            // m_stopBitsComboBox
            // 
            this.m_stopBitsComboBox.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.m_stopBitsComboBox.FormattingEnabled = true;
            this.m_stopBitsComboBox.Items.AddRange(new object[] {
            "0",
            "1",
            "1.5",
            "2"});
            this.m_stopBitsComboBox.Location = new System.Drawing.Point(430, 38);
            this.m_stopBitsComboBox.Name = "m_stopBitsComboBox";
            this.m_stopBitsComboBox.Size = new System.Drawing.Size(60, 21);
            this.m_stopBitsComboBox.TabIndex = 13;
            // 
            // m_monitorDataCheckBox
            // 
            this.m_monitorDataCheckBox.AutoSize = true;
            this.m_monitorDataCheckBox.CheckAlign = System.Drawing.ContentAlignment.MiddleRight;
            this.m_monitorDataCheckBox.Location = new System.Drawing.Point(375, 67);
            this.m_monitorDataCheckBox.Name = "m_monitorDataCheckBox";
            this.m_monitorDataCheckBox.Size = new System.Drawing.Size(87, 17);
            this.m_monitorDataCheckBox.TabIndex = 14;
            this.m_monitorDataCheckBox.Text = "Monitor Data";
            this.m_monitorDataCheckBox.UseVisualStyleBackColor = true;
            // 
            // m_rxTimeoutLabel
            // 
            this.m_rxTimeoutLabel.AutoSize = true;
            this.m_rxTimeoutLabel.Location = new System.Drawing.Point(20, 107);
            this.m_rxTimeoutLabel.Name = "m_rxTimeoutLabel";
            this.m_rxTimeoutLabel.Size = new System.Drawing.Size(64, 13);
            this.m_rxTimeoutLabel.TabIndex = 15;
            this.m_rxTimeoutLabel.Text = "Rx Timeout:";
            // 
            // m_rxTimeoutTextBox
            // 
            this.m_rxTimeoutTextBox.Location = new System.Drawing.Point(90, 104);
            this.m_rxTimeoutTextBox.Name = "m_rxTimeoutTextBox";
            this.m_rxTimeoutTextBox.Size = new System.Drawing.Size(63, 20);
            this.m_rxTimeoutTextBox.TabIndex = 16;
            // 
            // m_txTimeoutLabel
            // 
            this.m_txTimeoutLabel.AutoSize = true;
            this.m_txTimeoutLabel.Location = new System.Drawing.Point(21, 134);
            this.m_txTimeoutLabel.Name = "m_txTimeoutLabel";
            this.m_txTimeoutLabel.Size = new System.Drawing.Size(63, 13);
            this.m_txTimeoutLabel.TabIndex = 17;
            this.m_txTimeoutLabel.Text = "Tx Timeout:";
            // 
            // m_txTimeoutTextBox
            // 
            this.m_txTimeoutTextBox.Location = new System.Drawing.Point(90, 131);
            this.m_txTimeoutTextBox.Name = "m_txTimeoutTextBox";
            this.m_txTimeoutTextBox.Size = new System.Drawing.Size(63, 20);
            this.m_txTimeoutTextBox.TabIndex = 18;
            // 
            // m_addEntryButton
            // 
            this.m_addEntryButton.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.m_addEntryButton.Location = new System.Drawing.Point(1188, 64);
            this.m_addEntryButton.Name = "m_addEntryButton";
            this.m_addEntryButton.Size = new System.Drawing.Size(127, 22);
            this.m_addEntryButton.TabIndex = 19;
            this.m_addEntryButton.Text = "Add Port Setting";
            this.m_addEntryButton.UseVisualStyleBackColor = true;
            this.m_addEntryButton.Click += new System.EventHandler(this.m_addEntryButton_Click);
            // 
            // m_currentPortSettingsLabel
            // 
            this.m_currentPortSettingsLabel.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)));
            this.m_currentPortSettingsLabel.AutoSize = true;
            this.m_currentPortSettingsLabel.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.m_currentPortSettingsLabel.Location = new System.Drawing.Point(9, 186);
            this.m_currentPortSettingsLabel.Name = "m_currentPortSettingsLabel";
            this.m_currentPortSettingsLabel.Size = new System.Drawing.Size(129, 13);
            this.m_currentPortSettingsLabel.TabIndex = 20;
            this.m_currentPortSettingsLabel.Text = "Current Port Settings:";
            // 
            // m_portSettingsDataGridView
            // 
            this.m_portSettingsDataGridView.AllowUserToAddRows = false;
            this.m_portSettingsDataGridView.AllowUserToDeleteRows = false;
            this.m_portSettingsDataGridView.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.m_portSettingsDataGridView.ColumnHeadersHeightSizeMode = System.Windows.Forms.DataGridViewColumnHeadersHeightSizeMode.AutoSize;
            this.m_portSettingsDataGridView.Columns.AddRange(new System.Windows.Forms.DataGridViewColumn[] {
            this.DeviceName,
            this.PortName,
            this.BaudRate,
            this.ParitySetting,
            this.DataBits,
            this.StopBitSetting,
            this.RxTimeout,
            this.TxTimeout,
            this.MonitorData,
            this.HeaderData,
            this.HeaderLength,
            this.TrailerData,
            this.TrailerLength});
            this.m_portSettingsDataGridView.Location = new System.Drawing.Point(9, 202);
            this.m_portSettingsDataGridView.Name = "m_portSettingsDataGridView";
            this.m_portSettingsDataGridView.ReadOnly = true;
            this.m_portSettingsDataGridView.RowHeadersVisible = false;
            this.m_portSettingsDataGridView.Size = new System.Drawing.Size(1306, 253);
            this.m_portSettingsDataGridView.TabIndex = 21;
            // 
            // DeviceName
            // 
            this.DeviceName.HeaderText = "Device Name";
            this.DeviceName.Name = "DeviceName";
            this.DeviceName.ReadOnly = true;
            // 
            // PortName
            // 
            this.PortName.HeaderText = "Serial Port";
            this.PortName.Name = "PortName";
            this.PortName.ReadOnly = true;
            // 
            // BaudRate
            // 
            this.BaudRate.HeaderText = "Baud Rate";
            this.BaudRate.Name = "BaudRate";
            this.BaudRate.ReadOnly = true;
            // 
            // ParitySetting
            // 
            this.ParitySetting.HeaderText = "Parity";
            this.ParitySetting.Name = "ParitySetting";
            this.ParitySetting.ReadOnly = true;
            // 
            // DataBits
            // 
            this.DataBits.HeaderText = "Data Bits";
            this.DataBits.Name = "DataBits";
            this.DataBits.ReadOnly = true;
            // 
            // StopBitSetting
            // 
            this.StopBitSetting.HeaderText = "Stop Bits";
            this.StopBitSetting.Name = "StopBitSetting";
            this.StopBitSetting.ReadOnly = true;
            // 
            // RxTimeout
            // 
            this.RxTimeout.HeaderText = "Rx Timeout";
            this.RxTimeout.Name = "RxTimeout";
            this.RxTimeout.ReadOnly = true;
            // 
            // TxTimeout
            // 
            this.TxTimeout.HeaderText = "Tx Timeout";
            this.TxTimeout.Name = "TxTimeout";
            this.TxTimeout.ReadOnly = true;
            // 
            // MonitorData
            // 
            this.MonitorData.HeaderText = "Monitor Data";
            this.MonitorData.Name = "MonitorData";
            this.MonitorData.ReadOnly = true;
            // 
            // m_updatePortSettingsButton
            // 
            this.m_updatePortSettingsButton.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.m_updatePortSettingsButton.Location = new System.Drawing.Point(1188, 92);
            this.m_updatePortSettingsButton.Name = "m_updatePortSettingsButton";
            this.m_updatePortSettingsButton.Size = new System.Drawing.Size(127, 22);
            this.m_updatePortSettingsButton.TabIndex = 22;
            this.m_updatePortSettingsButton.Text = "Update Port Settings";
            this.m_updatePortSettingsButton.UseVisualStyleBackColor = true;
            this.m_updatePortSettingsButton.Click += new System.EventHandler(this.m_updatePortSettingsButton_Click);
            // 
            // m_removePortButton
            // 
            this.m_removePortButton.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.m_removePortButton.Location = new System.Drawing.Point(1188, 120);
            this.m_removePortButton.Name = "m_removePortButton";
            this.m_removePortButton.Size = new System.Drawing.Size(127, 22);
            this.m_removePortButton.TabIndex = 23;
            this.m_removePortButton.Text = "Remove Port Settings";
            this.m_removePortButton.UseVisualStyleBackColor = true;
            this.m_removePortButton.Click += new System.EventHandler(this.m_removePortButton_Click);
            // 
            // m_displayViewerButton
            // 
            this.m_displayViewerButton.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.m_displayViewerButton.Location = new System.Drawing.Point(1214, 10);
            this.m_displayViewerButton.Name = "m_displayViewerButton";
            this.m_displayViewerButton.Size = new System.Drawing.Size(98, 23);
            this.m_displayViewerButton.TabIndex = 24;
            this.m_displayViewerButton.Text = "Display Viewer";
            this.m_displayViewerButton.UseVisualStyleBackColor = true;
            this.m_displayViewerButton.Click += new System.EventHandler(this.m_displayViewerButton_Click);
            // 
            // m_currentDataSettingsLabel
            // 
            this.m_currentDataSettingsLabel.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)));
            this.m_currentDataSettingsLabel.AutoSize = true;
            this.m_currentDataSettingsLabel.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.m_currentDataSettingsLabel.Location = new System.Drawing.Point(6, 476);
            this.m_currentDataSettingsLabel.Name = "m_currentDataSettingsLabel";
            this.m_currentDataSettingsLabel.Size = new System.Drawing.Size(133, 13);
            this.m_currentDataSettingsLabel.TabIndex = 25;
            this.m_currentDataSettingsLabel.Text = "Current Data Settings:";
            // 
            // m_diDataGridView
            // 
            this.m_diDataGridView.AllowUserToAddRows = false;
            this.m_diDataGridView.AllowUserToDeleteRows = false;
            this.m_diDataGridView.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.m_diDataGridView.ColumnHeadersHeightSizeMode = System.Windows.Forms.DataGridViewColumnHeadersHeightSizeMode.AutoSize;
            this.m_diDataGridView.Columns.AddRange(new System.Windows.Forms.DataGridViewColumn[] {
            this.DataItem,
            this.MinLength,
            this.MaxLength});
            this.m_diDataGridView.Location = new System.Drawing.Point(9, 495);
            this.m_diDataGridView.Name = "m_diDataGridView";
            this.m_diDataGridView.ReadOnly = true;
            this.m_diDataGridView.RowHeadersVisible = false;
            this.m_diDataGridView.Size = new System.Drawing.Size(335, 270);
            this.m_diDataGridView.TabIndex = 26;
            // 
            // m_dataItemLabel
            // 
            this.m_dataItemLabel.AutoSize = true;
            this.m_dataItemLabel.Location = new System.Drawing.Point(410, 498);
            this.m_dataItemLabel.Name = "m_dataItemLabel";
            this.m_dataItemLabel.Size = new System.Drawing.Size(56, 13);
            this.m_dataItemLabel.TabIndex = 27;
            this.m_dataItemLabel.Text = "Data Item:";
            // 
            // m_minLengthLabel
            // 
            this.m_minLengthLabel.AutoSize = true;
            this.m_minLengthLabel.Location = new System.Drawing.Point(403, 523);
            this.m_minLengthLabel.Name = "m_minLengthLabel";
            this.m_minLengthLabel.Size = new System.Drawing.Size(63, 13);
            this.m_minLengthLabel.TabIndex = 28;
            this.m_minLengthLabel.Text = "Min Length:";
            // 
            // m_maxLengthLabel
            // 
            this.m_maxLengthLabel.AutoSize = true;
            this.m_maxLengthLabel.Location = new System.Drawing.Point(400, 548);
            this.m_maxLengthLabel.Name = "m_maxLengthLabel";
            this.m_maxLengthLabel.Size = new System.Drawing.Size(66, 13);
            this.m_maxLengthLabel.TabIndex = 29;
            this.m_maxLengthLabel.Text = "Max Length:";
            // 
            // m_dataItemTextBox
            // 
            this.m_dataItemTextBox.Location = new System.Drawing.Point(472, 495);
            this.m_dataItemTextBox.Name = "m_dataItemTextBox";
            this.m_dataItemTextBox.Size = new System.Drawing.Size(201, 20);
            this.m_dataItemTextBox.TabIndex = 30;
            // 
            // m_minLengthTextBox
            // 
            this.m_minLengthTextBox.Location = new System.Drawing.Point(472, 520);
            this.m_minLengthTextBox.Name = "m_minLengthTextBox";
            this.m_minLengthTextBox.Size = new System.Drawing.Size(78, 20);
            this.m_minLengthTextBox.TabIndex = 31;
            // 
            // m_maxLengthTextBox
            // 
            this.m_maxLengthTextBox.Location = new System.Drawing.Point(472, 545);
            this.m_maxLengthTextBox.Name = "m_maxLengthTextBox";
            this.m_maxLengthTextBox.Size = new System.Drawing.Size(78, 20);
            this.m_maxLengthTextBox.TabIndex = 32;
            // 
            // m_headerDataLabel
            // 
            this.m_headerDataLabel.AutoSize = true;
            this.m_headerDataLabel.Location = new System.Drawing.Point(180, 109);
            this.m_headerDataLabel.Name = "m_headerDataLabel";
            this.m_headerDataLabel.Size = new System.Drawing.Size(71, 13);
            this.m_headerDataLabel.TabIndex = 33;
            this.m_headerDataLabel.Text = "Header Data:";
            // 
            // m_headerLengthLabel
            // 
            this.m_headerLengthLabel.AutoSize = true;
            this.m_headerLengthLabel.Location = new System.Drawing.Point(170, 134);
            this.m_headerLengthLabel.Name = "m_headerLengthLabel";
            this.m_headerLengthLabel.Size = new System.Drawing.Size(81, 13);
            this.m_headerLengthLabel.TabIndex = 34;
            this.m_headerLengthLabel.Text = "Header Length:";
            // 
            // m_headerDataTextBox
            // 
            this.m_headerDataTextBox.Location = new System.Drawing.Point(257, 106);
            this.m_headerDataTextBox.Name = "m_headerDataTextBox";
            this.m_headerDataTextBox.Size = new System.Drawing.Size(76, 20);
            this.m_headerDataTextBox.TabIndex = 35;
            // 
            // m_headerLengthTextBox
            // 
            this.m_headerLengthTextBox.Location = new System.Drawing.Point(257, 131);
            this.m_headerLengthTextBox.Name = "m_headerLengthTextBox";
            this.m_headerLengthTextBox.Size = new System.Drawing.Size(76, 20);
            this.m_headerLengthTextBox.TabIndex = 36;
            // 
            // m_trailerDataLabel
            // 
            this.m_trailerDataLabel.AutoSize = true;
            this.m_trailerDataLabel.Location = new System.Drawing.Point(359, 109);
            this.m_trailerDataLabel.Name = "m_trailerDataLabel";
            this.m_trailerDataLabel.Size = new System.Drawing.Size(65, 13);
            this.m_trailerDataLabel.TabIndex = 37;
            this.m_trailerDataLabel.Text = "Trailer Data:";
            // 
            // m_trailerLengthLabel
            // 
            this.m_trailerLengthLabel.AutoSize = true;
            this.m_trailerLengthLabel.Location = new System.Drawing.Point(349, 134);
            this.m_trailerLengthLabel.Name = "m_trailerLengthLabel";
            this.m_trailerLengthLabel.Size = new System.Drawing.Size(75, 13);
            this.m_trailerLengthLabel.TabIndex = 38;
            this.m_trailerLengthLabel.Text = "Trailer Length:";
            // 
            // m_trailerDataTextBox
            // 
            this.m_trailerDataTextBox.Location = new System.Drawing.Point(430, 106);
            this.m_trailerDataTextBox.Name = "m_trailerDataTextBox";
            this.m_trailerDataTextBox.Size = new System.Drawing.Size(60, 20);
            this.m_trailerDataTextBox.TabIndex = 39;
            // 
            // m_trailerDataLengthTextBox
            // 
            this.m_trailerDataLengthTextBox.Location = new System.Drawing.Point(430, 131);
            this.m_trailerDataLengthTextBox.Name = "m_trailerDataLengthTextBox";
            this.m_trailerDataLengthTextBox.Size = new System.Drawing.Size(60, 20);
            this.m_trailerDataLengthTextBox.TabIndex = 40;
            // 
            // m_addDataItemButton
            // 
            this.m_addDataItemButton.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.m_addDataItemButton.Location = new System.Drawing.Point(1198, 495);
            this.m_addDataItemButton.Name = "m_addDataItemButton";
            this.m_addDataItemButton.Size = new System.Drawing.Size(114, 22);
            this.m_addDataItemButton.TabIndex = 41;
            this.m_addDataItemButton.Text = "Add Data Item";
            this.m_addDataItemButton.UseVisualStyleBackColor = true;
            this.m_addDataItemButton.Click += new System.EventHandler(this.m_addDataItemButton_Click);
            // 
            // m_updateDataItemButton
            // 
            this.m_updateDataItemButton.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.m_updateDataItemButton.Location = new System.Drawing.Point(1198, 523);
            this.m_updateDataItemButton.Name = "m_updateDataItemButton";
            this.m_updateDataItemButton.Size = new System.Drawing.Size(114, 22);
            this.m_updateDataItemButton.TabIndex = 42;
            this.m_updateDataItemButton.Text = "Update Data Item";
            this.m_updateDataItemButton.UseVisualStyleBackColor = true;
            this.m_updateDataItemButton.Click += new System.EventHandler(this.m_updateDataItemButton_Click);
            // 
            // m_removeDataItemButton
            // 
            this.m_removeDataItemButton.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.m_removeDataItemButton.Location = new System.Drawing.Point(1198, 551);
            this.m_removeDataItemButton.Name = "m_removeDataItemButton";
            this.m_removeDataItemButton.Size = new System.Drawing.Size(114, 22);
            this.m_removeDataItemButton.TabIndex = 43;
            this.m_removeDataItemButton.Text = "Remove Data Item";
            this.m_removeDataItemButton.UseVisualStyleBackColor = true;
            this.m_removeDataItemButton.Click += new System.EventHandler(this.m_removeDataItemButton_Click);
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
            this.MaxLength.HeaderText = "Maximum Length";
            this.MaxLength.Name = "MaxLength";
            this.MaxLength.ReadOnly = true;
            // 
            // HeaderData
            // 
            this.HeaderData.HeaderText = "Header Data";
            this.HeaderData.Name = "HeaderData";
            this.HeaderData.ReadOnly = true;
            // 
            // HeaderLength
            // 
            this.HeaderLength.HeaderText = "Header Length";
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
            // DbLibraryTesterForm
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(1330, 777);
            this.Controls.Add(this.m_removeDataItemButton);
            this.Controls.Add(this.m_updateDataItemButton);
            this.Controls.Add(this.m_addDataItemButton);
            this.Controls.Add(this.m_trailerDataLengthTextBox);
            this.Controls.Add(this.m_trailerDataTextBox);
            this.Controls.Add(this.m_trailerLengthLabel);
            this.Controls.Add(this.m_trailerDataLabel);
            this.Controls.Add(this.m_headerLengthTextBox);
            this.Controls.Add(this.m_headerDataTextBox);
            this.Controls.Add(this.m_headerLengthLabel);
            this.Controls.Add(this.m_headerDataLabel);
            this.Controls.Add(this.m_maxLengthTextBox);
            this.Controls.Add(this.m_minLengthTextBox);
            this.Controls.Add(this.m_dataItemTextBox);
            this.Controls.Add(this.m_maxLengthLabel);
            this.Controls.Add(this.m_minLengthLabel);
            this.Controls.Add(this.m_dataItemLabel);
            this.Controls.Add(this.m_diDataGridView);
            this.Controls.Add(this.m_currentDataSettingsLabel);
            this.Controls.Add(this.m_displayViewerButton);
            this.Controls.Add(this.m_removePortButton);
            this.Controls.Add(this.m_updatePortSettingsButton);
            this.Controls.Add(this.m_portSettingsDataGridView);
            this.Controls.Add(this.m_currentPortSettingsLabel);
            this.Controls.Add(this.m_addEntryButton);
            this.Controls.Add(this.m_txTimeoutTextBox);
            this.Controls.Add(this.m_txTimeoutLabel);
            this.Controls.Add(this.m_rxTimeoutTextBox);
            this.Controls.Add(this.m_rxTimeoutLabel);
            this.Controls.Add(this.m_monitorDataCheckBox);
            this.Controls.Add(this.m_stopBitsComboBox);
            this.Controls.Add(this.m_stopBitsLabel);
            this.Controls.Add(this.m_dataBitsTextBox);
            this.Controls.Add(this.m_dataBitsLabel);
            this.Controls.Add(this.m_parityComboBox);
            this.Controls.Add(this.m_parityLabel);
            this.Controls.Add(this.m_baudRateTextBox);
            this.Controls.Add(this.m_baudRateLabel);
            this.Controls.Add(this.m_serialPortComboBox);
            this.Controls.Add(this.m_serialPortLabel);
            this.Controls.Add(this.m_devNameTextBox);
            this.Controls.Add(this.m_deviceNameLabel);
            this.Name = "DbLibraryTesterForm";
            this.Text = "Database Interface Library Tester";
            ((System.ComponentModel.ISupportInitialize)(this.m_portSettingsDataGridView)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.m_diDataGridView)).EndInit();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Label m_deviceNameLabel;
        private System.Windows.Forms.TextBox m_devNameTextBox;
        private System.Windows.Forms.Label m_serialPortLabel;
        private System.Windows.Forms.ComboBox m_serialPortComboBox;
        private System.Windows.Forms.Label m_baudRateLabel;
        private System.Windows.Forms.TextBox m_baudRateTextBox;
        private System.Windows.Forms.Label m_parityLabel;
        private System.Windows.Forms.ComboBox m_parityComboBox;
        private System.Windows.Forms.Label m_dataBitsLabel;
        private System.Windows.Forms.TextBox m_dataBitsTextBox;
        private System.Windows.Forms.Label m_stopBitsLabel;
        private System.Windows.Forms.ComboBox m_stopBitsComboBox;
        private System.Windows.Forms.CheckBox m_monitorDataCheckBox;
        private System.Windows.Forms.Label m_rxTimeoutLabel;
        private System.Windows.Forms.TextBox m_rxTimeoutTextBox;
        private System.Windows.Forms.Label m_txTimeoutLabel;
        private System.Windows.Forms.TextBox m_txTimeoutTextBox;
        private System.Windows.Forms.Button m_addEntryButton;
        private System.Windows.Forms.Label m_currentPortSettingsLabel;
        private System.Windows.Forms.DataGridView m_portSettingsDataGridView;
        private System.Windows.Forms.DataGridViewTextBoxColumn DeviceName;
        private System.Windows.Forms.DataGridViewTextBoxColumn PortName;
        private System.Windows.Forms.DataGridViewTextBoxColumn BaudRate;
        private System.Windows.Forms.DataGridViewTextBoxColumn ParitySetting;
        private System.Windows.Forms.DataGridViewTextBoxColumn DataBits;
        private System.Windows.Forms.DataGridViewTextBoxColumn StopBitSetting;
        private System.Windows.Forms.DataGridViewTextBoxColumn RxTimeout;
        private System.Windows.Forms.DataGridViewTextBoxColumn TxTimeout;
        private System.Windows.Forms.DataGridViewTextBoxColumn MonitorData;
        private System.Windows.Forms.Button m_updatePortSettingsButton;
        private System.Windows.Forms.Button m_removePortButton;
        private System.Windows.Forms.Button m_displayViewerButton;
        private System.Windows.Forms.Label m_currentDataSettingsLabel;
        private System.Windows.Forms.DataGridView m_diDataGridView;
        private System.Windows.Forms.Label m_dataItemLabel;
        private System.Windows.Forms.Label m_minLengthLabel;
        private System.Windows.Forms.Label m_maxLengthLabel;
        private System.Windows.Forms.TextBox m_dataItemTextBox;
        private System.Windows.Forms.TextBox m_minLengthTextBox;
        private System.Windows.Forms.TextBox m_maxLengthTextBox;
        private System.Windows.Forms.Label m_headerDataLabel;
        private System.Windows.Forms.Label m_headerLengthLabel;
        private System.Windows.Forms.TextBox m_headerDataTextBox;
        private System.Windows.Forms.TextBox m_headerLengthTextBox;
        private System.Windows.Forms.Label m_trailerDataLabel;
        private System.Windows.Forms.Label m_trailerLengthLabel;
        private System.Windows.Forms.TextBox m_trailerDataTextBox;
        private System.Windows.Forms.TextBox m_trailerDataLengthTextBox;
        private System.Windows.Forms.Button m_addDataItemButton;
        private System.Windows.Forms.Button m_updateDataItemButton;
        private System.Windows.Forms.Button m_removeDataItemButton;
        private System.Windows.Forms.DataGridViewTextBoxColumn HeaderData;
        private System.Windows.Forms.DataGridViewTextBoxColumn HeaderLength;
        private System.Windows.Forms.DataGridViewTextBoxColumn TrailerData;
        private System.Windows.Forms.DataGridViewTextBoxColumn TrailerLength;
        private System.Windows.Forms.DataGridViewTextBoxColumn DataItem;
        private System.Windows.Forms.DataGridViewTextBoxColumn MinLength;
        private System.Windows.Forms.DataGridViewTextBoxColumn MaxLength;
    }
}

