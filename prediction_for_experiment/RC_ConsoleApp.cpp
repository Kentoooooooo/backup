// RC_ConsoleApp.cpp : アプリケーションのエントリ ポイントを定義します。
//

#include "stdafx.h"
#include "DLLMain.h"
#include <iostream>
#include "Windows.h"
#include <iomanip>

void dump(unsigned char *p, int n);

int main()
{
	InternalProcess::OSCParam oscp;
	InternalProcess::AWGParam awgp;
	char m[256], w[1024];
	BOOL tmp;
	
	oscp.DebugFlag = 0;
	oscp.Timeout = 5000;
	oscp.RecordDataLength = 10000;
	
	awgp.Timeout = 0;
	
	InternalProcess::Oscilloscope osc(&oscp, m, w);
	tmp = osc.InitOscillo();
	std::cout << "OSC init:" << tmp << std::endl;
	osc.SetSaveFileFormat("INTERNAL");

	//std::cout << oscp.HorizontalScale << std::endl;
	
	InternalProcess::AWG awgen(&awgp, m);
	tmp = awgen.InitAWG();
	std::cout << "AWG init:" << tmp << std::endl;

	float *wfm;
	char in_fname[128];
	InternalProcess::readwfm rwfm;

	rwfm.initreadwfm();
	sprintf_s(in_fname, 128, "./mask.txt");
	rwfm.makemask(0, in_fname);
	sprintf_s(in_fname, 128, "./santafe.txt");
	wfm = rwfm.out_wfm(in_fname);
	std::cout << "Read wfm" << std::endl;

	int mode_flag = 0;
	int output_wfm_num = 10;
	int memory_load_flag = 0;

	if (memory_load_flag == 1)
	{
		awgen.ClearMemory(1);
		awgen.ChgFunction(1, "ARB");
		tmp = awgen.TransportWFMdata(1, wfm, rwfm.wfm_num);
		std::cout << "Transport:" << tmp << std::endl;

		awgen.ClearMemory(2);
		awgen.ChgFunction(2, "ARB");
		tmp = awgen.TransportWFMdata(2, wfm, rwfm.wfm_num);
		std::cout << "Transport:" << tmp << std::endl;
	}

	awgen.SetParam();

	awgen.Burst(1, "ON");
	awgen.Burst(2, "ON");
	awgen.SYNC(2);
	awgen.ChannelOutSwitch(1);
	awgen.ChannelOutSwitch(2);

	time_t t = time(nullptr);
	struct tm lt;
	localtime_s(&lt, &t);
	
	for (int i = 0; i < output_wfm_num; i++)
	{
		if (mode_flag == 1)
		{
			osc.Acq_Mode_Chg(1, "ON");
			awgen.GenTRG();
			Sleep(2000);
			t = time(nullptr);
			localtime_s(&lt, &t);
			sprintf_s(in_fname, 128, "C:\\Documents and Settings\\OEM1\\Desktop\\temp\\out_%02d%02d%02d%02d%02d%02d_CH1.wfm\0", lt.tm_year - 100, lt.tm_mon + 1, lt.tm_mday, lt.tm_hour, lt.tm_min, lt.tm_sec);
			osc.SaveWfm(1, in_fname);
			Sleep(500);
			sprintf_s(in_fname, 128, "C:\\Documents and Settings\\OEM1\\Desktop\\temp\\out_%02d%02d%02d%02d%02d%02d_CH2.wfm\0", lt.tm_year - 100, lt.tm_mon + 1, lt.tm_mday, lt.tm_hour, lt.tm_min, lt.tm_sec);
			osc.SaveWfm(2, in_fname);
		}
		else
		{
			awgen.GenTRG();
			Sleep(1000);
		}
	}

	delete rwfm.mask;
	delete wfm;

	/*
	FILE *fp_out;
	fopen_s(&fp_out, "test.txt", "w");
	for (int i = 0; i < rwfm.coch_t_length; i++) {
		for (int j = 0; j < rwfm.node_num; j++) fprintf_s(fp_out, "%e ", wfm_db[i * rwfm.node_num * rwfm.node_interval + j * rwfm.node_interval]);
		fprintf_s(fp_out, "\n");
	}
	fclose(fp_out);
	fopen_s(&fp_out, "mask.txt", "w");
	for (int i = 0; i < 86; i++) {
		for (int j = 0; j < rwfm.node_num; j++) fprintf_s(fp_out, "%e ", rwfm.mask[i * rwfm.node_num + j]);
		fprintf_s(fp_out, "\n");
	}
	fclose(fp_out);
	*/
	

	//tmp = osc.GetWave();
	//std::cout << tmp << std::endl;
	
	osc.TermOscillo();

	awgen.ChannelOutSwitch(1);
	awgen.ChannelOutSwitch(2);
	awgen.Burst(1, "OFF");
	awgen.Burst(2, "OFF");
	awgen.TermAWG();

	system("pause");

    return 0;
}

void dump(unsigned char *p, int n) {
	int i;

	for (i = n - 1; i >= 0; i--) printf("%02x ", p[i]);
	printf("\n");
}
