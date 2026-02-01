
#ifndef _PIDCTRL_H_
#define _PIDCTRL_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#define USE_INTEGER_PID (1)

#define PID_PRECISION 10000 
#define PID_DEFAULT_MAXERRORSUM_INT 100000



#define PID_DEFAULT_MAXERRORSUM_FLOAT 1000.0f
/* PID control value */
typedef struct _PID_CONTROL_VAR_FLOAT
{
	float m_Kp;
	float m_Ki;
	float m_Kd;
	
	float m_ErrorSum;
	float m_PreError;
	float m_MaxErrorSum;
	float m_MaxOutput;
	float m_MinOutput;

	float m_fOutput;
	float m_fSlewRate;

	// anti-windup 관련 변수 추가 필요시 여기에 추가
	float m_Ka;
	float m_fBackCalcVal;
	float m_fLastNormalOutput;
	float m_fSaturVal;
	float m_fStaurErr;
}PIDCtrlFloat_t;

void InitPIDCtrlVarFloat(PIDCtrlFloat_t *l_pPIDCtrl);
void SetPIDGainFloat(PIDCtrlFloat_t *l_pstPIDCtrl, float l_fKp, float l_fKi, float l_fKd, float l_fKa);
void SetPIDConfigVarFloat(PIDCtrlFloat_t *l_pstPIDCtrl, float l_fMaxErrorSum, float l_fMaxOutput, float l_fMinOutput);
float CalcuatePIDOutputFloat(PIDCtrlFloat_t *l_pPIDCtrl, float l_fError);
float CalcPidFloat_Incremental(PIDCtrlFloat_t *l_pPIDCtrl, float l_fError);
void SetPIDAntiWindupGainFloat(PIDCtrlFloat_t *l_pstPIDCtrl, float l_fKa);




typedef struct _PID_CONTROL_VAR_INT_
{
	int m_Kp;
	int m_Ki;
	int m_Kd;
	int m_Ka;
	int m_ErrorSum;
	int m_PreError;
	int m_MaxErrorSum;
	int m_MaxOutput;
	int m_MinOutput;

	int m_iOutput;
	int m_iSlewRate;

	int m_iPresision;
}PIDCtrlInt_t;

void InitPIDCtrlVarInt(PIDCtrlInt_t * l_pPIDCtrl, int l_iPresision);
void SetPIDGainInt(PIDCtrlInt_t *l_pstPIDCtrl, float l_iKp, float l_iKi, float l_iKd, float l_iKa);
void SetPIDConfigVarInt(PIDCtrlInt_t *l_pstPIDCtrl, int32_t l_iMaxErrorSum, int32_t l_iMaxOutput, int32_t l_iMinOutput);
int CalcuatePIDOutputInt(PIDCtrlInt_t *l_pPIDCtrl, int l_iError);



#ifdef __cplusplus
}
#endif

#endif


