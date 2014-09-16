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
        public Boolean GetDispositionBeforeUpdate { get; set; }

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
            GetDispositionBeforeUpdate = false;
        }
    }

    public class BuildStringOptions
    {
        public String PerformAbortTag;
        public String BuildDateTag;
        public String BuildTimeTag;
        public List<PNGroupCodePair> PartNumberList;
        public List<GroupCodePair> TestCodeList;

        public BuildStringOptions()
        {
            PerformAbortTag = "GroupCode";
            BuildDateTag = "GroupCode";
            BuildTimeTag = "GroupCode";
            PartNumberList = new List<PNGroupCodePair>();
            TestCodeList = new List<GroupCodePair>();
        }

        public BuildStringOptions(bool addInitialListValue)
        {
            PerformAbortTag = "GroupCode";
            BuildDateTag = "GroupCode";
            BuildTimeTag = "GroupCode";
            PartNumberList = new List<PNGroupCodePair>();
            TestCodeList = new List<GroupCodePair>();

            if (addInitialListValue)
            {
                PartNumberList.Add(new PNGroupCodePair("PartNumberName", "GroupCode", 10));
                TestCodeList.Add(new GroupCodePair("TestCodeName", "GroupCode"));
            }
        }
    }

    public class GroupCodePair
    {
        public String ItemName;
        public String ItemGroupCode;

        public GroupCodePair()
        {
            ItemName = String.Empty;
            ItemGroupCode = String.Empty;
        }

        public GroupCodePair(String itemName, String itemGroupCode)
        {
            ItemName = itemName;
            ItemGroupCode = itemGroupCode;
        }
    }

    public class PNGroupCodePair
    {
        public String ItemName;
        public String ItemGroupCode;
        public Int32 PartNumberLength;

        public PNGroupCodePair()
        {
            ItemName = String.Empty;
            ItemGroupCode = String.Empty;
            PartNumberLength = 10;
        }

        public PNGroupCodePair(String itemName, String itemGroupCode, Int32 partNumberLength)
        {
            ItemName = itemName;
            ItemGroupCode = itemGroupCode;
            PartNumberLength = partNumberLength;
        }
    }
}
