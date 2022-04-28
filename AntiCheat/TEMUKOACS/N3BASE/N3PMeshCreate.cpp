

#include "N3PMeshCreate.h"
#include "N3PMesh.h"
#include "N3PMeshInstance.h"
#include "N3Mesh.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#endif

#define MAX_COLLAPSES 5000
#define MAX_INDEX_CHANGES 10000
#define MAX_VERTICES_PER_MATERIAL 10000

template <class T> void swap(T &a, T &b)
{
	T t;

	t = a;
	a = b;
	b = t;
}

void CN3PMeshCreate::swap_triangle(uint16_t *t1, uint16_t *t2)
{
	swap(t1[0], t2[0]);
	swap(t1[1], t2[1]);
	swap(t1[2], t2[2]);
}

float CN3PMeshCreate::GetTriCollapsesLoss(uint16_t* pTriIndex, bool bArea)
{
	
	D3DXVECTOR3 pts[3] = {
			D3DXVECTOR3(m_pVertices[pTriIndex[0]].x, m_pVertices[pTriIndex[0]].y, m_pVertices[pTriIndex[0]].z),
			D3DXVECTOR3(m_pVertices[pTriIndex[1]].x, m_pVertices[pTriIndex[1]].y, m_pVertices[pTriIndex[1]].z),
			D3DXVECTOR3(m_pVertices[pTriIndex[2]].x, m_pVertices[pTriIndex[2]].y, m_pVertices[pTriIndex[2]].z)};
	if (bArea)
	{
		
		D3DXVECTOR3 cross, V1, V2;
		V1 = pts[2] - pts[0];	V2 = pts[1] - pts[0];
		D3DXVec3Cross(&cross, &V1, &V2);
		return D3DXVec3Length(&cross);
	}
	else
	{
		
		D3DXVECTOR3 V1, V2, V3;
		V1 = pts[2] - pts[0];	V2 = pts[1] - pts[0];	V3 = pts[1] - pts[2];
		float fLoss = D3DXVec3Length(&V1) + D3DXVec3Length(&V2) + D3DXVec3Length(&V3) + 0.0001f;
		__ASSERT(fLoss > 0, "Loss value is less than 0");
		return fLoss;
	}
}

void CN3PMeshCreate::combine_modified(float &sofar, uint16_t *tri, int which, int what_to, bool bSumOfLoss)
{
	
	D3DXVECTOR3 pts[3] = {
			D3DXVECTOR3(m_pVertices[tri[0]].x, m_pVertices[tri[0]].y, m_pVertices[tri[0]].z),
			D3DXVECTOR3(m_pVertices[tri[1]].x, m_pVertices[tri[1]].y, m_pVertices[tri[1]].z),
			D3DXVECTOR3(m_pVertices[tri[2]].x, m_pVertices[tri[2]].y, m_pVertices[tri[2]].z) };

	
	D3DXVECTOR3 in_plane = pts[0];

	
	D3DXVECTOR3 oldcross, V1, V2;
	V1 = pts[2] - pts[0];	V2 = pts[1] - pts[0];
	D3DXVec3Cross(&oldcross, &V1, &V2);

	float oldarea = D3DXVec3Length(&oldcross);
	D3DXVECTOR3 oldnorm;
	D3DXVec3Normalize(&oldnorm, &oldcross);

	
	pts[which] = D3DXVECTOR3(m_pVertices[what_to].x, m_pVertices[what_to].y, m_pVertices[what_to].z);

	
	D3DXVECTOR3 newcross;
	V1 = pts[2] - pts[0];	V2 = pts[1] - pts[0];
	D3DXVec3Cross(&newcross, &V1, &V2);

	float newarea = D3DXVec3Length(&newcross);
	D3DXVECTOR3 newnorm;
	D3DXVec3Normalize(&newnorm, &newcross);

	
	float cosangdiff = D3DXVec3Dot(&newnorm, &oldnorm);

	
	V1 = D3DXVECTOR3(m_pVertices[what_to].x, m_pVertices[what_to].y, m_pVertices[what_to].z) - in_plane;
	float volume_change = T_Abs(D3DXVec3Dot(&V1, &oldcross));

	
	float weighted_angle_change = (1.0f - cosangdiff) * (oldarea + newarea);
	if (weighted_angle_change<0.0f) weighted_angle_change = 0.0f;	
	

	
	
	        volume_change = powf(        volume_change, .333333f);
	weighted_angle_change = powf(weighted_angle_change, .5f     ) * 5.f;

	
	float newval = weighted_angle_change + volume_change;

	if (bSumOfLoss)
	{
		sofar += newval;
	}
	else
	{
		
		if (newval > sofar) sofar = newval;
	}
}

