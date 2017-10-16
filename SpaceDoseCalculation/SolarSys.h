#pragma once
#include "Earth.h"
#include "Sun.h"
#include "Moon.h"
//以黄道为x，y平面，太阳在（0，0，0）处
//比例SCALE_OTHER_STAR为除地球外各星球的尺寸与真实尺寸的关系
//目前只画太阳和地球
//地球自转轴假设绕x轴顺时针转动23度
//地球到太阳的距离为149,600,000km
//假设地月距离始终为385,000km
//地球半径6 378.1km  自转周期1天，公转周期365天
//太阳半径695 500km  自转周期24.47天
//月球半径1 737.4km  距地球距离385,000km，
//这里的尺寸每单位为1000km
class SolarSys
{
public:
	SolarSys();
	~SolarSys();
	//时间的尺度(天/s)
	double myTimeScale;
	//earth
	double myRotAngle; //以度为单位的自转角度
	double myRevAngle; //以度为单位的公转角度
	//earth position

	static double DIS_EARTH_SUN;
	static double DIS_EARTH_MOON;

	static double SCALE_OTHER_STAR;
	static double SCALE_SATTELITE;

	Camera camera;
	Earth myEarth;
	Moon myMoon;
	Sun mySun;
	Mesh galaxyBackground;
	Shader backgroundShader;

	enum {un_inited, init_earth, init_moon, init_sun, finished } initState;

	bool Initialize();

	void SetMats(double dayCount);

	void Draw();
};

