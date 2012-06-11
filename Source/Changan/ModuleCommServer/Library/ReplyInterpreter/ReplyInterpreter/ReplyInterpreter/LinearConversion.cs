using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace ReplyInterpreter
{
    public class LinearConversion
    {

        public int m_sizeInBits;

        public float m_factor;
        public float m_offset;
        public SignedType m_signType;

        public float ConvertResponse(List<byte> rawData)
        {
            float value = 0;
                  
            return value;
        }
        public int ConvertResponse(List<byte> rawData)
        {
            int value = 0;

            return value;
        }

        public uint ConvertResponse(List<byte> rawData)
        {
            uint value = 0;
            return value;
        }

        public int ConvertByteListToInt(List<byte> rawData)
        {
            int result = 0;
            if (m_signType == SignedType.ONES_COMPLIMENT_SIGNED)
            {
                result = ByteListTo1sComplimentSignedInt(rawData);
            }
            else if (m_signType == SignedType.TWOS_COMPLIMENT_SIGNED)
            {
                result = ByteListTo2sComplimentSignedInt(rawData);
            }
            else if (m_signType == SignedType.SIGN_AND_MAGNITUDE)
            {
                result = ByteListToSignAndMagnitudeSignedInt(rawData);
            }
            else
            {
                result = ByteListToUnsignedInt(rawData);
            }
            return result;
        }
        public int ByteListToUnsignedInt(List<byte> rawData)
        {
            int result = 0;
            return result;
        }
        public int ByteListTo1sComplimentSignedInt(List<byte> rawData)
        {
            int result = 0;
            return result;
        }
        public int ByteListTo2sComplimentSignedInt(List<byte> rawData)
        {
            int result = 0;
            return result;
        }
        public int ByteListToSignAndMagnitudeSignedInt(List<byte> rawData)
        {
            int result = 0;
            return result;
        }



        public enum SignedType
        {
            UNSIGNED,
            ONES_COMPLIMENT_SIGNED,
            TWOS_COMPLIMENT_SIGNED,
            SIGN_AND_MAGNITUDE
        }
    }
}
