#pragma once

#define ESI_API __declspec(dllexport)

#define OK 0
#define FAIL -1
#define INVALID_INDEX -2
#define INVALID_PARAMETER -3
#define INVALID_STATE -4

//Enumeration of the available pressure units
enum PressureUnits
{
   bar,
   mbar,
   psi,
   MPa,
   Pa,
   mmH2O,
   mmHg,
   atm,
   kgcm2,
   kPa
};

//Enumeration of the available temperature units
enum TemperatureUnits
{
   C,
   K,
   F
};

const long long ticksTo1904 = 600527520000000000;
const long ticksPerSecond = 10000000;

extern "C"
{
   //SETUP AND INITIALISATION
   //Searches all serial ports to find pressure sensors.  The number of sensors that were found is returned in the SensorCount parameter.
   ESI_API int FindSensors(int* SensorCount);

   //The number of sensors that were found is returned in the SensorCount parameter.
   ESI_API int FindSensorsEx(int sensorType, int* SensorCount);

   //Retrieves information for a particular sensor
   ESI_API int GetSensorInfo(int SensorIndex, int* PortNumber, char* SerialNumber, int SerialNumberLength);

   //Retrieves information for a particular sensor
   ESI_API int GetSensorInfoEx(int SensorIndex, int* PortNumber, char* SerialNumber, int SerialNumberLength, int* IsFast);

   //Opens communications with a sensor and sets it ready for use
   ESI_API int UseSensor(int SensorIndex);

   //Used to check whether a sensor is in use
   ESI_API int IsSensorUsed(int SensorIndex, int* used);

   //Closes communications with a sensor and releases it
   ESI_API int ReleaseSensor(int SensorIndex);

   // Retrieves the manufacture date for the given sensor
   ESI_API int GetManufactureDate(int SensorIndex, long long* Date);

   // Retrieves the calibration date for the given sensor
   ESI_API int GetCalibrationDate(int SensorIndex, long long* Date);

   // Retrieves the manufacture date for the given sensor
   ESI_API int GetManufactureLabVIEWDate(int SensorIndex, long* Date);

   // Retrieves the calibration date for the given sensor
   ESI_API int GetCalibrationLabVIEWDate(int SensorIndex, long* Date);


   //INFORMATION
   //Gets the version of the API
   ESI_API int GetAPIVersion(char* Version, int Length);

   //Retrieves the pressure range for a given sensor.
   ESI_API int GetPressureRange(int SensorIndex, float* Range);

   //This function is given a pressure units code and a string containing the units in text form is returned in the UnitString parameter.
   ESI_API int GetPressureUnits(int Units, char* UnitString, int UnitStringLength);

   //This function is given a temperature units code and a string containing the units in text form is returned in the UnitString parameter.
   ESI_API int GetTemperatureUnits(int Units, char* UnitString, int UnitStringLength);


   //OPERATION
   //This function reads the pressure in the selected engineering units.
   ESI_API int Read(int SensorIndex, int Units, int Absolute, float Temperature, float* Pressure);
   
   //This function reads the temperature in the selected engineering units.
   ESI_API int ReadTemperature(int SensorIndex, int Units, float* Temperature);

   //This sets the difference between absolute and gauge pressures in bar.  The value is used in the Read function when giving absolute values.
   ESI_API int SetGaugeDifferential(int SensorIndex, float Differential);

   //This starts all the sensors monitoring.
   ESI_API int StartMonitoring(double PressureInterval, double TemperatureInterval);

   //This function stops data monitoring.
   ESI_API int StopMonitoring();

   //Retrieves data collected during monitoring.
   ESI_API int GetMonitorData(int SensorIndex, float* Time, float* Pressure, float* Temperature, int* Length, int Units, int Absolute, int Units2);

   //Gets the actual data rate achieved for a given requested rate.
   ESI_API int GetActualRate(double RequestedRate, double* ActualRate);

   //Zeros the sensor reading
   ESI_API int ZeroSensor(int SensorIndex, int Count, double Interval);




   //CLEAN-UP
   //This function must be called when the application code has finished using the library.  This closes communications and cleans up all system resources.
   ESI_API int CleanUp();
}