

#if !defined(AFX_N3IJoint_h__INCLUDED_)
#define AFX_N3IJoint_h__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif 

#pragma warning(disable : 4786)

#include "N3Transform.h"

#include <list>
class CN3Joint;
typedef std::list<CN3Joint*>::iterator	it_Joint;

const int MAX_JOINT_TRANSFORM = 64;

class CN3Joint : public CN3Transform
{
	friend class CN3Joint; 
public:
	__Quaternion			m_qOrient;		
	CN3AnimKey				m_KeyOrient;	

protected:
	CN3Joint*				m_pParent;
	std::list<CN3Joint*>	m_Children;

public:
	void ChildDelete(CN3Joint* pChild);

	void MatricesGet(__Matrix44 *pMtxs, int& nJointIndex);
	
	void Tick(float fFrm);
	bool TickAnimationKey(float fFrm);
	void ReCalcMatrix();
	void ReCalcMatrixBlended(float fFrm0, float fFrm1, float fWeight0);
	void ParentSet(CN3Joint* pParent);
	void ChildAdd(CN3Joint* pChild);
	CN3Joint* Parent() { return m_pParent; }
	int ChildCount() { return m_Children.size(); }
	CN3Joint* Child(size_t index)
	{
		if (index > m_Children.size()) return NULL;
		auto it = m_Children.begin();
		std::advance(it, index);
		return *it;
	}

	void NodeCount(int &nCount);
	BOOL FindPointerByID(int nID, CN3Joint *&pJoint);
	void Release();
	bool Load(HANDLE hFile);

	CN3Joint();
	virtual ~CN3Joint();

};

#endif 
