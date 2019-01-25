namespace ToyotaParameterEditor
{
    partial class NewVehicleDataForm
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
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(NewVehicleDataForm));
            this.m_vehicleNameLabel = new System.Windows.Forms.Label();
            this.m_vehicleNameTextBox = new System.Windows.Forms.TextBox();
            this.m_doneButton = new System.Windows.Forms.Button();
            this.m_vehicleLogoPictureBox = new System.Windows.Forms.PictureBox();
            this.m_selectImageButton = new System.Windows.Forms.Button();
            this.m_buttonNumberLabel = new System.Windows.Forms.Label();
            this.m_vehicleButtonNumberComboBox = new System.Windows.Forms.ComboBox();
            this.m_2wdRadioButton = new System.Windows.Forms.RadioButton();
            this.m_4wdRadioButton = new System.Windows.Forms.RadioButton();
            this.m_driveTrainLabel = new System.Windows.Forms.Label();
            this.m_cancelButton = new System.Windows.Forms.Button();
            this.label1 = new System.Windows.Forms.Label();
            this.m_performAbsTest = new System.Windows.Forms.CheckBox();
            ((System.ComponentModel.ISupportInitialize)(this.m_vehicleLogoPictureBox)).BeginInit();
            this.SuspendLayout();
            // 
            // m_vehicleNameLabel
            // 
            this.m_vehicleNameLabel.AutoSize = true;
            this.m_vehicleNameLabel.Font = new System.Drawing.Font("Microsoft Sans Serif", 10F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.m_vehicleNameLabel.Location = new System.Drawing.Point(12, 9);
            this.m_vehicleNameLabel.Name = "m_vehicleNameLabel";
            this.m_vehicleNameLabel.Size = new System.Drawing.Size(103, 18);
            this.m_vehicleNameLabel.TabIndex = 0;
            this.m_vehicleNameLabel.Text = "Vehicle Name:";
            // 
            // m_vehicleNameTextBox
            // 
            this.m_vehicleNameTextBox.Font = new System.Drawing.Font("Microsoft Sans Serif", 10F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.m_vehicleNameTextBox.Location = new System.Drawing.Point(117, 6);
            this.m_vehicleNameTextBox.Name = "m_vehicleNameTextBox";
            this.m_vehicleNameTextBox.Size = new System.Drawing.Size(165, 24);
            this.m_vehicleNameTextBox.TabIndex = 1;
            // 
            // m_doneButton
            // 
            this.m_doneButton.Location = new System.Drawing.Point(209, 194);
            this.m_doneButton.Name = "m_doneButton";
            this.m_doneButton.Size = new System.Drawing.Size(75, 23);
            this.m_doneButton.TabIndex = 8;
            this.m_doneButton.Text = "Done";
            this.m_doneButton.UseVisualStyleBackColor = true;
            this.m_doneButton.Click += new System.EventHandler(this.m_doneButton_Click);
            // 
            // m_vehicleLogoPictureBox
            // 
            this.m_vehicleLogoPictureBox.BackgroundImageLayout = System.Windows.Forms.ImageLayout.Stretch;
            this.m_vehicleLogoPictureBox.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D;
            this.m_vehicleLogoPictureBox.InitialImage = ((System.Drawing.Image)(resources.GetObject("m_vehicleLogoPictureBox.InitialImage")));
            this.m_vehicleLogoPictureBox.Location = new System.Drawing.Point(117, 76);
            this.m_vehicleLogoPictureBox.Name = "m_vehicleLogoPictureBox";
            this.m_vehicleLogoPictureBox.Size = new System.Drawing.Size(165, 44);
            this.m_vehicleLogoPictureBox.SizeMode = System.Windows.Forms.PictureBoxSizeMode.StretchImage;
            this.m_vehicleLogoPictureBox.TabIndex = 11;
            this.m_vehicleLogoPictureBox.TabStop = false;
            // 
            // m_selectImageButton
            // 
            this.m_selectImageButton.Location = new System.Drawing.Point(15, 86);
            this.m_selectImageButton.Name = "m_selectImageButton";
            this.m_selectImageButton.Size = new System.Drawing.Size(96, 23);
            this.m_selectImageButton.TabIndex = 3;
            this.m_selectImageButton.Text = "Select Image";
            this.m_selectImageButton.UseVisualStyleBackColor = true;
            this.m_selectImageButton.Click += new System.EventHandler(this.m_selectImageButton_Click);
            // 
            // m_buttonNumberLabel
            // 
            this.m_buttonNumberLabel.AutoSize = true;
            this.m_buttonNumberLabel.Font = new System.Drawing.Font("Microsoft Sans Serif", 10F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.m_buttonNumberLabel.Location = new System.Drawing.Point(8, 42);
            this.m_buttonNumberLabel.Name = "m_buttonNumberLabel";
            this.m_buttonNumberLabel.Size = new System.Drawing.Size(108, 18);
            this.m_buttonNumberLabel.TabIndex = 13;
            this.m_buttonNumberLabel.Text = "Button Number";
            // 
            // m_vehicleButtonNumberComboBox
            // 
            this.m_vehicleButtonNumberComboBox.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.m_vehicleButtonNumberComboBox.Font = new System.Drawing.Font("Microsoft Sans Serif", 10F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.m_vehicleButtonNumberComboBox.FormatString = "N0";
            this.m_vehicleButtonNumberComboBox.FormattingEnabled = true;
            this.m_vehicleButtonNumberComboBox.Items.AddRange(new object[] {
            "0",
            "1",
            "2",
            "3",
            "4",
            "5",
            "6",
            "7",
            "8",
            "9"});
            this.m_vehicleButtonNumberComboBox.Location = new System.Drawing.Point(117, 39);
            this.m_vehicleButtonNumberComboBox.MaxLength = 1;
            this.m_vehicleButtonNumberComboBox.Name = "m_vehicleButtonNumberComboBox";
            this.m_vehicleButtonNumberComboBox.Size = new System.Drawing.Size(39, 26);
            this.m_vehicleButtonNumberComboBox.Sorted = true;
            this.m_vehicleButtonNumberComboBox.TabIndex = 2;
            // 
            // m_2wdRadioButton
            // 
            this.m_2wdRadioButton.AutoSize = true;
            this.m_2wdRadioButton.Checked = true;
            this.m_2wdRadioButton.Font = new System.Drawing.Font("Microsoft Sans Serif", 10F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.m_2wdRadioButton.Location = new System.Drawing.Point(117, 136);
            this.m_2wdRadioButton.Name = "m_2wdRadioButton";
            this.m_2wdRadioButton.Size = new System.Drawing.Size(60, 22);
            this.m_2wdRadioButton.TabIndex = 4;
            this.m_2wdRadioButton.TabStop = true;
            this.m_2wdRadioButton.Text = "2WD";
            this.m_2wdRadioButton.UseVisualStyleBackColor = true;
            // 
            // m_4wdRadioButton
            // 
            this.m_4wdRadioButton.AutoSize = true;
            this.m_4wdRadioButton.Font = new System.Drawing.Font("Microsoft Sans Serif", 10F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.m_4wdRadioButton.Location = new System.Drawing.Point(175, 136);
            this.m_4wdRadioButton.Name = "m_4wdRadioButton";
            this.m_4wdRadioButton.Size = new System.Drawing.Size(99, 22);
            this.m_4wdRadioButton.TabIndex = 5;
            this.m_4wdRadioButton.TabStop = true;
            this.m_4wdRadioButton.Text = "4WD/AWD";
            this.m_4wdRadioButton.UseVisualStyleBackColor = true;
            // 
            // m_driveTrainLabel
            // 
            this.m_driveTrainLabel.AutoSize = true;
            this.m_driveTrainLabel.Font = new System.Drawing.Font("Microsoft Sans Serif", 10F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.m_driveTrainLabel.Location = new System.Drawing.Point(2, 138);
            this.m_driveTrainLabel.Name = "m_driveTrainLabel";
            this.m_driveTrainLabel.Size = new System.Drawing.Size(110, 18);
            this.m_driveTrainLabel.TabIndex = 19;
            this.m_driveTrainLabel.Text = "Drivetrain Type:";
            // 
            // m_cancelButton
            // 
            this.m_cancelButton.Location = new System.Drawing.Point(128, 194);
            this.m_cancelButton.Name = "m_cancelButton";
            this.m_cancelButton.Size = new System.Drawing.Size(75, 23);
            this.m_cancelButton.TabIndex = 7;
            this.m_cancelButton.Text = "Cancel";
            this.m_cancelButton.UseVisualStyleBackColor = true;
            this.m_cancelButton.Click += new System.EventHandler(this.m_cancelButton_Click);
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Font = new System.Drawing.Font("Microsoft Sans Serif", 10F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.label1.Location = new System.Drawing.Point(2, 165);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(132, 18);
            this.label1.TabIndex = 21;
            this.label1.Text = "Perform ABS Test:";
            // 
            // m_performAbsTest
            // 
            this.m_performAbsTest.AutoSize = true;
            this.m_performAbsTest.Location = new System.Drawing.Point(140, 167);
            this.m_performAbsTest.Name = "m_performAbsTest";
            this.m_performAbsTest.Size = new System.Drawing.Size(15, 14);
            this.m_performAbsTest.TabIndex = 6;
            this.m_performAbsTest.UseVisualStyleBackColor = true;
            // 
            // NewVehicleDataForm
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(292, 224);
            this.ControlBox = false;
            this.Controls.Add(this.m_performAbsTest);
            this.Controls.Add(this.label1);
            this.Controls.Add(this.m_cancelButton);
            this.Controls.Add(this.m_driveTrainLabel);
            this.Controls.Add(this.m_4wdRadioButton);
            this.Controls.Add(this.m_2wdRadioButton);
            this.Controls.Add(this.m_vehicleButtonNumberComboBox);
            this.Controls.Add(this.m_buttonNumberLabel);
            this.Controls.Add(this.m_selectImageButton);
            this.Controls.Add(this.m_vehicleLogoPictureBox);
            this.Controls.Add(this.m_doneButton);
            this.Controls.Add(this.m_vehicleNameTextBox);
            this.Controls.Add(this.m_vehicleNameLabel);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedToolWindow;
            this.MaximumSize = new System.Drawing.Size(308, 265);
            this.MinimumSize = new System.Drawing.Size(308, 265);
            this.Name = "NewVehicleDataForm";
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterParent;
            this.Text = "New Vehicle Data";
            ((System.ComponentModel.ISupportInitialize)(this.m_vehicleLogoPictureBox)).EndInit();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Label m_vehicleNameLabel;
        private System.Windows.Forms.TextBox m_vehicleNameTextBox;
        private System.Windows.Forms.Button m_doneButton;
        private System.Windows.Forms.PictureBox m_vehicleLogoPictureBox;
        private System.Windows.Forms.Button m_selectImageButton;
        private System.Windows.Forms.Label m_buttonNumberLabel;
        private System.Windows.Forms.ComboBox m_vehicleButtonNumberComboBox;
        private System.Windows.Forms.RadioButton m_2wdRadioButton;
        private System.Windows.Forms.RadioButton m_4wdRadioButton;
        private System.Windows.Forms.Label m_driveTrainLabel;
        private System.Windows.Forms.Button m_cancelButton;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.CheckBox m_performAbsTest;
    }
}