CN3PMeshCreate::CN3PMeshCreate()
{
	m_pVertices = NULL;
	m_pIndices = NULL;

	m_pCollapses = NULL;
	m_pAllIndexChanges = NULL;

	Release();
}

CN3PMeshCreate::~CN3PMeshCreate()
{
	CN3PMeshCreate::Release();
}

void CN3PMeshCreate::Release()
{
	if (m_pVertices) { delete[] m_pVertices; m_pVertices = NULL;}
	if (m_pIndices) { delete[] m_pIndices; m_pIndices = NULL; }

	if (m_pCollapses) { delete[] m_pCollapses; m_pCollapses = NULL;}
	if (m_pAllIndexChanges) { delete[] m_pAllIndexChanges; m_pAllIndexChanges = NULL;}

	m_pCollapseUpTo = NULL;
	m_iTotalIndexChanges = 0;
	m_iNumVertices = 0; m_iNumIndices = 0;
}

void CN3PMeshCreate::Collapse(uint16_t& pt_to, uint16_t& pt_from, float edge_val)
{
	
	SwapToEnd(pt_from, m_pCollapses, m_pCollapseUpTo, pt_to, pt_from);

	
	m_iNumVertices--;

	
	
	m_pCollapseUpTo->NumTrianglesToLose = 0;
	m_pCollapseUpTo->IndexChanges = NULL;
	m_pCollapseUpTo->NumIndicesToChange = 0;

	
	int tri_index;
	for (tri_index = 0; tri_index < m_iNumIndices;)
	{
		
		for (int pt = 0; pt < 3; pt++)
		{
			
			if (m_pIndices[tri_index + pt] == m_iNumVertices)
			{
				
				
				int pt2;

				
				for (pt2 = 0; pt2 < 3; pt2++)
				{
					if (m_pIndices[tri_index + pt2] == pt_to)
					{
						
						
						
						m_pCollapseUpTo->NumTrianglesToLose++;
						m_iNumIndices -= 3;

						if (tri_index == m_iNumIndices)
						{
							
							
							
							goto done_triangle_list;
						}

						
						swap_triangle(m_pIndices + tri_index, m_pIndices + m_iNumIndices);

						
						for (__PMCEdgeCollapse *c = m_pCollapses; c < m_pCollapseUpTo; c++)
						{
							
							int *ic;

							for (ic = c->IndexChanges;
								 ic < c->IndexChanges + c->NumIndicesToChange;
								 ic++)
							{
								if (*ic >= tri_index && *ic < tri_index + 3)
								{
									
									
									*ic += (m_iNumIndices - tri_index);
								}
								else
								if (*ic >= m_iNumIndices && *ic < m_iNumIndices + 3)
								{
									
									*ic -= (m_iNumIndices - tri_index);
								}
							}
						}

						
						
						
						
						goto try_same_triangle_location;
					}
				}

				
				
				
				

				
				if (!m_pCollapseUpTo->IndexChanges)
				{
					
					m_pCollapseUpTo->IndexChanges = m_pAllIndexChanges + m_iTotalIndexChanges;
				}

				
				__ASSERT(m_iTotalIndexChanges < MAX_INDEX_CHANGES, "You must increase MAX_INDEX_CHANGES");

				m_pAllIndexChanges[m_iTotalIndexChanges++] = tri_index + pt;
				m_pCollapseUpTo->NumIndicesToChange++;

				
				m_pIndices[tri_index + pt] = pt_to;

				
				goto try_another_triangle;
			}	
		}

try_another_triangle:
		
		tri_index += 3;

try_same_triangle_location:;
		
		
	}

done_triangle_list:

	

	
	m_pCollapseUpTo->NumVerticesToLose = 1; 

	
	int referred[MAX_VERTICES_PER_MATERIAL];
	memset(referred, 0, sizeof(referred));
	__ASSERT(m_iNumVertices < MAX_VERTICES_PER_MATERIAL, "You must increase MAX_VERTICES_PER_MATERIAL");

	int i;
	for (i = 0; i < m_iNumIndices; i++)
	{
		__ASSERT(m_pIndices[i] < m_iNumVertices, "indices array index overflow");
		referred[m_pIndices[i]] = 1;
	}

	
	for (i = 0; i < m_iNumVertices;)
	{
		if (!referred[i])
		{
			
			SwapToEnd(i, m_pCollapses, m_pCollapseUpTo + 1, pt_to, pt_from);

			
			swap(referred[i], referred[m_iNumVertices - 1]);

			
			m_pCollapseUpTo->NumVerticesToLose++;
			m_iNumVertices--;

			
			continue;
		}

		i++;
	}

	
	m_pCollapseUpTo->CollapseTo = pt_to;
	m_pCollapseUpTo->Value = edge_val;
	m_pCollapseUpTo++;
}

