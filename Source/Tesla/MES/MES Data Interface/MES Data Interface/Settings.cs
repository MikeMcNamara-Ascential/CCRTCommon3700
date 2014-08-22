using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.IO;
using System.Xml;
using System.Xml.Serialization;
using System.Windows.Forms;

namespace MES_Data_Interface
{
    public class Settings
    {
        public String QnxIpAddress { get; set; }
        public String QnxListenerIpAddress { get; set; }
        public String QnxListenerTcpPort { get; set; }
        public String MesHostName { get; set; }

        private static string m_settingsFile = Path.Combine(Application.StartupPath, "Settings.xml");

        public static Settings ReadSettings()
        {
            try
            {
                if (File.Exists(m_settingsFile))
                {
                    XmlSerializer serializer = new XmlSerializer(typeof(Settings));
                    FileStream fs = new FileStream(m_settingsFile, FileMode.Open);
                    XmlReader reader = new XmlTextReader(fs);
                    Settings value = ((Settings)serializer.Deserialize(reader));
                    fs.Close();
                    return value;
                }
                else
                {
                    return new Settings();
                }
            }
            catch
            {
                //MessageBox.Show("Error retrieving settings.");
                return new Settings();
            }
        }

        public void SaveSettings()
        {
            XmlSerializer serializer = new XmlSerializer(typeof(Settings));
            FileStream fs = new FileStream(m_settingsFile, FileMode.Create);
            serializer.Serialize(fs, this);
            fs.Close();
        }

        public Settings()
        {
            QnxIpAddress = String.Empty;
            QnxListenerIpAddress = String.Empty;
            QnxListenerTcpPort = String.Empty;
            MesHostName = String.Empty;
        }
    }
}
