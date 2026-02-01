#include "string.h"
#include "stdlib.h"
#include "pid_ctrl.h"
#include <stdint.h>

void SetPIDAntiWindupGainFloat(PIDCtrlFloat_t *l_pstPIDCtrl, float l_fKa){
	l_pstPIDCtrl->m_Ka = l_fKa;
}

void InitPIDParam(PIDCtrlFloat_t *l_pstPIDCtrl){
	l_pstPIDCtrl->m_ErrorSum = 0;
	l_pstPIDCtrl->m_PreError = 0;
	l_pstPIDCtrl->m_fOutput = 0;
	l_pstPIDCtrl->m_fBackCalcVal = 0;
	l_pstPIDCtrl->m_fLastNormalOutput = 0;
}


void InitPIDCtrlVarFloat(PIDCtrlFloat_t *l_pPIDCtrl)
{
	memset(l_pPIDCtrl, 0, sizeof(PIDCtrlFloat_t));

	l_pPIDCtrl->m_Kp = 1.0f;
	l_pPIDCtrl->m_MaxErrorSum = PID_DEFAULT_MAXERRORSUM_FLOAT;
}

void SetPIDGainFloat(PIDCtrlFloat_t *l_pstPIDCtrl, float l_fKp, float l_fKi, float l_fKd, float l_fKa)
{
	l_pstPIDCtrl->m_Kp = l_fKp;
	l_pstPIDCtrl->m_Ki = l_fKi;
	l_pstPIDCtrl->m_Kd = l_fKd;
	l_pstPIDCtrl->m_Ka = l_fKa;
	l_pstPIDCtrl->m_ErrorSum = 0;
	l_pstPIDCtrl->m_PreError = 0;
}

void SetPIDConfigVarFloat(PIDCtrlFloat_t *l_pstPIDCtrl, float l_fMaxErrorSum, float l_fMaxOutput, float l_fMinOutput)
{
	l_pstPIDCtrl->m_MaxErrorSum = l_fMaxErrorSum;
	l_pstPIDCtrl->m_MaxOutput = l_fMaxOutput;
	l_pstPIDCtrl->m_MinOutput = l_fMinOutput;
}

float CalcuatePIDOutputFloat(PIDCtrlFloat_t *l_pPIDCtrl, float l_fError)
{
	float l_fErrorDiff, l_fOutput, l_fRet;

	l_fErrorDiff = l_fError - l_pPIDCtrl->m_PreError;

	l_pPIDCtrl->m_ErrorSum += l_fError;

	l_fOutput = ( (l_fError*l_pPIDCtrl->m_Kp) + (l_pPIDCtrl->m_ErrorSum*l_pPIDCtrl->m_Ki) + (l_fErrorDiff*l_pPIDCtrl->m_Kd) );

	if( l_fOutput > l_pPIDCtrl->m_MaxOutput )
	{
		l_fRet = l_pPIDCtrl->m_MaxOutput;
	}
	else if( l_fOutput < l_pPIDCtrl->m_MinOutput )
	{
		l_fRet = l_pPIDCtrl->m_MinOutput;
	}
	else
		l_fRet = l_fOutput;

	if( l_pPIDCtrl->m_ErrorSum > l_pPIDCtrl->m_MaxErrorSum)
		l_pPIDCtrl->m_ErrorSum = l_pPIDCtrl->m_MaxErrorSum;
	else if( l_pPIDCtrl->m_ErrorSum < -l_pPIDCtrl->m_MaxErrorSum)
		l_pPIDCtrl->m_ErrorSum = -l_pPIDCtrl->m_MaxErrorSum;

	l_pPIDCtrl->m_PreError = l_fError;

	return l_fRet;
}



float CalcPidFloat_Incremental(PIDCtrlFloat_t *l_pPIDCtrl, float l_fError){

	float l_fErrorDiff, l_fPidOutput;


	l_fErrorDiff = l_fError - l_pPIDCtrl->m_PreError;


	l_pPIDCtrl->m_fStaurErr = l_pPIDCtrl->m_fLastNormalOutput - l_pPIDCtrl->m_fSaturVal ;
	l_pPIDCtrl->m_fBackCalcVal = l_pPIDCtrl->m_Ka * l_pPIDCtrl->m_fStaurErr;


	l_pPIDCtrl->m_ErrorSum += l_fError;

	// if(l_pPIDCtrl->m_PreError * l_fError < 0.0f){
	// 	l_pPIDCtrl->m_ErrorSum = 0;
	// }

	l_fPidOutput = ( (l_fError*l_pPIDCtrl->m_Kp) + (l_pPIDCtrl->m_ErrorSum*l_pPIDCtrl->m_Ki) + (l_fErrorDiff*l_pPIDCtrl->m_Kd) );

	// Incremental Pid
	l_pPIDCtrl->m_fOutput = l_pPIDCtrl->m_fOutput + l_fPidOutput;
	l_pPIDCtrl->m_fOutput = l_pPIDCtrl->m_fOutput + l_pPIDCtrl->m_fBackCalcVal;



	if(l_pPIDCtrl->m_fOutput > l_pPIDCtrl->m_fOutput + l_pPIDCtrl->m_fSlewRate){
		l_pPIDCtrl->m_fOutput = l_pPIDCtrl->m_fOutput + l_pPIDCtrl->m_fSlewRate;
	}
	else if(l_pPIDCtrl->m_fOutput < l_pPIDCtrl->m_fOutput - l_pPIDCtrl->m_fSlewRate){
		l_pPIDCtrl->m_fOutput = l_pPIDCtrl->m_fOutput - l_pPIDCtrl->m_fSlewRate;
	}



	if( l_pPIDCtrl->m_fOutput > l_pPIDCtrl->m_MaxOutput ) {

		l_pPIDCtrl->m_fOutput = l_pPIDCtrl->m_MaxOutput;
		l_pPIDCtrl->m_fSaturVal = l_pPIDCtrl->m_MaxOutput;
	}
	else if( l_pPIDCtrl->m_fOutput < l_pPIDCtrl->m_MinOutput ) {

		l_pPIDCtrl->m_fOutput = l_pPIDCtrl->m_MinOutput;
		l_pPIDCtrl->m_fSaturVal = l_pPIDCtrl->m_MinOutput;
	}
	else{

		l_pPIDCtrl->m_fSaturVal = l_pPIDCtrl->m_fOutput;
		l_pPIDCtrl->m_fLastNormalOutput = l_pPIDCtrl->m_fOutput;
	}
		





	if( l_pPIDCtrl->m_ErrorSum > l_pPIDCtrl->m_MaxErrorSum)
		l_pPIDCtrl->m_ErrorSum = l_pPIDCtrl->m_MaxErrorSum;
	else if( l_pPIDCtrl->m_ErrorSum < -l_pPIDCtrl->m_MaxErrorSum)
		l_pPIDCtrl->m_ErrorSum = -l_pPIDCtrl->m_MaxErrorSum;

	l_pPIDCtrl->m_PreError = l_fError;


	return l_pPIDCtrl->m_fOutput;
}