void CN3PMeshCreate::TryEdge(
					 int pt_a, int pt_b,
					 float &be_val,
					 uint16_t &be_to,
					 uint16_t &be_from,
					 bool &IsOver)
{
	
	
	

	float a_loss = 0.f;
	float b_loss = 0.f;
	if (m_PMCOption.bUseSumOfLoss)
	{
		a_loss = 0.f;
		b_loss = 0.f;
	}
	else
	{
		
		a_loss = -1000000000000000.f;
		b_loss = -1000000000000000.f;
	}

	
	for (uint16_t *tri = m_pIndices; tri < m_pIndices + m_iNumIndices; tri += 3)
	{
		if (tri[0] == pt_a)
		{
			if (tri[1] != pt_b && tri[2] != pt_b)
			{
				combine_modified(a_loss, tri, 0, pt_b, m_PMCOption.bUseSumOfLoss);
				__ASSERT(a_loss>=0.0f, "loss > 0이어야 한다.");
			}
			else
			{
				
				if (m_PMCOption.bTriangleWeight)
				{
					float t_loss = GetTriCollapsesLoss(tri, m_PMCOption.bArea) * m_PMCOption.fWeight;
					if (m_PMCOption.bUseSumOfLoss)
					{
						a_loss += t_loss;
						b_loss += t_loss;
					}
					else
					{
						if (t_loss > a_loss) a_loss = t_loss;
					}
					__ASSERT(a_loss>=0.0f && b_loss>=0.0f, "loss > 0이어야 한다.");
				}
				continue;
			}
		}
		else
		if (tri[1] == pt_a)
		{
			if (tri[2] != pt_b && tri[0] != pt_b)
			{
				combine_modified(a_loss, tri, 1, pt_b, m_PMCOption.bUseSumOfLoss);
				__ASSERT(a_loss>=0.0f, "loss > 0이어야 한다.");
			}
			else
			{
				
				if (m_PMCOption.bTriangleWeight)
				{
					float t_loss = GetTriCollapsesLoss(tri, m_PMCOption.bArea) * m_PMCOption.fWeight;
					if (m_PMCOption.bUseSumOfLoss)
					{
						a_loss += t_loss;
						b_loss += t_loss;
					}
					else
					{
						if (t_loss > a_loss) a_loss = t_loss;
					}
					__ASSERT(a_loss>=0.0f && b_loss>=0.0f, "loss > 0이어야 한다.");
				}
				continue;
			}
		}
		else
		if (tri[2] == pt_a)
		{
			if (tri[0] != pt_b && tri[1] != pt_b)
			{
				combine_modified(a_loss, tri, 2, pt_b, m_PMCOption.bUseSumOfLoss);
				__ASSERT(a_loss>=0.0f, "loss > 0이어야 한다.");
			}
			else
			{
				
				if (m_PMCOption.bTriangleWeight)
				{
					float t_loss = GetTriCollapsesLoss(tri, m_PMCOption.bArea) * m_PMCOption.fWeight;
					if (m_PMCOption.bUseSumOfLoss)
					{
						a_loss += t_loss;
						b_loss += t_loss;
					}
					else
					{
						if (t_loss > a_loss) a_loss = t_loss;
					}
					__ASSERT(a_loss>=0.0f && b_loss>=0.0f, "loss > 0이어야 한다.");
				}
				continue;
			}
		}
		else
		{
			
			if (tri[0] == pt_b)
			{
				combine_modified(b_loss, tri, 0, pt_a, m_PMCOption.bUseSumOfLoss);
				__ASSERT(b_loss>=0.0f, "loss > 0이어야 한다.");
			}
			else
			if (tri[1] == pt_b)
			{
				combine_modified(b_loss, tri, 1, pt_a, m_PMCOption.bUseSumOfLoss);
				__ASSERT(b_loss>=0.0f, "loss > 0이어야 한다.");
			}
			else
			if (tri[2] == pt_b)
			{
				combine_modified(b_loss, tri, 2, pt_a, m_PMCOption.bUseSumOfLoss);
				__ASSERT(b_loss>=0.0f, "loss > 0이어야 한다.");
			}
		}
	}
	__ASSERT(a_loss>=0.0f && b_loss>=0.0f, "loss > 0이어야 한다.");

#ifdef _SAME_VERTEXPOS
	float temp_a_loss = GetLossOfSamePosVertex(pt_b, pt_a);
	float temp_b_loss = GetLossOfSamePosVertex(pt_a, pt_b);
	__ASSERT(temp_a_loss>=0.0f && temp_b_loss>=0.0f, "loss > 0이어야 한다.");
	if (m_PMCOption.bUseSumOfLoss)
	{
		a_loss += temp_a_loss;
		b_loss += temp_b_loss;
	}
	else
	{
		if (temp_a_loss > a_loss) a_loss = temp_a_loss;
		if (temp_b_loss > b_loss) b_loss = temp_b_loss;
	}
#endif 

	
	
	

	

	
	__ASSERT(a_loss>=0.0f && b_loss>=0.0f, "loss > 0이어야 한다.");
	if (b_loss > a_loss)
	{
		swap(pt_a, pt_b);
		b_loss = a_loss;
	}

	
	float val = b_loss;

	

	
	if (val < be_val)
	{
		be_to = pt_a;
		be_from = pt_b;
		be_val = val;
		IsOver = false;
	}
}

