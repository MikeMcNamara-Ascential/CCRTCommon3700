using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Common.Classes
{
    public class ECUBuildData
    {
        
        public ECUBuildData()
        {
            m_softwareModulePartNumbers = new List<string>();
            m_requestID = new List<byte>();
            m_responseID = new List<byte>();
            m_programmingSuccess = false;
            m_performFlash = true;
        }
        public ECUBuildData(string ecuName, string vin, string partNo, string utilFilePartNo, List<String> swmiPartNos)
        {
            m_ecuName = ecuName;
            m_vin = vin;
            m_partNo = partNo;
            m_utilityFilePartNumber = utilFilePartNo;
            m_softwareModulePartNumbers = swmiPartNos;
            m_requestID = new List<byte>();
            m_responseID = new List<byte>();
            m_programmingSuccess = false;
            m_performFlash = true;
        }
        private String m_ecuName;
        private String m_vin;
        private String m_partNo;
        private String m_utilityFilePartNumber;
        private List<String> m_softwareModulePartNumbers;
        private List<byte> m_requestID;
        private List<byte> m_responseID;
        private bool m_programmingSuccess;
        private bool m_performFlash;

        public String ECUName
        {
            get { return m_ecuName; }
            set { m_ecuName = value; }
        }
        public String VIN
        {
            get { return m_vin; }
            set { m_vin = value; }
        }

        public String PartNumber
        {
            get { return m_partNo; }
            set { m_partNo = value; }
        }

        public String UtilityFilePartNumber
        {
            get { return m_utilityFilePartNumber; }
            set { m_utilityFilePartNumber = value; }
        }

        public List<String> SoftwareModulePartNumbers
        {
            get { return m_softwareModulePartNumbers; }
            set { m_softwareModulePartNumbers = value; }
        }

        public List<byte> RequestID
        {
            get { return m_requestID; }
            set { m_responseID = value; }
        }

        public List<byte> ResponseID
        {
            get { return m_responseID; }
            set { m_responseID = value; }
        }
        public bool ProgrammingSuccess
        {
            get { return m_programmingSuccess; }
            set { m_programmingSuccess = value; }
        }
        public bool PerformFlash
        {
            get { return m_performFlash; }
            set { m_performFlash = value; }
        }
    }
}
