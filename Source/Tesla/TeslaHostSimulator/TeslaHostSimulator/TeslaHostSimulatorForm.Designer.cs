namespace TeslaHostSimulator
{
    partial class TeslaHostSimulator
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
            System.Windows.Forms.DataGridViewCellStyle dataGridViewCellStyle5 = new System.Windows.Forms.DataGridViewCellStyle();
            System.Windows.Forms.DataGridViewCellStyle dataGridViewCellStyle6 = new System.Windows.Forms.DataGridViewCellStyle();
            System.Windows.Forms.DataGridViewCellStyle dataGridViewCellStyle7 = new System.Windows.Forms.DataGridViewCellStyle();
            System.Windows.Forms.DataGridViewCellStyle dataGridViewCellStyle8 = new System.Windows.Forms.DataGridViewCellStyle();
            this.menuStrip1 = new System.Windows.Forms.MenuStrip();
            this.fileToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.exitToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.setupToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.commPortToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.label1 = new System.Windows.Forms.Label();
            this.m_txMsgsComboBox = new System.Windows.Forms.ComboBox();
            this.m_txMsgButton = new System.Windows.Forms.Button();
            this.m_logTextBox = new System.Windows.Forms.TextBox();
            this.label2 = new System.Windows.Forms.Label();
            this.m_icmDataGridView = new System.Windows.Forms.DataGridView();
            this.RollerName = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.RollerSpeed = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.RollerForce = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.label3 = new System.Windows.Forms.Label();
            this.m_resultsDataGridView = new System.Windows.Forms.DataGridView();
            this.ResultRollerName = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.DragValues = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.BrakeValues = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.AbsMinValues = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.AbsMaxValues = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.menuStrip1.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.m_icmDataGridView)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.m_resultsDataGridView)).BeginInit();
            this.SuspendLayout();
            // 
            // menuStrip1
            // 
            this.menuStrip1.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.fileToolStripMenuItem,
            this.setupToolStripMenuItem});
            this.menuStrip1.Location = new System.Drawing.Point(0, 0);
            this.menuStrip1.Name = "menuStrip1";
            this.menuStrip1.Size = new System.Drawing.Size(632, 24);
            this.menuStrip1.TabIndex = 0;
            this.menuStrip1.Text = "menuStrip1";
            // 
            // fileToolStripMenuItem
            // 
            this.fileToolStripMenuItem.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.exitToolStripMenuItem});
            this.fileToolStripMenuItem.Name = "fileToolStripMenuItem";
            this.fileToolStripMenuItem.Size = new System.Drawing.Size(37, 20);
            this.fileToolStripMenuItem.Text = "File";
            // 
            // exitToolStripMenuItem
            // 
            this.exitToolStripMenuItem.Name = "exitToolStripMenuItem";
            this.exitToolStripMenuItem.Size = new System.Drawing.Size(92, 22);
            this.exitToolStripMenuItem.Text = "Exit";
            this.exitToolStripMenuItem.Click += new System.EventHandler(this.exitToolStripMenuItem_Click);
            // 
            // setupToolStripMenuItem
            // 
            this.setupToolStripMenuItem.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.commPortToolStripMenuItem});
            this.setupToolStripMenuItem.Name = "setupToolStripMenuItem";
            this.setupToolStripMenuItem.Size = new System.Drawing.Size(49, 20);
            this.setupToolStripMenuItem.Text = "Setup";
            // 
            // commPortToolStripMenuItem
            // 
            this.commPortToolStripMenuItem.Name = "commPortToolStripMenuItem";
            this.commPortToolStripMenuItem.Size = new System.Drawing.Size(136, 22);
            this.commPortToolStripMenuItem.Text = "Comm Port";
            this.commPortToolStripMenuItem.Click += new System.EventHandler(this.commPortToolStripMenuItem_Click);
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Font = new System.Drawing.Font("Microsoft Sans Serif", 10F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.label1.Location = new System.Drawing.Point(12, 58);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(98, 17);
            this.label1.TabIndex = 1;
            this.label1.Text = "TX Messages:";
            // 
            // m_txMsgsComboBox
            // 
            this.m_txMsgsComboBox.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.m_txMsgsComboBox.Font = new System.Drawing.Font("Microsoft Sans Serif", 10F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.m_txMsgsComboBox.FormattingEnabled = true;
            this.m_txMsgsComboBox.Items.AddRange(new object[] {
            "Start Force Measurement - Drag",
            "Start Force Measurement - Brake",
            "Stop Force Measurement - Drag",
            "Stop Force Measurement - Brake",
            "Get Measured Force - Drag",
            "Get Measured Force - Brake",
            "Start ABS Test",
            "Stop ABS Test",
            "Get ABS Results",
            "Start Test Cycle",
            "Stop Test Cycle"});
            this.m_txMsgsComboBox.Location = new System.Drawing.Point(116, 55);
            this.m_txMsgsComboBox.Name = "m_txMsgsComboBox";
            this.m_txMsgsComboBox.Size = new System.Drawing.Size(251, 24);
            this.m_txMsgsComboBox.TabIndex = 2;
            // 
            // m_txMsgButton
            // 
            this.m_txMsgButton.Font = new System.Drawing.Font("Microsoft Sans Serif", 10F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.m_txMsgButton.Location = new System.Drawing.Point(390, 51);
            this.m_txMsgButton.Name = "m_txMsgButton";
            this.m_txMsgButton.Size = new System.Drawing.Size(96, 31);
            this.m_txMsgButton.TabIndex = 3;
            this.m_txMsgButton.Text = "Send Msg";
            this.m_txMsgButton.UseVisualStyleBackColor = true;
            this.m_txMsgButton.Click += new System.EventHandler(this.m_txMsgButton_Click);
            // 
            // m_logTextBox
            // 
            this.m_logTextBox.Location = new System.Drawing.Point(15, 121);
            this.m_logTextBox.Multiline = true;
            this.m_logTextBox.Name = "m_logTextBox";
            this.m_logTextBox.ScrollBars = System.Windows.Forms.ScrollBars.Both;
            this.m_logTextBox.Size = new System.Drawing.Size(352, 150);
            this.m_logTextBox.TabIndex = 4;
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Location = new System.Drawing.Point(15, 102);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(79, 13);
            this.label2.TabIndex = 5;
            this.label2.Text = "Log Messages:";
            // 
            // m_icmDataGridView
            // 
            this.m_icmDataGridView.AllowUserToAddRows = false;
            this.m_icmDataGridView.AllowUserToDeleteRows = false;
            this.m_icmDataGridView.AllowUserToResizeColumns = false;
            this.m_icmDataGridView.AllowUserToResizeRows = false;
            this.m_icmDataGridView.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.m_icmDataGridView.AutoSizeColumnsMode = System.Windows.Forms.DataGridViewAutoSizeColumnsMode.Fill;
            this.m_icmDataGridView.AutoSizeRowsMode = System.Windows.Forms.DataGridViewAutoSizeRowsMode.AllCells;
            dataGridViewCellStyle5.Alignment = System.Windows.Forms.DataGridViewContentAlignment.MiddleCenter;
            dataGridViewCellStyle5.BackColor = System.Drawing.SystemColors.Control;
            dataGridViewCellStyle5.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            dataGridViewCellStyle5.ForeColor = System.Drawing.SystemColors.WindowText;
            dataGridViewCellStyle5.SelectionBackColor = System.Drawing.SystemColors.Highlight;
            dataGridViewCellStyle5.SelectionForeColor = System.Drawing.SystemColors.HighlightText;
            dataGridViewCellStyle5.WrapMode = System.Windows.Forms.DataGridViewTriState.True;
            this.m_icmDataGridView.ColumnHeadersDefaultCellStyle = dataGridViewCellStyle5;
            this.m_icmDataGridView.ColumnHeadersHeightSizeMode = System.Windows.Forms.DataGridViewColumnHeadersHeightSizeMode.AutoSize;
            this.m_icmDataGridView.Columns.AddRange(new System.Windows.Forms.DataGridViewColumn[] {
            this.RollerName,
            this.RollerSpeed,
            this.RollerForce});
            this.m_icmDataGridView.Location = new System.Drawing.Point(390, 121);
            this.m_icmDataGridView.Name = "m_icmDataGridView";
            this.m_icmDataGridView.ReadOnly = true;
            dataGridViewCellStyle6.Alignment = System.Windows.Forms.DataGridViewContentAlignment.MiddleCenter;
            dataGridViewCellStyle6.BackColor = System.Drawing.SystemColors.Control;
            dataGridViewCellStyle6.Font = new System.Drawing.Font("Microsoft Sans Serif", 10F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            dataGridViewCellStyle6.ForeColor = System.Drawing.SystemColors.WindowText;
            dataGridViewCellStyle6.Format = "N2";
            dataGridViewCellStyle6.NullValue = "0.00";
            dataGridViewCellStyle6.SelectionBackColor = System.Drawing.SystemColors.Highlight;
            dataGridViewCellStyle6.SelectionForeColor = System.Drawing.SystemColors.HighlightText;
            dataGridViewCellStyle6.WrapMode = System.Windows.Forms.DataGridViewTriState.True;
            this.m_icmDataGridView.RowHeadersDefaultCellStyle = dataGridViewCellStyle6;
            this.m_icmDataGridView.RowHeadersVisible = false;
            this.m_icmDataGridView.Size = new System.Drawing.Size(230, 150);
            this.m_icmDataGridView.TabIndex = 6;
            // 
            // RollerName
            // 
            this.RollerName.HeaderText = "Roller";
            this.RollerName.Name = "RollerName";
            this.RollerName.ReadOnly = true;
            // 
            // RollerSpeed
            // 
            this.RollerSpeed.HeaderText = "Speed";
            this.RollerSpeed.Name = "RollerSpeed";
            this.RollerSpeed.ReadOnly = true;
            // 
            // RollerForce
            // 
            this.RollerForce.HeaderText = "Force";
            this.RollerForce.Name = "RollerForce";
            this.RollerForce.ReadOnly = true;
            // 
            // label3
            // 
            this.label3.AutoSize = true;
            this.label3.Font = new System.Drawing.Font("Microsoft Sans Serif", 10F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.label3.Location = new System.Drawing.Point(12, 296);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(59, 17);
            this.label3.TabIndex = 7;
            this.label3.Text = "Results:";
            // 
            // m_resultsDataGridView
            // 
            this.m_resultsDataGridView.AllowUserToAddRows = false;
            this.m_resultsDataGridView.AllowUserToDeleteRows = false;
            this.m_resultsDataGridView.AllowUserToResizeColumns = false;
            this.m_resultsDataGridView.AllowUserToResizeRows = false;
            this.m_resultsDataGridView.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
            | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.m_resultsDataGridView.AutoSizeColumnsMode = System.Windows.Forms.DataGridViewAutoSizeColumnsMode.Fill;
            this.m_resultsDataGridView.AutoSizeRowsMode = System.Windows.Forms.DataGridViewAutoSizeRowsMode.AllCells;
            dataGridViewCellStyle7.Alignment = System.Windows.Forms.DataGridViewContentAlignment.MiddleCenter;
            dataGridViewCellStyle7.BackColor = System.Drawing.SystemColors.Control;
            dataGridViewCellStyle7.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            dataGridViewCellStyle7.ForeColor = System.Drawing.SystemColors.WindowText;
            dataGridViewCellStyle7.SelectionBackColor = System.Drawing.SystemColors.Highlight;
            dataGridViewCellStyle7.SelectionForeColor = System.Drawing.SystemColors.HighlightText;
            dataGridViewCellStyle7.WrapMode = System.Windows.Forms.DataGridViewTriState.True;
            this.m_resultsDataGridView.ColumnHeadersDefaultCellStyle = dataGridViewCellStyle7;
            this.m_resultsDataGridView.ColumnHeadersHeightSizeMode = System.Windows.Forms.DataGridViewColumnHeadersHeightSizeMode.AutoSize;
            this.m_resultsDataGridView.Columns.AddRange(new System.Windows.Forms.DataGridViewColumn[] {
            this.ResultRollerName,
            this.DragValues,
            this.BrakeValues,
            this.AbsMinValues,
            this.AbsMaxValues});
            dataGridViewCellStyle8.Alignment = System.Windows.Forms.DataGridViewContentAlignment.MiddleCenter;
            dataGridViewCellStyle8.BackColor = System.Drawing.SystemColors.Window;
            dataGridViewCellStyle8.Font = new System.Drawing.Font("Microsoft Sans Serif", 10F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            dataGridViewCellStyle8.ForeColor = System.Drawing.SystemColors.ControlText;
            dataGridViewCellStyle8.Format = "N2";
            dataGridViewCellStyle8.SelectionBackColor = System.Drawing.SystemColors.Highlight;
            dataGridViewCellStyle8.SelectionForeColor = System.Drawing.SystemColors.HighlightText;
            dataGridViewCellStyle8.WrapMode = System.Windows.Forms.DataGridViewTriState.False;
            this.m_resultsDataGridView.DefaultCellStyle = dataGridViewCellStyle8;
            this.m_resultsDataGridView.Location = new System.Drawing.Point(12, 316);
            this.m_resultsDataGridView.Name = "m_resultsDataGridView";
            this.m_resultsDataGridView.ReadOnly = true;
            this.m_resultsDataGridView.RowHeadersVisible = false;
            this.m_resultsDataGridView.Size = new System.Drawing.Size(608, 190);
            this.m_resultsDataGridView.TabIndex = 8;
            // 
            // ResultRollerName
            // 
            this.ResultRollerName.HeaderText = "Roller";
            this.ResultRollerName.Name = "ResultRollerName";
            this.ResultRollerName.ReadOnly = true;
            // 
            // DragValues
            // 
            this.DragValues.HeaderText = "Drag Results";
            this.DragValues.Name = "DragValues";
            this.DragValues.ReadOnly = true;
            // 
            // BrakeValues
            // 
            this.BrakeValues.HeaderText = "Brake Results";
            this.BrakeValues.Name = "BrakeValues";
            this.BrakeValues.ReadOnly = true;
            // 
            // AbsMinValues
            // 
            this.AbsMinValues.HeaderText = "ABS Results - Min";
            this.AbsMinValues.Name = "AbsMinValues";
            this.AbsMinValues.ReadOnly = true;
            // 
            // AbsMaxValues
            // 
            this.AbsMaxValues.HeaderText = "ABS Results - Max";
            this.AbsMaxValues.Name = "AbsMaxValues";
            this.AbsMaxValues.ReadOnly = true;
            // 
            // TeslaHostSimulator
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(632, 518);
            this.ControlBox = false;
            this.Controls.Add(this.m_resultsDataGridView);
            this.Controls.Add(this.label3);
            this.Controls.Add(this.m_icmDataGridView);
            this.Controls.Add(this.label2);
            this.Controls.Add(this.m_logTextBox);
            this.Controls.Add(this.m_txMsgButton);
            this.Controls.Add(this.m_txMsgsComboBox);
            this.Controls.Add(this.label1);
            this.Controls.Add(this.menuStrip1);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.Fixed3D;
            this.MainMenuStrip = this.menuStrip1;
            this.Name = "TeslaHostSimulator";
            this.Text = "Tesla Host Simulator";
            this.menuStrip1.ResumeLayout(false);
            this.menuStrip1.PerformLayout();
            ((System.ComponentModel.ISupportInitialize)(this.m_icmDataGridView)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.m_resultsDataGridView)).EndInit();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.MenuStrip menuStrip1;
        private System.Windows.Forms.ToolStripMenuItem fileToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem exitToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem setupToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem commPortToolStripMenuItem;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.ComboBox m_txMsgsComboBox;
        private System.Windows.Forms.Button m_txMsgButton;
        private System.Windows.Forms.TextBox m_logTextBox;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.DataGridView m_icmDataGridView;
        private System.Windows.Forms.DataGridViewTextBoxColumn RollerName;
        private System.Windows.Forms.DataGridViewTextBoxColumn RollerSpeed;
        private System.Windows.Forms.DataGridViewTextBoxColumn RollerForce;
        private System.Windows.Forms.Label label3;
        private System.Windows.Forms.DataGridView m_resultsDataGridView;
        private System.Windows.Forms.DataGridViewTextBoxColumn ResultRollerName;
        private System.Windows.Forms.DataGridViewTextBoxColumn DragValues;
        private System.Windows.Forms.DataGridViewTextBoxColumn BrakeValues;
        private System.Windows.Forms.DataGridViewTextBoxColumn AbsMinValues;
        private System.Windows.Forms.DataGridViewTextBoxColumn AbsMaxValues;
    }
}

