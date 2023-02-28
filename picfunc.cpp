#include <windows.h>
#include <stdio.h>
#include "picfunc.h"

// NMC Functions
extern "C" Func_NmcInit LoadNmcInit(HINSTANCE hModule){
    Func_NmcInit NmcInit;
    
    if (hModule == NULL) {
        printf("Error Boss\n");
        printf("%d",GetLastError());
        return 0;
    }

    NmcInit = (Func_NmcInit)GetProcAddress(hModule,"NmcInit");
    if (NmcInit == NULL) {
        printf("function null");
        return 0;
    }

    return NmcInit;
}

extern "C" Func_NmcGetModType LoadNmcGetModType(HINSTANCE hModule){
    Func_NmcGetModType NmcGetModType;
    
    if (hModule == NULL) {
        printf("Error Boss\n");
        printf("%d",GetLastError());
        return 0;
    }

    NmcGetModType = (Func_NmcGetModType)GetProcAddress(hModule,"NmcGetModType");
    if (NmcGetModType == NULL) {
        printf("function null");
        return 0;
    }

    return NmcGetModType;
}

extern "C" Func_NmcNoOp LoadNmcNoOp(HINSTANCE hModule){
    Func_NmcNoOp NmcNoOp;
    
    if (hModule == NULL) {
        printf("Error Boss\n");
        printf("%d",GetLastError());
        return 0;
    }

    NmcNoOp = (Func_NmcNoOp)GetProcAddress(hModule,"NmcNoOp");
    if (NmcNoOp == NULL) {
        printf("function null");
        return 0;
    }

    return NmcNoOp;
}

extern "C" Func_NmcGetStat LoadNmcGetStat(HINSTANCE hModule){
    Func_NmcGetStat NmcGetStat;
    
    if (hModule == NULL) {
        printf("Error Boss\n");
        printf("%d",GetLastError());
        return 0;
    }

    NmcGetStat = (Func_NmcGetStat)GetProcAddress(hModule,"NmcGetStat");
    if (NmcGetStat == NULL) {
        printf("function null");
        return 0;
    }

    return NmcGetStat;
}

extern "C" Func_NmcShutdown LoadNmcShutdown(HINSTANCE hModule){
    Func_NmcShutdown NmcShutdown;
    
    if (hModule == NULL) {
        printf("Error Boss\n");
        printf("%d",GetLastError());
        return 0;
    }

    NmcShutdown = (Func_NmcShutdown)GetProcAddress(hModule,"NmcShutdown");
    if (NmcShutdown == NULL) {
        printf("function null");
        return 0;
    }

    return NmcShutdown;
}



// Servo Functions
extern "C" Func_ServoResetPos LoadServoResetPos(HINSTANCE hModule){
    Func_ServoResetPos ServoResetPos;
    
    if (hModule == NULL) {
        printf("Error Boss\n");
        printf("%d",GetLastError());
        return 0;
    }

    ServoResetPos = (Func_ServoResetPos)GetProcAddress(hModule,"ServoResetPos");
    if (ServoResetPos == NULL) {
        printf("function null");
        return 0;
    }

    return ServoResetPos;
}


extern "C" Func_ServoStopMotor LoadServoStopMotor(HINSTANCE hModule){
    Func_ServoStopMotor ServoStopMotor;
    
    if (hModule == NULL) {
        printf("Error Boss\n");
        printf("%d",GetLastError());
        return 0;
    }

    ServoStopMotor = (Func_ServoStopMotor)GetProcAddress(hModule,"ServoStopMotor");
    if (ServoStopMotor == NULL) {
        printf("function null");
        return 0;
    }

    return ServoStopMotor;
}

extern "C" Func_ServoLoadTraj LoadServoLoadTraj(HINSTANCE hModule){
    Func_ServoLoadTraj ServoLoadTraj;
    
    if (hModule == NULL) {
        printf("Error Boss\n");
        printf("%d",GetLastError());
        return 0;
    }

    ServoLoadTraj = (Func_ServoLoadTraj)GetProcAddress(hModule,"ServoLoadTraj");
    if (ServoLoadTraj == NULL) {
        printf("function null");
        return 0;
    }

    return ServoLoadTraj;
}

extern "C" Func_ServoSetGain LoadServoSetGain(HINSTANCE hModule){
    Func_ServoSetGain ServoSetGain;
    
    if (hModule == NULL) {
        printf("Error Boss\n");
        printf("%d",GetLastError());
        return 0;
    }

    ServoSetGain = (Func_ServoSetGain)GetProcAddress(hModule,"ServoSetGain");
    if (ServoSetGain == NULL) {
        printf("function null");
        return 0;
    }

    return ServoSetGain;
}