class operationData
{
   int MARSno;
   int dayNo;

public:
	operationData(){};
   ~operationData(){};
   int GetMARSno() {return MARSno;}
   int GetdayNo() {return dayNo;}
   void SetMARSno(int aVal){MARSno=aVal;}
   void SetdayNo(int aVal){dayNo=aVal;}
};
