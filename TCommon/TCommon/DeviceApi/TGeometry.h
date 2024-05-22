#ifndef _TGEOMETRY_H_
#define _TGEOMETRY_H_

/*����ѧ��صļ���*/

template<typename T>
T Distance(T x0, T y0, T x1, T y1)
{
	return std::sqrt((x0 - x1)*(x0 - x1) + (y0 - y1)*((y0 - y1)));
}

//************************************
// ����: 	T x0��y0: ��
// ����: 	T linex1, liney1, linex2, liney2: ֱ������һ�����x/y����
// ����:	T
// ����:	���߾�
//************************************
template<typename T>
T Distance(T x0, T y0, T linex1, T liney1, T linex2, T liney2)
{
	T a, b, c, distance;
	a = (x0 - linex1)*(liney2 - liney1);
	b = (y0 - liney1)*(linex1 - linex2);
	c = a + b;
	c *= c;
	a = pow(liney2 - liney1, 2);
	b = pow(linex1 - linex2, 2);
	c /= (a + b);
	distance = sqrt(c);
	return distance;
}


#endif
