

#if !defined(AFX_N3PMeshCreate_h__INCLUDED_)
#define AFX_N3PMeshCreate_h__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif 
#include "N3Base.h"

#define _SAME_VERTEXPOS

class CN3Mesh;
class CN3PMesh;
class CN3PMeshCreate
{
public:
	CN3PMeshCreate();
	virtual ~CN3PMeshCreate();

public:
	struct __PMCreateOption
	{
		bool	bUseSumOfLoss;		
		bool	bTriangleWeight;	
		bool	bArea; 
		float	fWeight; 	
	};

protected:
	
	struct __PMCEdgeCollapse
	{
		float Value;
		
		int NumTrianglesToLose, NumIndicesToChange, NumVerticesToLose;
		int *IndexChanges;
		int CollapseTo;
	};

	void swap_triangle(uint16_t *t1, uint16_t *t2);
	float GetTriCollapsesLoss(uint16_t* pTriIndex, bool bArea);
	void combine_modified(float &sofar, uint16_t *tri, int which, int what_to, bool bSumOfLoss);

public:
	int ReGenerate(CN3PMesh* pPMesh); 
	
	void CreateCollapseList();

	
	CN3PMesh* CreateRendererMesh();

	__PMCreateOption m_PMCOption;

	
	bool ConvertFromN3Mesh(CN3Mesh* pN3Mesh);
	bool ConvertFromN3PMesh(CN3PMesh* pN3PMesh);

	void Release();

protected:
	

	
	int m_iTotalIndexChanges;
	int *m_pAllIndexChanges;

	
	__PMCEdgeCollapse *m_pCollapses, *m_pCollapseUpTo;

	bool FindACollapse(float &val_so_far);
	void TryEdge(int pt_a, int pt_b,
				 float &be_val,
				 uint16_t &be_a,
				 uint16_t &be_b,
				 bool &IsOver);
	void Collapse(uint16_t& pt_to, uint16_t& pt_from, float edge_val);

protected:

	
	
	__VertexT1* m_pVertices;
	uint16_t* m_pIndices;
	int m_iNumVertices, m_iNumIndices;

	
	int m_iOriginalNumVertices, m_iOriginalNumIndices;

	
	void SwapToEnd(uint16_t swapper, __PMCEdgeCollapse *collapses, __PMCEdgeCollapse *collapses_end, uint16_t &pt_to, uint16_t &pt_from);

#ifdef _SAME_VERTEXPOS
	float GetLossOfSamePosVertex(uint16_t pt_to, uint16_t pt_from);
#endif 

};

#endif 
