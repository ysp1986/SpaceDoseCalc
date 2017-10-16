#pragma once
#include "Earth.h"
#include "Sun.h"
#include "Moon.h"
//�ԻƵ�Ϊx��yƽ�棬̫���ڣ�0��0��0����
//����SCALE_OTHER_STARΪ�������������ĳߴ�����ʵ�ߴ�Ĺ�ϵ
//Ŀǰֻ��̫���͵���
//������ת�������x��˳ʱ��ת��23��
//����̫���ľ���Ϊ149,600,000km
//������¾���ʼ��Ϊ385,000km
//����뾶6 378.1km  ��ת����1�죬��ת����365��
//̫���뾶695 500km  ��ת����24.47��
//����뾶1 737.4km  ��������385,000km��
//����ĳߴ�ÿ��λΪ1000km
class SolarSys
{
public:
	SolarSys();
	~SolarSys();
	//ʱ��ĳ߶�(��/s)
	double myTimeScale;
	//earth
	double myRotAngle; //�Զ�Ϊ��λ����ת�Ƕ�
	double myRevAngle; //�Զ�Ϊ��λ�Ĺ�ת�Ƕ�
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

