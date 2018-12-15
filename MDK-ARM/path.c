#include <stdio.h>
#include <string.h>

#define PointCount 44
extern void DEBUG(int x);

const int PointX[PointCount] = 
	{   65,185, 30, 30, 70,117,184, 82,148,215,
	   183,112,141,219,180,114,259,252,200,158,
	   237,237, 40,124,165, 64, 10, 20, 39, 83,
	   259,185,  8, 12, 87,124,129, 31, 11, 27,
	    63, 65, 57, 31
	};
const int PointY[PointCount] = 
	{  185, 70,185,240,240,260,198, 98,162, 93,
		30, 39,125,202,161,224,126, 87,140,181,
		73, 33,108,186,143,259,260,102, 88, 69,
		15,  8,185, 58, 27,  8, 30,125,119, 18,
		13, 34, 68, 45
	};

typedef struct Edge{
    int from, current, target, dist;
		int next;
}Edge; 
Edge edge[200];
int edgeCount;
int first[PointCount][PointCount];
void addEdge(int from, int current, int target, int dist){// 以边的终点作为前向星列表头
    edgeCount++;
    edge[edgeCount].from = from;
    edge[edgeCount].current = current;
    edge[edgeCount].target = target;
    edge[edgeCount].dist = dist;
    edge[edgeCount].next = first[current][target];
    first[current][target] = edgeCount;
}
void initEdge(){
	memset(first, 0, sizeof first);
}

//-------------------------------------------------------------------
// 储存和提供每个状态对应的操作
// initOperation()
// addOperation(a, b, c, val)
// val getOperation(a, b, c)
//-------------------------------------------------------------------
const int OperationMapCapacity = 1007;

typedef struct Operation{
    int from, current, target, value;
}Operation;
Operation op[OperationMapCapacity];

int getOperationHash(int from, int current, int target){
    return (from * 129 + current * 332 + target * 29) % OperationMapCapacity;
}
void clearOperation(){   // 一定记得先调用clear
    memset(op, -1, sizeof op);
}
void addOperation(int from, int current, int target, int value){
    int hash = getOperationHash(from, current, target);
    while (op[hash].from != -1){
        hash++;
        if (hash > OperationMapCapacity)
            hash -= OperationMapCapacity;
    }
    op[hash].from = from;
    op[hash].current = current;
    op[hash].target = target;
    op[hash].value = value;
	addEdge(from, current, target, 1);
}
int getOperation(int from, int current, int target){
    int hash = getOperationHash(from, current, target);
    while (op[hash].from != -1 && (op[hash].from != from || op[hash].current != current || op[hash].target != target)){
        hash++;
        if (hash > OperationMapCapacity)
            hash -= OperationMapCapacity;
    }
    if (op[hash].from == -1)
        return -1;
    return op[hash].value;
}

//-------------------------------------------------------------------
// 将状态压缩成一个int，便于保存
// val operationEncoder(turnDeg, state)
// operationDecoder(value)
// initOperation()
//-------------------------------------------------------------------
#define GUIDE 0
#define GUIDEDELAYSEEK 1
#define GUIDENOSEEK 2
#define LSEEK 3
#define RSEEK 4
int operationEncoder(int turnDeg, int operation){
    return (turnDeg + 180) * 10 + operation;
}
/*void operationDecoder(int value){
	int turnDeg = value / 10 - 180;
	int operation = value % 10;
	if (turnDeg > 0)
		printf("右转 %3d\t\t", turnDeg);
	else if (turnDeg < 0)
		printf("左转 %3d\t\t", -turnDeg);
	else
		printf("方向不变\t\t");
	switch (operation){
		case 0:
			printf("普通目标导航");
			break;
		case 1:
			printf("延迟目标导航");
			break;
		case 2:
			printf("纯目标导航");
			break;
		case 3:
			printf("左巡线");
			break;
		case 4:
			printf("右巡线");
			break;
	}
	printf("\n");
}*/

