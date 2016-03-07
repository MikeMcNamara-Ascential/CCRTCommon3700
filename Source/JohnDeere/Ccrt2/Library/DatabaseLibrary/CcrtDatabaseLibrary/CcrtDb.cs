// 
//  ____  _     __  __      _        _ 
// |  _ \| |__ |  \/  | ___| |_ __ _| |
// | | | | '_ \| |\/| |/ _ \ __/ _` | |
// | |_| | |_) | |  | |  __/ || (_| | |
// |____/|_.__/|_|  |_|\___|\__\__,_|_|
//
// Auto-generated from main on 2012-02-01 10:06:16Z.
// Please visit http://code.google.com/p/dblinq2007/ for more information.
//
namespace CcrtDb
{
	using System;
	using System.ComponentModel;
	using System.Data;
#if MONO_STRICT
	using System.Data.Linq;
#else   // MONO_STRICT
	using DbLinq.Data.Linq;
	using DbLinq.Vendor;
#endif  // MONO_STRICT
	using System.Data.Linq.Mapping;
	using System.Diagnostics;
	
	
	public partial class Main : DataContext
	{
		
		#region Extensibility Method Declarations
		partial void OnCreated();
		#endregion
		
		
		public Main(string connectionString) : 
				base(connectionString)
		{
			this.OnCreated();
		}
		
		public Main(string connection, MappingSource mappingSource) : 
				base(connection, mappingSource)
		{
			this.OnCreated();
		}
		
		public Main(IDbConnection connection, MappingSource mappingSource) : 
				base(connection, mappingSource)
		{
			this.OnCreated();
		}
		
		public Table<InputDataItems> InputDataItems
		{
			get
			{
				return this.GetTable<InputDataItems>();
			}
		}
		
		public Table<MaintenanceReminders> MaintenanceReminders
		{
			get
			{
				return this.GetTable<MaintenanceReminders>();
			}
		}
		
		public Table<PortSettings> PortSettings
		{
			get
			{
				return this.GetTable<PortSettings>();
			}
		}
	}
	
	#region Start MONO_STRICT
#if MONO_STRICT

	public partial class Main
	{
		
		public Main(IDbConnection connection) : 
				base(connection)
		{
			this.OnCreated();
		}
	}
	#region End MONO_STRICT
	#endregion
#else     // MONO_STRICT
	
	public partial class Main
	{
		
		public Main(IDbConnection connection) : 
				base(connection, new DbLinq.Sqlite.SqliteVendor())
		{
			this.OnCreated();
		}
		
		public Main(IDbConnection connection, IVendor sqlDialect) : 
				base(connection, sqlDialect)
		{
			this.OnCreated();
		}
		
		public Main(IDbConnection connection, MappingSource mappingSource, IVendor sqlDialect) : 
				base(connection, mappingSource, sqlDialect)
		{
			this.OnCreated();
		}
	}
	#region End Not MONO_STRICT
	#endregion
#endif     // MONO_STRICT
	#endregion
	
	[Table(Name="main.InputDataItems")]
	public partial class InputDataItems : System.ComponentModel.INotifyPropertyChanging, System.ComponentModel.INotifyPropertyChanged
	{
		
		private static System.ComponentModel.PropertyChangingEventArgs emptyChangingEventArgs = new System.ComponentModel.PropertyChangingEventArgs("");
		
		private string _dataName;
		
		private int _maxLength;
		
		private int _minLength;
		
		#region Extensibility Method Declarations
		partial void OnCreated();
		
		partial void OnDataNameChanged();
		
		partial void OnDataNameChanging(string value);
		
		partial void OnMaxLengthChanged();
		
		partial void OnMaxLengthChanging(int value);
		
		partial void OnMinLengthChanged();
		
		partial void OnMinLengthChanging(int value);
		#endregion
		
		
		public InputDataItems()
		{
			this.OnCreated();
		}
		
		[Column(Storage="_dataName", Name="DataName", DbType="VARCHAR(64)", IsPrimaryKey=true, AutoSync=AutoSync.Never, CanBeNull=false)]
		[DebuggerNonUserCode()]
		public string DataName
		{
			get
			{
				return this._dataName;
			}
			set
			{
				if (((_dataName == value) 
							== false))
				{
					this.OnDataNameChanging(value);
					this.SendPropertyChanging();
					this._dataName = value;
					this.SendPropertyChanged("DataName");
					this.OnDataNameChanged();
				}
			}
		}
		
