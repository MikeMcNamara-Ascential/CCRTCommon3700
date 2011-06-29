using System;
using System.Collections.Generic;
using System.Text;
using System.Runtime.InteropServices;

namespace Tach
{
    static class TachDllInterface
    {
        [DllImport("BR_INS.dll")]
        private static extern int read_vin_tco(ref byte[] vin);

        [DllImport("BR_INS.dll")]
        private static extern int write_vin_tco(string vin);

        [DllImport("BR_INS.dll")]
        private static extern int read_data_tco(ref byte[] data);

        [DllImport("BR_INS.dll")]
        private static extern int write_data_tco(int day, int month, int year);

        [DllImport("BR_INS.dll")]
        private static extern int read_kl_tco(ref int k, ref int l);

        [DllImport("BR_INS.dll")]
        private static extern int write_kl_tco(ref int k, ref int l, int writek);

        public static int readVin(ref string vin)
        {
            int status;
            byte[] vinCharArray = new byte[30];
            status = read_vin_tco(ref vinCharArray);
            vin = vinCharArray.ToString();
            return status;
        }

        public static int writeVin(string vin)
        {
            //byte[] vinCharArray = new byte[30];
            //System.Text.ASCIIEncoding encoding = new System.Text.ASCIIEncoding();
            vin = vin + "\0";
            //vinCharArray = encoding.GetBytes(vin);
            //vinCharArray = vin.ToCharArray();
            //return write_vin_tco(ref vinCharArray);
            return write_vin_tco(vin);
        }

        public static int readDate(ref string date)
        {
            int status;
            byte[] dateCharArray = new byte[30];
            status = read_data_tco(ref dateCharArray);
            date = dateCharArray.ToString();
            return status;
        }

        public static int writeDate(int day, int month, int year)
        {
            return write_data_tco(day, month, year);
        }

        public static int readKl(ref int kValue, ref int lValue)
        {
            return read_kl_tco(ref kValue, ref lValue);
        }

        public static int writeK(int kValue)
        {
            int lValue = 0;
            return write_kl_tco(ref kValue, ref lValue, 1);
        }

        public static int writeL(int lValue)
        {
            int kValue = 0;
            return write_kl_tco(ref kValue, ref lValue, 0);
        }
    }
}
