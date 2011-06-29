namespace VcEditor
{
    partial class VehicleTypeEditorForm
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
            this.configureToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.vCFileLocationToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.m_vcNumberLabel = new System.Windows.Forms.Label();
            this.m_vcNumberTextBox = new System.Windows.Forms.TextBox();
            this.m_wheelbaseLabel = new System.Windows.Forms.Label();
            this.m_rearAxleWheelbaseTextBox = new System.Windows.Forms.TextBox();
            this.m_driveAxleLabel = new System.Windows.Forms.Label();
            this.m_driveAxleComboBox = new System.Windows.Forms.ComboBox();
            this.m_fourWdCheckBox = new System.Windows.Forms.CheckBox();
            this.m_dragForceGroupBox = new System.Windows.Forms.GroupBox();
            this.m_rearDragTextBox = new System.Windows.Forms.TextBox();
            this.m_frontDragTextBox = new System.Windows.Forms.TextBox();
            this.m_rearDragLabel = new System.Windows.Forms.Label();
            this.m_frontDragLabel = new System.Windows.Forms.Label();
            this.m_parkBrakeParamGroupBox = new System.Windows.Forms.GroupBox();
            this.m_maxPbForceTextBox = new System.Windows.Forms.TextBox();
            this.m_maxPbForceLabel = new System.Windows.Forms.Label();
            this.m_minPbForceTextBox = new System.Windows.Forms.TextBox();
            this.label1 = new System.Windows.Forms.Label();
            this.m_brakeForceGroupBox = new System.Windows.Forms.GroupBox();
            this.m_maxRearBrakeForceTextBox = new System.Windows.Forms.TextBox();
            this.m_minRearBrakeForceTextBox = new System.Windows.Forms.TextBox();
            this.m_maxRearBrakeForceLabel = new System.Windows.Forms.Label();
            this.m_minRearBrakeForceLabel = new System.Windows.Forms.Label();
            this.m_maxFrontBrakeForceTextBox = new System.Windows.Forms.TextBox();
            this.m_minFrontBrakeForceTextBox = new System.Windows.Forms.TextBox();
            this.m_maxFrontBrakeForceLabel = new System.Windows.Forms.Label();
            this.m_minBrakeForceLabel = new System.Windows.Forms.Label();
            this.m_vinCharLabel = new System.Windows.Forms.Label();
            this.m_vinCharTextBox = new System.Windows.Forms.TextBox();
            this.menuStrip1.SuspendLayout();
            this.m_dragForceGroupBox.SuspendLayout();
            this.m_parkBrakeParamGroupBox.SuspendLayout();
            this.m_brakeForceGroupBox.SuspendLayout();
            this.SuspendLayout();
            // 
            // menuStrip1
            // 
            this.menuStrip1.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.fileToolStripMenuItem,
            this.configureToolStripMenuItem});
            this.menuStrip1.Location = new System.Drawing.Point(0, 0);
            this.menuStrip1.Name = "menuStrip1";
            this.menuStrip1.Size = new System.Drawing.Size(540, 24);
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
            // configureToolStripMenuItem
            // 
            this.configureToolStripMenuItem.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.vCFileLocationToolStripMenuItem});
            this.configureToolStripMenuItem.Name = "configureToolStripMenuItem";
            this.configureToolStripMenuItem.Size = new System.Drawing.Size(66, 20);
            this.configureToolStripMenuItem.Text = "Configure";
            // 
            // vCFileLocationToolStripMenuItem
            // 
            this.vCFileLocationToolStripMenuItem.Name = "vCFileLocationToolStripMenuItem";
            this.vCFileLocationToolStripMenuItem.Size = new System.Drawing.Size(169, 22);
            this.vCFileLocationToolStripMenuItem.Text = "Vehicle File Location";
            this.vCFileLocationToolStripMenuItem.Click += new System.EventHandler(this.vCFileLocationToolStripMenuItem_Click);
            // 
            // m_vcNumberLabel
            // 
            this.m_vcNumberLabel.AutoSize = true;
            this.m_vcNumberLabel.Font = new System.Drawing.Font("Microsoft Sans Serif", 9.75F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.m_vcNumberLabel.ForeColor = System.Drawing.Color.Black;
            this.m_vcNumberLabel.Location = new System.Drawing.Point(13, 60);
            this.m_vcNumberLabel.Name = "m_vcNumberLabel";
            this.m_vcNumberLabel.Size = new System.Drawing.Size(104, 16);
            this.m_vcNumberLabel.TabIndex = 1;
            this.m_vcNumberLabel.Text = "Vehicle Type:";
            // 
            // m_vcNumberTextBox
            // 
            this.m_vcNumberTextBox.BackColor = System.Drawing.Color.Khaki;
            this.m_vcNumberTextBox.Font = new System.Drawing.Font("Microsoft Sans Serif", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.m_vcNumberTextBox.Location = new System.Drawing.Point(164, 57);
            this.m_vcNumberTextBox.Name = "m_vcNumberTextBox";
            this.m_vcNumberTextBox.Size = new System.Drawing.Size(149, 22);
            this.m_vcNumberTextBox.TabIndex = 2;
            this.m_vcNumberTextBox.TextChanged += new System.EventHandler(this.m_vcNumberTextBox_TextChanged);
            // 
            // m_wheelbaseLabel
            // 
            this.m_wheelbaseLabel.AutoSize = true;
            this.m_wheelbaseLabel.Font = new System.Drawing.Font("Microsoft Sans Serif", 10F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.m_wheelbaseLabel.Location = new System.Drawing.Point(12, 118);
            this.m_wheelbaseLabel.Name = "m_wheelbaseLabel";
            this.m_wheelbaseLabel.Size = new System.Drawing.Size(134, 17);
            this.m_wheelbaseLabel.TabIndex = 12;
            this.m_wheelbaseLabel.Text = "Wheelbase (mm):";
            // 
            // m_rearAxleWheelbaseTextBox
            // 
            this.m_rearAxleWheelbaseTextBox.BackColor = System.Drawing.Color.Khaki;
            this.m_rearAxleWheelbaseTextBox.Font = new System.Drawing.Font("Microsoft Sans Serif", 10F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.m_rearAxleWheelbaseTextBox.Location = new System.Drawing.Point(164, 115);
            this.m_rearAxleWheelbaseTextBox.MaxLength = 4;
            this.m_rearAxleWheelbaseTextBox.Name = "m_rearAxleWheelbaseTextBox";
            this.m_rearAxleWheelbaseTextBox.Size = new System.Drawing.Size(91, 23);
            this.m_rearAxleWheelbaseTextBox.TabIndex = 13;
            this.m_rearAxleWheelbaseTextBox.TextChanged += new System.EventHandler(this.m_rearAxleWheelbaseTextBox_TextChanged);
            // 
            // m_driveAxleLabel
            // 
            this.m_driveAxleLabel.AutoSize = true;
            this.m_driveAxleLabel.Font = new System.Drawing.Font("Microsoft Sans Serif", 10F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.m_driveAxleLabel.Location = new System.Drawing.Point(290, 118);
            this.m_driveAxleLabel.Name = "m_driveAxleLabel";
            this.m_driveAxleLabel.Size = new System.Drawing.Size(86, 17);
            this.m_driveAxleLabel.TabIndex = 15;
            this.m_driveAxleLabel.Text = "Drive Axle:";
            // 
            // m_driveAxleComboBox
            // 
            this.m_driveAxleComboBox.BackColor = System.Drawing.Color.Khaki;
            this.m_driveAxleComboBox.FormattingEnabled = true;
            this.m_driveAxleComboBox.Items.AddRange(new object[] {
            "Front",
            "Rear"});
            this.m_driveAxleComboBox.Location = new System.Drawing.Point(382, 114);
            this.m_driveAxleComboBox.Name = "m_driveAxleComboBox";
            this.m_driveAxleComboBox.Size = new System.Drawing.Size(91, 21);
            this.m_driveAxleComboBox.TabIndex = 16;
            this.m_driveAxleComboBox.SelectedIndexChanged += new System.EventHandler(this.m_driveAxleComboBox_SelectedIndexChanged);
            // 
            // m_fourWdCheckBox
            // 
            this.m_fourWdCheckBox.AutoSize = true;
            this.m_fourWdCheckBox.Font = new System.Drawing.Font("Microsoft Sans Serif", 10F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.m_fourWdCheckBox.Location = new System.Drawing.Point(423, 59);
            this.m_fourWdCheckBox.Name = "m_fourWdCheckBox";
            this.m_fourWdCheckBox.Size = new System.Drawing.Size(61, 21);
            this.m_fourWdCheckBox.TabIndex = 17;
            this.m_fourWdCheckBox.Text = "4WD";
            this.m_fourWdCheckBox.UseVisualStyleBackColor = true;
            this.m_fourWdCheckBox.CheckedChanged += new System.EventHandler(this.m_fourWdCheckBox_CheckedChanged);
            // 
            // m_dragForceGroupBox
            // 
            this.m_dragForceGroupBox.Controls.Add(this.m_rearDragTextBox);
            this.m_dragForceGroupBox.Controls.Add(this.m_frontDragTextBox);
            this.m_dragForceGroupBox.Controls.Add(this.m_rearDragLabel);
            this.m_dragForceGroupBox.Controls.Add(this.m_frontDragLabel);
            this.m_dragForceGroupBox.Location = new System.Drawing.Point(16, 151);
            this.m_dragForceGroupBox.Name = "m_dragForceGroupBox";
            this.m_dragForceGroupBox.Size = new System.Drawing.Size(264, 104);
            this.m_dragForceGroupBox.TabIndex = 18;
            this.m_dragForceGroupBox.TabStop = false;
            this.m_dragForceGroupBox.Text = "Drag Force Parameters";
            // 
            // m_rearDragTextBox
            // 
            this.m_rearDragTextBox.Location = new System.Drawing.Point(178, 59);
            this.m_rearDragTextBox.Name = "m_rearDragTextBox";
            this.m_rearDragTextBox.Size = new System.Drawing.Size(66, 20);
            this.m_rearDragTextBox.TabIndex = 3;
            this.m_rearDragTextBox.TextChanged += new System.EventHandler(this.m_rearDragTextBox_TextChanged);
            // 
            // m_frontDragTextBox
            // 
            this.m_frontDragTextBox.Location = new System.Drawing.Point(178, 26);
            this.m_frontDragTextBox.Name = "m_frontDragTextBox";
            this.m_frontDragTextBox.Size = new System.Drawing.Size(66, 20);
            this.m_frontDragTextBox.TabIndex = 2;
            this.m_frontDragTextBox.TextChanged += new System.EventHandler(this.m_frontDragTextBox_TextChanged);
            // 
            // m_rearDragLabel
            // 
            this.m_rearDragLabel.AutoSize = true;
            this.m_rearDragLabel.Location = new System.Drawing.Point(6, 62);
            this.m_rearDragLabel.Name = "m_rearDragLabel";
            this.m_rearDragLabel.Size = new System.Drawing.Size(160, 13);
            this.m_rearDragLabel.TabIndex = 1;
            this.m_rearDragLabel.Text = "Maximum Rear Drag Force (kgf):";
            // 
            // m_frontDragLabel
            // 
            this.m_frontDragLabel.AutoSize = true;
            this.m_frontDragLabel.Location = new System.Drawing.Point(6, 29);
            this.m_frontDragLabel.Name = "m_frontDragLabel";
            this.m_frontDragLabel.Size = new System.Drawing.Size(161, 13);
            this.m_frontDragLabel.TabIndex = 0;
            this.m_frontDragLabel.Text = "Maximum Front Drag Force (kgf):";
            // 
            // m_parkBrakeParamGroupBox
            // 
            this.m_parkBrakeParamGroupBox.Controls.Add(this.m_maxPbForceTextBox);
            this.m_parkBrakeParamGroupBox.Controls.Add(this.m_maxPbForceLabel);
            this.m_parkBrakeParamGroupBox.Controls.Add(this.m_minPbForceTextBox);
            this.m_parkBrakeParamGroupBox.Controls.Add(this.label1);
            this.m_parkBrakeParamGroupBox.Location = new System.Drawing.Point(286, 151);
            this.m_parkBrakeParamGroupBox.Name = "m_parkBrakeParamGroupBox";
            this.m_parkBrakeParamGroupBox.Size = new System.Drawing.Size(237, 104);
            this.m_parkBrakeParamGroupBox.TabIndex = 19;
            this.m_parkBrakeParamGroupBox.TabStop = false;
            this.m_parkBrakeParamGroupBox.Text = "Park Brake Parameters";
            // 
            // m_maxPbForceTextBox
            // 
            this.m_maxPbForceTextBox.Location = new System.Drawing.Point(176, 59);
            this.m_maxPbForceTextBox.Name = "m_maxPbForceTextBox";
            this.m_maxPbForceTextBox.Size = new System.Drawing.Size(55, 20);
            this.m_maxPbForceTextBox.TabIndex = 3;
            this.m_maxPbForceTextBox.TextChanged += new System.EventHandler(this.m_maxPbForceTextBox_TextChanged);
            // 
            // m_maxPbForceLabel
            // 
            this.m_maxPbForceLabel.AutoSize = true;
            this.m_maxPbForceLabel.Location = new System.Drawing.Point(6, 62);
            this.m_maxPbForceLabel.Name = "m_maxPbForceLabel";
            this.m_maxPbForceLabel.Size = new System.Drawing.Size(164, 13);
            this.m_maxPbForceLabel.TabIndex = 2;
            this.m_maxPbForceLabel.Text = "Maximum Park Brake Force (kgf):";
            // 
            // m_minPbForceTextBox
            // 
            this.m_minPbForceTextBox.Location = new System.Drawing.Point(173, 26);
            this.m_minPbForceTextBox.Name = "m_minPbForceTextBox";
            this.m_minPbForceTextBox.Size = new System.Drawing.Size(58, 20);
            this.m_minPbForceTextBox.TabIndex = 1;
            this.m_minPbForceTextBox.TextChanged += new System.EventHandler(this.m_minPbForceTextBox_TextChanged);
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(6, 29);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(161, 13);
            this.label1.TabIndex = 0;
            this.label1.Text = "Minimum Park Brake Force (kgf):";
            // 
            // m_brakeForceGroupBox
            // 
            this.m_brakeForceGroupBox.Controls.Add(this.m_maxRearBrakeForceTextBox);
            this.m_brakeForceGroupBox.Controls.Add(this.m_minRearBrakeForceTextBox);
            this.m_brakeForceGroupBox.Controls.Add(this.m_maxRearBrakeForceLabel);
            this.m_brakeForceGroupBox.Controls.Add(this.m_minRearBrakeForceLabel);
            this.m_brakeForceGroupBox.Controls.Add(this.m_maxFrontBrakeForceTextBox);
            this.m_brakeForceGroupBox.Controls.Add(this.m_minFrontBrakeForceTextBox);
            this.m_brakeForceGroupBox.Controls.Add(this.m_maxFrontBrakeForceLabel);
            this.m_brakeForceGroupBox.Controls.Add(this.m_minBrakeForceLabel);
            this.m_brakeForceGroupBox.Location = new System.Drawing.Point(16, 261);
            this.m_brakeForceGroupBox.Name = "m_brakeForceGroupBox";
            this.m_brakeForceGroupBox.Size = new System.Drawing.Size(507, 100);
            this.m_brakeForceGroupBox.TabIndex = 20;
            this.m_brakeForceGroupBox.TabStop = false;
            this.m_brakeForceGroupBox.Text = "Brkae Force Parameters";
            // 
            // m_maxRearBrakeForceTextBox
            // 
            this.m_maxRearBrakeForceTextBox.Location = new System.Drawing.Point(447, 65);
            this.m_maxRearBrakeForceTextBox.Name = "m_maxRearBrakeForceTextBox";
            this.m_maxRearBrakeForceTextBox.Size = new System.Drawing.Size(54, 20);
            this.m_maxRearBrakeForceTextBox.TabIndex = 7;
            this.m_maxRearBrakeForceTextBox.TextChanged += new System.EventHandler(this.m_maxRearBrakeForceTextBox_TextChanged);
            // 
            // m_minRearBrakeForceTextBox
            // 
            this.m_minRearBrakeForceTextBox.Location = new System.Drawing.Point(446, 24);
            this.m_minRearBrakeForceTextBox.Name = "m_minRearBrakeForceTextBox";
            this.m_minRearBrakeForceTextBox.Size = new System.Drawing.Size(55, 20);
            this.m_minRearBrakeForceTextBox.TabIndex = 6;
            this.m_minRearBrakeForceTextBox.TextChanged += new System.EventHandler(this.m_minRearBrakeForceTextBox_TextChanged);
            // 
            // m_maxRearBrakeForceLabel
            // 
            this.m_maxRearBrakeForceLabel.AutoSize = true;
            this.m_maxRearBrakeForceLabel.Location = new System.Drawing.Point(276, 68);
            this.m_maxRearBrakeForceLabel.Name = "m_maxRearBrakeForceLabel";
            this.m_maxRearBrakeForceLabel.Size = new System.Drawing.Size(165, 13);
            this.m_maxRearBrakeForceLabel.TabIndex = 5;
            this.m_maxRearBrakeForceLabel.Text = "Maximum Rear Brake Force (kgf):";
            // 
            // m_minRearBrakeForceLabel
            // 
            this.m_minRearBrakeForceLabel.AutoSize = true;
            this.m_minRearBrakeForceLabel.Location = new System.Drawing.Point(276, 27);
            this.m_minRearBrakeForceLabel.Name = "m_minRearBrakeForceLabel";
            this.m_minRearBrakeForceLabel.Size = new System.Drawing.Size(162, 13);
            this.m_minRearBrakeForceLabel.TabIndex = 4;
            this.m_minRearBrakeForceLabel.Text = "Minimum Rear Brake Force (kgf):";
            // 
            // m_maxFrontBrakeForceTextBox
            // 
            this.m_maxFrontBrakeForceTextBox.Location = new System.Drawing.Point(178, 65);
            this.m_maxFrontBrakeForceTextBox.Name = "m_maxFrontBrakeForceTextBox";
            this.m_maxFrontBrakeForceTextBox.Size = new System.Drawing.Size(66, 20);
            this.m_maxFrontBrakeForceTextBox.TabIndex = 3;
            this.m_maxFrontBrakeForceTextBox.TextChanged += new System.EventHandler(this.m_maxFrontBrakeForceTextBox_TextChanged);
            // 
            // m_minFrontBrakeForceTextBox
            // 
            this.m_minFrontBrakeForceTextBox.Location = new System.Drawing.Point(178, 24);
            this.m_minFrontBrakeForceTextBox.Name = "m_minFrontBrakeForceTextBox";
            this.m_minFrontBrakeForceTextBox.Size = new System.Drawing.Size(66, 20);
            this.m_minFrontBrakeForceTextBox.TabIndex = 2;
            this.m_minFrontBrakeForceTextBox.TextChanged += new System.EventHandler(this.m_minFrontBrakeForceTextBox_TextChanged);
            // 
            // m_maxFrontBrakeForceLabel
            // 
            this.m_maxFrontBrakeForceLabel.AutoSize = true;
            this.m_maxFrontBrakeForceLabel.Location = new System.Drawing.Point(6, 68);
            this.m_maxFrontBrakeForceLabel.Name = "m_maxFrontBrakeForceLabel";
            this.m_maxFrontBrakeForceLabel.Size = new System.Drawing.Size(166, 13);
            this.m_maxFrontBrakeForceLabel.TabIndex = 1;
            this.m_maxFrontBrakeForceLabel.Text = "Maximum Front Brake Force (kgf):";
            // 
            // m_minBrakeForceLabel
            // 
            this.m_minBrakeForceLabel.AutoSize = true;
            this.m_minBrakeForceLabel.Location = new System.Drawing.Point(6, 27);
            this.m_minBrakeForceLabel.Name = "m_minBrakeForceLabel";
            this.m_minBrakeForceLabel.Size = new System.Drawing.Size(163, 13);
            this.m_minBrakeForceLabel.TabIndex = 0;
            this.m_minBrakeForceLabel.Text = "Minimum Front Brake Force (kgf):";
            // 
            // m_vinCharLabel
            // 
            this.m_vinCharLabel.AutoSize = true;
            this.m_vinCharLabel.Font = new System.Drawing.Font("Microsoft Sans Serif", 10F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.m_vinCharLabel.Location = new System.Drawing.Point(13, 89);
            this.m_vinCharLabel.Name = "m_vinCharLabel";
            this.m_vinCharLabel.Size = new System.Drawing.Size(122, 17);
            this.m_vinCharLabel.TabIndex = 21;
            this.m_vinCharLabel.Text = "VIN Characters:";
            // 
            // m_vinCharTextBox
            // 
            this.m_vinCharTextBox.BackColor = System.Drawing.Color.Khaki;
            this.m_vinCharTextBox.Location = new System.Drawing.Point(164, 88);
            this.m_vinCharTextBox.Name = "m_vinCharTextBox";
            this.m_vinCharTextBox.Size = new System.Drawing.Size(91, 20);
            this.m_vinCharTextBox.TabIndex = 22;
            this.m_vinCharTextBox.TextChanged += new System.EventHandler(this.m_vinCharTextBox_TextChanged);
            // 
            // VehicleTypeEditorForm
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.BackColor = System.Drawing.Color.DodgerBlue;
            this.ClientSize = new System.Drawing.Size(540, 388);
            this.Controls.Add(this.m_vinCharTextBox);
            this.Controls.Add(this.m_vinCharLabel);
            this.Controls.Add(this.m_brakeForceGroupBox);
            this.Controls.Add(this.m_parkBrakeParamGroupBox);
            this.Controls.Add(this.m_dragForceGroupBox);
            this.Controls.Add(this.m_fourWdCheckBox);
            this.Controls.Add(this.m_driveAxleComboBox);
            this.Controls.Add(this.m_driveAxleLabel);
            this.Controls.Add(this.m_rearAxleWheelbaseTextBox);
            this.Controls.Add(this.m_wheelbaseLabel);
            this.Controls.Add(this.m_vcNumberTextBox);
            this.Controls.Add(this.m_vcNumberLabel);
            this.Controls.Add(this.menuStrip1);
            this.MainMenuStrip = this.menuStrip1;
            this.Name = "VehicleTypeEditorForm";
            this.Text = "Vehicle Editor";
            this.menuStrip1.ResumeLayout(false);
            this.menuStrip1.PerformLayout();
            this.m_dragForceGroupBox.ResumeLayout(false);
            this.m_dragForceGroupBox.PerformLayout();
            this.m_parkBrakeParamGroupBox.ResumeLayout(false);
            this.m_parkBrakeParamGroupBox.PerformLayout();
            this.m_brakeForceGroupBox.ResumeLayout(false);
            this.m_brakeForceGroupBox.PerformLayout();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.MenuStrip menuStrip1;
        private System.Windows.Forms.ToolStripMenuItem fileToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem openToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem saveToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem exitToolStripMenuItem;
        private System.Windows.Forms.Label m_vcNumberLabel;
        private System.Windows.Forms.TextBox m_vcNumberTextBox;
        private System.Windows.Forms.Label m_wheelbaseLabel;
        private System.Windows.Forms.TextBox m_rearAxleWheelbaseTextBox;
        private System.Windows.Forms.ToolStripMenuItem newToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem configureToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem vCFileLocationToolStripMenuItem;
        private System.Windows.Forms.Label m_driveAxleLabel;
        private System.Windows.Forms.ComboBox m_driveAxleComboBox;
        private System.Windows.Forms.CheckBox m_fourWdCheckBox;
        private System.Windows.Forms.GroupBox m_dragForceGroupBox;
        private System.Windows.Forms.TextBox m_rearDragTextBox;
        private System.Windows.Forms.TextBox m_frontDragTextBox;
        private System.Windows.Forms.Label m_rearDragLabel;
        private System.Windows.Forms.Label m_frontDragLabel;
        private System.Windows.Forms.GroupBox m_parkBrakeParamGroupBox;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.TextBox m_maxPbForceTextBox;
        private System.Windows.Forms.Label m_maxPbForceLabel;
        private System.Windows.Forms.TextBox m_minPbForceTextBox;
        private System.Windows.Forms.GroupBox m_brakeForceGroupBox;
        private System.Windows.Forms.TextBox m_maxRearBrakeForceTextBox;
        private System.Windows.Forms.TextBox m_minRearBrakeForceTextBox;
        private System.Windows.Forms.Label m_maxRearBrakeForceLabel;
        private System.Windows.Forms.Label m_minRearBrakeForceLabel;
        private System.Windows.Forms.TextBox m_maxFrontBrakeForceTextBox;
        private System.Windows.Forms.TextBox m_minFrontBrakeForceTextBox;
        private System.Windows.Forms.Label m_maxFrontBrakeForceLabel;
        private System.Windows.Forms.Label m_minBrakeForceLabel;
        private System.Windows.Forms.Label m_vinCharLabel;
        private System.Windows.Forms.TextBox m_vinCharTextBox;
    }
}

