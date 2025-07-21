#pragma once
#include <SDHCI.h>
#include <File.h>

class Logger {
public:
    Logger();
    bool begin();
    bool appendLog(const char* message);
    const char* createMessage(unsigned long currentTime, const String& currentDate, 
                        int state, double lat, double lng, double alt,
                        double distance, double direction, int mr_pwm, int ml_pwm,
                        int mOutputTime, int cds, double ax, double ay, double az,
                        double gx, double gy, double gz, double mx, double my, double mz,
                        double roll, double pitch, double heading);
private:
    SDClass _sd;
    File _myFile;
    const String CSV_HEADER = "time,date,mode,lat,lng,alt,distance,direction,mr_pwm,ml_pwm,mOutputTime,cds,ax,ay,az,gx,gy,gz,mx,my,mz,roll,pitch,heading";
    char _logBuffer[300];
    
    bool sdInit();
    bool createLogFile();
    void tweliteSend(const char* message);
};