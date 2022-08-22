namespace MesReportGenerator
{
    partial class CcrtReportGenerator
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
            this.components = new System.ComponentModel.Container();
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(CcrtReportGenerator));
            this.m_checkForResultsButton = new System.Windows.Forms.Button();
            this.label1 = new System.Windows.Forms.Label();
            this.label2 = new System.Windows.Forms.Label();
            this.label3 = new System.Windows.Forms.Label();
            this.m_browseSchemaDirectory = new System.Windows.Forms.Button();
            this.m_browseReportDirectory = new System.Windows.Forms.Button();
            this.m_browseResultDirectory = new System.Windows.Forms.Button();
            this.m_schema = new System.Windows.Forms.ComboBox();
            this.m_reportDirectory = new System.Windows.Forms.ComboBox();
            this.m_resultDirectory = new System.Windows.Forms.ComboBox();
            this.m_msgTextBox = new System.Windows.Forms.TextBox();
            this.m_msgLabel = new System.Windows.Forms.Label();
            this.m_reportTimer = new System.Windows.Forms.Timer(this.components);
            this.m_selectResultFile = new System.Windows.Forms.Button();
            this.label4 = new System.Windows.Forms.Label();
            this.m_archiveDirectory = new System.Windows.Forms.ComboBox();
            this.label5 = new System.Windows.Forms.Label();
            this.m_browseArchiveDirectory = new System.Windows.Forms.Button();
            this.label6 = new System.Windows.Forms.Label();
            this.m_QNXaddress = new System.Windows.Forms.TextBox();
            this.m_updateQNXaddress = new System.Windows.Forms.Button();
            this.SuspendLayout();
            // 
            // m_checkForResultsButton
            // 
            this.m_checkForResultsButton.Location = new System.Drawing.Point(608, 175);
            this.m_checkForResultsButton.Name = "m_checkForResultsButton";
            this.m_checkForResultsButton.Size = new System.Drawing.Size(163, 22);
            this.m_checkForResultsButton.TabIndex = 0;
            this.m_checkForResultsButton.Text = "Check For Results";
            this.m_checkForResultsButton.UseVisualStyleBackColor = true;
            this.m_checkForResultsButton.Click += new System.EventHandler(this.button1_Click);
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.label1.Location = new System.Drawing.Point(67, 116);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(98, 13);
            this.label1.TabIndex = 30;
            this.label1.Text = "Report Schema:";
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.label2.Location = new System.Drawing.Point(67, 61);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(147, 13);
            this.label2.TabIndex = 29;
            this.label2.Text = "MES Report Destination:";
            // 
            // label3
            // 
            this.label3.AutoSize = true;
            this.label3.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.label3.Location = new System.Drawing.Point(67, 35);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(160, 13);
            this.label3.TabIndex = 28;
            this.label3.Text = "Raw Test Result Directory:";
            // 
            // m_browseSchemaDirectory
            // 
            this.m_browseSchemaDirectory.Location = new System.Drawing.Point(666, 112);
            this.m_browseSchemaDirectory.Name = "m_browseSchemaDirectory";
            this.m_browseSchemaDirectory.Size = new System.Drawing.Size(68, 21);
            this.m_browseSchemaDirectory.TabIndex = 27;
            this.m_browseSchemaDirectory.Text = "Browse";
            this.m_browseSchemaDirectory.UseVisualStyleBackColor = true;
            this.m_browseSchemaDirectory.Click += new System.EventHandler(this.m_browseSchemaDirectory_Click);
            // 
            // m_browseReportDirectory
            // 
            this.m_browseReportDirectory.Location = new System.Drawing.Point(666, 58);
            this.m_browseReportDirectory.Name = "m_browseReportDirectory";
            this.m_browseReportDirectory.Size = new System.Drawing.Size(68, 21);
            this.m_browseReportDirectory.TabIndex = 26;
            this.m_browseReportDirectory.Text = "Browse";
            this.m_browseReportDirectory.UseVisualStyleBackColor = true;
            this.m_browseReportDirectory.Click += new System.EventHandler(this.m_browseReportDirectory_Click);
            // 
            // m_browseResultDirectory
            // 
            this.m_browseResultDirectory.Location = new System.Drawing.Point(666, 31);
            this.m_browseResultDirectory.Name = "m_browseResultDirectory";
            this.m_browseResultDirectory.Size = new System.Drawing.Size(68, 21);
            this.m_browseResultDirectory.TabIndex = 25;
            this.m_browseResultDirectory.Text = "Browse";
            this.m_browseResultDirectory.UseVisualStyleBackColor = true;
            this.m_browseResultDirectory.Click += new System.EventHandler(this.m_browseResultDirectory_Click);
            // 
            // m_schema
            // 
            this.m_schema.FormattingEnabled = true;
            this.m_schema.Location = new System.Drawing.Point(233, 112);
            this.m_schema.Name = "m_schema";
            this.m_schema.Size = new System.Drawing.Size(390, 21);
            this.m_schema.TabIndex = 24;
            this.m_schema.SelectedIndexChanged += new System.EventHandler(this.m_schema_SelectedIndexChanged);
            this.m_schema.KeyPress += new System.Windows.Forms.KeyPressEventHandler(this.m_schema_KeyPress);
            // 
            // m_reportDirectory
            // 
            this.m_reportDirectory.FormattingEnabled = true;
            this.m_reportDirectory.Location = new System.Drawing.Point(233, 58);
            this.m_reportDirectory.Name = "m_reportDirectory";
            this.m_reportDirectory.Size = new System.Drawing.Size(390, 21);
            this.m_reportDirectory.TabIndex = 23;
            this.m_reportDirectory.SelectedIndexChanged += new System.EventHandler(this.m_reportDirectory_SelectedIndexChanged);
            this.m_reportDirectory.KeyPress += new System.Windows.Forms.KeyPressEventHandler(this.m_reportDirectory_KeyPress);
            // 
            // m_resultDirectory
            // 
            this.m_resultDirectory.FormattingEnabled = true;
            this.m_resultDirectory.Location = new System.Drawing.Point(233, 31);
            this.m_resultDirectory.Name = "m_resultDirectory";
            this.m_resultDirectory.Size = new System.Drawing.Size(390, 21);
            this.m_resultDirectory.TabIndex = 22;
            this.m_resultDirectory.SelectedIndexChanged += new System.EventHandler(this.m_resultDirectory_SelectedIndexChanged);
            this.m_resultDirectory.KeyPress += new System.Windows.Forms.KeyPressEventHandler(this.m_resultDirectory_KeyPress);
            // 
            // m_msgTextBox
            // 
            this.m_msgTextBox.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.m_msgTextBox.Location = new System.Drawing.Point(59, 203);
            this.m_msgTextBox.Multiline = true;
            this.m_msgTextBox.Name = "m_msgTextBox";
            this.m_msgTextBox.ReadOnly = true;
            this.m_msgTextBox.ScrollBars = System.Windows.Forms.ScrollBars.Both;
            this.m_msgTextBox.Size = new System.Drawing.Size(712, 109);
            this.m_msgTextBox.TabIndex = 20;
            // 
            // m_msgLabel
            // 
            this.m_msgLabel.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.m_msgLabel.AutoSize = true;
            this.m_msgLabel.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.m_msgLabel.Location = new System.Drawing.Point(67, 187);
            this.m_msgLabel.Name = "m_msgLabel";
            this.m_msgLabel.Size = new System.Drawing.Size(67, 13);
            this.m_msgLabel.TabIndex = 19;
            this.m_msgLabel.Text = "Messages:";
            // 
            // m_reportTimer
            // 
            this.m_reportTimer.Enabled = true;
            this.m_reportTimer.Interval = 1000;
            this.m_reportTimer.Tick += new System.EventHandler(this.m_reportTimer_Tick_1);
            // 
            // m_selectResultFile
            // 
            this.m_selectResultFile.Location = new System.Drawing.Point(666, 318);
            this.m_selectResultFile.Name = "m_selectResultFile";
            this.m_selectResultFile.Size = new System.Drawing.Size(68, 21);
            this.m_selectResultFile.TabIndex = 31;
            this.m_selectResultFile.Text = "Browse";
            this.m_selectResultFile.UseVisualStyleBackColor = true;
            this.m_selectResultFile.Click += new System.EventHandler(this.m_selectResultFile_Click);
            // 
            // label4
            // 
            this.label4.AutoSize = true;
            this.label4.Location = new System.Drawing.Point(423, 322);
            this.label4.Name = "label4";
            this.label4.Size = new System.Drawing.Size(243, 13);
            this.label4.TabIndex = 32;
            this.label4.Text = "To select a single file for MES Report conversion: ";
            // 
            // m_archiveDirectory
            // 
            this.m_archiveDirectory.FormattingEnabled = true;
            this.m_archiveDirectory.Location = new System.Drawing.Point(233, 85);
            this.m_archiveDirectory.Name = "m_archiveDirectory";
            this.m_archiveDirectory.Size = new System.Drawing.Size(390, 21);
            this.m_archiveDirectory.TabIndex = 33;
            this.m_archiveDirectory.SelectedIndexChanged += new System.EventHandler(this.m_archiveDirectory_SelectedIndexChanged);
            // 
            // label5
            // 
            this.label5.AutoSize = true;
            this.label5.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.label5.Location = new System.Drawing.Point(67, 89);
            this.label5.Name = "label5";
            this.label5.Size = new System.Drawing.Size(123, 13);
            this.label5.TabIndex = 34;
            this.label5.Text = "Raw Result Archive:";
            // 
            // m_browseArchiveDirectory
            // 
            this.m_browseArchiveDirectory.Location = new System.Drawing.Point(666, 85);
            this.m_browseArchiveDirectory.Name = "m_browseArchiveDirectory";
            this.m_browseArchiveDirectory.Size = new System.Drawing.Size(68, 21);
            this.m_browseArchiveDirectory.TabIndex = 35;
            this.m_browseArchiveDirectory.Text = "Browse";
            this.m_browseArchiveDirectory.UseVisualStyleBackColor = true;
            this.m_browseArchiveDirectory.Click += new System.EventHandler(this.m_browseArchiveDirectory_Click);
            // 
            // label6
            // 
            this.label6.AutoSize = true;
            this.label6.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.label6.Location = new System.Drawing.Point(67, 142);
            this.label6.Name = "label6";
            this.label6.Size = new System.Drawing.Size(53, 13);
            this.label6.TabIndex = 36;
            this.label6.Text = "QNX IP:";
            // 
            // m_QNXaddress
            // 
            this.m_QNXaddress.Location = new System.Drawing.Point(233, 139);
            this.m_QNXaddress.Name = "m_QNXaddress";
            this.m_QNXaddress.Size = new System.Drawing.Size(390, 20);
            this.m_QNXaddress.TabIndex = 37;
            // 
            // m_updateQNXaddress
            // 
            this.m_updateQNXaddress.Location = new System.Drawing.Point(666, 138);
            this.m_updateQNXaddress.Name = "m_updateQNXaddress";
            this.m_updateQNXaddress.Size = new System.Drawing.Size(68, 21);
            this.m_updateQNXaddress.TabIndex = 38;
            this.m_updateQNXaddress.Text = "Update";
            this.m_updateQNXaddress.UseVisualStyleBackColor = true;
            this.m_updateQNXaddress.Click += new System.EventHandler(this.m_updateQNXaddress_Click);
            // 
            // CcrtReportGenerator
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(841, 346);
            this.Controls.Add(this.m_updateQNXaddress);
            this.Controls.Add(this.m_QNXaddress);
            this.Controls.Add(this.label6);
            this.Controls.Add(this.m_browseArchiveDirectory);
            this.Controls.Add(this.label5);
            this.Controls.Add(this.m_archiveDirectory);
            this.Controls.Add(this.label4);
            this.Controls.Add(this.m_selectResultFile);
            this.Controls.Add(this.label1);
            this.Controls.Add(this.label2);
            this.Controls.Add(this.label3);
            this.Controls.Add(this.m_browseSchemaDirectory);
            this.Controls.Add(this.m_browseReportDirectory);
            this.Controls.Add(this.m_browseResultDirectory);
            this.Controls.Add(this.m_schema);
            this.Controls.Add(this.m_reportDirectory);
            this.Controls.Add(this.m_resultDirectory);
            this.Controls.Add(this.m_msgTextBox);
            this.Controls.Add(this.m_msgLabel);
            this.Controls.Add(this.m_checkForResultsButton);
            this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
            this.Name = "CcrtReportGenerator";
            this.Text = "Report Generator";
            this.FormClosing += new System.Windows.Forms.FormClosingEventHandler(this.CcrtReportGenerator_FormClosing);
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Button m_checkForResultsButton;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.Label label3;
        private System.Windows.Forms.Button m_browseSchemaDirectory;
        private System.Windows.Forms.Button m_browseReportDirectory;
        private System.Windows.Forms.Button m_browseResultDirectory;
        private System.Windows.Forms.ComboBox m_schema;
        private System.Windows.Forms.ComboBox m_reportDirectory;
        private System.Windows.Forms.ComboBox m_resultDirectory;
        private System.Windows.Forms.TextBox m_msgTextBox;
        private System.Windows.Forms.Label m_msgLabel;
        private System.Windows.Forms.Timer m_reportTimer;
        private System.Windows.Forms.Button m_selectResultFile;
        private System.Windows.Forms.Label label4;
        private System.Windows.Forms.ComboBox m_archiveDirectory;
        private System.Windows.Forms.Label label5;
        private System.Windows.Forms.Button m_browseArchiveDirectory;
        private System.Windows.Forms.Label label6;
        private System.Windows.Forms.TextBox m_QNXaddress;
        private System.Windows.Forms.Button m_updateQNXaddress;
    }
}

