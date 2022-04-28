

#if !defined(AFX_N3RIVER_H__D0171C53_F631_4EC3_9D42_B4B754093FAC__INCLUDED_)
#define AFX_N3RIVER_H__D0171C53_F631_4EC3_9D42_B4B754093FAC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif 

#include "N3Base.h"

#define MAX_RIVER_TEX	32

class CN3River : public CN3Base  
{
public:
	CN3River();
	virtual ~CN3River();

	struct __VertexRiver
	{
	public:
		float x,y,z;
		float nx,ny,nz;
		D3DCOLOR	color;
		float u,v, u2,v2;
		void Set(float sx, float sy,float sz, float snx, float sny, float snz, D3DCOLOR scolor, float su, float sv, float su2, float sv2)
		{
			x = sx, y = sy, z = sz;
			nx = snx, y = sny, z = sny;
			color = scolor;
			u = su, v = sv;
			u2 = su2, v2 = sv2;
		}
	};

	struct _RIVER_DIFF
	{
		float	fDiff;
		float	fWeight;
	};

	struct _RIVER_INFO
	{
		int iVC;
		int iIC;
		__VertexRiver		*pVertices;
		uint16_t				*pwIndex;
		_RIVER_DIFF			*pDiff;

		BOOL				m_bTick2Rand;
		__Vector3			m_vCenterPo;	
		float				m_fRadius;		

		CN3Texture			*m_pTexWave;

		_RIVER_INFO() {
			pVertices = NULL; pwIndex = NULL; pDiff = NULL;
			m_pTexWave = NULL;
		}
		~_RIVER_INFO() {
			if (pVertices)
				delete[] pVertices, pVertices = NULL;
			if (pwIndex)
				delete[] pwIndex, pwIndex = NULL;
			if (pDiff)
				delete[] pDiff, pDiff = NULL;
			m_pTexWave = NULL;
		}
	};

protected:
	_RIVER_INFO			*m_pRiverInfo;
	int					m_iRiverCount;

	CN3Texture			*m_pTexRiver[MAX_RIVER_TEX];

	float				m_fTexIndex;
	void				UpdateWaterPositions();
public:
	virtual bool	Load(HANDLE hFile);
	void			Render();
	void			Tick();
};

#endif 
