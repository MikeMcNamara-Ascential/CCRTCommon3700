using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using Common.Classes;
using LoggingInterface;

namespace FlashStationMainForm
{
    public partial class BarcodeEntryForm : Form
    {
        Form1 m_frm1;
        Logger m_logger;
        public BarcodeEntryForm(Form1 frm1, Logger logger)
        {
            InitializeComponent();
            m_frm1 = frm1;
            m_logger = logger;
        }

        private void m_submitButton_Click(object sender, EventArgs e)
        {
            m_frm1.SubmitBarcodeValue(m_barcodeTextBox.Text);
            m_logger.Log("INFO:  Submitted barcode Value: " + m_barcodeTextBox.Text); 
            this.Close();
        }
    }
}
