

#ifndef __N3FXGROUP_H__
#define __N3FXGROUP_H__

#include "N3BaseFileAccess.h"
#include "N3FXDef.h"

#include <list>

class CN3FXGroup : public CN3BaseFileAccess  
{
public:
	int			m_iVersion;
	std::list<__FXBInfo*> FXBList;
	
public:
	int	GetCount() { return FXBList.size(); }
	__FXBInfo* GetFXBInfo(int idx);

	virtual bool	Load(HANDLE hFile); 
	bool	Save(HANDLE hFile);	
	
	CN3FXGroup();	
	virtual ~CN3FXGroup();
};

#endif 