		[Column(Storage="_maxLength", Name="MaxLength", DbType="INTEGER", AutoSync=AutoSync.Never, CanBeNull=false)]
		[DebuggerNonUserCode()]
		public int MaxLength
		{
			get
			{
				return this._maxLength;
			}
			set
			{
				if ((_maxLength != value))
				{
					this.OnMaxLengthChanging(value);
					this.SendPropertyChanging();
					this._maxLength = value;
					this.SendPropertyChanged("MaxLength");
					this.OnMaxLengthChanged();
				}
			}
		}
		
		[Column(Storage="_minLength", Name="MinLength", DbType="INTEGER", AutoSync=AutoSync.Never, CanBeNull=false)]
		[DebuggerNonUserCode()]
		public int MinLength
		{
			get
			{
				return this._minLength;
			}
			set
			{
				if ((_minLength != value))
				{
					this.OnMinLengthChanging(value);
					this.SendPropertyChanging();
					this._minLength = value;
					this.SendPropertyChanged("MinLength");
					this.OnMinLengthChanged();
				}
			}
		}
		
		public event System.ComponentModel.PropertyChangingEventHandler PropertyChanging;
		
		public event System.ComponentModel.PropertyChangedEventHandler PropertyChanged;
		
		protected virtual void SendPropertyChanging()
		{
			System.ComponentModel.PropertyChangingEventHandler h = this.PropertyChanging;
			if ((h != null))
			{
				h(this, emptyChangingEventArgs);
			}
		}
		
		protected virtual void SendPropertyChanged(string propertyName)
		{
			System.ComponentModel.PropertyChangedEventHandler h = this.PropertyChanged;
			if ((h != null))
			{
				h(this, new System.ComponentModel.PropertyChangedEventArgs(propertyName));
			}
		}
	}
	
	[Table(Name="main.MaintenanceReminders")]
	public partial class MaintenanceReminders
	{
		
		private float _elapsedTime;
		
		private string _faultText;
		
		private string _intervalUnits;
		
		private int _maintenanceInterval;
		
		private string _maintenanceItem;
		
		private string _maintenanceNotes;
		
		#region Extensibility Method Declarations
		partial void OnCreated();
		
		partial void OnElapsedTimeChanged();
		
		partial void OnElapsedTimeChanging(float value);
		
		partial void OnFaultTextChanged();
		
		partial void OnFaultTextChanging(string value);
		
		partial void OnIntervalUnitsChanged();
		
		partial void OnIntervalUnitsChanging(string value);
		
		partial void OnMaintenanceIntervalChanged();
		
		partial void OnMaintenanceIntervalChanging(int value);
		
		partial void OnMaintenanceItemChanged();
		
		partial void OnMaintenanceItemChanging(string value);
		
		partial void OnMaintenanceNotesChanged();
		
		partial void OnMaintenanceNotesChanging(string value);
		#endregion
		
		
		public MaintenanceReminders()
		{
			this.OnCreated();
		}
		
		[Column(Storage="_elapsedTime", Name="ElapsedTime", DbType="FLOAT", AutoSync=AutoSync.Never, CanBeNull=false)]
		[DebuggerNonUserCode()]
		public float ElapsedTime
		{
			get
			{
				return this._elapsedTime;
			}
			set
			{
				if ((_elapsedTime != value))
				{
					this.OnElapsedTimeChanging(value);
					this._elapsedTime = value;
					this.OnElapsedTimeChanged();
				}
			}
		}
		
		[Column(Storage="_faultText", Name="FaultText", DbType="VARCHAR(512)", AutoSync=AutoSync.Never, CanBeNull=false)]
		[DebuggerNonUserCode()]
		public string FaultText
		{
			get
			{
				return this._faultText;
			}
			set
			{
				if (((_faultText == value) 
							== false))
				{
					this.OnFaultTextChanging(value);
					this._faultText = value;
					this.OnFaultTextChanged();
				}
			}
		}
		
		[Column(Storage="_intervalUnits", Name="IntervalUnits", DbType="VARCHAR(20)", AutoSync=AutoSync.Never, CanBeNull=false)]
		[DebuggerNonUserCode()]
		public string IntervalUnits
		{
			get
			{
				return this._intervalUnits;
			}
			set
			{
				if (((_intervalUnits == value) 
							== false))
				{
					this.OnIntervalUnitsChanging(value);
					this._intervalUnits = value;
					this.OnIntervalUnitsChanged();
				}
			}
		}
		
