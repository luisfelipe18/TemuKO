

#ifndef __N3FXMGR_H__
#define __N3FXMGR_H__

#pragma warning(disable : 4786)

#include "N3BaseFileAccess.h"
#include "N3FXDef.h"

#include <list>
#include <map>
#include "N3UIWndBase.h"

class CN3FXBundleGame;

typedef struct __FXBundleOrigin	
{
	class CN3FXBundleGame*	pBundle;
	int						iNum;
	float					fLimitedTime;
	
	__FXBundleOrigin()
	{
		pBundle = NULL;
		iNum = 0;
		fLimitedTime = 0.0f;
	}
} FXBUNDLEORIGIN, *LPFXBUNDLEORIGIN;

typedef std::map<std::string, LPFXBUNDLEORIGIN> stlMAP_BUNDLEORIGIN;
typedef stlMAP_BUNDLEORIGIN::value_type stlMAP_BUNDLEORIGIN_VALUE;
typedef stlMAP_BUNDLEORIGIN::iterator stlMAP_BUNDLEORIGIN_IT;

typedef std::list<CN3FXBundleGame*> stlLIST_BUNDLEGAME;

typedef stlLIST_BUNDLEGAME::iterator stlLIST_BUNDLEGAME_IT;

class CN3FXMgr : public CGameBase
{
public:
	
	stlLIST_BUNDLEGAME			m_ListBundle;

	stlMAP_BUNDLEORIGIN			m_OriginBundle;

	float						m_fOriginLimitedTime;
	
public:
	void	TriggerBundle(int SourceID, int SourceJoint, int FXID, int TargetID, int TargetJoint, int idx = 0, int MoveType = FX_BUNDLE_MOVE_NONE, uint32_t iVer = N3FORMAT_VER_DEFAULT);
	void	TriggerBundle(int SourceID, int SourceJoint, int FXID, __Vector3 TargetPos, int idx = 0, int MoveType = FX_BUNDLE_MOVE_NONE, uint32_t iVer = N3FORMAT_VER_DEFAULT);
	void	Stop(int SourceID, int TargetID, int FXID = -1, int idx = 0, bool immediately = false);
	void	StopMine();
	void	Tick();
	void	Render();
	bool	CheckCollisionSphere(__Vector3 vSp, __Vector3 vEp, __Vector3 vCp, float fRadius, __Vector3* vCol);

	void	SetBundlePos(int FXID, int idx, __Vector3& vPos);

	void	ClearAll();		
	CN3FXMgr();	
	virtual ~CN3FXMgr();
};

#endif 
