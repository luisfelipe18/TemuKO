

#if !defined(AFX_N3GLOBALEFFECTMNG_H__E95C3268_DD68_45AE_AA24_DC64B11A1B08__INCLUDED_)
#define AFX_N3GLOBALEFFECTMNG_H__E95C3268_DD68_45AE_AA24_DC64B11A1B08__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif 

#include "N3Base.h"

enum {	GEW_CLEAR=0,			
		GEW_DRIZZLE_RAIN=1,		
		GEW_RAINY=2,			
		GEW_HEAVY_RAIN=3,		
		GEW_SNOW1=11,			
		GEW_SNOW2=12,			
		GEW_HEAVY_SNOW=13		
};

class CN3GERain;
class CN3GESnow;
class CN3GlobalEffectMng : public CN3Base  
{
public:
	CN3GlobalEffectMng();
	virtual ~CN3GlobalEffectMng();

public:
protected:
	float			m_fCellSize;	
	POINT			m_CurCellPos;	
	__Vector3		m_vPos[9];		

	
	CN3GERain*		m_pGERain;		
	CN3GESnow*		m_pGESnow;		

public:
	virtual void	Release();
	void			Tick();
	void			Render();

	void			WeatherSetRainy(int iPercent);
	void			WeatherSetSnow(int iPercent);
	void			WeatherSetClean();

protected:

};

#endif 