		[Column(Storage="_maintenanceInterval", Name="MaintenanceInterval", DbType="INTEGER", AutoSync=AutoSync.Never, CanBeNull=false)]
		[DebuggerNonUserCode()]
		public int MaintenanceInterval
		{
			get
			{
				return this._maintenanceInterval;
			}
			set
			{
				if ((_maintenanceInterval != value))
				{
					this.OnMaintenanceIntervalChanging(value);
					this._maintenanceInterval = value;
					this.OnMaintenanceIntervalChanged();
				}
			}
		}
		
		[Column(Storage="_maintenanceItem", Name="MaintenanceItem", DbType="VARCHAR(128)", AutoSync=AutoSync.Never, CanBeNull=false)]
		[DebuggerNonUserCode()]
		public string MaintenanceItem
		{
			get
			{
				return this._maintenanceItem;
			}
			set
			{
				if (((_maintenanceItem == value) 
							== false))
				{
					this.OnMaintenanceItemChanging(value);
					this._maintenanceItem = value;
					this.OnMaintenanceItemChanged();
				}
			}
		}
		
		[Column(Storage="_maintenanceNotes", Name="MaintenanceNotes", DbType="TEXT", AutoSync=AutoSync.Never)]
		[DebuggerNonUserCode()]
		public string MaintenanceNotes
		{
			get
			{
				return this._maintenanceNotes;
			}
			set
			{
				if (((_maintenanceNotes == value) 
							== false))
				{
					this.OnMaintenanceNotesChanging(value);
					this._maintenanceNotes = value;
					this.OnMaintenanceNotesChanged();
				}
			}
		}
	}
	
	[Table(Name="main.PortSettings")]
	public partial class PortSettings : System.ComponentModel.INotifyPropertyChanging, System.ComponentModel.INotifyPropertyChanged
	{
		
		private static System.ComponentModel.PropertyChangingEventArgs emptyChangingEventArgs = new System.ComponentModel.PropertyChangingEventArgs("");
		
		private int _baudRate;
		
		private int _dataBits;
		
		private string _deviceName;
		
		private string _deviceType;
		
		private string _headerData;
		
		private int _headerLength;
		
		private bool _monitorData;
		
		private string _parity;
		
		private string _portName;
		
		private int _rxTimeout;
		
		private string _serverName;
		
		private float _stopBits;
		
		private string _trailerData;
		
		private int _trailerLength;
		
		private int _txTimeout;
		
		#region Extensibility Method Declarations
		partial void OnCreated();
		
		partial void OnBaudRateChanged();
		
		partial void OnBaudRateChanging(int value);
		
		partial void OnDataBitsChanged();
		
		partial void OnDataBitsChanging(int value);
		
		partial void OnDeviceNameChanged();
		
		partial void OnDeviceNameChanging(string value);
		
		partial void OnDeviceTypeChanged();
		
		partial void OnDeviceTypeChanging(string value);
		
		partial void OnHeaderDataChanged();
		
		partial void OnHeaderDataChanging(string value);
		
		partial void OnHeaderLengthChanged();
		
		partial void OnHeaderLengthChanging(int value);
		
		partial void OnMonitorDataChanged();
		
		partial void OnMonitorDataChanging(bool value);
		
		partial void OnParityChanged();
		
		partial void OnParityChanging(string value);
		
		partial void OnPortNameChanged();
		
		partial void OnPortNameChanging(string value);
		
		partial void OnRxTimeoutChanged();
		
		partial void OnRxTimeoutChanging(int value);
		
		partial void OnServerNameChanged();
		
		partial void OnServerNameChanging(string value);
		
		partial void OnStopBitsChanged();
		
		partial void OnStopBitsChanging(float value);
		
		partial void OnTrailerDataChanged();
		
		partial void OnTrailerDataChanging(string value);
		
		partial void OnTrailerLengthChanged();
		
		partial void OnTrailerLengthChanging(int value);
		
		partial void OnTxTimeoutChanged();
		
		partial void OnTxTimeoutChanging(int value);
		#endregion
		
		
		public PortSettings()
		{
			this.OnCreated();
		}
		
