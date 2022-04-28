

#if !defined(AFX_N3FXPLUG_H__32183758_2BF2_456F_B6AA_CBE9C248CDDE__INCLUDED_)
#define AFX_N3FXPLUG_H__32183758_2BF2_456F_B6AA_CBE9C248CDDE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif 

#include "N3BaseFileAccess.h"

#include <vector>

class CN3FXPlugPart : public CN3BaseFileAccess  
{
public:
	CN3FXPlugPart();
	virtual ~CN3FXPlugPart();

public:
protected:
	class CN3FXBundle*	m_pFXB;
	int					m_nRefIndex;	

	__Vector3			m_vOffsetPos;	
	__Vector3			m_vOffsetDir;	

public:
	void				Tick(const __Matrix44& mtxParent);
	void				Tick(const class CN3Chr* pChr);
	void				Render();
	virtual void		Release();
	virtual bool		Load(HANDLE hFile);

	const CN3FXBundle*	GetFXB() const {return m_pFXB;}
	void				SetFXB(const std::string& strFN);
	int					GetRefIndex() const {return m_nRefIndex;}
	void				SetRefIdx(int nRefIndex) {m_nRefIndex = nRefIndex;}
	void				StopFXB(bool bImmediately);
	void				TriggerFXB();
protected:
};

class CN3FXPlug : public CN3BaseFileAccess  
{

public:
	CN3FXPlug();
	virtual ~CN3FXPlug();

public:
protected:
	std::vector<class CN3FXPlugPart*> m_FXPParts;

public:
	void			Tick(const CN3Chr* pChr);
	void			Render();
	virtual void	Release();
	virtual bool	Load(HANDLE hFile);

	void			StopAll(bool bImmediately = false);	
	void			TriggerAll();						
protected:
};

#endif 
