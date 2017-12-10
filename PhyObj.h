#pragma once
#include "Vec.h"
class CPhyEnv;
class CPhyObj
{
public:
	double m_rec; //恢复系数(0~1)
	double m_mass; //质量
	SPointF m_pos; //渲染位置
	
	SVecF m_vel; //速度
	SVecF m_acc; //期望加速度

	SVecF m_realAcc; //真实加速度(考虑物理系统内的碰撞、摩擦)

	Image *m_pImg;
	CRgn *m_pRgn; //图像区域
	
	CPhyObj(double mass, SPointF pos, SVecF vel, SVecF acc, Image *pImg, double rec);
	bool IsCollision(CPhyObj &ano, bool sharpChange = true, double tDelta = 0.3);
	void Collision(CPhyObj &ano);
	bool SolveCollision(CPhyEnv *env, double tDelta);

	bool CanGo(CPhyEnv *env, SVecF displace);
	void Go(CPhyEnv *env, double tDelta);
	~CPhyObj();
};