		[Column(Storage="_baudRate", Name="BaudRate", DbType="INTEGER", AutoSync=AutoSync.Never, CanBeNull=false)]
		[DebuggerNonUserCode()]
		public int BaudRate
		{
			get
			{
				return this._baudRate;
			}
			set
			{
				if ((_baudRate != value))
				{
					this.OnBaudRateChanging(value);
					this.SendPropertyChanging();
					this._baudRate = value;
					this.SendPropertyChanged("BaudRate");
					this.OnBaudRateChanged();
				}
			}
		}
		
		[Column(Storage="_dataBits", Name="DataBits", DbType="INTEGER", AutoSync=AutoSync.Never, CanBeNull=false)]
		[DebuggerNonUserCode()]
		public int DataBits
		{
			get
			{
				return this._dataBits;
			}
			set
			{
				if ((_dataBits != value))
				{
					this.OnDataBitsChanging(value);
					this.SendPropertyChanging();
					this._dataBits = value;
					this.SendPropertyChanged("DataBits");
					this.OnDataBitsChanged();
				}
			}
		}
		
		[Column(Storage="_deviceName", Name="DeviceName", DbType="VARCHAR(32)", IsPrimaryKey=true, AutoSync=AutoSync.Never, CanBeNull=false)]
		[DebuggerNonUserCode()]
		public string DeviceName
		{
			get
			{
				return this._deviceName;
			}
			set
			{
				if (((_deviceName == value) 
							== false))
				{
					this.OnDeviceNameChanging(value);
					this.SendPropertyChanging();
					this._deviceName = value;
					this.SendPropertyChanged("DeviceName");
					this.OnDeviceNameChanged();
				}
			}
		}
		
		[Column(Storage="_deviceType", Name="DeviceType", DbType="VARCHAR(64)", AutoSync=AutoSync.Never)]
		[DebuggerNonUserCode()]
		public string DeviceType
		{
			get
			{
				return this._deviceType;
			}
			set
			{
				if (((_deviceType == value) 
							== false))
				{
					this.OnDeviceTypeChanging(value);
					this.SendPropertyChanging();
					this._deviceType = value;
					this.SendPropertyChanged("DeviceType");
					this.OnDeviceTypeChanged();
				}
			}
		}
		
		[Column(Storage="_headerData", Name="HeaderData", DbType="VARCHAR(32)", AutoSync=AutoSync.Never)]
		[DebuggerNonUserCode()]
		public string HeaderData
		{
			get
			{
				return this._headerData;
			}
			set
			{
				if (((_headerData == value) 
							== false))
				{
					this.OnHeaderDataChanging(value);
					this.SendPropertyChanging();
					this._headerData = value;
					this.SendPropertyChanged("HeaderData");
					this.OnHeaderDataChanged();
				}
			}
		}
		
		[Column(Storage="_headerLength", Name="HeaderLength", DbType="INTEGER", AutoSync=AutoSync.Never, CanBeNull=false)]
		[DebuggerNonUserCode()]
		public int HeaderLength
		{
			get
			{
				return this._headerLength;
			}
			set
			{
				if ((_headerLength != value))
				{
					this.OnHeaderLengthChanging(value);
					this.SendPropertyChanging();
					this._headerLength = value;
					this.SendPropertyChanged("HeaderLength");
					this.OnHeaderLengthChanged();
				}
			}
		}
		
		[Column(Storage="_monitorData", Name="MonitorData", DbType="BOOLEAN", AutoSync=AutoSync.Never, CanBeNull=false)]
		[DebuggerNonUserCode()]
		public bool MonitorData
		{
			get
			{
				return this._monitorData;
			}
			set
			{
				if ((_monitorData != value))
				{
					this.OnMonitorDataChanging(value);
					this.SendPropertyChanging();
					this._monitorData = value;
					this.SendPropertyChanged("MonitorData");
					this.OnMonitorDataChanged();
				}
			}
		}
		
		[Column(Storage="_parity", Name="Parity", DbType="VARCHAR(8)", AutoSync=AutoSync.Never, CanBeNull=false)]
		[DebuggerNonUserCode()]
		public string Parity
		{
			get
			{
				return this._parity;
			}
			set
			{
				if (((_parity == value) 
							== false))
				{
					this.OnParityChanging(value);
					this.SendPropertyChanging();
					this._parity = value;
					this.SendPropertyChanged("Parity");
					this.OnParityChanged();
				}
			}
		}
		