void initOperation(){
		clearOperation();
        addOperation(0,0,2,operationEncoder(0,GUIDENOSEEK));
        addOperation(0,0,32,operationEncoder(0,GUIDENOSEEK));
        addOperation(0,2,3,operationEncoder(90,GUIDE));
        addOperation(0,32,38,operationEncoder(-90,GUIDE));
        addOperation(1,1,10,operationEncoder(0,GUIDENOSEEK));
        addOperation(1,1,31,operationEncoder(0,GUIDENOSEEK));
        addOperation(1,10,36,operationEncoder(90,GUIDE));
        addOperation(1,31,30,operationEncoder(-90,GUIDE));
        addOperation(10,31,30,operationEncoder(-90,GUIDE));
        addOperation(10,36,11,operationEncoder(15,GUIDE));
        addOperation(11,29,12,operationEncoder(90,GUIDE));
        addOperation(11,29,22,operationEncoder(-10,GUIDENOSEEK));
        addOperation(11,29,28,operationEncoder(-30,GUIDENOSEEK));
        addOperation(11,29,34,operationEncoder(-135,GUIDENOSEEK));
        addOperation(11,34,29,operationEncoder(135,GUIDENOSEEK));
        addOperation(11,34,35,operationEncoder(-120,GUIDE));
        addOperation(11,34,40,operationEncoder(0,GUIDENOSEEK));
        addOperation(12,13,16,operationEncoder(0,RSEEK));
        addOperation(12,14,15,operationEncoder(-90,GUIDE));
        addOperation(12,8,7,operationEncoder(-135,GUIDE));
        addOperation(12,9,17,operationEncoder(-45,GUIDENOSEEK));
        addOperation(12,9,20,operationEncoder(0,GUIDEDELAYSEEK));
        addOperation(13,16,17,operationEncoder(0,GUIDENOSEEK));
        addOperation(13,16,20,operationEncoder(30,GUIDENOSEEK));
        addOperation(14,13,16,operationEncoder(0,RSEEK));
        addOperation(14,15,25,operationEncoder(0,GUIDENOSEEK));
        addOperation(14,15,4,operationEncoder(-30,GUIDENOSEEK));
        addOperation(14,15,5,operationEncoder(0,RSEEK));
        addOperation(15,25,26,operationEncoder(-45,GUIDE));
        addOperation(15,4,23,operationEncoder(-150,GUIDE));
        addOperation(15,5,6,operationEncoder(0,RSEEK));
        addOperation(16,17,18,operationEncoder(135,GUIDE));
        addOperation(16,20,21,operationEncoder(-30,GUIDE));
        addOperation(17,18,13,operationEncoder(0,RSEEK));
        addOperation(17,18,15,operationEncoder(0,GUIDEDELAYSEEK));
        addOperation(17,18,19,operationEncoder(0,GUIDENOSEEK));
        addOperation(17,18,24,operationEncoder(-45,GUIDENOSEEK));
        addOperation(17,20,21,operationEncoder(-45,GUIDE));
        addOperation(18,13,16,operationEncoder(0,RSEEK));
        addOperation(18,15,25,operationEncoder(0,GUIDENOSEEK));
        addOperation(18,15,4,operationEncoder(-30,GUIDENOSEEK));
        addOperation(18,15,5,operationEncoder(0,RSEEK));
        addOperation(18,19,7,operationEncoder(-90,GUIDE));
        addOperation(18,24,9,operationEncoder(-135,GUIDE));
        addOperation(19,15,25,operationEncoder(0,GUIDENOSEEK));
        addOperation(19,15,4,operationEncoder(-30,GUIDENOSEEK));
        addOperation(19,15,5,operationEncoder(0,RSEEK));
        addOperation(19,7,22,operationEncoder(0,RSEEK));
        addOperation(19,7,28,operationEncoder(30,GUIDENOSEEK));
        addOperation(19,7,29,operationEncoder(-45,GUIDENOSEEK));
        addOperation(2,3,4,operationEncoder(90,GUIDENOSEEK));
        addOperation(2,32,38,operationEncoder(-90,GUIDE));
        addOperation(20,21,10,operationEncoder(90,GUIDE));
        addOperation(21,10,31,operationEncoder(-90,GUIDENOSEEK));
        addOperation(21,10,36,operationEncoder(0,GUIDE));
        addOperation(22,37,2,operationEncoder(15,GUIDE));
        addOperation(23,19,15,operationEncoder(-135,GUIDE));
        addOperation(23,24,13,operationEncoder(-90,GUIDE));
        addOperation(23,7,22,operationEncoder(0,RSEEK));
        addOperation(23,7,28,operationEncoder(30,GUIDENOSEEK));
        addOperation(23,7,29,operationEncoder(-45,GUIDENOSEEK));
        addOperation(23,9,17,operationEncoder(-45,GUIDENOSEEK));
        addOperation(23,9,20,operationEncoder(0,GUIDENOSEEK));
        addOperation(24,13,16,operationEncoder(0,RSEEK));
        addOperation(24,9,17,operationEncoder(-45,GUIDENOSEEK));
        addOperation(24,9,20,operationEncoder(0,GUIDENOSEEK));
        addOperation(25,26,32,operationEncoder(-90,GUIDE));
        addOperation(26,32,2,operationEncoder(-90,GUIDENOSEEK));
        addOperation(26,32,38,operationEncoder(0,GUIDE));
        addOperation(27,28,22,operationEncoder(-135,GUIDENOSEEK));
        addOperation(27,28,29,operationEncoder(-30,GUIDENOSEEK));
        addOperation(27,28,33,operationEncoder(120,GUIDENOSEEK));
        addOperation(27,28,42,operationEncoder(0,GUIDENOSEEK));
        addOperation(27,33,28,operationEncoder(-150,GUIDENOSEEK));
        addOperation(27,33,39,operationEncoder(-30,GUIDENOSEEK));
        addOperation(28,22,37,operationEncoder(-30,GUIDE));
        addOperation(28,29,12,operationEncoder(-60,GUIDE));
        addOperation(28,29,34,operationEncoder(60,GUIDENOSEEK));
        addOperation(28,33,39,operationEncoder(-60,GUIDENOSEEK));
        addOperation(28,42,41,operationEncoder(45,GUIDENOSEEK));
        addOperation(28,42,43,operationEncoder(100,GUIDENOSEEK));
        addOperation(29,12,13,operationEncoder(0,GUIDENOSEEK));
        addOperation(29,12,14,operationEncoder(0,GUIDENOSEEK));
        addOperation(29,12,8,operationEncoder(-45,GUIDENOSEEK));
        addOperation(29,12,9,operationEncoder(0,RSEEK));
        addOperation(29,22,37,operationEncoder(10,GUIDE));
        addOperation(29,28,22,operationEncoder(80,GUIDENOSEEK));
        addOperation(29,28,33,operationEncoder(-60,GUIDENOSEEK));
        addOperation(29,28,42,operationEncoder(-150,GUIDENOSEEK));
        addOperation(29,34,35,operationEncoder(-60,GUIDE));
        addOperation(29,34,40,operationEncoder(60,GUIDENOSEEK));
        addOperation(3,4,23,operationEncoder(45,GUIDE));
        addOperation(3,4,25,operationEncoder(-90,GUIDENOSEEK));
        addOperation(3,4,5,operationEncoder(-30,GUIDE));
        addOperation(30,17,18,operationEncoder(-45,GUIDE));
        addOperation(30,17,20,operationEncoder(-120,GUIDENOSEEK));
        addOperation(31,10,36,operationEncoder(-90,GUIDE));
        addOperation(31,30,17,operationEncoder(-90,GUIDE));
        addOperation(32,2,3,operationEncoder(-90,GUIDE));
        addOperation(32,38,27,operationEncoder(-30,GUIDE));
        addOperation(33,28,22,operationEncoder(-45,GUIDENOSEEK));
        addOperation(33,28,29,operationEncoder(120,GUIDENOSEEK));
        addOperation(33,28,42,operationEncoder(60,GUIDENOSEEK));
        addOperation(33,39,40,operationEncoder(-45,GUIDENOSEEK));
        addOperation(34,29,12,operationEncoder(45,GUIDE));
        addOperation(34,29,22,operationEncoder(30,GUIDENOSEEK));
        addOperation(34,29,28,operationEncoder(-45,GUIDENOSEEK));
        addOperation(34,35,31,operationEncoder(30,GUIDE));
        addOperation(34,40,39,operationEncoder(30,GUIDENOSEEK));
        addOperation(35,31,10,operationEncoder(-90,GUIDENOSEEK));
        addOperation(35,31,30,operationEncoder(0,GUIDE));
        addOperation(36,11,29,operationEncoder(10,GUIDENOSEEK));
        addOperation(36,11,34,operationEncoder(-90,GUIDENOSEEK));
        addOperation(37,2,3,operationEncoder(0,GUIDE));
        addOperation(37,2,32,operationEncoder(-90,GUIDENOSEEK));
        addOperation(38,27,28,operationEncoder(-15,GUIDENOSEEK));
        addOperation(38,27,33,operationEncoder(60,GUIDENOSEEK));
        addOperation(39,33,28,operationEncoder(100,GUIDENOSEEK));
        addOperation(39,40,34,operationEncoder(-45,GUIDENOSEEK));
        addOperation(4,23,19,operationEncoder(-45,GUIDENOSEEK));
        addOperation(4,23,24,operationEncoder(0,GUIDENOSEEK));
        addOperation(4,23,7,operationEncoder(0,RSEEK));
        addOperation(4,23,9,operationEncoder(0,GUIDEDELAYSEEK));
        addOperation(4,5,6,operationEncoder(30,RSEEK));
        addOperation(40,34,29,operationEncoder(-60,GUIDENOSEEK));
        addOperation(40,34,35,operationEncoder(60,GUIDE));
        addOperation(40,39,33,operationEncoder(60,GUIDENOSEEK));
        addOperation(41,42,28,operationEncoder(-45,GUIDENOSEEK));
        addOperation(41,43,42,operationEncoder(120,GUIDENOSEEK));
        addOperation(42,28,22,operationEncoder(45,GUIDENOSEEK));
        addOperation(42,28,29,operationEncoder(160,GUIDENOSEEK));
        addOperation(42,28,33,operationEncoder(-60,GUIDENOSEEK));
        addOperation(42,41,43,operationEncoder(120,GUIDENOSEEK));
        addOperation(42,43,41,operationEncoder(-120,GUIDENOSEEK));
        addOperation(43,41,42,operationEncoder(-120,GUIDENOSEEK));
        addOperation(43,42,28,operationEncoder(-100,GUIDENOSEEK));
        addOperation(5,6,14,operationEncoder(-45,GUIDENOSEEK));
        addOperation(5,6,15,operationEncoder(0,RSEEK));
        addOperation(5,6,7,operationEncoder(0,GUIDEDELAYSEEK));
        addOperation(5,6,8,operationEncoder(0,GUIDENOSEEK));
        addOperation(6,14,13,operationEncoder(-135,GUIDE));
        addOperation(6,15,25,operationEncoder(0,GUIDEDELAYSEEK));
        addOperation(6,15,4,operationEncoder(-30,GUIDEDELAYSEEK));
        addOperation(6,7,22,operationEncoder(0,RSEEK));
        addOperation(6,7,28,operationEncoder(30,GUIDENOSEEK));
        addOperation(6,7,29,operationEncoder(-70,GUIDENOSEEK));
        addOperation(6,8,9,operationEncoder(-90,GUIDE));
        addOperation(7,22,37,operationEncoder(0,GUIDE));
        addOperation(7,28,33,operationEncoder(-30,GUIDENOSEEK));
        addOperation(7,28,42,operationEncoder(-120,GUIDENOSEEK));
        addOperation(7,29,12,operationEncoder(-120,GUIDE));
        addOperation(7,29,28,operationEncoder(100,GUIDENOSEEK));
        addOperation(7,29,34,operationEncoder(0,GUIDENOSEEK));
        addOperation(8,7,22,operationEncoder(0,RSEEK));
        addOperation(8,7,28,operationEncoder(30,GUIDENOSEEK));
        addOperation(8,7,29,operationEncoder(-70,GUIDENOSEEK));
        addOperation(8,9,17,operationEncoder(-30,GUIDENOSEEK));
        addOperation(8,9,20,operationEncoder(0,GUIDENOSEEK));
        addOperation(9,17,18,operationEncoder(-150,GUIDE));
        addOperation(9,20,21,operationEncoder(40,GUIDE));
}