bool CN3PMeshCreate::FindACollapse(float &val_so_far)
{
	
	uint16_t *tri;

	float be_val = 10000000000000000000000000000000000.f; 
	bool IsOver = true;

	
	uint16_t be_index_a;
	uint16_t be_index_b;

	for (tri = m_pIndices; tri < m_pIndices + m_iNumIndices; tri += 3)
	{
		
		TryEdge(tri[0], tri[1], be_val, be_index_a, be_index_b, IsOver);
		TryEdge(tri[1], tri[2], be_val, be_index_a, be_index_b, IsOver);
		TryEdge(tri[2], tri[0], be_val, be_index_a, be_index_b, IsOver);
	}

	
	if (IsOver) return false;

	
	val_so_far += be_val;

	
	Collapse(be_index_a, be_index_b, val_so_far);

#ifdef _SAME_VERTEXPOS
	
	int i;
	for (i=0; i<m_iNumVertices; ++i)
	{
		if (m_pVertices[be_index_b].x == m_pVertices[i].x &&
			m_pVertices[be_index_b].y == m_pVertices[i].y &&
			m_pVertices[be_index_b].z == m_pVertices[i].z )
		{
			uint16_t index_from = i;
			if (be_index_a < m_iNumVertices && index_from != be_index_a)
				Collapse(be_index_a, index_from, val_so_far);
		}
	}
#endif	

	return true;
}

