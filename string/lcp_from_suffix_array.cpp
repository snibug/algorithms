/*
Toru Kasai, Gunho Lee, Hiroki Arimura, Setsuo Arikawa, and Kunsoo Park
Linear-Time Longest-Common-Prefix Computation in Suffix Arrays and Its Applications

여기에 suffix array 상에서 인접한 suffix들끼리의 lcp를 선형시간에 계산하는 알고리즘이 나와있어요. 아마 링크가 죽지 않았다면 http://www.springerlink.com/content/n56u1t2k65fhr7fe/ 에서 확인할 수 있을거예요.

*/

struct LCPFromSuffixArray
{
	int n; // data length
	int data[MAX_N]; // data.
	int suffixArray[MAX_N]; // input suffix array index.

	int rank[MAX_N]; // temporary

	int height[MAX_N-1]; // output lcp array

	void GetHeight()
	{
		for(int i = 0; i < n; i ++) rank[suffixArray[i]] = i;
		int h = 0;
		for(int i = 0; i < n; i ++) {
			if(rank[i] == 0) continue;
			int j = suffixArray[rank[i]-1];
			while(data[i+h] == data[j+h]) h++;
			height[rank[i]-1] = h;
			if(h > 0) h--;
		}
	}
};