//-------------------------------------------------------------------
// x, y是否在四边形x1y1-x4y4内，四边形可以顺时针也可以逆时针
// bool isInside(...)
//-------------------------------------------------------------------
int isInside(int x, int y, int x1, int y1, int x2, int y2, int x3, int y3, int x4, int y4){
#define cross(x, y, x1, y1, x2, y2) ((x2-x1)*(y-y1) - (y2-y1)*(x-x1))
    if (cross(x, y, x1, y1, x2, y2) >= 0)
        return cross(x, y, x2, y2, x3, y3) >= 0 && cross(x, y, x3, y3, x4, y4) >= 0 && cross(x, y, x4, y4, x1, y1) >= 0;
    else
        return cross(x, y, x2, y2, x3, y3) < 0 && cross(x, y, x3, y3, x4, y4) < 0 && cross(x, y, x4, y4, x1, y1) < 0;
}

//-------------------------------------------------------------------
// 对于位置x,y 得到可能的所有状态 比如在14号点的位置有可能是12-13 12-14 14-15 18-19等等 应该是需要把所有initEdge函数里出现过的二元对(如 addEdge(2, 3, 4, 1) 的二元对包含2-3和3-4)都定一遍位 大概需要定位一百多个矩形 对于在环岛的非矩形 可以大致定位 或者直接用多个矩形拼起来也行
// getPossibleStates(x, y, 状态计数, 从哪来, 目的地)
//-------------------------------------------------------------------
void getPossibleStates(int x, int y, int *stateCount, int *from, int *to){
#define insert(fr, t) { from[*stateCount] = fr; to[*stateCount] = t; (*stateCount)++; }
    *stateCount = 0;
    if (isInside(x, y, 54, 195, 54, 175, 80, 175, 80, 195)) insert(0, 0);
    if (isInside(x, y, 0, 195, 0, 175, 80, 175, 80, 195)) insert(0, 32);
    if (isInside(x, y, 175, 80, 175, 20, 195, 20, 195, 80)) insert(1, 10);
    if (isInside(x, y, 175, 20, 175, 0, 270, 0, 270, 20)) insert(31, 30);
    if (isInside(x, y, 59, 245, 97, 269, 122, 257, 71, 225)) insert(4, 5);
    if (isInside(x, y, 100, 270, 186, 186, 235, 235, 200, 270)) insert(5, 6);
    if (isInside(x, y, 110, 185, 125, 200, 165, 190, 165, 175)) insert(23, 19);
    if (isInside(x, y, 100, 220, 125, 250, 193, 193, 143, 184)) insert(6, 15);
    if (isInside(x, y, 124, 226, 110, 213, 68, 231, 67, 249)) insert(15, 4);
    if (isInside(x, y, 80, 108, 93, 94, 28, 76, 26, 96)) insert(7, 28);
    if (isInside(x, y, 143, 181, 127, 166, 213, 83, 227, 95)) insert(8, 9);
    if (isInside(x, y, 213, 108, 195, 84, 270, 80, 270, 95)) insert(9, 17);
    if (isInside(x, y, 225, 80, 222, 20, 250, 20, 250, 85)) insert(20, 21);
    if (isInside(x, y, 205, 205, 250, 120, 270, 120, 270, 270)) insert(13, 16);
    if (isInside(x, y, 250, 135, 250, 75, 270, 75, 270, 135)) insert(16, 17);
    if (isInside(x, y, 75, 270, 0, 270, 0, 250, 75, 250)) insert(25, 26);
    if (isInside(x, y, 210, 140, 155, 196, 143, 181, 196, 127)) insert(18, 19);
    if (isInside(x, y, 175, 40, 175, 20, 250, 20, 250, 40)) insert(21, 10);
    if (isInside(x, y, 167, 125, 228, 201, 216, 216, 150, 150)) insert(24, 13);
    if (isInside(x, y, 11, 98, 24, 110, 47, 97, 39, 77)) insert(27, 28);
    if (isInside(x, y, 19, 166, 19, 250, 40, 250, 40, 166)) insert(2, 3);
    if (isInside(x, y, 174, 56, 174, 80, 194, 80, 194, 56)) insert(1, 1);
    if (isInside(x, y, 121, 22, 127, 41, 195, 41, 195, 20)) insert(10, 36);
    if (isInside(x, y, 124, 21, 93, 39, 102, 54, 134, 41)) insert(36, 11);
    if (isInside(x, y, 0, 195, 40, 195, 40, 175, 0, 175)) insert(2, 32);
    if (isInside(x, y, 50, 118, 124, 200, 162, 162, 84, 85)) insert(23, 7);
    if (isInside(x, y, 171, 209, 195, 198, 191, 152, 174, 148)) insert(6, 14);
    if (isInside(x, y, 122, 172, 179, 223, 203, 203, 144, 145)) insert(6, 8);
    if (isInside(x, y, 31, 101, 44, 119, 89, 100, 78, 82)) insert(7, 22);
    if (isInside(x, y, 37, 95, 18, 131, 38, 135, 55, 102)) insert(22, 37);
    if (isInside(x, y, 20, 120, 20, 193, 40, 193, 43, 122)) insert(37, 2);
    if (isInside(x, y, 78, 25, 90, 38, 124, 20, 117, 1)) insert(34, 35);
    if (isInside(x, y, 122, 21, 191, 21, 191, 0, 115, 0)) insert(35, 31);
    if (isInside(x, y, 184, 138, 201, 151, 269, 89, 256, 76)) insert(17, 18);
    if (isInside(x, y, 176, 40, 195, 40, 195, 0, 176, 0)) insert(10, 31);
    if (isInside(x, y, 125, 125, 216, 216, 230, 200, 138, 111)) insert(12, 13);
    if (isInside(x, y, 186, 138, 101, 223, 114, 237, 201, 151)) insert(14, 15);
    if (isInside(x, y, 97, 269, 115, 270, 122, 223, 107, 214)) insert(15, 5);
    if (isInside(x, y, 198, 125, 100, 223, 114, 237, 213, 139)) insert(18, 15);
    if (isInside(x, y, 70, 99, 164, 207, 185, 188, 83, 86)) insert(19, 7);
    if (isInside(x, y, 0, 195, 40, 195, 40, 175, 0, 175)) insert(32, 2);
    if (isInside(x, y, 176, 40, 195, 40, 195, 0, 176, 0)) insert(31, 10);
    if (isInside(x, y, 80, 195, 80, 175, 20, 175, 20, 195)) insert(0, 2);
    if (isInside(x, y, 0, 195, 20, 195, 20, 120, 0, 112)) insert(32, 38);
    if (isInside(x, y, 0, 117, 15, 133, 33, 100, 14, 87)) insert(38, 27);
    if (isInside(x, y, 175, 80, 195, 80, 195, 0, 175, 0)) insert(1, 31);
    if (isInside(x, y, 20, 250, 85, 250, 85, 230, 20, 230)) insert(3, 4);
    if (isInside(x, y, 66, 227, 66, 250, 137, 186, 122, 172)) insert(4, 23);
    if (isInside(x, y, 113, 183, 125, 198, 227, 96, 213, 82)) insert(23, 9);
    if (isInside(x, y, 113, 183, 125, 198, 177, 143, 164, 131)) insert(23, 24);
    if (isInside(x, y, 182, 211, 196, 195, 85, 85, 70, 99)) insert(6, 7);
    if (isInside(x, y, 65, 262, 77, 270, 126, 226, 109, 216)) insert(15, 25);
    if (isInside(x, y, 70, 99, 79, 61, 98, 62, 88, 116)) insert(7, 29);
    if (isInside(x, y, 165, 165, 180, 150, 230, 200, 218, 218)) insert(14, 13);
    if (isInside(x, y, 72, 72, 84, 57, 150, 123, 137, 137)) insert(29, 12);
    if (isInside(x, y, 203, 92, 215, 108, 250, 70, 227, 70)) insert(9, 20);
    if (isInside(x, y, 112, 26, 111, 49, 75, 33, 78, 13)) insert(11, 34);
    if (isInside(x, y, 129, 131, 169, 149, 226, 98, 213, 83)) insert(12, 9);
    if (isInside(x, y, 128, 128, 140, 113, 195, 164, 180, 180)) insert(12, 14);
    if (isInside(x, y, 250, 136, 270, 136, 250, 70, 227, 70)) insert(16, 20);
    if (isInside(x, y, 197, 126, 240, 183, 211, 211, 168, 168)) insert(18, 13);
    if (isInside(x, y, 200, 150, 200, 130, 160, 130, 160, 150)) insert(18, 24);
    if (isInside(x, y, 151, 144, 164, 158, 227, 96, 213, 82)) insert(24, 9);
	if (isInside(x, y, 135, 121, 137, 174, 153, 173, 151, 124)) insert(12, 8);
	if (isInside(x, y, 67, 98, 145, 178, 159, 164, 81, 83)) insert(8, 7);
	if (isInside(x, y, 100, 223, 114, 237, 170, 186, 155, 173)) insert(19, 15);
    if (isInside(x, y, 0, 270, 20, 270, 20, 175, 0, 175)) insert(26, 32);
	if (isInside(x, y, 248, 0, 270, 0, 270, 95, 249, 95)) insert(30, 17);
	if (isInside(x, y, 67, 230, 85, 229, 92, 269, 68, 270)) insert(4, 25);
	if (isInside(x, y, 0, 56, 33, 98, 46, 83, 17, 50)) insert(28, 33);
	if (isInside(x, y, 0, 56, 33, 98, 46, 83, 17, 50)) insert(33, 28);
	if (isInside(x, y, 0, 65, 20, 66, 41, 21, 0, 0)) insert(33, 39);
	if (isInside(x, y, 0, 65, 20, 66, 41, 21, 0, 0)) insert(39, 33);
	if (isInside(x, y, 20, 0, 26, 29, 69, 22, 73, 0)) insert(39, 40);
	if (isInside(x, y, 20, 0, 26, 29, 69, 22, 73, 0)) insert(40, 39);
	if (isInside(x, y, 54, 20, 84, 42, 100, 29, 59, 0)) insert(40, 34);
	if (isInside(x, y, 54, 20, 84, 42, 100, 29, 59, 0)) insert(34, 40);
	if (isInside(x, y, 71, 78, 88, 80, 94, 21, 79, 21)) insert(29, 34);
	if (isInside(x, y, 71, 78, 88, 80, 94, 21, 79, 21)) insert(34, 29);
	if (isInside(x, y, 32, 95, 88, 79, 79, 60, 30, 82)) insert(29, 28);
	if (isInside(x, y, 32, 95, 88, 79, 79, 60, 30, 82)) insert(28, 29);
	if (isInside(x, y, 10, 100, 32, 102, 19, 52, 0, 55)) insert(27, 33);
	if (isInside(x, y, 69, 72, 85, 81, 114, 47, 103, 31)) insert(11, 29);
	if (isInside(x, y, 28, 108, 45, 116, 91, 73, 77, 60)) insert(29, 22);
	if (isInside(x, y, 28, 88, 41, 95, 67, 69, 54, 60)) insert(28, 42);
	if (isInside(x, y, 18, 58, 48, 82, 68, 73, 24, 33)) insert(42, 43);
	if (isInside(x, y, 18, 58, 48, 82, 68, 73, 24, 33)) insert(43, 42);
	if (isInside(x, y, 32, 26, 19, 55, 77, 39, 53, 21)) insert(41, 43);
	if (isInside(x, y, 32, 26, 19, 55, 77, 39, 53, 21)) insert(43, 41);
	if (isInside(x, y, 46, 81, 69, 74, 82, 46, 63, 25)) insert(41, 42);
	if (isInside(x, y, 46, 81, 69, 74, 82, 46, 63, 25)) insert(42, 41);
	if (isInside(x, y, 223, 73, 268, 102, 268, 80, 229, 63)) insert(17, 20);
}