void CN3PMeshCreate::CreateCollapseList()
{
	m_iOriginalNumVertices = m_iNumVertices;
	m_iOriginalNumIndices  = m_iNumIndices ;

	
	__ASSERT(!m_pCollapses, "Collapses pointer is not NULL!");

	m_pCollapses = new __PMCEdgeCollapse[MAX_COLLAPSES];
	__ASSERT(m_pCollapses, "Failed to create collapses buffer"); 
	m_pCollapseUpTo = m_pCollapses;

	m_pAllIndexChanges = new int[MAX_INDEX_CHANGES];
	__ASSERT(m_pAllIndexChanges, "Failed to create Index Change buffer");
	m_iTotalIndexChanges = 0;

	
	float start_val = 0.f;
	while (FindACollapse(start_val))
	{
	}
}

CN3PMesh *CN3PMeshCreate::CreateRendererMesh()
{
	CN3PMesh *pPMesh = new CN3PMesh;

	pPMesh->m_iTotalIndexChanges = m_iTotalIndexChanges;
	if (m_iTotalIndexChanges>0)
	{
		pPMesh->m_pAllIndexChanges = new int[m_iTotalIndexChanges];
		__ASSERT(pPMesh->m_pAllIndexChanges, "Index change buffer is NULL"); 
		memcpy(pPMesh->m_pAllIndexChanges, m_pAllIndexChanges, m_iTotalIndexChanges * sizeof(m_pAllIndexChanges[0]));
	}

	if (m_pCollapseUpTo - m_pCollapses > 0)
	{
		pPMesh->m_pCollapses = new CN3PMesh::__EdgeCollapse[m_pCollapseUpTo - m_pCollapses];
		__ASSERT(pPMesh->m_pCollapses, "Collpases pointer is NULL!");
	}

	pPMesh->m_iNumCollapses = m_pCollapseUpTo - m_pCollapses;

	
	int i;

	float fTempValue = 0.0f;
	for (i = 0; i < pPMesh->m_iNumCollapses; i++)
	{
		__PMCEdgeCollapse &src  = m_pCollapses[pPMesh->m_iNumCollapses - i - 1];
		CN3PMesh::__EdgeCollapse &dest = pPMesh->m_pCollapses[i];

		dest.CollapseTo = src.CollapseTo;
		dest.iIndexChanges = (src.IndexChanges - m_pAllIndexChanges);
		dest.NumIndicesToChange = src.NumIndicesToChange;
		dest.NumIndicesToLose = src.NumTrianglesToLose * 3;
		dest.NumVerticesToLose = src.NumVerticesToLose;
		dest.bShouldCollapse = (fTempValue == src.Value ? true : false);
		fTempValue = src.Value;
	}

	
	pPMesh->m_iMaxNumIndices  = m_iOriginalNumIndices ;
	pPMesh->m_iMaxNumVertices = m_iOriginalNumVertices;
	pPMesh->m_iMinNumIndices  = m_iNumIndices ;
	pPMesh->m_iMinNumVertices = m_iNumVertices;

	pPMesh->Create(pPMesh->m_iMaxNumVertices, pPMesh->m_iMaxNumIndices);

#ifdef _USE_VERTEXBUFFER
	HRESULT hr;
	uint8_t* pByte;
	hr = pPMesh->m_pVB->Lock(0, 0, &pByte, 0);
	CopyMemory(pByte, m_pVertices, pPMesh->m_iMaxNumVertices*sizeof(__VertexT1));
	pPMesh->m_pVB->Unlock();

	hr = pPMesh->m_pIB->Lock(0, 0, &pByte, 0);
	CopyMemory(pByte, m_pIndices, pPMesh->m_iMaxNumIndices*sizeof(uint16_t));
	pPMesh->m_pIB->Unlock();

#else
	
	memcpy(pPMesh->m_pIndices, m_pIndices, pPMesh->m_iMaxNumIndices * sizeof(uint16_t));

	
	
	for (int j = 0; j < pPMesh->m_iMaxNumVertices; j++)
	{
		pPMesh->m_pVertices[j] = m_pVertices[j];
	}
#endif

	return pPMesh;
}

