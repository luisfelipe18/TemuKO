

#if !defined __N3UIDBCLBUTTON_H__
#define __N3UIDBCLBUTTON_H__

#if _MSC_VER > 1000
#pragma once
#endif 

#include "N3UIImage.h"

class CN3UIDBCLButton  : public CN3UIImage
{
public:
	CN3UIDBCLButton();
	virtual ~CN3UIDBCLButton();

	virtual uint32_t	MouseProc(uint32_t dwFlags, const POINT& ptCur, const POINT& ptOld);
};

#endif 
