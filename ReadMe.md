# YZH物理引擎 & 基于此的愤怒的小鸟demo

核心类是CPhyEnv（物理环境）、CPhyObj（物理对象），观念是泛化物体（甚至button、分数条都可以成为特殊的物体），点击或移动物体会有路由功能转发到相应的物体上，并调用在引擎内注册的回调函数。

![界面](https://github.com/yuezhihan/yzh-physics-engine-demo/blob/master/res/cfb1e933feee1f0f7d30b9aab6b7787c.jpg)

## 主要特点

* 非弹性碰撞
* Alpha通道

```cpp
CPhyObj::CPhyObj(double mass, SPointF pos, SVecF vel, SVecF acc, Image *pImg, double rec)
    :m_attach(0), m_mass(mass), m_rec(rec), m_vel(vel), m_acc(acc), m_pos(pos), m_pImg(pImg), m_realAcc(m_acc)
{
    m_pRgn = new CRgn();
    CRgn &m_rgn = *m_pRgn;
    PixelFormat pf = m_pImg->GetPixelFormat();
    int width = m_pImg->GetWidth();
    int height = m_pImg->GetHeight();

    if (GetPixelFormatSize(pf) == 32) {
        BitmapData Bitdata;
        Rect rect(0, 0, width, height);
        ((Bitmap*)m_pImg)->LockBits(&rect, ImageLockModeRead, pf, &Bitdata);
        char *p = (char*)Bitdata.Scan0;
        m_rgn.CreateRectRgn(0, 0, 0, 0);
        for (int i = 0; i<height; ++i) {
            for (int j = 0; j<width; ++j) {
                int j2;
                for (j2 = j; j2 < width; ++j2) {
                    if (*(p + 4 * width * i + 4 * j2 + 3) == 0)
                        break;
                }
                if (j2>j) {
                    CRgn trgn;
                    trgn.CreateRectRgn(j, i, j2, i + 1);
                    CombineRgn(m_rgn, trgn, m_rgn, RGN_OR);
                }
            }
        }
        ((Bitmap*)m_pImg)->UnlockBits(&Bitdata);
    }
    else m_rgn.CreateRectRgn(0, 0, width, height);
}
```

* 像素级的碰撞判定

```cpp
bool CPhyObj::IsCollision(CPhyObj &ano, bool shapeChange, double tDelta)
{
    if (&ano == this) return false;
    CRgn rgn1, rgn2;
    rgn1.CreateRectRgn(0, 0, 0, 0);
    rgn2.CreateRectRgn(0, 0, 0, 0);
    rgn1.CopyRgn(m_pRgn);
    rgn2.CopyRgn(ano.m_pRgn);
    rgn1.OffsetRgn((int)(m_pos.x),(int)(m_pos.y));
    rgn2.OffsetRgn((int)(ano.m_pos.x), (int)(ano.m_pos.y));

    CRgn rgnOvlap;
    rgnOvlap.CreateRectRgn(0, 0, 0, 0);
    int res = rgnOvlap.CombineRgn(&rgn1, &rgn2, RGN_AND);
    if (res == COMPLEXREGION || res == SIMPLEREGION) { //当前帧有接触
        if (!shapeChange) {
            return true;
        }
        //相对运动趋势判定（下一帧接触变大）
        CRect rect;
        rgnOvlap.GetRgnBox(&rect);
        
        SVecF displace1 = m_vel * tDelta + m_realAcc * 0.5 * tDelta * tDelta;
        SVecF displace2 = ano.m_vel * tDelta + ano.m_realAcc * 0.5 * tDelta * tDelta;
        rgn1.OffsetRgn(-(int)(m_pos.x) + (int)(m_pos.x + displace1.x), -(int)(m_pos.y) + (int)(m_pos.y + displace1.y));
        rgn2.OffsetRgn(-(int)(ano.m_pos.x) + (int)(ano.m_pos.x + displace2.x), -(int)(ano.m_pos.y) + (int)(ano.m_pos.y + displace2.y));

        CRgn rgnOvlap_;
        rgnOvlap_.CreateRectRgn(0, 0, 0, 0);
        rgnOvlap_.CombineRgn(&rgn1, &rgn2, RGN_AND);
        CRect rect_;
        rgnOvlap_.GetRgnBox(&rect_);
        return rect_.Height() * rect_.Width() > rect.Height() * rect.Width();
    } else return false;
}
```

* 泛化物体
* 路由
* 质点系
* 碰撞回调

```cpp
void CPhyEnv::Update()
{
    using namespace std;
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
    vector< pair<CPhyObj*, CPhyObj*> > vec;
    for (int i = 0; i < n; ++i) {
        int cnt = 0;
        while (!m_objs[i].SolveCollision(this, tDelta, vec)) {
            ++cnt;
            if (cnt >= 5) break;
        }
    }
    sort(vec.begin(), vec.end());
    vec.erase( unique(vec.begin(), vec.end()), vec.end() );
    if (m_colfunc != NULL) {
        for (auto iter = vec.begin(); iter != vec.end(); ++iter)
            m_colfunc(iter->first, iter->second);
    }
    for (auto iter = m_objs.begin(); iter != m_objs.end(); ++iter) {
        iter->Go(this, tDelta);
    }
    m_time = theApp.GetTime();
}
```

## BUG

    BUG1: 两物体碰撞后，一个物体先改变运动状态，导致这两个物体反复发生碰撞

    BUG2: 有加速度时，碰撞判定错误，导致物体下陷
    碰撞后速度即使相同，因加速度不同，物体仍然会下陷！
    另一个物体理论上也会具有相同的加速度。。。物体A的加速度也可能改变。。
    碰撞时，设定共同的瞬时加速度？
    多个物体同时碰地面如何处理？？
    每次update，并查集 合并有交集的物体，然后计算 实际加速度，它的值不影响原加速度
    摩擦力无法处理？物理系统来处理物体间的相互作用

    BUG3: 反弹界面方向错误！

![光滑](https://github.com/yuezhihan/yzh-physics-engine-demo/blob/master/res/光滑.gif)

## 问题

    碰撞检测中模拟的位移(下一秒位移过大，要缩小防止穿墙；下一秒位移过小，要放大以防检测不到位移)
    “形变”不会自动恢复，用加速度恢复？？还是强制置为合理位置？？还是上次go之前就判定？？
    速度过大（A帧和A+1帧直接穿过物体）、速度过小（整数级别的碰撞检测失效）都会导致“穿墙”现象
    摩擦力的处理？ 
    反弹界面方向？