int CN3PMeshCreate::ReGenerate(CN3PMesh *pPMesh)
{
	if(NULL == pPMesh) return -1;
	
	this->Release();
	this->ConvertFromN3PMesh(pPMesh); 
	this->CreateCollapseList(); 

	pPMesh->m_iTotalIndexChanges = m_iTotalIndexChanges;
	if (m_iTotalIndexChanges>0)
	{
		delete [] pPMesh->m_pAllIndexChanges;
		pPMesh->m_pAllIndexChanges = new int[m_iTotalIndexChanges];
		__ASSERT(pPMesh->m_pAllIndexChanges, "Index change buffer is NULL"); 
		memcpy(pPMesh->m_pAllIndexChanges, m_pAllIndexChanges, m_iTotalIndexChanges * sizeof(m_pAllIndexChanges[0]));
	}

	if (m_pCollapseUpTo - m_pCollapses > 0)
	{
		delete [] pPMesh->m_pCollapses;
		pPMesh->m_pCollapses = new CN3PMesh::__EdgeCollapse[m_pCollapseUpTo - m_pCollapses];
		__ASSERT(pPMesh->m_pCollapses, "Collpases pointer is NULL!");
	}

	pPMesh->m_iNumCollapses = m_pCollapseUpTo - m_pCollapses;

	
	int i;

	float fTempValue = 0.0f;
	for (i = 0; i < pPMesh->m_iNumCollapses; i++)
	{
		__PMCEdgeCollapse &src  = m_pCollapses[pPMesh->m_iNumCollapses - i - 1];
		CN3PMesh::__EdgeCollapse &dest = pPMesh->m_pCollapses[i];

		dest.CollapseTo = src.CollapseTo;
		if(src.IndexChanges && m_pAllIndexChanges) 
			dest.iIndexChanges = (src.IndexChanges - m_pAllIndexChanges);
		else
			dest.iIndexChanges = 0;
		dest.NumIndicesToChange = src.NumIndicesToChange;
		dest.NumIndicesToLose = src.NumTrianglesToLose * 3;
		dest.NumVerticesToLose = src.NumVerticesToLose;
		dest.bShouldCollapse = (fTempValue == src.Value ? true : false);
		fTempValue = src.Value;
	}

	
	pPMesh->m_iMinNumIndices  = m_iNumIndices ;
	pPMesh->m_iMinNumVertices = m_iNumVertices;

	pPMesh->Create(m_iOriginalNumVertices, m_iOriginalNumIndices);

#ifdef _USE_VERTEXBUFFER
	HRESULT hr;
	uint8_t* pByte;
	hr = pPMesh->m_pVB->Lock(0, 0, &pByte, 0);
	CopyMemory(pByte, m_pVertices, pPMesh->m_iMaxNumVertices*sizeof(__VertexT1));
	pPMesh->m_pVB->Unlock();

	hr = pPMesh->m_pIB->Lock(0, 0, &pByte, 0);
	CopyMemory(pByte, m_pIndices, pPMesh->m_iMaxNumIndices*sizeof(uint16_t));
	pPMesh->m_pIB->Unlock();

#else
	
	memcpy(pPMesh->m_pIndices, m_pIndices, pPMesh->m_iMaxNumIndices * sizeof(uint16_t));

	
	
	for (int j = 0; j < pPMesh->m_iMaxNumVertices; j++)
	{
		pPMesh->m_pVertices[j] = m_pVertices[j];
	}
#endif

	return 0;
}

