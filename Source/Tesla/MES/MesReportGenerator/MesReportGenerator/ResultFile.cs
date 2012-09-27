using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

//NOT USED

namespace MesReportGenerator
{
    class ResultFile
    {
        public string filename;
        public int sequenceNumber;
        public string overallResult;

        public ResultFile() { }

        public ResultFile(string resultFileName)
        {
            string[] lineA = resultFileName.Split('_');
            string[] lineB = lineA[1].Split('.');

            overallResult = lineA[0];
            filename = lineB[0];
            sequenceNumber = Convert.ToInt16( lineB[1]);
        }
    }
}
