namespace UtilityFileInterpreterViewer
{
    partial class UtilityFileDisplay
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
            this.m_headerDataGridView = new System.Windows.Forms.DataGridView();
            this.Checksum = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.ModuleID = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.PartNumber = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.DesignLevel = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.HeaderType = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.InterpreterType = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.RoutineOffset = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.Addressing = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.DataAddress = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.DataBytesInMessage = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.label1 = new System.Windows.Forms.Label();
            this.m_interpInstDataGridView = new System.Windows.Forms.DataGridView();
            this.Step = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.OpCode = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.ActionFields = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.GotoFields = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.label2 = new System.Windows.Forms.Label();
            this.m_routineDataGridView = new System.Windows.Forms.DataGridView();
            this.AddressInfo = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.LengthOfRoutine = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.Data = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.label3 = new System.Windows.Forms.Label();
            this.m_apiHeaderDataGridView = new System.Windows.Forms.DataGridView();
            this.dataGridViewTextBoxColumn1 = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.dataGridViewTextBoxColumn2 = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.dataGridViewTextBoxColumn3 = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.dataGridViewTextBoxColumn4 = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.dataGridViewTextBoxColumn5 = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.dataGridViewTextBoxColumn6 = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.dataGridViewTextBoxColumn7 = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.dataGridViewTextBoxColumn8 = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.dataGridViewTextBoxColumn9 = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.dataGridViewTextBoxColumn10 = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.dataGridViewTextBoxColumn11 = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.label4 = new System.Windows.Forms.Label();
            this.m_calFilesDataGridView = new System.Windows.Forms.DataGridView();
            this.label5 = new System.Windows.Forms.Label();
            this.dataGridViewTextBoxColumn14 = new System.Windows.Forms.DataGridViewTextBoxColumn();
            ((System.ComponentModel.ISupportInitialize)(this.m_headerDataGridView)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.m_interpInstDataGridView)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.m_routineDataGridView)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.m_apiHeaderDataGridView)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.m_calFilesDataGridView)).BeginInit();
            this.SuspendLayout();
            // 
            // m_headerDataGridView
            // 
            this.m_headerDataGridView.AllowUserToAddRows = false;
            this.m_headerDataGridView.AllowUserToDeleteRows = false;
            this.m_headerDataGridView.ColumnHeadersHeightSizeMode = System.Windows.Forms.DataGridViewColumnHeadersHeightSizeMode.AutoSize;
            this.m_headerDataGridView.Columns.AddRange(new System.Windows.Forms.DataGridViewColumn[] {
            this.Checksum,
            this.ModuleID,
            this.PartNumber,
            this.DesignLevel,
            this.HeaderType,
            this.InterpreterType,
            this.RoutineOffset,
            this.Addressing,
            this.DataAddress,
            this.DataBytesInMessage});
            this.m_headerDataGridView.Location = new System.Drawing.Point(30, 234);
            this.m_headerDataGridView.Name = "m_headerDataGridView";
            this.m_headerDataGridView.ReadOnly = true;
            this.m_headerDataGridView.Size = new System.Drawing.Size(1044, 79);
            this.m_headerDataGridView.TabIndex = 0;
            // 
            // Checksum
            // 
            this.Checksum.HeaderText = "Checksum";
            this.Checksum.Name = "Checksum";
            this.Checksum.ReadOnly = true;
            // 
            // ModuleID
            // 
            this.ModuleID.HeaderText = "Module ID";
            this.ModuleID.Name = "ModuleID";
            this.ModuleID.ReadOnly = true;
            // 
            // PartNumber
            // 
            this.PartNumber.HeaderText = "Part Number";
            this.PartNumber.Name = "PartNumber";
            this.PartNumber.ReadOnly = true;
            // 
            // DesignLevel
            // 
            this.DesignLevel.HeaderText = "Design Level";
            this.DesignLevel.Name = "DesignLevel";
            this.DesignLevel.ReadOnly = true;
            // 
            // HeaderType
            // 
            this.HeaderType.HeaderText = "Header Type";
            this.HeaderType.Name = "HeaderType";
            this.HeaderType.ReadOnly = true;
            // 
            // InterpreterType
            // 
            this.InterpreterType.HeaderText = "Interpreter Type";
            this.InterpreterType.Name = "InterpreterType";
            this.InterpreterType.ReadOnly = true;
            // 
            // RoutineOffset
            // 
            this.RoutineOffset.HeaderText = "Offset";
            this.RoutineOffset.Name = "RoutineOffset";
            this.RoutineOffset.ReadOnly = true;
            // 
            // Addressing
            // 
            this.Addressing.HeaderText = "Type of Addressing";
            this.Addressing.Name = "Addressing";
            this.Addressing.ReadOnly = true;
            // 
            // DataAddress
            // 
            this.DataAddress.HeaderText = "Data Address";
            this.DataAddress.Name = "DataAddress";
            this.DataAddress.ReadOnly = true;
            // 
            // DataBytesInMessage
            // 
            this.DataBytesInMessage.HeaderText = "# of Data Bytes in Message";
            this.DataBytesInMessage.Name = "DataBytesInMessage";
            this.DataBytesInMessage.ReadOnly = true;
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Font = new System.Drawing.Font("Microsoft Sans Serif", 15.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.label1.Location = new System.Drawing.Point(25, 206);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(82, 25);
            this.label1.TabIndex = 1;
            this.label1.Text = "Header";
            // 
            // m_interpInstDataGridView
            // 
            this.m_interpInstDataGridView.AllowUserToAddRows = false;
            this.m_interpInstDataGridView.AllowUserToDeleteRows = false;
            this.m_interpInstDataGridView.ColumnHeadersHeightSizeMode = System.Windows.Forms.DataGridViewColumnHeadersHeightSizeMode.AutoSize;
            this.m_interpInstDataGridView.Columns.AddRange(new System.Windows.Forms.DataGridViewColumn[] {
            this.Step,
            this.OpCode,
            this.ActionFields,
            this.GotoFields});
            this.m_interpInstDataGridView.Location = new System.Drawing.Point(30, 344);
            this.m_interpInstDataGridView.Name = "m_interpInstDataGridView";
            this.m_interpInstDataGridView.ReadOnly = true;
            this.m_interpInstDataGridView.Size = new System.Drawing.Size(1044, 150);
            this.m_interpInstDataGridView.TabIndex = 2;
            // 
            // Step
            // 
            this.Step.HeaderText = "Step";
            this.Step.Name = "Step";
            this.Step.ReadOnly = true;
            // 
            // OpCode
            // 
            this.OpCode.HeaderText = "Op-Code";
            this.OpCode.Name = "OpCode";
            this.OpCode.ReadOnly = true;
            // 
            // ActionFields
            // 
            this.ActionFields.HeaderText = "Action Fields";
            this.ActionFields.Name = "ActionFields";
            this.ActionFields.ReadOnly = true;
            // 
            // GotoFields
            // 
            this.GotoFields.HeaderText = "Goto Fields";
            this.GotoFields.Name = "GotoFields";
            this.GotoFields.ReadOnly = true;
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Font = new System.Drawing.Font("Microsoft Sans Serif", 15.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.label2.Location = new System.Drawing.Point(25, 316);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(226, 25);
            this.label2.TabIndex = 3;
            this.label2.Text = "Interpreter Instructions";
            // 
            // m_routineDataGridView
            // 
            this.m_routineDataGridView.AllowUserToAddRows = false;
            this.m_routineDataGridView.AllowUserToDeleteRows = false;
            this.m_routineDataGridView.ColumnHeadersHeightSizeMode = System.Windows.Forms.DataGridViewColumnHeadersHeightSizeMode.AutoSize;
            this.m_routineDataGridView.Columns.AddRange(new System.Windows.Forms.DataGridViewColumn[] {
            this.AddressInfo,
            this.LengthOfRoutine,
            this.Data});
            this.m_routineDataGridView.Location = new System.Drawing.Point(30, 525);
            this.m_routineDataGridView.Name = "m_routineDataGridView";
            this.m_routineDataGridView.ReadOnly = true;
            this.m_routineDataGridView.Size = new System.Drawing.Size(1044, 150);
            this.m_routineDataGridView.TabIndex = 4;
            // 
            // AddressInfo
            // 
            this.AddressInfo.HeaderText = "Address Information";
            this.AddressInfo.Name = "AddressInfo";
            this.AddressInfo.ReadOnly = true;
            // 
            // LengthOfRoutine
            // 
            this.LengthOfRoutine.HeaderText = "Length of Rouitine";
            this.LengthOfRoutine.Name = "LengthOfRoutine";
            this.LengthOfRoutine.ReadOnly = true;
            // 
            // Data
            // 
            this.Data.HeaderText = "Data";
            this.Data.Name = "Data";
            this.Data.ReadOnly = true;
            // 
            // label3
            // 
            this.label3.AutoSize = true;
            this.label3.Font = new System.Drawing.Font("Microsoft Sans Serif", 15.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.label3.Location = new System.Drawing.Point(25, 497);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(97, 25);
            this.label3.TabIndex = 5;
            this.label3.Text = "Routines";
            // 
            // m_apiHeaderDataGridView
            // 
            this.m_apiHeaderDataGridView.AllowUserToAddRows = false;
            this.m_apiHeaderDataGridView.AllowUserToDeleteRows = false;
            this.m_apiHeaderDataGridView.ColumnHeadersHeightSizeMode = System.Windows.Forms.DataGridViewColumnHeadersHeightSizeMode.AutoSize;
            this.m_apiHeaderDataGridView.Columns.AddRange(new System.Windows.Forms.DataGridViewColumn[] {
            this.dataGridViewTextBoxColumn1,
            this.dataGridViewTextBoxColumn2,
            this.dataGridViewTextBoxColumn3,
            this.dataGridViewTextBoxColumn4,
            this.dataGridViewTextBoxColumn5,
            this.dataGridViewTextBoxColumn6,
            this.dataGridViewTextBoxColumn7,
            this.dataGridViewTextBoxColumn8,
            this.dataGridViewTextBoxColumn9,
            this.dataGridViewTextBoxColumn10,
            this.dataGridViewTextBoxColumn11});
            this.m_apiHeaderDataGridView.Location = new System.Drawing.Point(30, 53);
            this.m_apiHeaderDataGridView.Name = "m_apiHeaderDataGridView";
            this.m_apiHeaderDataGridView.ReadOnly = true;
            this.m_apiHeaderDataGridView.Size = new System.Drawing.Size(1044, 150);
            this.m_apiHeaderDataGridView.TabIndex = 6;
            // 
            // dataGridViewTextBoxColumn1
            // 
            this.dataGridViewTextBoxColumn1.HeaderText = "Format Type";
            this.dataGridViewTextBoxColumn1.Name = "dataGridViewTextBoxColumn1";
            this.dataGridViewTextBoxColumn1.ReadOnly = true;
            // 
            // dataGridViewTextBoxColumn2
            // 
            this.dataGridViewTextBoxColumn2.HeaderText = "Part Number";
            this.dataGridViewTextBoxColumn2.Name = "dataGridViewTextBoxColumn2";
            this.dataGridViewTextBoxColumn2.ReadOnly = true;
            // 
            // dataGridViewTextBoxColumn3
            // 
            this.dataGridViewTextBoxColumn3.HeaderText = "Block Number";
            this.dataGridViewTextBoxColumn3.Name = "dataGridViewTextBoxColumn3";
            this.dataGridViewTextBoxColumn3.ReadOnly = true;
            // 
            // dataGridViewTextBoxColumn4
            // 
            this.dataGridViewTextBoxColumn4.HeaderText = "Number of Blocks";
            this.dataGridViewTextBoxColumn4.Name = "dataGridViewTextBoxColumn4";
            this.dataGridViewTextBoxColumn4.ReadOnly = true;
            // 
            // dataGridViewTextBoxColumn5
            // 
            this.dataGridViewTextBoxColumn5.HeaderText = "Data Creation Date";
            this.dataGridViewTextBoxColumn5.Name = "dataGridViewTextBoxColumn5";
            this.dataGridViewTextBoxColumn5.ReadOnly = true;
            // 
            // dataGridViewTextBoxColumn6
            // 
            this.dataGridViewTextBoxColumn6.HeaderText = "Data Type";
            this.dataGridViewTextBoxColumn6.Name = "dataGridViewTextBoxColumn6";
            this.dataGridViewTextBoxColumn6.ReadOnly = true;
            // 
            // dataGridViewTextBoxColumn7
            // 
            this.dataGridViewTextBoxColumn7.HeaderText = "Spare";
            this.dataGridViewTextBoxColumn7.Name = "dataGridViewTextBoxColumn7";
            this.dataGridViewTextBoxColumn7.ReadOnly = true;
            // 
            // dataGridViewTextBoxColumn8
            // 
            this.dataGridViewTextBoxColumn8.HeaderText = "Number of Address Bytes";
            this.dataGridViewTextBoxColumn8.Name = "dataGridViewTextBoxColumn8";
            this.dataGridViewTextBoxColumn8.ReadOnly = true;
            // 
            // dataGridViewTextBoxColumn9
            // 
            this.dataGridViewTextBoxColumn9.HeaderText = "Number of Data Bytes";
            this.dataGridViewTextBoxColumn9.Name = "dataGridViewTextBoxColumn9";
            this.dataGridViewTextBoxColumn9.ReadOnly = true;
            // 
            // dataGridViewTextBoxColumn10
            // 
            this.dataGridViewTextBoxColumn10.HeaderText = "CRC type";
            this.dataGridViewTextBoxColumn10.Name = "dataGridViewTextBoxColumn10";
            this.dataGridViewTextBoxColumn10.ReadOnly = true;
            // 
            // dataGridViewTextBoxColumn11
            // 
            this.dataGridViewTextBoxColumn11.HeaderText = "Number Of CRC Bytes";
            this.dataGridViewTextBoxColumn11.Name = "dataGridViewTextBoxColumn11";
            this.dataGridViewTextBoxColumn11.ReadOnly = true;
            // 
            // label4
            // 
            this.label4.AutoSize = true;
            this.label4.Font = new System.Drawing.Font("Microsoft Sans Serif", 15.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.label4.Location = new System.Drawing.Point(25, 25);
            this.label4.Name = "label4";
            this.label4.Size = new System.Drawing.Size(121, 25);
            this.label4.TabIndex = 7;
            this.label4.Text = "API Header";
            // 
            // m_calFilesDataGridView
            // 
            this.m_calFilesDataGridView.AllowUserToAddRows = false;
            this.m_calFilesDataGridView.AllowUserToDeleteRows = false;
            this.m_calFilesDataGridView.ColumnHeadersHeightSizeMode = System.Windows.Forms.DataGridViewColumnHeadersHeightSizeMode.AutoSize;
            this.m_calFilesDataGridView.Columns.AddRange(new System.Windows.Forms.DataGridViewColumn[] {
            this.dataGridViewTextBoxColumn14});
            this.m_calFilesDataGridView.Location = new System.Drawing.Point(30, 732);
            this.m_calFilesDataGridView.Name = "m_calFilesDataGridView";
            this.m_calFilesDataGridView.ReadOnly = true;
            this.m_calFilesDataGridView.Size = new System.Drawing.Size(1044, 134);
            this.m_calFilesDataGridView.TabIndex = 8;
            // 
            // label5
            // 
            this.label5.AutoSize = true;
            this.label5.Font = new System.Drawing.Font("Microsoft Sans Serif", 15.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.label5.Location = new System.Drawing.Point(25, 704);
            this.label5.Name = "label5";
            this.label5.Size = new System.Drawing.Size(89, 25);
            this.label5.TabIndex = 9;
            this.label5.Text = "Cal files";
            // 
            // dataGridViewTextBoxColumn14
            // 
            this.dataGridViewTextBoxColumn14.HeaderText = "Data";
            this.dataGridViewTextBoxColumn14.Name = "dataGridViewTextBoxColumn14";
            this.dataGridViewTextBoxColumn14.ReadOnly = true;
            // 
            // UtilityFileDisplay
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(1097, 878);
            this.Controls.Add(this.label5);
            this.Controls.Add(this.m_calFilesDataGridView);
            this.Controls.Add(this.label4);
            this.Controls.Add(this.m_apiHeaderDataGridView);
            this.Controls.Add(this.label3);
            this.Controls.Add(this.m_routineDataGridView);
            this.Controls.Add(this.label2);
            this.Controls.Add(this.m_interpInstDataGridView);
            this.Controls.Add(this.label1);
            this.Controls.Add(this.m_headerDataGridView);
            this.Name = "UtilityFileDisplay";
            this.Text = "UtilityFileDisplay";
            ((System.ComponentModel.ISupportInitialize)(this.m_headerDataGridView)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.m_interpInstDataGridView)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.m_routineDataGridView)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.m_apiHeaderDataGridView)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.m_calFilesDataGridView)).EndInit();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.DataGridView m_headerDataGridView;
        private System.Windows.Forms.DataGridViewTextBoxColumn Checksum;
        private System.Windows.Forms.DataGridViewTextBoxColumn ModuleID;
        private System.Windows.Forms.DataGridViewTextBoxColumn PartNumber;
        private System.Windows.Forms.DataGridViewTextBoxColumn DesignLevel;
        private System.Windows.Forms.DataGridViewTextBoxColumn HeaderType;
        private System.Windows.Forms.DataGridViewTextBoxColumn InterpreterType;
        private System.Windows.Forms.DataGridViewTextBoxColumn RoutineOffset;
        private System.Windows.Forms.DataGridViewTextBoxColumn Addressing;
        private System.Windows.Forms.DataGridViewTextBoxColumn DataAddress;
        private System.Windows.Forms.DataGridViewTextBoxColumn DataBytesInMessage;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.DataGridView m_interpInstDataGridView;
        private System.Windows.Forms.DataGridViewTextBoxColumn Step;
        private System.Windows.Forms.DataGridViewTextBoxColumn OpCode;
        private System.Windows.Forms.DataGridViewTextBoxColumn ActionFields;
        private System.Windows.Forms.DataGridViewTextBoxColumn GotoFields;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.DataGridView m_routineDataGridView;
        private System.Windows.Forms.DataGridViewTextBoxColumn AddressInfo;
        private System.Windows.Forms.DataGridViewTextBoxColumn LengthOfRoutine;
        private System.Windows.Forms.DataGridViewTextBoxColumn Data;
        private System.Windows.Forms.Label label3;
        private System.Windows.Forms.DataGridView m_apiHeaderDataGridView;
        private System.Windows.Forms.DataGridViewTextBoxColumn dataGridViewTextBoxColumn1;
        private System.Windows.Forms.DataGridViewTextBoxColumn dataGridViewTextBoxColumn2;
        private System.Windows.Forms.DataGridViewTextBoxColumn dataGridViewTextBoxColumn3;
        private System.Windows.Forms.DataGridViewTextBoxColumn dataGridViewTextBoxColumn4;
        private System.Windows.Forms.DataGridViewTextBoxColumn dataGridViewTextBoxColumn5;
        private System.Windows.Forms.DataGridViewTextBoxColumn dataGridViewTextBoxColumn6;
        private System.Windows.Forms.DataGridViewTextBoxColumn dataGridViewTextBoxColumn7;
        private System.Windows.Forms.DataGridViewTextBoxColumn dataGridViewTextBoxColumn8;
        private System.Windows.Forms.DataGridViewTextBoxColumn dataGridViewTextBoxColumn9;
        private System.Windows.Forms.DataGridViewTextBoxColumn dataGridViewTextBoxColumn10;
        private System.Windows.Forms.DataGridViewTextBoxColumn dataGridViewTextBoxColumn11;
        private System.Windows.Forms.Label label4;
        private System.Windows.Forms.DataGridView m_calFilesDataGridView;
        private System.Windows.Forms.DataGridViewTextBoxColumn dataGridViewTextBoxColumn14;
        private System.Windows.Forms.Label label5;
    }
}