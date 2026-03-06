#include "sys.h"

#define AIN1 PBout(4)
#define AIN2 PBout(3)
#define BIN1 PAout(15)
#define BIN2 PAout(12)
/*------------Car Driving Base and Mode Programs-----------*/
void Direction(FunctionalState left,FunctionalState right);
void Pack();
void Speed(u8 ls,u8 rs);
void Angle(u8 ang);
void MarkFollow();
void AvoidObstacle();
void MoveFollow();