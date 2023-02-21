// TelegrameTest.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "TelegramBuilder.h"
#include "TelegramScheme.h"
#include "SmartADODB.h"
#include <sstream>
#include <stdio.h>
#include <fstream>
#include <iostream>
#include "SimpleADO.h"

class _declspec(dllexport) DB : public SimpleADO 
{
public:
	DB::DB(void)
	{
		this->ConnectionString = L"";
	}
	DB::DB(wstring connstr)
	{
		this->ConnectionString = connstr;
	}
	DB::~DB(void)
	{

	}

	void DB::Query(string no)
	{
		_CommandPtr cmd = this->GetCommand();
		_bstr_t temp = no.c_str();

		this->AddParameter("work_date",DataTypeEnum::adVarChar,16,temp,cmd);
		
		cmd->CommandText = L"select * from defectentry where EntryCoilNo = ?";

		_RecordsetPtr ds = cmd->Execute(NULL, NULL, adCmdText);
		if(!ds->adoEOF)
	    {
		}
	}
protected:
	wstring DB::GetConnectionString()
	{
		wstring str = L"Provider=MSDAORA;User Id=baotousteel;Password=dbmaster;Data Source=ORACLE_HOME";
		return str;
	}
};

void testCommondata2()
{
	wstring name = L"D://project//ShareLib//bin//Lt.xml";
	TextTelegramBuilder* tb = new TextTelegramBuilder(name);


	wstring name2 = L"D:\\project\\BaotouSteel\\Data\\L3L2tel_def.xml";
	TextTelegramBuilder* Bulider = new TextTelegramBuilder(name2);
	string MID = "4505";
	string DataBlock;

	CommonData2 Data;
	Data.setString("COIL_NO", "0000000000000001");
	Data.setString("REPLAY_FLAG", "0");
	Data.setString("REMARK", "");

	Bulider->Encode(MID, DataBlock, Data);

	////对象-->电文D0031200YZD11012D0022000YZD22226D0053800YZD45060
	CommonData2 data2;
	data2.setString("COIL_NO","20130904G1111102");
	data2.setInt("Length",20);
	data2.setFloat("width",5.50);
	data2.setString("DateTime","20131224163948");

	
	CommonDataList values;
	CommonData2 V;
	V.setString("DefectType","D001");
	V.setString("DefectWidthPos","3800");
	V.setString("DefectIntensity","YZD4");
	V.setInt("DefectStart", 50);
	V.setInt("DefectEnd" , 91);	

	CommonData2 V1;
	V1.setString("DefectType","D002");
	V1.setString("DefectWidthPos","3800");
	V1.setString("DefectIntensity","YZD4");
	V1.setInt("DefectStart", 54);
	V1.setInt("DefectEnd" , 90);

	CommonData2 V2;
	V2.setString("DefectType","D002");
	V2.setString("DefectWidthPos","3800");
	V2.setString("DefectIntensity","YZD4");
	V2.setInt("DefectStart", 54);
	V2.setInt("DefectEnd" , 90);

	values.Add(V);
	values.Add(V1);
	values.Add(V2);

	data2.setVector("Defect", values);


	
	string msg;
	string M = "4501";
	tb->Encode(M, msg, data2);

	CommonData2 data3;
	tb->Decode(M,msg,data3);


	wstring s = data3.ToDebugString();

	data3.Clear();
	getchar();
}

