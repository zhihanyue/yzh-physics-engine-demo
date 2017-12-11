#include "stdafx.h"
#include "PhyEnv.h"


CPhyEnv::CPhyEnv() :m_speed(0.01)
{
	m_time = theApp.GetTime();
}

CPhyEnv::~CPhyEnv()
{
	for (int i = 0, _m = m_objs.size(); i < _m; ++i) {
		delete m_objs[i].m_pImg;
		delete m_objs[i].m_pRgn;
	}
}

void CPhyEnv::Display(CDC *pDC, SPointF pos)
{
	//CBrush br = CBrush(RGB(255, 0, 0));
	//pDC->FillRgn(m_objs[2].m_pRgn, &br);

	Graphics graph(pDC->GetSafeHdc());
	for (int i = 0, _m = m_objs.size(); i < _m; ++i) {
		CPhyObj &tobj = m_objs[i];
		graph.DrawImage(tobj.m_pImg,(int)(pos.x + tobj.m_pos.x),(int)(pos.y + tobj.m_pos.y)
			,tobj.m_pImg->GetWidth(), tobj.m_pImg->GetHeight());
	}
}

void CPhyEnv::Update()
{
	int n = m_objs.size();
	uset_p.resize(n);
	for (int i = 0; i < n; ++i)
		uset_p[i] = i;
	for (int i = 0; i < n; ++i)
		for (int j = i + 1; j < n; ++j) {
			if (m_objs[i].IsCollision(m_objs[j], false)) {
				uset_p[uset_find(i)] = uset_find(j);
			}
		}
	for (int i = 0; i < n; ++i) {
		std::vector<int> v;
		for (int j = 0; j < n; ++j) {
			if (uset_find(j) == i) {
				v.push_back(j);
			}
		}

		double M = 0.0;
		SVecF F(0.0,0.0);
		int _m = v.size();
		for (int j = 0; j < _m; ++j) {
			M += m_objs[v[j]].m_mass;
			F = F + m_objs[v[j]].m_acc * m_objs[v[j]].m_mass;
		}
		SVecF realA = F / M;
		for (int j = 0; j < _m; ++j) {
			m_objs[v[j]].m_realAcc = realA;
		}
	}

	double tDelta = (theApp.GetTime() - m_time) * m_speed;
	for (int i = 0; i < n; ++i) {
		//m_objs[i].Go(this, tDelta);
		int cnt = 0;
		while (!m_objs[i].SolveCollision(this, tDelta)) {
			//TRACE("Going...\n");
			++cnt;
			if (cnt >= 5) break;
		}
		//BUG: 不能等走过以后再去处理碰撞！！！
		//TRACE("Gone!\n");
	}

	for (int i = 0; i < n; ++i) {
		m_objs[i].Go(this, tDelta);
		//TRACE("Gone!\n");
	}
	m_time = theApp.GetTime();
}

CPhyObj *CPhyEnv::Route(SPointF pos)
{
	for (int i = m_objs.size() - 1; i >= 0; --i) {
		CPhyObj &obj = m_objs[i];
		if (obj.m_pRgn->PtInRegion(CPoint((int)pos.x - (int)obj.m_pos.x, (int)pos.y - (int)obj.m_pos.y))) {
			return &obj;
		}
	}
	return NULL;
}
