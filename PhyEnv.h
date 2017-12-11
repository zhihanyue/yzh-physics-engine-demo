#pragma once
#include "PhyObj.h"

class CPhyEnv
{
public:
	double m_speed;
	std::vector<CPhyObj> m_objs;
	CPhyEnv();
	~CPhyEnv();
	void Display(CDC *pDC, SPointF pos = SPointF(0, 0));
	void Update();
	CPhyObj *Route(SPointF pos);
	int uset_find(int x) {
		return (uset_p[x] == x) ? x : (uset_p[x] = uset_find(uset_p[x]));
	}
private:
	LONGLONG m_time;
	std::vector<int> uset_p;
};

