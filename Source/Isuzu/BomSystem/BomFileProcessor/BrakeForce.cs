using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace BomFileProcessor
{
    public class BrakeForce
    {
        //-----------------------------------------------------------------------------------------
        // Member Functions
        //-----------------------------------------------------------------------------------------
        /// <summary>
        /// Constructor to use for enumeration purposes.
        /// </summary>
        public BrakeForce()
        {   // nothing to do here
        }

        /// <summary>
        /// Construct a new BrakeForce set from the supplied values.
        /// </summary>
        /// <param name="modelCode">Model code of the vehicle this set belongs to.</param>
        /// <param name="frontMaxDrag">Maximum drag force for each front wheel.</param>
        /// <param name="rearMaxDrag">Maximum drag force for each rear wheel.</param>
        /// <param name="frontMinBrake">Minimum brake force required for each front wheel.</param>
        /// <param name="frontMaxBrake">Maximum brake force allowed for each front wheel.</param>
        /// <param name="rearMinBrake">Minimum brake force required for each rear wheel.</param>
        /// <param name="rearMaxBrake">Maximum brake force allowed for each rear wheel.</param>
        /// <param name="frontBalanceOffset">Front side to side brake balance offset from 50%.</param>
        /// <param name="rearBalanceOffset">REar side to sode brake balance offset from 50%.</param>
        public BrakeForce(String modelCode, Double frontMaxDrag, Double rearMaxDrag,
                          Double frontMinBrake, Double frontMaxBrake, Double rearMinBrake, Double rearMaxBrake,
                          Double frontBalanceOffset, Double rearBalanceOffset, Double frontToRearBalMin,
                          Double frontToRearBalMax)
        {
            ModelCode = modelCode;
            FrontMaxDragForce = frontMaxDrag;
            RearMaxDragForce = rearMaxDrag;
            FrontMinBrakeForce = frontMinBrake;
            FrontMaxBrakeForce = frontMaxBrake;
            RearMinBrakeForce = rearMinBrake;
            RearMaxBrakeForce = rearMaxBrake;
            FrontBalanceOffset = frontBalanceOffset;
            RearBalanceOffset = rearBalanceOffset;
            FrontToRearBalanceMin = frontToRearBalMin;
            FrontToRearBalanceMax = frontToRearBalMax;
        }

        //-----------------------------------------------------------------------------------------
        // Member Variables and Properties
        //-----------------------------------------------------------------------------------------
        /// <summary>
        /// Model Code this parameter set belongs to.
        /// </summary>
        private String m_modelCode;
        /// <summary>
        /// Get/Set the model code this parameter set belongs to.
        /// </summary>
        public String ModelCode
        {
            get { return m_modelCode; }
            set { m_modelCode = value; }
        }

        /// <summary>
        /// Maximum drag force allowed on the front axle.
        /// </summary>
        private Double m_frontMaxDragForce;
        /// <summary>
        /// Get/Set the maximum drag force allowed for each front wheel
        /// </summary>
        public Double FrontMaxDragForce
        {
            get { return m_frontMaxDragForce; }
            set { m_frontMaxDragForce = value; }
        }

        /// <summary>
        /// Maximum drag force allowed on the rear axle.
        /// </summary>
        private Double m_rearMaxDragForce;
        /// <summary>
        /// Get/Set the maximum drag force allowed for eac rear wheel.
        /// </summary>
        public Double RearMaxDragForce
        {
            get { return m_rearMaxDragForce; }
            set { m_rearMaxDragForce = value; }
        }

        /// <summary>
        /// minimum brake force required on the front axle.
        /// </summary>
        private Double m_frontMinBrakeForce;
        /// <summary>
        /// Get/Set the minimum required brake force for each front wheel.
        /// </summary>
        public Double FrontMinBrakeForce
        {
            get { return m_frontMinBrakeForce; }
            set { m_frontMinBrakeForce = value; }
        }

        /// <summary>
        /// Maximum brake force allowed on the front axle.
        /// </summary>
        private Double m_frontMaxBrakeForce;
        /// <summary>
        /// Get/Set the maximum brake force allowed for each front wheel.
        /// </summary>
        public Double FrontMaxBrakeForce
        {
            get { return m_frontMaxBrakeForce; }
            set { m_frontMaxBrakeForce = value; }
        }

        /// <summary>
        /// Minimum brake force required on the rear axle.
        /// </summary>
        private Double m_rearMinBrakeForce;
        /// <summary>
        /// Get/Set the minimum brake force required for each rear wheel.
        /// </summary>
        public Double RearMinBrakeForce
        {
            get { return m_rearMinBrakeForce; }
            set { m_rearMinBrakeForce = value; }
        }

        /// <summary>
        /// Maximum brake force allowed on the rear axle.
        /// </summary>
        private Double m_rearMaxBrakeForce;
        /// <summary>
        /// Get/Set the maximum brake force allowed on each rear wheel.
        /// </summary>
        public Double RearMaxBrakeForce
        {
            get { return m_rearMaxBrakeForce; }
            set { m_rearMaxBrakeForce = value; }
        }

        /// <summary>
        /// Front side to side balance offset from 50%
        /// </summary>
        private Double m_frontBalanceOffset;
        /// <summary>
        /// Get/Set the front side to side balance offset from 50%.
        /// </summary>
        public Double FrontBalanceOffset
        {
            get { return m_frontBalanceOffset; }
            set { m_frontBalanceOffset = value; }
        }

        /// <summary>
        /// Rear side to side balance offset from 50%
        /// </summary>
        private Double m_rearBalanceOffset;
        /// <summary>
        /// Get/Set the rear side to side balance offset from 50%.
        /// </summary>
        public Double RearBalanceOffset
        {
            get { return m_rearBalanceOffset; }
            set { m_rearBalanceOffset = value; }
        }

        /// <summary>
        /// Front to rear balance minimum
        /// </summary>
        private Double m_frontToRearBalanceMin;
        /// <summary>
        /// Get/Set the front to rear balance minimum.
        /// </summary>
        public Double FrontToRearBalanceMin
        {
            get { return m_frontToRearBalanceMin; }
            set { m_frontToRearBalanceMin = value; }
        }

        /// <summary>
        /// Front to rear balance maximum
        /// </summary>
        private Double m_frontToRearBalanceMax;
        /// <summary>
        /// Get/Set the front to rear balance maximum.
        /// </summary>
        public Double FrontToRearBalanceMax
        {
            get { return m_frontToRearBalanceMax; }
            set { m_frontToRearBalanceMax = value; }
        }
    }
}
