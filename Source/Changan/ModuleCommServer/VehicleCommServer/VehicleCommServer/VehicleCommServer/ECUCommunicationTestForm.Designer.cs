namespace VehicleCommServer
{
    partial class ECUCommunicationTestForm
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
            System.Windows.Forms.DataGridViewCellStyle dataGridViewCellStyle1 = new System.Windows.Forms.DataGridViewCellStyle();
            System.Windows.Forms.DataGridViewCellStyle dataGridViewCellStyle2 = new System.Windows.Forms.DataGridViewCellStyle();
            System.Windows.Forms.DataGridViewCellStyle dataGridViewCellStyle3 = new System.Windows.Forms.DataGridViewCellStyle();
            System.Windows.Forms.DataGridViewCellStyle dataGridViewCellStyle4 = new System.Windows.Forms.DataGridViewCellStyle();
            System.Windows.Forms.DataGridViewCellStyle dataGridViewCellStyle5 = new System.Windows.Forms.DataGridViewCellStyle();
            System.Windows.Forms.DataGridViewCellStyle dataGridViewCellStyle6 = new System.Windows.Forms.DataGridViewCellStyle();
            System.Windows.Forms.DataGridViewCellStyle dataGridViewCellStyle7 = new System.Windows.Forms.DataGridViewCellStyle();
            System.Windows.Forms.DataGridViewCellStyle dataGridViewCellStyle8 = new System.Windows.Forms.DataGridViewCellStyle();
            System.Windows.Forms.DataGridViewCellStyle dataGridViewCellStyle9 = new System.Windows.Forms.DataGridViewCellStyle();
            this.m_channelComboBox = new System.Windows.Forms.ComboBox();
            this.m_channelLabel = new System.Windows.Forms.Label();
            this.m_moduleIDDataGridView = new System.Windows.Forms.DataGridView();
            this.ModuleID0 = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.ModuleID1 = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.ModuleID2 = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.ModuleID3 = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.m_headerLabel = new System.Windows.Forms.Label();
            this.m_messageLabel = new System.Windows.Forms.Label();
            this.m_transmitMessageDataGridView = new System.Windows.Forms.DataGridView();
            this.m00 = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.m01 = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.m02 = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.m03 = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.m04 = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.m05 = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.m06 = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.m07 = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.m_sendMessageButton = new System.Windows.Forms.Button();
            this.m_textBoxStatus = new System.Windows.Forms.TextBox();
            this.m_deviceComboBox = new System.Windows.Forms.ComboBox();
            this.m_deviceLabel = new System.Windows.Forms.Label();
            this.m_responseIDLabel = new System.Windows.Forms.Label();
            this.m_responseIDDataGridView = new System.Windows.Forms.DataGridView();
            this.dataGridViewTextBoxColumn1 = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.dataGridViewTextBoxColumn2 = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.dataGridViewTextBoxColumn3 = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.dataGridViewTextBoxColumn4 = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.m_stopMsgButton = new System.Windows.Forms.Button();
            this.m_startMsgButton = new System.Windows.Forms.Button();
            this.m_responseExpectedCheckBox = new System.Windows.Forms.CheckBox();
            this.m_startThreadButton = new System.Windows.Forms.Button();
            this.m_stopThreadsButton = new System.Windows.Forms.Button();
            this.m_globalRequestCheckBox = new System.Windows.Forms.CheckBox();
            this.m_perform5BaudInitBtn = new System.Windows.Forms.Button();
            this.m_performFastInitBtn = new System.Windows.Forms.Button();
            this.m_setParamBtn = new System.Windows.Forms.Button();
            this.m_setDevParamCmbo = new System.Windows.Forms.ComboBox();
            this.m_setDevParamTxtBox = new System.Windows.Forms.TextBox();
            this.m_getDevParamTxtBox = new System.Windows.Forms.TextBox();
            this.m_getDevParamCmbo = new System.Windows.Forms.ComboBox();
            this.m_getParamBtn = new System.Windows.Forms.Button();
            ((System.ComponentModel.ISupportInitialize)(this.m_moduleIDDataGridView)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.m_transmitMessageDataGridView)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.m_responseIDDataGridView)).BeginInit();
            this.SuspendLayout();
            // 
            // m_channelComboBox
            // 
            this.m_channelComboBox.FormattingEnabled = true;
            this.m_channelComboBox.Location = new System.Drawing.Point(358, 18);
            this.m_channelComboBox.Name = "m_channelComboBox";
            this.m_channelComboBox.Size = new System.Drawing.Size(157, 21);
            this.m_channelComboBox.TabIndex = 2;
            this.m_channelComboBox.SelectedIndexChanged += new System.EventHandler(this.m_channelComboBox_SelectedIndexChanged);
            // 
            // m_channelLabel
            // 
            this.m_channelLabel.AutoSize = true;
            this.m_channelLabel.Location = new System.Drawing.Point(303, 21);
            this.m_channelLabel.Name = "m_channelLabel";
            this.m_channelLabel.Size = new System.Drawing.Size(49, 13);
            this.m_channelLabel.TabIndex = 1;
            this.m_channelLabel.Text = "Channel:";
            // 
            // m_moduleIDDataGridView
            // 
            this.m_moduleIDDataGridView.AllowUserToAddRows = false;
            this.m_moduleIDDataGridView.AllowUserToDeleteRows = false;
            dataGridViewCellStyle1.Alignment = System.Windows.Forms.DataGridViewContentAlignment.MiddleLeft;
            dataGridViewCellStyle1.BackColor = System.Drawing.SystemColors.Control;
            dataGridViewCellStyle1.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(134)));
            dataGridViewCellStyle1.ForeColor = System.Drawing.SystemColors.WindowText;
            dataGridViewCellStyle1.SelectionBackColor = System.Drawing.SystemColors.Highlight;
            dataGridViewCellStyle1.SelectionForeColor = System.Drawing.SystemColors.HighlightText;
            dataGridViewCellStyle1.WrapMode = System.Windows.Forms.DataGridViewTriState.True;
            this.m_moduleIDDataGridView.ColumnHeadersDefaultCellStyle = dataGridViewCellStyle1;
            this.m_moduleIDDataGridView.ColumnHeadersHeightSizeMode = System.Windows.Forms.DataGridViewColumnHeadersHeightSizeMode.AutoSize;
            this.m_moduleIDDataGridView.Columns.AddRange(new System.Windows.Forms.DataGridViewColumn[] {
            this.ModuleID0,
            this.ModuleID1,
            this.ModuleID2,
            this.ModuleID3});
            dataGridViewCellStyle2.Alignment = System.Windows.Forms.DataGridViewContentAlignment.MiddleLeft;
            dataGridViewCellStyle2.BackColor = System.Drawing.SystemColors.Window;
            dataGridViewCellStyle2.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(134)));
            dataGridViewCellStyle2.ForeColor = System.Drawing.SystemColors.ControlText;
            dataGridViewCellStyle2.SelectionBackColor = System.Drawing.SystemColors.Highlight;
            dataGridViewCellStyle2.SelectionForeColor = System.Drawing.SystemColors.HighlightText;
            dataGridViewCellStyle2.WrapMode = System.Windows.Forms.DataGridViewTriState.False;
            this.m_moduleIDDataGridView.DefaultCellStyle = dataGridViewCellStyle2;
            this.m_moduleIDDataGridView.Location = new System.Drawing.Point(88, 45);
            this.m_moduleIDDataGridView.Name = "m_moduleIDDataGridView";
            dataGridViewCellStyle3.Alignment = System.Windows.Forms.DataGridViewContentAlignment.MiddleLeft;
            dataGridViewCellStyle3.BackColor = System.Drawing.SystemColors.Control;
            dataGridViewCellStyle3.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(134)));
            dataGridViewCellStyle3.ForeColor = System.Drawing.SystemColors.WindowText;
            dataGridViewCellStyle3.SelectionBackColor = System.Drawing.SystemColors.Highlight;
            dataGridViewCellStyle3.SelectionForeColor = System.Drawing.SystemColors.HighlightText;
            dataGridViewCellStyle3.WrapMode = System.Windows.Forms.DataGridViewTriState.True;
            this.m_moduleIDDataGridView.RowHeadersDefaultCellStyle = dataGridViewCellStyle3;
            this.m_moduleIDDataGridView.Size = new System.Drawing.Size(244, 57);
            this.m_moduleIDDataGridView.TabIndex = 3;
            // 
            // ModuleID0
            // 
            this.ModuleID0.HeaderText = "ID0";
            this.ModuleID0.Name = "ModuleID0";
            this.ModuleID0.Width = 50;
            // 
            // ModuleID1
            // 
            this.ModuleID1.HeaderText = "ID1";
            this.ModuleID1.Name = "ModuleID1";
            this.ModuleID1.Width = 50;
            // 
            // ModuleID2
            // 
            this.ModuleID2.HeaderText = "ID2";
            this.ModuleID2.Name = "ModuleID2";
            this.ModuleID2.Width = 50;
            // 
            // ModuleID3
            // 
            this.ModuleID3.HeaderText = "ID3";
            this.ModuleID3.Name = "ModuleID3";
            this.ModuleID3.Width = 50;
            // 
            // m_headerLabel
            // 
            this.m_headerLabel.AutoSize = true;
            this.m_headerLabel.Location = new System.Drawing.Point(23, 45);
            this.m_headerLabel.Name = "m_headerLabel";
            this.m_headerLabel.Size = new System.Drawing.Size(64, 13);
            this.m_headerLabel.TabIndex = 3;
            this.m_headerLabel.Text = "Request ID:";
            // 
            // m_messageLabel
            // 
            this.m_messageLabel.AutoSize = true;
            this.m_messageLabel.Location = new System.Drawing.Point(23, 124);
            this.m_messageLabel.Name = "m_messageLabel";
            this.m_messageLabel.Size = new System.Drawing.Size(53, 13);
            this.m_messageLabel.TabIndex = 4;
            this.m_messageLabel.Text = "Message:";
            // 
            // m_transmitMessageDataGridView
            // 
            dataGridViewCellStyle4.Alignment = System.Windows.Forms.DataGridViewContentAlignment.MiddleLeft;
            dataGridViewCellStyle4.BackColor = System.Drawing.SystemColors.Control;
            dataGridViewCellStyle4.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(134)));
            dataGridViewCellStyle4.ForeColor = System.Drawing.SystemColors.WindowText;
            dataGridViewCellStyle4.SelectionBackColor = System.Drawing.SystemColors.Highlight;
            dataGridViewCellStyle4.SelectionForeColor = System.Drawing.SystemColors.HighlightText;
            dataGridViewCellStyle4.WrapMode = System.Windows.Forms.DataGridViewTriState.True;
            this.m_transmitMessageDataGridView.ColumnHeadersDefaultCellStyle = dataGridViewCellStyle4;
            this.m_transmitMessageDataGridView.ColumnHeadersHeightSizeMode = System.Windows.Forms.DataGridViewColumnHeadersHeightSizeMode.AutoSize;
            this.m_transmitMessageDataGridView.Columns.AddRange(new System.Windows.Forms.DataGridViewColumn[] {
            this.m00,
            this.m01,
            this.m02,
            this.m03,
            this.m04,
            this.m05,
            this.m06,
            this.m07});
            dataGridViewCellStyle5.Alignment = System.Windows.Forms.DataGridViewContentAlignment.MiddleLeft;
            dataGridViewCellStyle5.BackColor = System.Drawing.SystemColors.Window;
            dataGridViewCellStyle5.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(134)));
            dataGridViewCellStyle5.ForeColor = System.Drawing.SystemColors.ControlText;
            dataGridViewCellStyle5.SelectionBackColor = System.Drawing.SystemColors.Highlight;
            dataGridViewCellStyle5.SelectionForeColor = System.Drawing.SystemColors.HighlightText;
            dataGridViewCellStyle5.WrapMode = System.Windows.Forms.DataGridViewTriState.False;
            this.m_transmitMessageDataGridView.DefaultCellStyle = dataGridViewCellStyle5;
            this.m_transmitMessageDataGridView.Location = new System.Drawing.Point(88, 124);
            this.m_transmitMessageDataGridView.Name = "m_transmitMessageDataGridView";
            dataGridViewCellStyle6.Alignment = System.Windows.Forms.DataGridViewContentAlignment.MiddleLeft;
            dataGridViewCellStyle6.BackColor = System.Drawing.SystemColors.Control;
            dataGridViewCellStyle6.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(134)));
            dataGridViewCellStyle6.ForeColor = System.Drawing.SystemColors.WindowText;
            dataGridViewCellStyle6.SelectionBackColor = System.Drawing.SystemColors.Highlight;
            dataGridViewCellStyle6.SelectionForeColor = System.Drawing.SystemColors.HighlightText;
            dataGridViewCellStyle6.WrapMode = System.Windows.Forms.DataGridViewTriState.True;
            this.m_transmitMessageDataGridView.RowHeadersDefaultCellStyle = dataGridViewCellStyle6;
            this.m_transmitMessageDataGridView.Size = new System.Drawing.Size(443, 75);
            this.m_transmitMessageDataGridView.TabIndex = 5;
            // 
            // m00
            // 
            this.m00.HeaderText = "00";
            this.m00.Name = "m00";
            this.m00.Width = 50;
            // 
            // m01
            // 
            this.m01.HeaderText = "01";
            this.m01.Name = "m01";
            this.m01.Width = 50;
            // 
            // m02
            // 
            this.m02.HeaderText = "02";
            this.m02.Name = "m02";
            this.m02.Width = 50;
            // 
            // m03
            // 
            this.m03.HeaderText = "03";
            this.m03.Name = "m03";
            this.m03.Width = 50;
            // 
            // m04
            // 
            this.m04.HeaderText = "04";
            this.m04.Name = "m04";
            this.m04.Width = 50;
            // 
            // m05
            // 
            this.m05.HeaderText = "05";
            this.m05.Name = "m05";
            this.m05.Width = 50;
            // 
            // m06
            // 
            this.m06.HeaderText = "06";
            this.m06.Name = "m06";
            this.m06.Width = 50;
            // 
            // m07
            // 
            this.m07.HeaderText = "07";
            this.m07.Name = "m07";
            this.m07.Width = 50;
            // 
            // m_sendMessageButton
            // 
            this.m_sendMessageButton.Location = new System.Drawing.Point(88, 424);
            this.m_sendMessageButton.Name = "m_sendMessageButton";
            this.m_sendMessageButton.Size = new System.Drawing.Size(87, 27);
            this.m_sendMessageButton.TabIndex = 6;
            this.m_sendMessageButton.Text = "Send Message";
            this.m_sendMessageButton.UseVisualStyleBackColor = true;
            this.m_sendMessageButton.Click += new System.EventHandler(this.m_sendMessageButton_Click);
            // 
            // m_textBoxStatus
            // 
            this.m_textBoxStatus.Font = new System.Drawing.Font("Courier New", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.m_textBoxStatus.Location = new System.Drawing.Point(88, 258);
            this.m_textBoxStatus.Multiline = true;
            this.m_textBoxStatus.Name = "m_textBoxStatus";
            this.m_textBoxStatus.ScrollBars = System.Windows.Forms.ScrollBars.Vertical;
            this.m_textBoxStatus.Size = new System.Drawing.Size(473, 160);
            this.m_textBoxStatus.TabIndex = 7;
            // 
            // m_deviceComboBox
            // 
            this.m_deviceComboBox.FormattingEnabled = true;
            this.m_deviceComboBox.Location = new System.Drawing.Point(88, 18);
            this.m_deviceComboBox.Name = "m_deviceComboBox";
            this.m_deviceComboBox.Size = new System.Drawing.Size(157, 21);
            this.m_deviceComboBox.TabIndex = 1;
            this.m_deviceComboBox.SelectedIndexChanged += new System.EventHandler(this.m_deviceComboBox_SelectedIndexChanged);
            // 
            // m_deviceLabel
            // 
            this.m_deviceLabel.AutoSize = true;
            this.m_deviceLabel.Location = new System.Drawing.Point(23, 21);
            this.m_deviceLabel.Name = "m_deviceLabel";
            this.m_deviceLabel.Size = new System.Drawing.Size(44, 13);
            this.m_deviceLabel.TabIndex = 9;
            this.m_deviceLabel.Text = "Device:";
            // 
            // m_responseIDLabel
            // 
            this.m_responseIDLabel.AutoSize = true;
            this.m_responseIDLabel.Location = new System.Drawing.Point(338, 45);
            this.m_responseIDLabel.Name = "m_responseIDLabel";
            this.m_responseIDLabel.Size = new System.Drawing.Size(72, 13);
            this.m_responseIDLabel.TabIndex = 11;
            this.m_responseIDLabel.Text = "Response ID:";
            // 
            // m_responseIDDataGridView
            // 
            dataGridViewCellStyle7.Alignment = System.Windows.Forms.DataGridViewContentAlignment.MiddleLeft;
            dataGridViewCellStyle7.BackColor = System.Drawing.SystemColors.Control;
            dataGridViewCellStyle7.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(134)));
            dataGridViewCellStyle7.ForeColor = System.Drawing.SystemColors.WindowText;
            dataGridViewCellStyle7.SelectionBackColor = System.Drawing.SystemColors.Highlight;
            dataGridViewCellStyle7.SelectionForeColor = System.Drawing.SystemColors.HighlightText;
            dataGridViewCellStyle7.WrapMode = System.Windows.Forms.DataGridViewTriState.True;
            this.m_responseIDDataGridView.ColumnHeadersDefaultCellStyle = dataGridViewCellStyle7;
            this.m_responseIDDataGridView.ColumnHeadersHeightSizeMode = System.Windows.Forms.DataGridViewColumnHeadersHeightSizeMode.AutoSize;
            this.m_responseIDDataGridView.Columns.AddRange(new System.Windows.Forms.DataGridViewColumn[] {
            this.dataGridViewTextBoxColumn1,
            this.dataGridViewTextBoxColumn2,
            this.dataGridViewTextBoxColumn3,
            this.dataGridViewTextBoxColumn4});
            dataGridViewCellStyle8.Alignment = System.Windows.Forms.DataGridViewContentAlignment.MiddleLeft;
            dataGridViewCellStyle8.BackColor = System.Drawing.SystemColors.Window;
            dataGridViewCellStyle8.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(134)));
            dataGridViewCellStyle8.ForeColor = System.Drawing.SystemColors.ControlText;
            dataGridViewCellStyle8.SelectionBackColor = System.Drawing.SystemColors.Highlight;
            dataGridViewCellStyle8.SelectionForeColor = System.Drawing.SystemColors.HighlightText;
            dataGridViewCellStyle8.WrapMode = System.Windows.Forms.DataGridViewTriState.False;
            this.m_responseIDDataGridView.DefaultCellStyle = dataGridViewCellStyle8;
            this.m_responseIDDataGridView.Location = new System.Drawing.Point(416, 45);
            this.m_responseIDDataGridView.Name = "m_responseIDDataGridView";
            dataGridViewCellStyle9.Alignment = System.Windows.Forms.DataGridViewContentAlignment.MiddleLeft;
            dataGridViewCellStyle9.BackColor = System.Drawing.SystemColors.Control;
            dataGridViewCellStyle9.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(134)));
            dataGridViewCellStyle9.ForeColor = System.Drawing.SystemColors.WindowText;
            dataGridViewCellStyle9.SelectionBackColor = System.Drawing.SystemColors.Highlight;
            dataGridViewCellStyle9.SelectionForeColor = System.Drawing.SystemColors.HighlightText;
            dataGridViewCellStyle9.WrapMode = System.Windows.Forms.DataGridViewTriState.True;
            this.m_responseIDDataGridView.RowHeadersDefaultCellStyle = dataGridViewCellStyle9;
            this.m_responseIDDataGridView.Size = new System.Drawing.Size(244, 73);
            this.m_responseIDDataGridView.TabIndex = 4;
            // 
            // dataGridViewTextBoxColumn1
            // 
            this.dataGridViewTextBoxColumn1.HeaderText = "ID0";
            this.dataGridViewTextBoxColumn1.Name = "dataGridViewTextBoxColumn1";
            this.dataGridViewTextBoxColumn1.Width = 50;
            // 
            // dataGridViewTextBoxColumn2
            // 
            this.dataGridViewTextBoxColumn2.HeaderText = "ID1";
            this.dataGridViewTextBoxColumn2.Name = "dataGridViewTextBoxColumn2";
            this.dataGridViewTextBoxColumn2.Width = 50;
            // 
            // dataGridViewTextBoxColumn3
            // 
            this.dataGridViewTextBoxColumn3.HeaderText = "ID2";
            this.dataGridViewTextBoxColumn3.Name = "dataGridViewTextBoxColumn3";
            this.dataGridViewTextBoxColumn3.Width = 50;
            // 
            // dataGridViewTextBoxColumn4
            // 
            this.dataGridViewTextBoxColumn4.HeaderText = "ID3";
            this.dataGridViewTextBoxColumn4.Name = "dataGridViewTextBoxColumn4";
            this.dataGridViewTextBoxColumn4.Width = 50;
            // 
            // m_stopMsgButton
            // 
            this.m_stopMsgButton.Location = new System.Drawing.Point(474, 424);
            this.m_stopMsgButton.Name = "m_stopMsgButton";
            this.m_stopMsgButton.Size = new System.Drawing.Size(87, 27);
            this.m_stopMsgButton.TabIndex = 12;
            this.m_stopMsgButton.Text = "Stop Msg";
            this.m_stopMsgButton.UseVisualStyleBackColor = true;
            this.m_stopMsgButton.Click += new System.EventHandler(this.m_stopMsgButton_Click);
            // 
            // m_startMsgButton
            // 
            this.m_startMsgButton.Location = new System.Drawing.Point(381, 424);
            this.m_startMsgButton.Name = "m_startMsgButton";
            this.m_startMsgButton.Size = new System.Drawing.Size(87, 27);
            this.m_startMsgButton.TabIndex = 13;
            this.m_startMsgButton.Text = "Start Msg";
            this.m_startMsgButton.UseVisualStyleBackColor = true;
            this.m_startMsgButton.Click += new System.EventHandler(this.m_startMsgButton_Click);
            // 
            // m_responseExpectedCheckBox
            // 
            this.m_responseExpectedCheckBox.AutoSize = true;
            this.m_responseExpectedCheckBox.Checked = true;
            this.m_responseExpectedCheckBox.CheckState = System.Windows.Forms.CheckState.Checked;
            this.m_responseExpectedCheckBox.Location = new System.Drawing.Point(538, 124);
            this.m_responseExpectedCheckBox.Name = "m_responseExpectedCheckBox";
            this.m_responseExpectedCheckBox.Size = new System.Drawing.Size(122, 17);
            this.m_responseExpectedCheckBox.TabIndex = 14;
            this.m_responseExpectedCheckBox.Text = "Response Expected";
            this.m_responseExpectedCheckBox.UseVisualStyleBackColor = true;
            // 
            // m_startThreadButton
            // 
            this.m_startThreadButton.Location = new System.Drawing.Point(573, 258);
            this.m_startThreadButton.Name = "m_startThreadButton";
            this.m_startThreadButton.Size = new System.Drawing.Size(87, 27);
            this.m_startThreadButton.TabIndex = 15;
            this.m_startThreadButton.Text = "Start Thread";
            this.m_startThreadButton.UseVisualStyleBackColor = true;
            this.m_startThreadButton.Click += new System.EventHandler(this.m_startThreadButton_Click);
            // 
            // m_stopThreadsButton
            // 
            this.m_stopThreadsButton.Location = new System.Drawing.Point(573, 291);
            this.m_stopThreadsButton.Name = "m_stopThreadsButton";
            this.m_stopThreadsButton.Size = new System.Drawing.Size(87, 27);
            this.m_stopThreadsButton.TabIndex = 16;
            this.m_stopThreadsButton.Text = "Stop Threads";
            this.m_stopThreadsButton.UseVisualStyleBackColor = true;
            this.m_stopThreadsButton.Click += new System.EventHandler(this.m_stopThreadsButton_Click);
            // 
            // m_globalRequestCheckBox
            // 
            this.m_globalRequestCheckBox.AutoSize = true;
            this.m_globalRequestCheckBox.Location = new System.Drawing.Point(537, 147);
            this.m_globalRequestCheckBox.Name = "m_globalRequestCheckBox";
            this.m_globalRequestCheckBox.Size = new System.Drawing.Size(114, 17);
            this.m_globalRequestCheckBox.TabIndex = 17;
            this.m_globalRequestCheckBox.Text = "All Nodes Request";
            this.m_globalRequestCheckBox.UseVisualStyleBackColor = true;
            // 
            // m_perform5BaudInitBtn
            // 
            this.m_perform5BaudInitBtn.Location = new System.Drawing.Point(88, 205);
            this.m_perform5BaudInitBtn.Name = "m_perform5BaudInitBtn";
            this.m_perform5BaudInitBtn.Size = new System.Drawing.Size(87, 27);
            this.m_perform5BaudInitBtn.TabIndex = 18;
            this.m_perform5BaudInitBtn.Text = "5 Baud Init";
            this.m_perform5BaudInitBtn.UseVisualStyleBackColor = true;
            this.m_perform5BaudInitBtn.Click += new System.EventHandler(this.m_perform5BaudInitBtn_Click);
            // 
            // m_performFastInitBtn
            // 
            this.m_performFastInitBtn.Location = new System.Drawing.Point(181, 205);
            this.m_performFastInitBtn.Name = "m_performFastInitBtn";
            this.m_performFastInitBtn.Size = new System.Drawing.Size(87, 27);
            this.m_performFastInitBtn.TabIndex = 19;
            this.m_performFastInitBtn.Text = "Fast Init";
            this.m_performFastInitBtn.UseVisualStyleBackColor = true;
            this.m_performFastInitBtn.Click += new System.EventHandler(this.m_performFastInitBtn_Click);
            // 
            // m_setParamBtn
            // 
            this.m_setParamBtn.Location = new System.Drawing.Point(315, 499);
            this.m_setParamBtn.Name = "m_setParamBtn";
            this.m_setParamBtn.Size = new System.Drawing.Size(64, 27);
            this.m_setParamBtn.TabIndex = 20;
            this.m_setParamBtn.Text = "Set Param";
            this.m_setParamBtn.UseVisualStyleBackColor = true;
            this.m_setParamBtn.Click += new System.EventHandler(this.m_setParamBtn_Click);
            // 
            // m_setDevParamCmbo
            // 
            this.m_setDevParamCmbo.FormattingEnabled = true;
            this.m_setDevParamCmbo.Location = new System.Drawing.Point(88, 499);
            this.m_setDevParamCmbo.Name = "m_setDevParamCmbo";
            this.m_setDevParamCmbo.Size = new System.Drawing.Size(115, 21);
            this.m_setDevParamCmbo.TabIndex = 21;
            // 
            // m_setDevParamTxtBox
            // 
            this.m_setDevParamTxtBox.Location = new System.Drawing.Point(209, 499);
            this.m_setDevParamTxtBox.Name = "m_setDevParamTxtBox";
            this.m_setDevParamTxtBox.Size = new System.Drawing.Size(100, 20);
            this.m_setDevParamTxtBox.TabIndex = 22;
            // 
            // m_getDevParamTxtBox
            // 
            this.m_getDevParamTxtBox.Location = new System.Drawing.Point(511, 499);
            this.m_getDevParamTxtBox.Name = "m_getDevParamTxtBox";
            this.m_getDevParamTxtBox.ReadOnly = true;
            this.m_getDevParamTxtBox.Size = new System.Drawing.Size(100, 20);
            this.m_getDevParamTxtBox.TabIndex = 25;
            // 
            // m_getDevParamCmbo
            // 
            this.m_getDevParamCmbo.FormattingEnabled = true;
            this.m_getDevParamCmbo.Location = new System.Drawing.Point(390, 499);
            this.m_getDevParamCmbo.Name = "m_getDevParamCmbo";
            this.m_getDevParamCmbo.Size = new System.Drawing.Size(115, 21);
            this.m_getDevParamCmbo.TabIndex = 24;
            // 
            // m_getParamBtn
            // 
            this.m_getParamBtn.Location = new System.Drawing.Point(617, 499);
            this.m_getParamBtn.Name = "m_getParamBtn";
            this.m_getParamBtn.Size = new System.Drawing.Size(78, 27);
            this.m_getParamBtn.TabIndex = 23;
            this.m_getParamBtn.Text = "Get Param";
            this.m_getParamBtn.UseVisualStyleBackColor = true;
            this.m_getParamBtn.Click += new System.EventHandler(this.m_getParamBtn_Click);
            // 
            // ECUCommunicationTestForm
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(707, 558);
            this.Controls.Add(this.m_getDevParamTxtBox);
            this.Controls.Add(this.m_getDevParamCmbo);
            this.Controls.Add(this.m_getParamBtn);
            this.Controls.Add(this.m_setDevParamTxtBox);
            this.Controls.Add(this.m_setDevParamCmbo);
            this.Controls.Add(this.m_setParamBtn);
            this.Controls.Add(this.m_performFastInitBtn);
            this.Controls.Add(this.m_perform5BaudInitBtn);
            this.Controls.Add(this.m_globalRequestCheckBox);
            this.Controls.Add(this.m_stopThreadsButton);
            this.Controls.Add(this.m_startThreadButton);
            this.Controls.Add(this.m_responseExpectedCheckBox);
            this.Controls.Add(this.m_startMsgButton);
            this.Controls.Add(this.m_stopMsgButton);
            this.Controls.Add(this.m_responseIDDataGridView);
            this.Controls.Add(this.m_responseIDLabel);
            this.Controls.Add(this.m_deviceLabel);
            this.Controls.Add(this.m_deviceComboBox);
            this.Controls.Add(this.m_textBoxStatus);
            this.Controls.Add(this.m_sendMessageButton);
            this.Controls.Add(this.m_transmitMessageDataGridView);
            this.Controls.Add(this.m_messageLabel);
            this.Controls.Add(this.m_headerLabel);
            this.Controls.Add(this.m_moduleIDDataGridView);
            this.Controls.Add(this.m_channelLabel);
            this.Controls.Add(this.m_channelComboBox);
            this.Name = "ECUCommunicationTestForm";
            this.Text = "ECUCommunicationTestForm";
            this.FormClosing += new System.Windows.Forms.FormClosingEventHandler(this.ECUCommunicationTestForm_FormClosing);
            ((System.ComponentModel.ISupportInitialize)(this.m_moduleIDDataGridView)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.m_transmitMessageDataGridView)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.m_responseIDDataGridView)).EndInit();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.ComboBox m_channelComboBox;
        private System.Windows.Forms.Label m_channelLabel;
        private System.Windows.Forms.DataGridView m_moduleIDDataGridView;
        private System.Windows.Forms.DataGridViewTextBoxColumn ModuleID0;
        private System.Windows.Forms.DataGridViewTextBoxColumn ModuleID1;
        private System.Windows.Forms.DataGridViewTextBoxColumn ModuleID2;
        private System.Windows.Forms.DataGridViewTextBoxColumn ModuleID3;
        private System.Windows.Forms.Label m_headerLabel;
        private System.Windows.Forms.Label m_messageLabel;
        private System.Windows.Forms.DataGridView m_transmitMessageDataGridView;
        private System.Windows.Forms.DataGridViewTextBoxColumn m00;
        private System.Windows.Forms.DataGridViewTextBoxColumn m01;
        private System.Windows.Forms.DataGridViewTextBoxColumn m02;
        private System.Windows.Forms.DataGridViewTextBoxColumn m03;
        private System.Windows.Forms.DataGridViewTextBoxColumn m04;
        private System.Windows.Forms.DataGridViewTextBoxColumn m05;
        private System.Windows.Forms.DataGridViewTextBoxColumn m06;
        private System.Windows.Forms.DataGridViewTextBoxColumn m07;
        private System.Windows.Forms.Button m_sendMessageButton;
        private System.Windows.Forms.TextBox m_textBoxStatus;
        private System.Windows.Forms.ComboBox m_deviceComboBox;
        private System.Windows.Forms.Label m_deviceLabel;
        private System.Windows.Forms.Label m_responseIDLabel;
        private System.Windows.Forms.DataGridView m_responseIDDataGridView;
        private System.Windows.Forms.DataGridViewTextBoxColumn dataGridViewTextBoxColumn1;
        private System.Windows.Forms.DataGridViewTextBoxColumn dataGridViewTextBoxColumn2;
        private System.Windows.Forms.DataGridViewTextBoxColumn dataGridViewTextBoxColumn3;
        private System.Windows.Forms.DataGridViewTextBoxColumn dataGridViewTextBoxColumn4;
        private System.Windows.Forms.Button m_stopMsgButton;
        private System.Windows.Forms.Button m_startMsgButton;
        private System.Windows.Forms.CheckBox m_responseExpectedCheckBox;
        private System.Windows.Forms.Button m_startThreadButton;
        private System.Windows.Forms.Button m_stopThreadsButton;
        private System.Windows.Forms.CheckBox m_globalRequestCheckBox;
        private System.Windows.Forms.Button m_perform5BaudInitBtn;
        private System.Windows.Forms.Button m_performFastInitBtn;
        private System.Windows.Forms.Button m_setParamBtn;
        private System.Windows.Forms.ComboBox m_setDevParamCmbo;
        private System.Windows.Forms.TextBox m_setDevParamTxtBox;
        private System.Windows.Forms.TextBox m_getDevParamTxtBox;
        private System.Windows.Forms.ComboBox m_getDevParamCmbo;
        private System.Windows.Forms.Button m_getParamBtn;
    }
}