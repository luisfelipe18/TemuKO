

#if !defined(AFX_N3WORLDMANAGER_H__624E2205_DA37_483B_815A_B77C12F6C352__INCLUDED_)
#define AFX_N3WORLDMANAGER_H__624E2205_DA37_483B_815A_B77C12F6C352__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif 

#include "GameBase.h"
#include "N3WorldBase.h"

class CN3Shape;
class CN3Sun;

class CN3WorldManager : public CGameBase  
{
	bool									m_bIndoor;
	CN3WorldBase*				   m_pActiveWorld;

public:
	void InitWorld(int iZoneID, const __Vector3& vPosPlayer);

	CN3WorldManager();
	virtual ~CN3WorldManager();

	void Tick();

	bool IsIndoor() {	return m_bIndoor;	}
	
	
	CN3WorldBase* GetActiveWorld() 
	{

		return m_pActiveWorld;	
	}
};

#endif 