void testDB()
{
	////测试数据库
	wstring connstring = L"Provider = MSDAORA;User Id=baotousteel;Password=dbmaster;Data Source=ORACLE_HOME";
	wstring scheme = L"D:\\project\\BaotouSteel\\Data\\ORM.XML";
	ADODB* ado = new ADODB(connstring, DbType::Oracle, scheme);

	CommonData2 PDI;
	PDI.setString("PRODUCT_PLAN_NO","1236547895");
	PDI.setString("ENTRY_COIL_NO","A013121315123654");
	PDI.setString("REPAIR_FLAG","2");
	PDI.setString("SG_SIGN","2102365456232145");
	PDI.setString("SG_NO","11111111");
	PDI.setFloat("ENTRY_MAT_THICK",12.69);
	PDI.setInt("ENTRY_MAT_WIDTH","78956");
	PDI.setInt("ENTRY_MAT_LEN","12303");
	PDI.setInt("ENTRY_MAT_WT","45632");
	PDI.setInt("ENTRY_MAT_INNER_DIA","231");
	PDI.setInt("ENTRY_MAT_OUTER_DIA","7895");
	PDI.setInt("ENTRY_SLEEVE_FLAG","1");
	PDI.setString("SLEEVE_CODE","1");
	PDI.setString("MAT_MIXED_FLAG","1");
	PDI.setString("MAT_MIXED_MODE","20");
	PDI.setString("EXIT_COIL_NO","1236547895963256");
	PDI.setInt("EXIT_COIL_WT","12365");
	PDI.setInt("EXIT_COIL_WT_MIN","12345");
	PDI.setInt("EXIT_COIL_WT_MAX","12345");
	PDI.setString("NEXT_PROCESS_NO","20");
	PDI.setString("ANNEALING_CODE","123659845632");
	PDI.setString("COATING_CODE","2546");
	PDI.setInt("COATING_THICK","13654");
	PDI.setInt("ALLOYCOMPENSATION","12365");
	PDI.setInt("EXIT_COIL_INNER_DIAMETER","777");
	PDI.setInt("EXIT_SLEEVE_FLAG","1");
    PDI.setString("EXIT_SLEEVE_CODE","1");
    
	PDI.Clear();

	int a = 1;
	//测试添加
	//1
	//int result = ado->Insert("pdi",PDI);//增加
	//2 判断存在
	//ado->Add("pdi","ENTRY_COIL_NO",PDI);
	
	//查询测试
	//1
	//list<CommonData2> list = ado->Select("pdi");//查询
	//2
	//list<CommonData2> list2 = ado->Select("pdi","ENTRY_COIL_NO = 'A013121315123654'");
	//3
	//CommonData2 con;
	//con.setString("ENTRY_COILNO","A013121315123654");
	//con.setString("ENTRY_COIL_NO","A013121315123654");
	//list<CommonData2> list2 = ado->Select("pdi",con);

	//测试更新
	//1
	//CommonData2 con2;
	//con2.setString("ENTRY_COIL_NO","A013121315123654");
	//ado->Update("pdi",PDI,con2);
	//2
	//_variant_t vv = "A013121315123654";
	//ado->Update("pdi",PDI,"ENTRY_COIL_NO",vv);

	//删除数据
	//1
	//_variant_t U = "A013121315123654";
	//ado->Delete("pdi","ENTRY_COIL_NO",U);
	//2
	//CommonData2 del;
	//del.setString("ENTRY_COIL_NO","2013121315123654");
	//ado->Delete("pdi",del);

	//exist?
	//_variant_t Z = "A013121315123654";
	//bool flag = ado->Exist("pdi","ENTRY_COIL_NO",Z);
	


	///复杂数据的DB操作测试
	CommonData2 data2;
	data2.setString("Entry_Coil_No","20130904G1111102");
	data2.setString("DefectNumber","03");

	
	CommonDataList values;
	CommonData2 V;
	V.setString("DefectType","D01");
	V.setString("DefectWidthPos","3");
	V.setString("DefectIntensity","Y");
	V.setInt("DefectStart", 50);
	V.setInt("DefectEnd" , 91);

	values.Add(V);
	data2.setVector("Defect", values);

	//保存
	CommonDataList& DF = data2.getVector("Defect");
	DF.ResetRow();
	while(DF.NextRow())
	{

		DF.setString("Entry_Coil_No",data2.getString("Entry_Coil_No"));
		DF.setString("DefectNumber",data2.getString("DefectNumber"));
		ado->Add("defectentry", "EntryCoilNo", DF.Get());
	}
}

void testSADO()
{
	DB* ado = new DB( L"Provider=OraOLEDB.Oracle;User Id=baotousteel;Password=dbmaster;Data Source=ORACLE_HOME");
	ado->Query("20130904G1111102");
}

void testCopy()
{
	CommonData2 data2;
	data2.setString("Entry_Coil_No","20130904G1111102");
	data2.setString("DefectNumber","03");

	
	CommonDataList values;
	SmartPointer<CommonData2> V = new CommonData2();
	V->setString("DefectType","D01");
	V->setString("DefectWidthPos","3");
	V->setString("DefectIntensity","Y");
	V->setInt("DefectStart", 50);
	V->setInt("DefectEnd" , 91);

	values.Add(V);
	data2.setVector("Defect", values);

	CommonData2 dd = data2;
	string str = dd.getString("Entry_Coil_No");
	string str2 = dd.getString("DefectNumber");
	CommonDataList& DF = data2.getVector("Defect");
	DF.ResetRow();
	while(DF.NextRow())
	{
		string s1 = DF.getString("DefectType");
		string s2 = DF.getString("DefectWidthPos");
		string s3 = DF.getString("DefectIntensity");
		string s4 = DF.getString("DefectStart");
		string s5 = DF.getString("DefectEnd");
	}

	getchar();
}

void testBinary()
{
	wstring name = L"D://project//ShareLib//bin//L1tel_def.xml";
	BinaryTelegramBuilder Bulider(name);

	short MID = 1001;
	string DataBlock;

	CommonData2 Data; 
	Data.setInt("ESK",0x01010101);

	Bulider.Encode(MID, DataBlock, Data);
	getchar();
}

void testdelimiter()
{

	wstring name2 = L"D:\\project\\BaotouSteel\\Data\\L3L2tel_def.xml";
	DelimiterTelegramBuilder* Bulider = new DelimiterTelegramBuilder(name2, "/");
	string MID = "4505";
	string DataBlock;

	CommonData2 Data;
	Data.setString("COIL_NO", "0000000000000001");
	Data.setString("REPLAY_FLAG", "0");
	Data.setString("REMARK", "");

	Bulider->Encode(MID, DataBlock, Data);

	CommonData2 Data2;
	Bulider->Decode(MID,DataBlock,Data2);

	getchar();
}

int _tmain(int argc, _TCHAR* argv[])
{
	CoInitialize(NULL);
	try
	{
		CommonData2 d;
		d.setFloat("A",12.5);
		d.setFloat("A",13);
		double t = d.getFloat("A");

		d.setInt("B",12);
		d.setInt("B",13);
		int m = d.getInt("B");


		d.setString("C","12.5");
		d.setString("C","13");
		string n = d.getString("C");

		testdelimiter();
		testBinary();
		testCommondata2();
		testDB();
		testSADO();
		testCopy();
		getchar();
	}
	catch(Exception& e)
	{
		wcout<<e.GetDescription()<<endl;
	}
	catch(_com_error& e)
	{
		wcout<<e.Description()<<endl;
	}
	catch(exception& e)
	{
		cout<<e.what()<<endl;
	}
	catch(...)
	{
		cout<<"";
	}
	CoUninitialize();
	return 0;
}