void CN3PMeshCreate::SwapToEnd(uint16_t swapper, __PMCEdgeCollapse *collapses, __PMCEdgeCollapse *collapses_end, uint16_t &pt_to, uint16_t &pt_from)
{
	
	

	

	
	__VertexT1 *v = &m_pVertices[swapper];

	
	swap(*v, m_pVertices[m_iNumVertices - 1]);

	

	
	
	int tri_index;
	for (tri_index = 0; tri_index < m_iOriginalNumIndices; tri_index++)
	{
		
		if (m_pIndices[tri_index] == swapper) 
		{
			m_pIndices[tri_index] = m_iNumVertices - 1;
		}
		else
		if (m_pIndices[tri_index] == m_iNumVertices - 1)
		{
			m_pIndices[tri_index] = swapper;
		}
	}

	
	for (__PMCEdgeCollapse *c = collapses; c < collapses_end; c++)
	{
		if (c->CollapseTo == swapper)
		{
			c->CollapseTo = m_iNumVertices - 1;
		}
		else
		if (c->CollapseTo == m_iNumVertices - 1)
		{
			c->CollapseTo = swapper;
		}
	}

	
	if (pt_to == m_iNumVertices - 1)
	{
		pt_to = swapper;
	}
	else
	if (pt_to == swapper)
	{
		pt_to = m_iNumVertices - 1;
	}

	if (pt_from == m_iNumVertices - 1)
	{
		pt_from = swapper;
	}
	else
	if (pt_from == swapper)
	{
		pt_from = m_iNumVertices - 1;
	}
}

bool CN3PMeshCreate::ConvertFromN3Mesh(CN3Mesh* pN3Mesh)	
{
	if (pN3Mesh == NULL) return false;
	Release();

	
	m_iNumVertices = pN3Mesh->VertexCount();
	m_iNumIndices = pN3Mesh->IndexCount();
	if (m_iNumVertices<=0 || m_iNumIndices<=0) return false;

	
	if (pN3Mesh->Vertices())
	{
		m_pVertices = new __VertexT1[m_iNumVertices];
		CopyMemory(m_pVertices, pN3Mesh->Vertices(), m_iNumVertices*sizeof(__VertexT1));
	}
	else return false;

	
	if (pN3Mesh->Indices())
	{
		m_pIndices = new uint16_t[m_iNumIndices];
		CopyMemory(m_pIndices, pN3Mesh->Indices(), sizeof(uint16_t)*m_iNumIndices);
	}
	else return false;

	m_iOriginalNumVertices = m_iNumVertices;
	m_iOriginalNumIndices  = m_iNumIndices;

	return true;
}

