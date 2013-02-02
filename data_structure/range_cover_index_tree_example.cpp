// Interval 갱신 / 받아오는 인덱스 트리 예제 구현.
//
// 이 코드는 interval을 cover하고, 이미 cover했던 적이 있는 구간을 정확히 uncover하는 경우를 구현.
//
// 얻을 수 있는 값은 lentree에서 대표값들을 취합하면 cover된 영역의 크기를 구할 수 있다.
// 즉 length of union of covers.

const int B_T = 65536;

int lentree[B_T*2];
int addtree[B_T*2];

inline void calclen(unsigned int pos, int nowlen)
{
	if(addtree[pos]) lentree[pos] = nowlen;
	else if (pos < B_T) lentree[pos] = lentree[pos << 1] + lentree[(pos<<1) + 1];
	else lentree[pos] = 0;
}

/**/
void cover(unsigned int x1, unsigned int x2)
{
	x1 += B_T, x2 += B_T;
	unsigned int ox1 = x1, ox2 = x2;
	int nowlen = 1;
	while(x1 <= x2)
	{
		if (x1 % 2u == 1u)
		{
			addtree[x1] ++;
			lentree[x1] = nowlen;
		}
		if (x2 % 2u == 0u)
		{
			addtree[x2] ++;
			lentree[x2] = nowlen;
		}
		x1 = (x1+1)>>1;
		x2 = (x2-1)>>1;
		nowlen += nowlen;
	}
	nowlen = 1;
	while(ox1 >= 1 && ox2 >= 1)
	{
		calclen(ox1,nowlen);
		calclen(ox2,nowlen);

		ox1 >>= 1, ox2 >>= 1;
		nowlen += nowlen;
	}
}

void decover(unsigned int x1, unsigned int x2)
{
	x1 += B_T, x2 += B_T;
	unsigned int ox1 = x1, ox2 = x2;
	while(x1 <= x2)
	{
		if (x1 % 2u == 1u)
		{
			if ((--addtree[x1]) == 0)
			{
				calclen(x1, 0);
			}
		}
		if (x2 % 2u == 0u)
		{
			if ((--addtree[x2]) == 0)
			{
				calclen(x2, 0);
			}
		}
		x1 = (x1+1)>>1;
		x2 = (x2-1)>>1;
	}
	int nowlen = 1;
	while(ox1 >= 1 && ox2 >= 1)
	{
		calclen(ox1,nowlen);
		calclen(ox2,nowlen);

		ox1 >>= 1, ox2 >>= 1;
		nowlen += nowlen;
	}
}

