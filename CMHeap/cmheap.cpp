#include <stdio.h>
#include <iostream>
#include <windows.h>
#include <stdlib.h>
#include <unordered_map>
#include <vector>
#include <time.h>
//#include <map>
#include "CMHeap.h"
using namespace std;
#define START_FILE_NO 1 
#define END_FILE_NO 5 
struct FIVE_TUPLE{	char key[10];	};
typedef vector<FIVE_TUPLE> TRACE;
TRACE traces[END_FILE_NO - START_FILE_NO + 1];
void ReadInTraces(const char *trace_prefix)
{
	for(int datafileCnt = START_FILE_NO; datafileCnt <= END_FILE_NO; ++datafileCnt)
	{
		char datafileName[100];
		sprintf(datafileName, "%s%d.dat", trace_prefix, datafileCnt - 1);
		FILE *fin = fopen(datafileName, "rb");
		FIVE_TUPLE tmp_five_tuple;
		traces[datafileCnt - 1].clear();
		while(fread(&tmp_five_tuple, 1, 10, fin) == 10)
		{
			traces[datafileCnt - 1].push_back(tmp_five_tuple);
		}
		fclose(fin);
		printf("Successfully read in %s, %ld packets\n", datafileName, traces[datafileCnt - 1].size());
	}
	printf("\n");
}
int main()
{
	long t1=GetTickCount(); 
	ReadInTraces("data/");
#define HEAP_CAPACITY (150 * 1024 / 64)
	CMHeap<4, HEAP_CAPACITY> *cmheap = NULL;
	for(int datafileCnt = START_FILE_NO; datafileCnt <= END_FILE_NO; ++datafileCnt)
	{
		unordered_map<string, int> Real_Freq;
		cmheap = new CMHeap<4, HEAP_CAPACITY>(10 * 1024);
		int packet_cnt = (int)traces[datafileCnt - 1].size();
		for(int i = 0; i < packet_cnt; ++i)
		{
			long long num = 0;
			for(int j=0;j<10;j++)
				num = num*10 + traces[datafileCnt - 1][i].key[j]-'0';
			uint8_t tmp[4] = {(num>>24)%256,(num>>16)%256,(num>>8)%256,(num%256)};
			cmheap->insert((uint8_t*)tmp);
			//cmheap->insert((uint8_t*)(traces[datafileCnt - 1][i].key));
			string str((const char*)(traces[datafileCnt - 1][i].key), 4);
			Real_Freq[str]++;
		}
	//cout<<(uint8_t*)(traces[datafileCnt - 1][1].key)<<endl;
	//printf("capacity = %d , w = %d , d = 3\n",);
#define HEAVY_HITTER_THRESHOLD(total_packet) (total_packet * 1 / 1000)
		vector< pair<string, uint32_t> > heavy_hitters;
		cmheap->get_heavy_hitters(HEAVY_HITTER_THRESHOLD(packet_cnt), heavy_hitters);
		printf("%d.dat: ", datafileCnt - 1);
		printf("heavy hitters: <srcIP, count>, threshold=%d\n", HEAVY_HITTER_THRESHOLD(packet_cnt));
		printf("The number of heavy hitter is %d\n",(int)heavy_hitters.size());
	/*	map<string, int> ::reverse_iterator iter;
		int per_flow_num=0;
		for(iter = Real_Freq.rbegin() ; iter != Real_Freq.rend() ;iter++)
		{
			if(iter->second > 100) per_flow_num++;
		}
		printf("Ture hitter = %d\n",per_flow_num);*/
		for(int i = 0, j = 0; i < (int)heavy_hitters.size(); ++i)
		{
			uint32_t srcIP;
			memcpy(&srcIP, heavy_hitters[i].first.c_str(), 4);
			printf("<%.8x, %d>", srcIP, (int)heavy_hitters[i].second);
			if(++j % 5 == 0)
				printf("\n");
			else printf("\t");
		}
		printf("\n");
	
		 
		
		delete cmheap;
		Real_Freq.clear();
	}
	long t2=GetTickCount();
	cout<<"Time: "<<(t2-t1)<<endl;
}	
