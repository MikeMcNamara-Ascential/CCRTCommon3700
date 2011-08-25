using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;

namespace FlashStationMainForm
{
    public partial class TimedDialogBox : Form
    {
        public TimedDialogBox(string dialogText, string labelText, string buttonText, int timeOut)
        {
            InitializeComponent();
            this.Text = dialogText;
            button1.Enabled = false;
            label1.Text = labelText;
            button1.Text = buttonText;
            //m_countDownLabel.Text = (timeOut / 1000).ToString();
            m_timerTimeout = timeOut;
            timer1.Interval = 1000;
            m_timerTickCount = 0;
            timer1.Enabled = true;
        }
        public int m_timerTimeout;
        public int m_timerTickCount;

        private void timer1_Tick(object sender, EventArgs e)
        {
            m_timerTickCount++;
            //m_countDownLabel.Text = ((m_timerTimeout - (m_timerTickCount * timer1.Interval)) / 1000).ToString();
            if ((m_timerTickCount * timer1.Interval) == m_timerTimeout)
            {
                button1.Enabled = true;
                timer1.Enabled = false;
            }
        }

        private void button1_Click(object sender, EventArgs e)
        {
            Close();
        }
    }
}
