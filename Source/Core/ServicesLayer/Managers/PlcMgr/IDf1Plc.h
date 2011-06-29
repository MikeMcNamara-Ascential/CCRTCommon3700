class IPlcDll
{
public:
	IPlcDll( const char *dllPath);

	~IPlcDll();


	PlcHandle_t PlcOpen( PlcConfig_t *plcConfig);
	
	int PlcClose( PlcHandle_t *plc);
	
	int PlcRead( PlcHandle_t plc, uint32_t *buff, uint32_t count);
	
	int PlcWrite( PlcHandle_t plc, const uint32_t *buff, uint32_t count);
	
	void PlcInit(void);

private:



}

