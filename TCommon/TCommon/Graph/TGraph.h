#ifndef _TGRAPH_H_
#define _TGRAPH_H_

#include <QPixmap>

class TGraph
{
	//需要先完善拓扑上的结构判定
	static bool CombinImages(const QList<QPixmap> &lstPix, QPixmap &pix);
};


#endif // !_TGRAPH_H_
