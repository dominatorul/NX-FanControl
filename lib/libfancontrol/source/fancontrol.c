#include "fancontrol.h"
#include "tmp451.h"

//Fan curve table
const TemperaturePoint defaultTable[] =
{
    { .temperature_c = 25.0, .fanLevel_f = 0.10 },
    { .temperature_c = 30.0, .fanLevel_f = 0.20 },
    { .temperature_c = 35.0, .fanLevel_f = 0.30 },
    { .temperature_c = 40.0, .fanLevel_f = 0.40 },
    { .temperature_c = 45.0, .fanLevel_f = 0.50 },
    { .temperature_c = 50.0, .fanLevel_f = 0.60 },
    { .temperature_c = 55.0, .fanLevel_f = 0.70 },
    { .temperature_c = 60.0, .fanLevel_f = 0.80 },
    { .temperature_c = 65.0, .fanLevel_f = 0.90 },
    { .temperature_c = 70.0, .fanLevel_f = 1.00 }
};


TemperaturePoint *fanControllerTable;

//Fan
Thread FanControllerThread;
bool fanControllerThreadExit = false;

//Log
char logPath[PATH_MAX];

//Power management
Event powerStateChangeEvent;
bool isPowerStateInitialized = false;

void CreateDir(char *dir)
{
    char dirPath[PATH_MAX];

    for(int i = 0; i < PATH_MAX; i++)
    {
        if(*(dir + i) == '/' && access(dirPath, F_OK) == -1)
        {
            mkdir(dirPath, 0777);
        }
        dirPath[i] = *(dir + i);
    }
}

void InitLog()
{
    if(access(LOG_DIR, F_OK) == -1)
        CreateDir(LOG_DIR);

    if(access(LOG_FILE, F_OK) != -1)
        remove(LOG_FILE);
}

void WriteLog(char *buffer)
{
    FILE *log = fopen(LOG_FILE, "a");
    if(log != NULL)
    {
        fprintf(log, "%s\n", buffer);  
    }
    fclose(log);
}

void WriteConfigFile(TemperaturePoint *table)
{
    if(table == NULL)
    {
        table = malloc(sizeof(defaultTable));
        memcpy(table, defaultTable, sizeof(defaultTable));
    }

    if(access(CONFIG_DIR, F_OK) == -1)
        CreateDir(CONFIG_DIR);

    FILE *config = fopen(CONFIG_FILE, "w");
    fwrite(table, TABLE_SIZE, 1, config);
    fclose(config);
}



void ReadConfigFile(TemperaturePoint **table_out)
{
    InitLog();

    *table_out = malloc(sizeof(defaultTable));
    memcpy(*table_out, defaultTable, sizeof(defaultTable));

    if(access(CONFIG_DIR, F_OK) == -1)
    {
        CreateDir(CONFIG_DIR);
        WriteConfigFile(NULL);
    }
    else
    {
        if(access(CONFIG_FILE, F_OK) == -1)
        {
            WriteConfigFile(NULL);
        }
        else
        {
            FILE *config = fopen(CONFIG_FILE, "r");
            fread(*table_out, TABLE_SIZE, 1, config);
            fclose(config);
        }
    }    
}

bool IsSystemAwake()
{
    // Check if system is in sleep mode by checking applet state
    // appletGetOperationMode returns the current operation mode
    AppletOperationMode opMode = appletGetOperationMode();
    
    // If in handheld or console mode, system is awake
    // If in any other mode (like sleep), we consider it asleep
    return (opMode == AppletOperationMode_Handheld || opMode == AppletOperationMode_Console);
}

void InitFanController(TemperaturePoint *table)
{
    fanControllerTable = table;

    if(R_FAILED(threadCreate(&FanControllerThread, FanControllerThreadFunction, NULL, NULL, 0x4000, 0x3F, -2)))
    {
        WriteLog("Error creating FanControllerThread");
        diagAbortWithResult(MAKERESULT(Module_Libnx, LibnxError_ShouldNotHappen));
    }
}

