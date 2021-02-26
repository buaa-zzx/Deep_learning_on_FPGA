#include <iostream>  
#include <iomanip>
#include <fstream>  
#include <sstream>  
#include "stdlib.h"
#include <cstdlib>
#include<ctime>
#include <opencv2/core/core.hpp>  
#include <opencv2/highgui/highgui.hpp>  
#include <opencv2/imgproc/imgproc.hpp>
#include "conv.h"
using namespace std;
using namespace cv;
float input[32][32] = {0};

float POOL24_DRAM[16][14][14] = { 0 };

float C5_DRAM[120] = { 0 };
float C6_DRAM[84] = { 0 };
float C7_DRAM[10] = { 0 };

float W_CONV1[6][1][5][5] = { 0 };
float W_CONV3[16][6][5][5] = { 0 };
float W_CONV5[120][16][5][5] = { 0 };

float b_conv1[6] = { 0 };
float b_conv3[16] = { 0 };
float b_conv5[120] = { 0 };

float WFC6[120 * 84+2000] = { 0 };
float WFC7[84 * 10+2000] = { 0 };

float b_fc6[84] = { 0 };
float b_fc7[10] = { 0 };

void read_parameters();
void print2files(const char* filename, float* para_array, int para_num);
void print_int8_2files(const char* filename, int8_t* para_array, int para_num);
void read_bin_float(const char* filename, float* para_array, int para_num);
void read_bin_int(const char* filename, int* para_array, int para_num);

float pic_in[8952 + 1000][32 * 32] = { 0 };//输入
int8_t labels[8952];//标签，用于测试模型参数准确度


int main()
{
	read_parameters();

	int result[8952] = { 0 };
	int err_cnt = 0;

		char str2read_func[200];//
		sprintf_s(str2read_func, "E:\\lenet\\dataset\\vali_data");
		read_bin_float(str2read_func, &pic_in[0][0], 8952*32*32);



		int err_location[50] = { 0 };
		for (int i = 0; i < 8952; i++)
		{
			top_fun(&pic_in[i][0], &W_CONV1[0][0][0][0], &POOL24_DRAM[0][0][0], b_conv1,1);
			top_fun(&POOL24_DRAM[0][0][0], &W_CONV3[0][0][0][0], &POOL24_DRAM[0][0][0], b_conv3,2);
			top_fun(&POOL24_DRAM[0][0][0], &W_CONV5[0][0][0][0], &C5_DRAM[0], b_conv5,3);
			top_fun(C5_DRAM, WFC6, C6_DRAM, b_fc6,4);
			top_fun(C6_DRAM, WFC7, C7_DRAM, b_fc7,5);
			int max_arg = 0;
			float max = -100000;
			for (int i = 0; i < 10; i++)
			{
				if (C7_DRAM[i] > max)
				{
					max_arg = i;
					max = C7_DRAM[i];
				}
			}
			result[i] = max_arg;
			if (result[i] != labels[i])
			{
				err_location[err_cnt] = i;
				err_cnt++;
			}
			if(i%1000==0)
				cout << i <<"/8952"<< endl;
		}
	cout << "err  = " << err_cnt << endl;
	for(int i=0;i<50;i++)
			cout << " " << err_location[i] ;
	cout << "Recognition rate is " << (  1-   float(err_cnt) / 8952.0  )   * 100 << '%' << endl;
	
	while (1);
	return 0;

}


void read_mdl(const char* filename, float* para_array)
{

	FILE* fp;
	fp = fopen(filename, "r+");
	int i = 0;
	int b;
	while (!feof(fp))
	{
		b = fscanf(fp, "%f\r\n", para_array + i);
		i++;
	}
	fclose(fp);
}
void read_label(const char* filename, int8_t* para_array)
{

	FILE* fp;
	fp = fopen(filename, "r+");
	int i = 0;
	int b;
	while (!feof(fp))
	{
		b = fscanf(fp, "%d\r\n", para_array + i);
		i++;
	}
	fclose(fp);
}