		[Column(Storage="_portName", Name="PortName", DbType="VARCHAR(8)", AutoSync=AutoSync.Never)]
		[DebuggerNonUserCode()]
		public string PortName
		{
			get
			{
				return this._portName;
			}
			set
			{
				if (((_portName == value) 
							== false))
				{
					this.OnPortNameChanging(value);
					this.SendPropertyChanging();
					this._portName = value;
					this.SendPropertyChanged("PortName");
					this.OnPortNameChanged();
				}
			}
		}
		
		[Column(Storage="_rxTimeout", Name="RxTimeout", DbType="INTEGER", AutoSync=AutoSync.Never, CanBeNull=false)]
		[DebuggerNonUserCode()]
		public int RxTimeout
		{
			get
			{
				return this._rxTimeout;
			}
			set
			{
				if ((_rxTimeout != value))
				{
					this.OnRxTimeoutChanging(value);
					this.SendPropertyChanging();
					this._rxTimeout = value;
					this.SendPropertyChanged("RxTimeout");
					this.OnRxTimeoutChanged();
				}
			}
		}
		
		[Column(Storage="_serverName", Name="ServerName", DbType="VARCHAR(128)", AutoSync=AutoSync.Never, CanBeNull=false)]
		[DebuggerNonUserCode()]
		public string ServerName
		{
			get
			{
				return this._serverName;
			}
			set
			{
				if (((_serverName == value) 
							== false))
				{
					this.OnServerNameChanging(value);
					this.SendPropertyChanging();
					this._serverName = value;
					this.SendPropertyChanged("ServerName");
					this.OnServerNameChanged();
				}
			}
		}
		
		[Column(Storage="_stopBits", Name="StopBits", DbType="FLOAT", AutoSync=AutoSync.Never, CanBeNull=false)]
		[DebuggerNonUserCode()]
		public float StopBits
		{
			get
			{
				return this._stopBits;
			}
			set
			{
				if ((_stopBits != value))
				{
					this.OnStopBitsChanging(value);
					this.SendPropertyChanging();
					this._stopBits = value;
					this.SendPropertyChanged("StopBits");
					this.OnStopBitsChanged();
				}
			}
		}
		
		[Column(Storage="_trailerData", Name="TrailerData", DbType="VARCHAR(32)", AutoSync=AutoSync.Never)]
		[DebuggerNonUserCode()]
		public string TrailerData
		{
			get
			{
				return this._trailerData;
			}
			set
			{
				if (((_trailerData == value) 
							== false))
				{
					this.OnTrailerDataChanging(value);
					this.SendPropertyChanging();
					this._trailerData = value;
					this.SendPropertyChanged("TrailerData");
					this.OnTrailerDataChanged();
				}
			}
		}
		
		[Column(Storage="_trailerLength", Name="TrailerLength", DbType="INTEGER", AutoSync=AutoSync.Never, CanBeNull=false)]
		[DebuggerNonUserCode()]
		public int TrailerLength
		{
			get
			{
				return this._trailerLength;
			}
			set
			{
				if ((_trailerLength != value))
				{
					this.OnTrailerLengthChanging(value);
					this.SendPropertyChanging();
					this._trailerLength = value;
					this.SendPropertyChanged("TrailerLength");
					this.OnTrailerLengthChanged();
				}
			}
		}
		
		[Column(Storage="_txTimeout", Name="TxTimeout", DbType="INTEGER", AutoSync=AutoSync.Never, CanBeNull=false)]
		[DebuggerNonUserCode()]
		public int TxTimeout
		{
			get
			{
				return this._txTimeout;
			}
			set
			{
				if ((_txTimeout != value))
				{
					this.OnTxTimeoutChanging(value);
					this.SendPropertyChanging();
					this._txTimeout = value;
					this.SendPropertyChanged("TxTimeout");
					this.OnTxTimeoutChanged();
				}
			}
		}
		
		public event System.ComponentModel.PropertyChangingEventHandler PropertyChanging;
		
		public event System.ComponentModel.PropertyChangedEventHandler PropertyChanged;
		
		protected virtual void SendPropertyChanging()
		{
			System.ComponentModel.PropertyChangingEventHandler h = this.PropertyChanging;
			if ((h != null))
			{
				h(this, emptyChangingEventArgs);
			}
		}
		
		protected virtual void SendPropertyChanged(string propertyName)
		{
			System.ComponentModel.PropertyChangedEventHandler h = this.PropertyChanged;
			if ((h != null))
			{
				h(this, new System.ComponentModel.PropertyChangedEventArgs(propertyName));
			}
		}
	}
}
