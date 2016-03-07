using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace InputDataItemLibrary
{
    public class InputDataItem
    {
        //-----------------------------------------------------------------------------------------
        // Class Functions
        //-----------------------------------------------------------------------------------------

        /// <summary>
        /// Class constructor.
        /// </summary>
        /// <param name="itemName">Name of the data item being created.</param>
        public InputDataItem(String itemName)
        {
            ItemSettings = new DataItemSettings();
            ItemSettings.DataName = itemName;
            m_dataItemForm = new InputDataItemForm();
        }

        /// <summary>
        /// Class Constructor.
        /// </summary>
        /// <param name="itemSettings">Item settings for this data item.</param>
        public InputDataItem(DataItemSettings itemSettings)
        {
            ItemSettings = itemSettings;
            m_dataItemForm = new InputDataItemForm();
        }

        /// <summary>
        /// Display the settings for this data item.
        /// </summary>
        public void ShowDataItemSettings()
        {
            m_dataItemForm.DisplaySettings(ItemSettings);
        }

        /// <summary>
        /// Update the settings for this data item with the provided item settings.
        /// </summary>
        /// <param name="itemSettings">New data item settings for this data item.</param>
        public void UpdateDataItemSettings(DataItemSettings itemSettings)
        {
            ItemSettings = itemSettings;
        }

        /// <summary>
        /// Update the settings for this data item with the provided item settings.
        /// </summary>
        /// <param name="dataName">Name of the data item.</param>
        /// <param name="minLength">Minimum data length.</param>
        /// <param name="maxLength">Maximum data length.</param>
        public void UpdatedataItemSettings(String dataName, Int32 minLength, Int32 maxLength)
        {
            ItemSettings.DataName = dataName;
            ItemSettings.MinDataLength = minLength;
            ItemSettings.MaxDataLength = maxLength;
        }

        /// <summary>
        /// Validate if the data matches the criteria for this data item.
        /// </summary>
        /// <param name="inputData">Input data to validate.</param>
        /// <param name="byteCount">Number of bytes in the input data.</param>
        /// <returns>Flag indicating if the input data matches the criteria for this data item.</returns>
        public Boolean ValidateInputData(byte[] inputData, Int32 byteCount)
        {
            Boolean validData = false;
            // Verify the length is correct
            if ((ValidMinDataLength <= byteCount) && (byteCount <= ValidMaxDataLength))
            {   // Length is good
                validData = true;
            }
            // Return the flag indicating if the data input is valid for this data item
            return validData;
        }

        /// <summary>
        /// Validate if the input data matches the criteria for this data item.
        /// </summary>
        /// <param name="inputData">Input data to validate.</param>
        /// <param name="byteCount">Number of bytes in the input data.</param>
        /// <returns>Flag indicating if the input data matches the criteria for this data item.</returns>
        public Boolean ValidateInputData(char[] inputData, Int32 byteCount)
        {
            Boolean validData = false;
            // Verify if the length is correct
            if ((ValidMinDataLength <= byteCount) && (byteCount <= ValidMaxDataLength))
            {   // Length is good
                validData = true;
            }
            // Return the flag indicating if the input data is valid for this data item
            return validData;
        }

        /// <summary>
        /// Determine if the input data matches the match criteria.
        /// </summary>
        /// <param name="inputdata">Data to evaluate.</param>
        /// <param name="matchData">Data to match.</param>
        /// <param name="startIndex">Index into the inputData to start the evaluation.</param>
        /// <param name="length">Number of bytes to match.</param>
        /// <returns>Flag indicating if the inputdata matches the criteria.</returns>
        private Boolean DataMatches(byte[] inputdata, String matchData, Int32 startIndex, Int32 length)
        {
            Boolean dataMatches = true;
            for (Int32 index = startIndex; (index < length) && dataMatches; index++)
            {   // Check each byte to make sure it matches
                dataMatches = (inputdata[index] == Convert.ToByte(matchData[index - startIndex]));
            }
            return dataMatches;
        }

        /// <summary>
        /// Determine if the input data matches the match criteria.
        /// </summary>
        /// <param name="inputData">Data to evaluate.</param>
        /// <param name="matchData">Data to match.</param>
        /// <param name="startIndex">Index into the inputData to start the evaluation.</param>
        /// <param name="length">Number of bytes to match.</param>
        /// <returns>Flag indicating if the inputData matches the criteria.</returns>
        private Boolean DataMatches(char[] inputData, String matchData, Int32 startIndex, Int32 length)
        {
            return matchData == new String(inputData, startIndex, length);
        }



        //-----------------------------------------------------------------------------------------
        // Class Properties
        //-----------------------------------------------------------------------------------------

        /// <summary>
        /// Get/Set the name of the data item
        /// </summary>
        public DataItemSettings ItemSettings
        {
            get { return m_dataItemSettings; }
            set { m_dataItemSettings = value; }
        }

        /// <summary>
        /// Get the minimum data length required for this data item.
        /// </summary>
        public Int32 ValidMinDataLength
        {
            get { return ItemSettings.MinDataLength; }
        }

        /// <summary>
        /// Get the maximum allowable data length for this data item.
        /// </summary>
        public Int32 ValidMaxDataLength
        {
            get { return ItemSettings.MaxDataLength; }
        }



        //-----------------------------------------------------------------------------------------
        // Class Variables
        //-----------------------------------------------------------------------------------------

        /// <summary>
        /// Settings for this data item.
        /// </summary>
        private DataItemSettings m_dataItemSettings;

        /// <summary>
        /// Form for displaying the settings for this data item.
        /// </summary>
        private InputDataItemForm m_dataItemForm;
    }
}