unsigned short d[PointCount][PointCount];
char vis[PointCount][PointCount];
int  F[40], T[40], qF[1000], qT[1000];
int getNextStep(int from, int current, int X, int Y){
    int stateCount, i, j, head=0, tail=0, queueCount=0, nextStep=-1, k;
    memset(d, 0x3f, sizeof d);
    getPossibleStates(X, Y, &stateCount, F, T);
    for (i = 0; i < stateCount; ++i){
        d[F[i]][T[i]] = 0;
        vis[F[i]][T[i]] = 1;
        qF[i] = F[i];
        qT[i] = T[i];
    }
    tail = queueCount = stateCount;
    while (queueCount > 0){
        i = qF[head]; j = qT[head];
        ++head; --queueCount;
        if (head >= 1000) head = 0;
        for (k = first[i][j]; k; k = edge[k].next){
            if (d[edge[k].from][i] > d[i][j] + edge[k].dist){
                d[edge[k].from][i] = d[i][j] + edge[k].dist;
                if (edge[k].from == from && i == current)
                    nextStep = j;
                if (!vis[edge[k].from][i]){
                    vis[edge[k].from][i] = 1;
                    qF[tail] = edge[k].from; qT[tail] = i;
                    tail++; queueCount++;
                    if (tail >= 1000) tail = 0;
                }
            }
				}
        vis[i][j] = 0;
    }
    return nextStep;
}
