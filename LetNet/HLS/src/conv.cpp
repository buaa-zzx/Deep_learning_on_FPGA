#include "conv.h"
#include "math.h"
//bram 280  dsp 220
static float Bias[120] = { 0 };
static float IN1[32][32] = { 0 };//2 bram

static float IN35[16][14][14] = { 0 };//16 bram
static float POOL_24[16][14][14];//16 bram
static float OUT13[16][28][28] = { 0 };//12 bram

static float OUT567[120] = { 0 };
static float IN67[120] = { 0 };

#define parrell_num_3layer 4  //定义一个并行度 方便后面根据资源数目修改
static float W1[6][5][5] = { 0 };//第一层单独占6个bram
static float W35[parrell_num_3layer][8][5][5] = { 0 };//3层和5层 8输出并行 8输入并行 占64个bram  128dsp

static float W6[84][120] = { 0 };//第6层全连接 2输出并行 8输入并行 32dsp
static float W7[10][84] = { 0 };//第7层全连接 10输出并行 2输入并行 40dsp
float REG_float(float in)
{
#pragma HLS INLINE off
#pragma HLS INTERFACE register port=return
	return in;
}
int32_t REG(int32_t in)
{
#pragma HLS INLINE off
#pragma HLS INTERFACE register port=return
	int32_t ans;
	ans = in;
	return ans;
}
void top_fun(float* In_DRAM, float* W_DRAM, float* Out_DRAM, float* Bias_DRAM, int8_t layer)
{
	/*************AXI接口定义**************/
#pragma HLS INTERFACE m_axi depth=32 port=In_DRAM bundle=Data
#pragma HLS INTERFACE m_axi depth=32 port=W_DRAM  bundle=Data
#pragma HLS INTERFACE m_axi depth=32 port=Out_DRAM  bundle=Data
#pragma HLS INTERFACE m_axi depth=32 port=Bias_DRAM  bundle=Data
	/*************s_axi相关定义**************/
#pragma HLS INTERFACE s_axilite port=In_DRAM bundle=Ctrl_bus
#pragma HLS INTERFACE s_axilite port=W_DRAM bundle=Ctrl_bus
#pragma HLS INTERFACE s_axilite port=Out_DRAM bundle=Ctrl_bus
#pragma HLS INTERFACE s_axilite port=Bias_DRAM bundle=Ctrl_bus

#pragma HLS INTERFACE s_axilite port=layer bundle=Ctrl_bus
#pragma HLS INTERFACE s_axilite port=return bundle=Ctrl_bus
	/*************数组分组情况**************/
#pragma HLS ARRAY_PARTITION variable=IN35 complete dim=1
#pragma HLS ARRAY_PARTITION variable=POOL_24 complete dim=1
#pragma HLS ARRAY_PARTITION variable=OUT13 complete dim=1
#pragma HLS ARRAY_PARTITION variable=W1 complete dim=1

#pragma HLS ARRAY_PARTITION variable=W35 complete dim=1
#pragma HLS ARRAY_PARTITION variable=W35 complete dim=2

#pragma HLS ARRAY_PARTITION variable=OUT567 complete dim=0
//#pragma HLS ARRAY_PARTITION variable=IN67 complete dim=0
//
//#pragma HLS ARRAY_PARTITION variable=W6 complete dim=0
//#pragma HLS ARRAY_PARTITION variable=W7 complete dim=0


	if (layer == 1)
	{
		memcpy((void*)Bias, (const void*)(Bias_DRAM), sizeof(float) * 6);
		memcpy((void*)IN1, (const void*)(In_DRAM), sizeof(float) * 32 * 32);
		memcpy((void*)W1, (const void*)(W_DRAM), sizeof(float) * 6 * 5 * 5);

		for (int outc = 0; outc < 28; outc++)
		{
		clear1:
			for (int outr = 0; outr < 28; outr++)
			{
#pragma HLS PIPELINE
				for (int outchl = 0; outchl < 6; outchl++)
				{
					OUT13[outchl][outr][outc] = 0;
				}
			}
		}

		for (int kr = 0; kr < 5; kr++)
		{
			for (int kc = 0; kc < 5; kc++)
			{
			row1:
				for (int r = 0; r < 28; r++)
				{
				cow1:
					for (int c = 0; c < 28; c++)
					{
#pragma HLS PIPELINE
						for (int cho = 0; cho < 6; cho++)
						{
							OUT13[cho][r][c] += IN1[r + kr][c + kc] * W1[cho][kr][kc];
						}
					}
				}
			}
		}
	active_r1:
		for (int r = 0; r < 28; r++)
		{
		active_c1:
			for (int c = 0; c < 28; c++)
			{
#pragma HLS PIPELINE
				for (int cho = 0; cho < 6; cho++)
				{
					OUT13[cho][r][c] = (OUT13[cho][r][c] + Bias[cho]) > 0 ? (OUT13[cho][r][c] + Bias[cho]) : 0;
				}
			}
		}
		/////////////////////////池化
	pool_r1:
		for (int r = 0; r < 14; r++)
		{
		pool_c1:
			for (int c = 0; c < 14; c++)
			{
#pragma HLS PIPELINE
				for (int cho = 0; cho < 6; cho++)
				{
					float temp1, temp2, temp3;
					temp1 = REG_float(OUT13[cho][r * 2 + 0][c * 2 + 0] + OUT13[cho][r * 2 + 0][c * 2 + 1]);
					temp2 = REG_float(OUT13[cho][r * 2 + 1][c * 2 + 0] + OUT13[cho][r * 2 + 1][c * 2 + 1]);
					temp3 = REG_float((temp1 + temp2) * 0.25);

					POOL_24[cho][r][c] = temp3;

				}
			}
		}
		memcpy((void*)(Out_DRAM), (const void*)POOL_24, sizeof(float) * 6 * 14 * 14);
	}
	else if (layer == 2)
	{
		memcpy((void*)Bias, (const void*)(Bias_DRAM), sizeof(float) * 16);
		memcpy((void*)IN35, (const void*)(In_DRAM), sizeof(float) * 6 * 14 * 14);



		for (int outc = 0; outc < 28; outc++)
		{
		clear2:
			for (int outr = 0; outr < 28; outr++)
			{
#pragma HLS PIPELINE
				for (int outchl = 0; outchl < 16; outchl++)
				{
					OUT13[outchl][outr][outc] = 0;
				}
			}
		}

		for (int chl_o = 0; chl_o < 16; chl_o += parrell_num_3layer)
		{
			for (int chl_in = 0; chl_in < 8; chl_in += 8)
			{
				for (int copy_t = 0; copy_t < parrell_num_3layer; copy_t++)
					memcpy((void*)(&W35[0][0][0][0] + copy_t * 8 * 5 * 5), (const void*)(W_DRAM + copy_t * 6 * 5 * 5 + chl_o * 6 * 5 * 5), sizeof(float) * 6 * 5 * 5);

				for (int kr = 0; kr < 5; kr++)
				{
					for (int kc = 0; kc < 5; kc++)
					{
					row2:
						for (int r = 0; r < 10; r++)
						{
						col2:
							for (int c = 0; c < 10; c++)
							{
#pragma HLS PIPELINE
								for (int chi = 0; chi < 6; chi++)
								{
									for (int cho = 0; cho < parrell_num_3layer; cho++)
									{
										OUT13[cho + chl_o][r][c] += IN35[chi][r + kr][c + kc] * W35[cho][chi][kr][kc];
									}
								}

							}
						}
					}
				}
			}
		}

	active_r2:
		for (int r = 0; r < 10; r++)
		{
		active_c2:
			for (int c = 0; c < 10; c++)
			{
#pragma HLS PIPELINE
				for (int cho = 0; cho < 16; cho++)
				{
					OUT13[cho][r][c] = (OUT13[cho][r][c] + Bias[cho]) > 0 ? (OUT13[cho][r][c] + Bias[cho]) : 0;
				}
			}
		}
		/////////////////////////池化
	pool_r2:
		for (int r = 0; r < 5; r++)
		{
		pool_c2:
			for (int c = 0; c < 5; c++)
			{
#pragma HLS PIPELINE
				for (int cho = 0; cho < 16; cho++)
				{

					float temp1, temp2, temp3;
					temp1 = REG_float(OUT13[cho][r * 2 + 0][c * 2 + 0] + OUT13[cho][r * 2 + 0][c * 2 + 1]);
					temp2 = REG_float(OUT13[cho][r * 2 + 1][c * 2 + 0] + OUT13[cho][r * 2 + 1][c * 2 + 1]);
					temp3 = REG_float((temp1 + temp2) * 0.25);

					POOL_24[cho][r][c] = temp3;
				}
			}
		}
		memcpy((void*)(Out_DRAM), (const void*)POOL_24, sizeof(float) * 16 * 14 * 14);
	}

	else if (layer == 3)
	{
		memcpy((void*)Bias, (const void*)(Bias_DRAM), sizeof(float) * 120);
		memcpy((void*)IN35, (const void*)(In_DRAM), sizeof(float) * 16 * 14 * 14);


	clear3:
		for (int outchl = 0; outchl < 120; outchl++)
		{
#pragma HLS PIPELINE
			OUT567[outchl] = 0;
		}


		for (int chl_o = 0; chl_o < 120; chl_o += parrell_num_3layer)
		{
			for (int chl_in = 0; chl_in < 16; chl_in += 8)
			{
				for (int copy_t = 0; copy_t < parrell_num_3layer; copy_t++)
				{
					int address1, address2, address3;
					address1 = REG(copy_t * 16 * 5 * 5);
					address2 = REG(chl_in * 5 * 5);
					address3 = REG(chl_o * 16 * 5 * 5);

					memcpy((void*)(&W35[0][0][0][0] + copy_t * 8 * 5 * 5), (const void*)(W_DRAM + address1 + address2 + address3), sizeof(float) * 8 * 5 * 5);
				}

			kr3:
				for (int kr = 0; kr < 5; kr++)
				{
				kc3:
					for (int kc = 0; kc < 5; kc++)
					{
						for (int chi = 0; chi < 8; chi++)
						{
#pragma HLS PIPELINE
							for (int cho = 0; cho < parrell_num_3layer; cho++)
							{
								OUT567[cho + chl_o] += IN35[chi + chl_in][kr][kc] * W35[cho][chi][kr][kc];
							}
						}
					}
				}
			}
		}
	active3:
		for (int cho = 0; cho < 120; cho++)
		{
#pragma HLS PIPELINE
			float temp;
			temp = OUT567[cho] + Bias[cho];
			OUT567[cho] = temp > 0 ? temp : 0;
		}
		memcpy((void*)(Out_DRAM), (const void*)OUT567, sizeof(float) * 120);
	}

	else if (layer == 4)
	{
		memcpy((void*)Bias, (const void*)(Bias_DRAM), sizeof(float) * 84);
		memcpy((void*)IN67, (const void*)(In_DRAM), sizeof(float) * 120);
		memcpy((void*)W6, (const void*)(W_DRAM), sizeof(float) * 120 * 84);

		for (int outchl = 0; outchl < 120; outchl++)
		{
#pragma HLS PIPELINE
			OUT567[outchl] = 0;
		}

		fc6_cho:
		for (int cho = 0; cho < 84; cho++)
		{
			fc6_chi:
			for (int chi = 0; chi < 120; chi++)
			{
//#pragma HLS UNROLL factor=10
				OUT567[cho] += IN67[chi] * W6[cho][chi];
			}
		}

		fc6_active:
		for (int cho = 0; cho < 84; cho++)
		{
#pragma HLS PIPELINE
			OUT567[cho] = (OUT567[cho] + Bias[cho]) > 0 ? (OUT567[cho] + Bias[cho]) : 0;
		}

		memcpy((void*)(Out_DRAM), (const void*)OUT567, sizeof(float) * 84);
	}
	else if (layer == 5)
	{
		memcpy((void*)Bias, (const void*)(Bias_DRAM), sizeof(float) * 10);
		memcpy((void*)IN67, (const void*)(In_DRAM), sizeof(float) * 84);
		memcpy((void*)W7, (const void*)(W_DRAM), sizeof(float) * 10 * 84);

	clear5:
		for (int outchl = 0; outchl < 120; outchl++)
		{
#pragma HLS PIPELINE
			OUT567[outchl] = 0;
		}
		fc7_chi:
		for (int chi = 0; chi < 84; chi++)
		{
			fc7_cho:
			for (int cho = 0; cho < 10; cho++)
			{
//#pragma HLS UNROLL factor=10
				OUT567[cho] += IN67[chi] * W7[cho][chi];
			}
		}

		memcpy((void*)(Out_DRAM), (const void*)OUT567, sizeof(float) * 10);
	}



}
