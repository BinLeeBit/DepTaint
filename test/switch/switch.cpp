
#include<stdio.h>
 
int main()
{
	int __attribute__((annotate("tainted"))) exam_score;
        int extra_point = 20;
        int total_score;
        char level;
 
	scanf("%d",&exam_score);
        total_score = exam_score*0.7 + extra_point*0.3;

	switch(exam_score / 10)
	{
		case 10:
			level = 'A';
			break; 
		case 9:
			level = 'A';
			break;
		case 8:
			level = 'B';
			break;
		case 7:
			level = 'C';
			break;
		case 6:
			level = 'D';
			break;
		default:
			level = 'E';
			break;
	}
 
	return 0;
}