bool CN3PMeshCreate::ConvertFromN3PMesh(CN3PMesh* pN3PMesh)
{
	if (pN3PMesh == NULL) return false;
	Release();

	CN3PMesh* pPMeshTmp = CN3Base::s_MngPMesh.Get(pN3PMesh->FileName()); 
	CN3PMeshInstance PMeshInst(pN3PMesh);
	PMeshInst.SetLODByNumVertices(pN3PMesh->GetMaxNumVertices());

	
	m_iNumVertices = PMeshInst.GetNumVertices();
	m_iNumIndices = PMeshInst.GetNumIndices();
	if (m_iNumVertices<=0 || m_iNumIndices<=0) return false;

#ifdef _USE_VERTEXBUFFER
	
	LPDIRECT3DVERTEXBUFFER8 pVB = PMeshInst.GetVertexBuffer();
	if (pVB)
	{
		m_pVertices = new __VertexT1[m_iNumVertices];

		uint8_t* pByte;
		HRESULT hr = pVB->Lock(0, 0, &pByte, D3DLOCK_READONLY);
		if (FAILED(hr)) return false;

		CopyMemory(m_pVertices, pByte, m_iNumVertices*sizeof(__VertexT1));
		pVB->Unlock();
	}
	
	LPDIRECT3DINDEXBUFFER8 pIB = PMeshInst.GetIndexBuffer();
	if (pIB)
	{
		m_pIndices = new uint16_t[m_iNumIndices];

		uint8_t* pByte;
		HRESULT hr = pIB->Lock(0, 0, &pByte, D3DLOCK_READONLY);
		if (FAILED(hr)) return false;

		CopyMemory(m_pIndices, pByte, m_iNumIndices*sizeof(uint16_t));
		pIB->Unlock();
	}
#else
	
	if (PMeshInst.GetVertices())
	{
		m_pVertices = new __VertexT1[m_iNumVertices];
		CopyMemory(m_pVertices, PMeshInst.GetVertices(), m_iNumVertices*sizeof(__VertexT1));
	}
	else return false;

	
	if (PMeshInst.GetIndices())
	{
		m_pIndices = new uint16_t[m_iNumIndices];
		CopyMemory(m_pIndices, PMeshInst.GetIndices(), sizeof(uint16_t)*m_iNumIndices);
	}
	else return false;
#endif

	m_iOriginalNumVertices = m_iNumVertices;
	m_iOriginalNumIndices  = m_iNumIndices ;

	return true;
}

#ifdef _SAME_VERTEXPOS
float CN3PMeshCreate::GetLossOfSamePosVertex(uint16_t pt_to, uint16_t pt_from)
{
	__ASSERT(pt_to<m_iNumVertices && pt_from<m_iNumVertices && m_pVertices && m_pIndices, "Pointer is NULL");
	float fLoss = 0.0f;

	float x = m_pVertices[pt_from].x;
	float y = m_pVertices[pt_from].y;
	float z = m_pVertices[pt_from].z;

	int i;
	for (i=0; i<m_iNumVertices; ++i)
	{
		
		if (i != pt_to && i != pt_from &&
			m_pVertices[i].x == x &&
			m_pVertices[i].y == y &&
			m_pVertices[i].z == z )
		{
			
			uint16_t* tri;
			for (tri = m_pIndices; tri<m_pIndices+m_iNumIndices; tri += 3)
			{
				if (tri[0] == i)
				{
					if (tri[1] != pt_to && tri[2] != pt_to) combine_modified(fLoss, tri, 0, pt_to, m_PMCOption.bUseSumOfLoss);
					else
						
					if (m_PMCOption.bTriangleWeight)
					{
						float t_loss = GetTriCollapsesLoss(tri, m_PMCOption.bArea) * m_PMCOption.fWeight;
						if (m_PMCOption.bUseSumOfLoss) fLoss += t_loss;
						else if (t_loss > fLoss) fLoss = t_loss;
					}
				}
				else if (tri[1] == i)
				{
					if (tri[0] != pt_to && tri[2] != pt_to) combine_modified(fLoss, tri, 1, pt_to, m_PMCOption.bUseSumOfLoss);
					else
						
					if (m_PMCOption.bTriangleWeight)
					{
						float t_loss = GetTriCollapsesLoss(tri, m_PMCOption.bArea) * m_PMCOption.fWeight;
						if (m_PMCOption.bUseSumOfLoss) fLoss += t_loss;
						else if (t_loss > fLoss) fLoss = t_loss;
					}
				}
				else if (tri[2] == i)
				{
					if (tri[0] != pt_to && tri[1] != pt_to) combine_modified(fLoss, tri, 2, pt_to, m_PMCOption.bUseSumOfLoss);
					else
						
					if (m_PMCOption.bTriangleWeight)
					{
						float t_loss = GetTriCollapsesLoss(tri, m_PMCOption.bArea) * m_PMCOption.fWeight;
						if (m_PMCOption.bUseSumOfLoss) fLoss += t_loss;
						else if (t_loss > fLoss) fLoss = t_loss;
					}
				}
			}
		}
	}
	return fLoss;
}
#endif 
