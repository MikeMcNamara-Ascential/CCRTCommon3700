namespace Sample
{
    partial class Form1
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
            this.cmdDetectDevices = new System.Windows.Forms.Button();
            this.txtDevices = new System.Windows.Forms.TextBox();
            this.cmdReadVoltage = new System.Windows.Forms.Button();
            this.txtVoltage = new System.Windows.Forms.TextBox();
            this.cmdReadFaults = new System.Windows.Forms.Button();
            this.txtReadFaults = new System.Windows.Forms.TextBox();
            this.button1 = new System.Windows.Forms.Button();
            this.button2 = new System.Windows.Forms.Button();
            this.SuspendLayout();
            // 
            // cmdDetectDevices
            // 
            this.cmdDetectDevices.Location = new System.Drawing.Point(13, 13);
            this.cmdDetectDevices.Name = "cmdDetectDevices";
            this.cmdDetectDevices.Size = new System.Drawing.Size(154, 23);
            this.cmdDetectDevices.TabIndex = 0;
            this.cmdDetectDevices.Text = "Detect J2534 Devices";
            this.cmdDetectDevices.UseVisualStyleBackColor = true;
            this.cmdDetectDevices.Click += new System.EventHandler(this.cmdDetectDevices_Click);
            // 
            // txtDevices
            // 
            this.txtDevices.Location = new System.Drawing.Point(173, 15);
            this.txtDevices.Multiline = true;
            this.txtDevices.Name = "txtDevices";
            this.txtDevices.ScrollBars = System.Windows.Forms.ScrollBars.Both;
            this.txtDevices.Size = new System.Drawing.Size(429, 160);
            this.txtDevices.TabIndex = 1;
            // 
            // cmdReadVoltage
            // 
            this.cmdReadVoltage.Location = new System.Drawing.Point(12, 188);
            this.cmdReadVoltage.Name = "cmdReadVoltage";
            this.cmdReadVoltage.Size = new System.Drawing.Size(155, 23);
            this.cmdReadVoltage.TabIndex = 2;
            this.cmdReadVoltage.Text = "Read Voltage";
            this.cmdReadVoltage.UseVisualStyleBackColor = true;
            this.cmdReadVoltage.Click += new System.EventHandler(this.cmdReadVoltage_Click);
            // 
            // txtVoltage
            // 
            this.txtVoltage.Location = new System.Drawing.Point(173, 190);
            this.txtVoltage.Name = "txtVoltage";
            this.txtVoltage.Size = new System.Drawing.Size(429, 20);
            this.txtVoltage.TabIndex = 3;
            // 
            // cmdReadFaults
            // 
            this.cmdReadFaults.Location = new System.Drawing.Point(12, 217);
            this.cmdReadFaults.Name = "cmdReadFaults";
            this.cmdReadFaults.Size = new System.Drawing.Size(155, 23);
            this.cmdReadFaults.TabIndex = 4;
            this.cmdReadFaults.Text = "Read Faults";
            this.cmdReadFaults.UseVisualStyleBackColor = true;
            this.cmdReadFaults.Click += new System.EventHandler(this.cmdReadFaults_Click);
            // 
            // txtReadFaults
            // 
            this.txtReadFaults.Location = new System.Drawing.Point(174, 219);
            this.txtReadFaults.Name = "txtReadFaults";
            this.txtReadFaults.Size = new System.Drawing.Size(428, 20);
            this.txtReadFaults.TabIndex = 5;
            // 
            // button1
            // 
            this.button1.Location = new System.Drawing.Point(12, 312);
            this.button1.Name = "button1";
            this.button1.Size = new System.Drawing.Size(155, 23);
            this.button1.TabIndex = 6;
            this.button1.Text = "Create Pass Through Msgs";
            this.button1.UseVisualStyleBackColor = true;
            this.button1.Click += new System.EventHandler(this.button1_Click);
            // 
            // button2
            // 
            this.button2.Location = new System.Drawing.Point(12, 283);
            this.button2.Name = "button2";
            this.button2.Size = new System.Drawing.Size(155, 23);
            this.button2.TabIndex = 7;
            this.button2.Text = "open pass through";
            this.button2.UseVisualStyleBackColor = true;
            this.button2.Click += new System.EventHandler(this.button2_Click);
            // 
            // Form1
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(614, 430);
            this.Controls.Add(this.button2);
            this.Controls.Add(this.button1);
            this.Controls.Add(this.txtReadFaults);
            this.Controls.Add(this.cmdReadFaults);
            this.Controls.Add(this.txtVoltage);
            this.Controls.Add(this.cmdReadVoltage);
            this.Controls.Add(this.txtDevices);
            this.Controls.Add(this.cmdDetectDevices);
            this.Name = "Form1";
            this.Text = "J2534DotNet Sample";
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Button cmdDetectDevices;
        private System.Windows.Forms.TextBox txtDevices;
        private System.Windows.Forms.Button cmdReadVoltage;
        private System.Windows.Forms.TextBox txtVoltage;
        private System.Windows.Forms.Button cmdReadFaults;
        private System.Windows.Forms.TextBox txtReadFaults;
        private System.Windows.Forms.Button button1;
        private System.Windows.Forms.Button button2;
    }
}