void InitPIDCtrlVarInt(PIDCtrlInt_t *l_pPIDCtrl, int l_iPresision)
{
	memset(l_pPIDCtrl, 0, sizeof(PIDCtrlInt_t));

	l_pPIDCtrl->m_MaxErrorSum = PID_DEFAULT_MAXERRORSUM_INT;
	l_pPIDCtrl->m_iPresision = l_iPresision;
}

void SetPIDGainInt(PIDCtrlInt_t *l_pstPIDCtrl, float l_iKp, float l_iKi, float l_iKd, float l_iKa)
{
	l_pstPIDCtrl->m_Kp = (int)(l_iKp * l_pstPIDCtrl->m_iPresision);
	l_pstPIDCtrl->m_Ki = (int)(l_iKi * l_pstPIDCtrl->m_iPresision);
	l_pstPIDCtrl->m_Kd = (int)(l_iKd * l_pstPIDCtrl->m_iPresision);
	l_pstPIDCtrl->m_Ka = (int)(l_iKa * l_pstPIDCtrl->m_iPresision);
}

void SetPIDConfigVarInt(PIDCtrlInt_t *l_pstPIDCtrl, int32_t l_iMaxErrorSum, int32_t l_iMaxOutput, int32_t l_iMinOutput)
{
	l_pstPIDCtrl->m_MaxErrorSum = l_iMaxErrorSum;
	l_pstPIDCtrl->m_MaxOutput = l_iMaxOutput;
	l_pstPIDCtrl->m_MinOutput = l_iMinOutput;
}

int CalcuatePIDOutputInt(PIDCtrlInt_t *l_pPIDCtrl, int l_iError)
{
	int iErrorDiff, iOutput;


	iErrorDiff = l_iError - l_pPIDCtrl->m_PreError;

	l_pPIDCtrl->m_ErrorSum += l_iError;

	if(l_pPIDCtrl->m_PreError * l_iError < 0){
		l_pPIDCtrl->m_ErrorSum = 0;
	}

	iOutput = ( (l_iError*l_pPIDCtrl->m_Kp) + (l_pPIDCtrl->m_ErrorSum*l_pPIDCtrl->m_Ki) + (iErrorDiff*l_pPIDCtrl->m_Kd) );
	iOutput = iOutput / l_pPIDCtrl->m_iPresision;


	// Incremental Pid
	l_pPIDCtrl->m_iOutput = l_pPIDCtrl->m_iOutput + iOutput;



	if(l_pPIDCtrl->m_iOutput > l_pPIDCtrl->m_MaxOutput){
		l_pPIDCtrl->m_iOutput = l_pPIDCtrl->m_iOutput + l_pPIDCtrl->m_iSlewRate;
	}
	else if(l_pPIDCtrl->m_iOutput < l_pPIDCtrl->m_iOutput - l_pPIDCtrl->m_iSlewRate){
		l_pPIDCtrl->m_iOutput = l_pPIDCtrl->m_iOutput - l_pPIDCtrl->m_iSlewRate;
	}



	if( l_pPIDCtrl->m_iOutput > l_pPIDCtrl->m_MaxOutput )
	{
		l_pPIDCtrl->m_iOutput = l_pPIDCtrl->m_MaxOutput;
	}
	else if( l_pPIDCtrl->m_iOutput < l_pPIDCtrl->m_MinOutput )
	{
		l_pPIDCtrl->m_iOutput = l_pPIDCtrl->m_MinOutput;
	}
	else
		l_pPIDCtrl->m_iOutput = l_pPIDCtrl->m_iOutput;

	if( l_pPIDCtrl->m_ErrorSum > l_pPIDCtrl->m_MaxErrorSum)
		l_pPIDCtrl->m_ErrorSum = l_pPIDCtrl->m_MaxErrorSum;
	else if( l_pPIDCtrl->m_ErrorSum < -l_pPIDCtrl->m_MaxErrorSum)
		l_pPIDCtrl->m_ErrorSum = -l_pPIDCtrl->m_MaxErrorSum;

	l_pPIDCtrl->m_PreError = l_iError;


	return l_pPIDCtrl->m_iOutput;
}