void FanControllerThreadFunction(void*)
{
    FanController fc;
    float fanLevelSet_f = 0;
    float temperatureC_f = 0;
    u64 awakeSleepTime = 250000000ULL; // 0.25 second when awake (250ms - responsive)
    u64 sleepSleepTime = 5000000000ULL; // 5 seconds when in sleep
    int sleepCheckCounter = 0;

    Result rs = fanOpenController(&fc, 0x3D000001);
    if(R_FAILED(rs))
    {
        WriteLog("Error opening fanController");
        diagAbortWithResult(MAKERESULT(Module_Libnx, LibnxError_ShouldNotHappen));
    }

    while(!fanControllerThreadExit)
    {
        // Check if system is awake every 20 iterations (~5 seconds) to reduce overhead
        sleepCheckCounter++;
        if(sleepCheckCounter >= 20)
        {
            bool isAwake = IsSystemAwake();
            sleepCheckCounter = 0;
            
            // If system is asleep, use longer sleep interval
            if(!isAwake)
            {
                svcSleepThread(sleepSleepTime);
                continue;
            }
        }

        rs = Tmp451GetSocTemp(&temperatureC_f);
        if(R_FAILED(rs))
        {
            WriteLog("tsSessionGetTemperature error");
            diagAbortWithResult(MAKERESULT(Module_Libnx, LibnxError_ShouldNotHappen));
        }

        if(temperatureC_f >= 0 && temperatureC_f <= fanControllerTable->temperature_c)
        {
            float m = 0;
            float q = 0;

            m = fanControllerTable->fanLevel_f / fanControllerTable->temperature_c;
            q = 0 - m;

            fanLevelSet_f = (m * temperatureC_f) + q;

        }else if(temperatureC_f >= (fanControllerTable + 9)->temperature_c)
        {
            fanLevelSet_f = (fanControllerTable + 9)->fanLevel_f;
        }else
        {
            for(int i = 0; i < (TABLE_SIZE/sizeof(TemperaturePoint)) - 1; i++)
            {
                if(temperatureC_f >= (fanControllerTable + i)->temperature_c && temperatureC_f <= (fanControllerTable + i + 1)->temperature_c)
                {
                    float m = 0;
                    float q = 0;

                    m = ((fanControllerTable + i + 1)->fanLevel_f - (fanControllerTable + i)->fanLevel_f ) / ((fanControllerTable + i + 1)->temperature_c - (fanControllerTable + i)->temperature_c);
                    q = (fanControllerTable + i)->fanLevel_f - (m * (fanControllerTable + i)->temperature_c);

                    fanLevelSet_f = (m * temperatureC_f) + q;
                    break;
                }
            }
        }

        // Always update fan speed for immediate response
        rs = fanControllerSetRotationSpeedLevel(&fc, fanLevelSet_f);
        if(R_FAILED(rs))
        {
            WriteLog("fanControllerSetRotationSpeedLevel error");
            diagAbortWithResult(MAKERESULT(Module_Libnx, LibnxError_ShouldNotHappen));
        }
        
        // Use responsive sleep time when awake (250ms)
        svcSleepThread(awakeSleepTime);
    }

    fanControllerClose(&fc);
}

void StartFanControllerThread()
{
    if(R_FAILED(threadStart(&FanControllerThread)))
    {
        WriteLog("Error starting FanControllerThread");
        diagAbortWithResult(MAKERESULT(Module_Libnx, LibnxError_ShouldNotHappen));
    }
}

void CloseFanControllerThread()
{   
    Result rs;
    fanControllerThreadExit = true;
    rs = threadWaitForExit(&FanControllerThread);
    if(R_FAILED(rs))
    {
        WriteLog("Error waiting fanControllerThread");
        diagAbortWithResult(MAKERESULT(Module_Libnx, LibnxError_ShouldNotHappen));
    }
    threadClose(&FanControllerThread);
    fanControllerThreadExit = false;
    free(fanControllerTable);
}

void WaitFanController()
{
    if(R_FAILED(threadWaitForExit(&FanControllerThread)))
    {
        WriteLog("Error waiting fanControllerThread");
        diagAbortWithResult(MAKERESULT(Module_Libnx, LibnxError_ShouldNotHappen));
    }
}