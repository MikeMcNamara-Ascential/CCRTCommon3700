namespace UtilityFileInterpreterViewer
{
    partial class OpCodeTestProgram
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
            this.m_startTestButton = new System.Windows.Forms.Button();
            this.m_testDataGridView = new System.Windows.Forms.DataGridView();
            this.TestName = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.Result = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.m_gmlanTestsDataGridView = new System.Windows.Forms.DataGridView();
            this.dataGridViewTextBoxColumn1 = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.dataGridViewTextBoxColumn2 = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.label1 = new System.Windows.Forms.Label();
            this.label2 = new System.Windows.Forms.Label();
            this.button1 = new System.Windows.Forms.Button();
            ((System.ComponentModel.ISupportInitialize)(this.m_testDataGridView)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.m_gmlanTestsDataGridView)).BeginInit();
            this.SuspendLayout();
            // 
            // m_startTestButton
            // 
            this.m_startTestButton.Location = new System.Drawing.Point(23, 20);
            this.m_startTestButton.Name = "m_startTestButton";
            this.m_startTestButton.Size = new System.Drawing.Size(75, 23);
            this.m_startTestButton.TabIndex = 0;
            this.m_startTestButton.Text = "Start Test";
            this.m_startTestButton.UseVisualStyleBackColor = true;
            this.m_startTestButton.Click += new System.EventHandler(this.m_startTestButton_Click);
            // 
            // m_testDataGridView
            // 
            this.m_testDataGridView.AllowUserToAddRows = false;
            this.m_testDataGridView.AllowUserToDeleteRows = false;
            this.m_testDataGridView.ColumnHeadersHeightSizeMode = System.Windows.Forms.DataGridViewColumnHeadersHeightSizeMode.AutoSize;
            this.m_testDataGridView.Columns.AddRange(new System.Windows.Forms.DataGridViewColumn[] {
            this.TestName,
            this.Result});
            this.m_testDataGridView.Location = new System.Drawing.Point(23, 75);
            this.m_testDataGridView.Name = "m_testDataGridView";
            this.m_testDataGridView.ReadOnly = true;
            this.m_testDataGridView.Size = new System.Drawing.Size(244, 179);
            this.m_testDataGridView.TabIndex = 1;
            // 
            // TestName
            // 
            this.TestName.HeaderText = "Test Name";
            this.TestName.Name = "TestName";
            this.TestName.ReadOnly = true;
            // 
            // Result
            // 
            this.Result.HeaderText = "Result";
            this.Result.Name = "Result";
            this.Result.ReadOnly = true;
            // 
            // m_gmlanTestsDataGridView
            // 
            this.m_gmlanTestsDataGridView.AllowUserToAddRows = false;
            this.m_gmlanTestsDataGridView.AllowUserToDeleteRows = false;
            this.m_gmlanTestsDataGridView.ColumnHeadersHeightSizeMode = System.Windows.Forms.DataGridViewColumnHeadersHeightSizeMode.AutoSize;
            this.m_gmlanTestsDataGridView.Columns.AddRange(new System.Windows.Forms.DataGridViewColumn[] {
            this.dataGridViewTextBoxColumn1,
            this.dataGridViewTextBoxColumn2});
            this.m_gmlanTestsDataGridView.Location = new System.Drawing.Point(23, 272);
            this.m_gmlanTestsDataGridView.Name = "m_gmlanTestsDataGridView";
            this.m_gmlanTestsDataGridView.ReadOnly = true;
            this.m_gmlanTestsDataGridView.Size = new System.Drawing.Size(244, 179);
            this.m_gmlanTestsDataGridView.TabIndex = 2;
            // 
            // dataGridViewTextBoxColumn1
            // 
            this.dataGridViewTextBoxColumn1.HeaderText = "Test Name";
            this.dataGridViewTextBoxColumn1.Name = "dataGridViewTextBoxColumn1";
            this.dataGridViewTextBoxColumn1.ReadOnly = true;
            // 
            // dataGridViewTextBoxColumn2
            // 
            this.dataGridViewTextBoxColumn2.HeaderText = "Result";
            this.dataGridViewTextBoxColumn2.Name = "dataGridViewTextBoxColumn2";
            this.dataGridViewTextBoxColumn2.ReadOnly = true;
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(20, 59);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(98, 13);
            this.label1.TabIndex = 3;
            this.label1.Text = "Common Op Codes";
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Location = new System.Drawing.Point(20, 256);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(95, 13);
            this.label2.TabIndex = 4;
            this.label2.Text = "GMLAN Op Codes";
            // 
            // button1
            // 
            this.button1.Location = new System.Drawing.Point(192, 20);
            this.button1.Name = "button1";
            this.button1.Size = new System.Drawing.Size(75, 23);
            this.button1.TabIndex = 5;
            this.button1.Text = "Mem Test";
            this.button1.UseVisualStyleBackColor = true;
            this.button1.Click += new System.EventHandler(this.button1_Click);
            // 
            // OpCodeTestProgram
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(292, 463);
            this.Controls.Add(this.button1);
            this.Controls.Add(this.label2);
            this.Controls.Add(this.label1);
            this.Controls.Add(this.m_gmlanTestsDataGridView);
            this.Controls.Add(this.m_testDataGridView);
            this.Controls.Add(this.m_startTestButton);
            this.Name = "OpCodeTestProgram";
            this.Text = "OpCodeTestProgram";
            ((System.ComponentModel.ISupportInitialize)(this.m_testDataGridView)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.m_gmlanTestsDataGridView)).EndInit();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Button m_startTestButton;
        private System.Windows.Forms.DataGridView m_testDataGridView;
        private System.Windows.Forms.DataGridViewTextBoxColumn TestName;
        private System.Windows.Forms.DataGridViewTextBoxColumn Result;
        private System.Windows.Forms.DataGridView m_gmlanTestsDataGridView;
        private System.Windows.Forms.DataGridViewTextBoxColumn dataGridViewTextBoxColumn1;
        private System.Windows.Forms.DataGridViewTextBoxColumn dataGridViewTextBoxColumn2;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.Button button1;
    }
}