void print2files(const char* filename, float* para_array, int para_num)
{
	//下面这个是输出二进制类型 采用fwrite

	FILE* fp;//conv5_pic_out[256][22][22]; //
	fp = fopen(filename, "wb");
	fwrite(para_array, 4* para_num, 1, fp);

	fclose(fp);
}
void print_int8_2files(const char* filename, int8_t* para_array, int para_num)
{
	//下面这个是输出二进制类型 采用fwrite

	FILE* fp;//conv5_pic_out[256][22][22]; //
	fp = fopen(filename, "wb");
	fwrite(para_array, 1, para_num, fp);

	fclose(fp);
}
void read_bin_float(const char* filename, float* para_array, int para_num)
{
	FILE* fp;
	fp = fopen(filename, "rb");
	fread(para_array,  4, para_num , fp);
	fclose(fp);
}
void read_bin_int(const char* filename, int8_t* para_array, int para_num)
{
	FILE* fp;
	fp = fopen(filename, "rb");
	fread(para_array, 1, para_num, fp);
	fclose(fp);
}
void read_parameters()
{

#ifdef is_convert_2bin
	///////////读取weight
	read_mdl("..\\..\\python_code\\parameter\\conv1.0.weight.txt", &W_CONV1[0][0][0][0]);
	read_mdl("..\\..\\python_code\\parameter\\conv2.0.weight.txt", &W_CONV3[0][0][0][0]);
	read_mdl("..\\..\\python_code\\parameter\\conv3.0.weight.txt", &W_CONV5[0][0][0][0]);

	read_mdl("..\\..\\python_code\\parameter\\fc2.0.weight.txt", &WFC6[0]);
	read_mdl("..\\..\\python_code\\parameter\\fc3.weight.txt", &WFC7[0]);

	//读取bias
	read_mdl("..\\..\\python_code\\parameter\\conv1.0.bias.txt", &b_conv1[0]);
	read_mdl("..\\..\\python_code\\parameter\\conv2.0.bias.txt", &b_conv3[0]);
	read_mdl("..\\..\\python_code\\parameter\\conv3.0.bias.txt", &b_conv5[0]);

	read_mdl("..\\..\\python_code\\parameter\\fc2.0.bias.txt", &b_fc6[0]);
	read_mdl("..\\..\\python_code\\parameter\\fc3.bias.txt", &b_fc7[0]);

	read_label("..\\..\\dataset\\test.txt", &labels[0]);



	print2files("..\\..\\python_code\\parameter\\conv1_w", &W_CONV1[0][0][0][0], 6 * 5 * 5);
	print2files("..\\..\\python_code\\parameter\\conv3_w", &W_CONV3[0][0][0][0], 16*6 * 5 * 5);
	print2files("..\\..\\python_code\\parameter\\conv5_w", &W_CONV5[0][0][0][0], 120 * 16 * 5 * 5);

	print2files("..\\..\\python_code\\parameter\\fc6_w", &WFC6[0], 10080);
	print2files("..\\..\\python_code\\parameter\\fc7_w", &WFC7[0], 840);

	print2files("..\\..\\python_code\\parameter\\b_conv1", &b_conv1[0], 6);
	print2files("..\\..\\python_code\\parameter\\b_conv3", &b_conv3[0], 16);
	print2files("..\\..\\python_code\\parameter\\b_conv5", &b_conv5[0], 120);
	print2files("..\\..\\python_code\\parameter\\b_fc6", &b_fc6[0], 84);
	print2files("..\\..\\python_code\\parameter\\b_fc7", &b_fc7[0], 10);
	print_int8_2files("..\\..\\python_code\\parameter\\test_label", &labels[0], 8952);
#else

	read_bin_float("E:\\lenet\\python_code\\parameter\\conv1_w", &W_CONV1[0][0][0][0], 6 * 5 * 5);
	read_bin_float("E:\\lenet\\python_code\\parameter\\conv3_w", &W_CONV3[0][0][0][0], 16 * 6 * 5 * 5);
	read_bin_float("E:\\lenet\\python_code\\parameter\\conv5_w", &W_CONV5[0][0][0][0], 120 * 16 * 5 * 5);

	read_bin_float("E:\\lenet\\python_code\\parameter\\fc6_w", &WFC6[0], 10080);
	read_bin_float("E:\\lenet\\python_code\\parameter\\fc7_w", &WFC7[0], 840);

	read_bin_float(".E:\\lenet\\python_code\\parameter\\b_conv1", &b_conv1[0], 6);
	read_bin_float("E:\\lenet\\python_code\\parameter\\b_conv3", &b_conv3[0], 16);
	read_bin_float("E:\\lenet\\python_code\\parameter\\b_conv5", &b_conv5[0], 120);
	read_bin_float("E:\\lenet\\python_code\\parameter\\b_fc6", &b_fc6[0], 84);
	read_bin_float("E:\\lenet\\python_code\\parameter\\b_fc7", &b_fc7[0], 10);

	read_bin_int("E:\\lenet\\python_code\\parameter\\test_label", &labels[0], 8952);



#endif

}

