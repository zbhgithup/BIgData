#define _CRT_SECURE_NO_WARNINGS

#include "BigData.h"

#include <algorithm>


const INT64 UNINT = 0xcccccccccccccccc;
const INT64 MaxValue = 9223372036854775807;
const INT64 MinValue = -9223372036854775808;
BigData::BigData(INT64 value = UNINT)
:_value(value)
{     //���ַŵ��ַ�����
	char symbol = '+';	
	if(value < 0)
	{
		symbol = '-';
		value = 0 - value;
	}
	_strData.append(1,symbol);
	if(0 == value)
		_strData.append(1,value + '0');
	while(value > 0)
	{
		_strData.append(1,value%10+'0');
		value = value/10;
	}
        reverse(_strData.begin()+1,_strData.end());
}


BigData::BigData(const std::string& strData)
	:_value(0)
    ,_strData("+0")
{
	if(_strData.empty())          //���˵��ַ���Ϊ�յ�����           
		return;
	char *pData  = (char*)strData.c_str();//��stringת��Ϊconst char*

	while(isspace(*pData))           //���˵��ַ���ǰ��Ϊ�ո� "     1234"
		pData++;
	if('\0' == *pData)                 //���˵�ȫ�ǿո���          ����
		return;
	char symbol = *pData;
	if('+' == *pData ||'-' == *pData) //���˵�ǰ���з��ŵ�
		pData++;
	else if(*pData >='0'&&*pData<= '9')  //ǰ��û�з����ж��Ƿ��ǺϷ�������
		symbol = '+';
	else 
		return;
	while('0' == *pData)      //���˵�ǰ��ȫΪ0������
		pData++;
	if('\0' == *pData)         //ȫ��Ϊ0
		return;
	_strData.resize(strlen(pData)+1);
	_strData[0] = symbol;
	size_t count = 1;
	while(*pData >= '0'&& *pData <= '9')
	{
		_value = _value*10+*pData-'0';   //���ַ���������д��_value��
										//value�е����ݲ��ɿ������ܻ�����
		_strData[count++] = *pData;
		pData++;
	}
	if(symbol == '-')
		_value = 0 - _value;
	_strData.resize(count);   //���ռ仹ԭ  �磺123456abc789��
							  //ԭ��������13���ռ�����ֻ��Ҫ7���ռ�
}

BigData BigData::operator+(const BigData& b)
{
	if(!IsINT64Overflow()&&!b.IsINT64Overflow())
	{
		if(_strData[0] != b._strData[0])
			return BigData(_value+b._value);
		else if((_strData[0] == '+' && MaxValue - _value >=b._value)||
			     (_strData[0] == '-' && MinValue - _value <= b._value))
		         return BigData(_value + b._value);
	}
	//������һ��������������Ч���ݷ�Χ�ڵ�����
	if(_strData[0] == b._strData[0])  //����ͬ��
		return BigData(Add(_strData, b._strData));
	//������Ȼ����������                           
	if('+' == _strData[0])//�����Ӹ��� ����Ϊ����5+��-3���൱��5-3  
						//             ����Ϊ����5+��-7���൱��5-7  
	{					//	           ���ǰ�b._strData���ű�Ϊ����
		BigData b1(b._strData);
		b1._strData[0] = '+';
		return BigData(Sub(_strData, b1._strData)); //���������൱�ڼ���
	}
	else if('-' == _strData[0])   // ����������  ����Ϊ���� -5+7  �൱�� 7-5
	{							  //             ����Ϊ���� -5+3  �൱�� 3-5
		BigData b1(_strData);
		b1._strData[0] = '+';       //             ���ǰ�_strData��Ϊ���������Ҳ���������
		return BigData(Sub(b._strData, b1._strData));
	}
        
}

BigData BigData::operator-(const BigData& b)
{
	if(!IsINT64Overflow() && !b.IsINT64Overflow())   //�ж������Ƿ�����Ч��Χ��
	{
		//����Ч��Χ�ڣ����Բ����жϷ���
		if(_strData[0] == b._strData[0])            
			return BigData(_value - b._value);              //ͬ������
		else if((_strData[0] == '+' && b._value + MaxValue >= _value)||
			(_strData[0] = '-' && b._value + MinValue <= _value))              
			return BigData(_value - b._value);
	}
	if(_strData[0] == '+' && b._strData[0] == '+')
		return BigData(Sub(_strData, b._strData));
	                      //��������  ����������   ����Ϊ�� 5 - ��-7��= 5+7   5-��-3��= 5+3 5-��-12�� = 5+12
	else if(_strData[0] == '-' && b._strData[0] == '-')
	{
		BigData b1(_strData);
		BigData b2(b._strData);
		b1._strData[0] = '+';
		b2._strData[0] = '+';
		return BigData(Sub(b2._strData, b1._strData));
	}
	else if( '+' == _strData[0] )
	{
		BigData b1(b._strData);        //�൱�ڰ�b._strData��Ϊ��������
		b1._strData[0] = '+'; 
		return BigData(Add(_strData, b1._strData)); 
	}
	else  //   ����������   ����Ϊ��  -5 -7 = -��5+7�� = -5 +(-7)
	{  
		BigData b1(b._strData);
		b1._strData[0] = '-';             //�൱�ڰ�b._strData��Ϊ��������
		return BigData(Add(_strData, b1._strData));
	}
		
}
BigData BigData::operator*(const BigData& b)
{
	if (!IsINT64Overflow() && !b.IsINT64Overflow())
	{
		if(_value == 0 || b._value == 0)
			return BigData(0);
		if(_value == 1 )
			return BigData(b._value);
		if(_value == -1)
			return BigData(0 - b._value);
		if(b._value == 1 )
			return BigData(_value);
		if(b._value == -1)
			return BigData(0 - _value);
		if(_strData.size() +b._strData.size() < 19)
			return BigData(_value * b._value);
	}
		return BigData(Mul(_strData, b._strData));
}

BigData BigData::operator/(const BigData& b)
{
	if(0 == b._value)
	{
		std::cout << "����Ϊ0" <<std::endl;
		return BigData(0);
	}
	//��Ϊ0
	//��ĸΪ0 ���߷��Ӵ��ڷ�ĸ
	if( "+0" == _strData || _strData.size() < b._strData.size()
		|| (_strData.size() == b._strData.size() && 
		strcmp( _strData.c_str()+1, b._strData.c_str()+1 ) < 0 ) )
		return BigData(0);
	//����1��-1
	if( strcmp(_strData.c_str() +1, b._strData.c_str() +1) == 0 )
	{
		BigData b1(1);
		if(_strData[0] != b._strData[0] )
		{
			b1._value = -1;
			b1._strData[0] = '-';
		}
		return b1;
	}
	//���ڷ�ĸ
	if( strcmp( b._strData.c_str() +1 , "1") == 0)
	{
		BigData b1(_strData);
		if(_strData[0] != b._strData[0])
		{
			b1._strData[0] = '-';
		}
		return b1;
	}
	if(!IsINT64Overflow() && !b.IsINT64Overflow())
		return BigData( _value /b._value );
	else
		return BigData( Div( _strData, b._strData ) );
}


std::string BigData::Div(std::string left, std::string right)
{
	char symbol = '+';
	if( left[0] != right[0])
		symbol = '-';
	std::string strRet;
	strRet.append(1,symbol);
	                          //�жϷ���
	int  LSize = left.size()-1;
	int RSize = right.size()-1;
	int len = RSize;
	char *pLeft = (char *)(left.c_str()+1);
	char *pRight = (char*)(right.c_str()+1);
	while(  *(pLeft +len -1) != '\0')
	{
		//�������߱��ұ�С����Ϊ0��
		if( !IsleftBig( pLeft, len, pRight, RSize))
		{
			len++;
			strRet.append( 1, '0');
			continue;
		}
		else
		{
			strRet.append( 1,Subloop( pLeft,len, pRight, RSize));
			len++;
		}
	}
	return strRet;
}


std::string BigData::Mul(std::string left, std::string right)
{
	int LSize = left.size();
	int RSize = right.size();
	char symbol = '+';
	if(left[0] != right[0])
		symbol = '-';
	//�̵ķ�����ѭ��
	if(LSize>RSize)
	{
		std::swap(left, right);
		std::swap(LSize,RSize);
	}
	std::string strRet;
	strRet.resize(LSize+RSize-1,'0');
	strRet[0] = symbol;
	char step = 0;
	char offset = 0; //��λ
	for(size_t i = 1; i< LSize; ++i)
	{
		char cLeft = left[LSize-i] - '0';
		step = 0; //ÿ�ν���֮��Ҫ����λ��0
		if(cLeft == 0)
		{
			offset++;    //������0ֱ����������0�������壬����Ҫ������λ
			continue;
		}
		for(size_t j = 1; j<RSize; ++j)
		{
			char temp = cLeft * (right[RSize - j] - '0') + step;
			temp = temp + strRet[LSize + RSize - 1 - j - offset] - '0';//Ҫ�����ϴε�
			step = temp/10;
			strRet[LSize + RSize - 1 - j - offset] = temp % 10 +'0';        
		}
		strRet[LSize - 1 - offset] = step + '0';      //����λstep���ڴ�ѭ������ǰ��strRet[LSize+RSize - 1 -RSize - offset]��λ��
		offset++;
	}
	return strRet;

}




std::string BigData::Sub(std::string left, std::string right)//��������
{
	//���ȼӷ�Ҫ�жϷ���
	int LSize = left.size();
	int RSize = right.size();
	char symbol = '+';
	if(LSize<RSize||                            //�����������Ҳ�������
		( LSize==RSize && left<right ))
	{
		std::swap(left,right);
		std::swap(LSize,RSize);	

		if(right[0] == '+')        //�����ķ��ź��������ұߵ�ֵ�÷����෴
			symbol = '-';
	}
	std::string strRet;
	strRet.resize(LSize);
	strRet[0] = symbol;
	for(size_t idx = 1; idx < LSize; ++idx)
	{
		char temp = left[LSize-idx] - '0';
		if(idx < RSize)
			temp -= right[RSize-idx] - '0';
		if(temp<0)
		{  
			left[LSize-idx-1] -= 1;
			temp +=10;
		}
		strRet[LSize-idx] = temp + '0';
	}
	return strRet;
}

std::string BigData::Add(std::string left,std::string right)  //�����ӷ�
{
	//������������С�����ұ�
	int LSize = left.size();
	int RSize = right.size();
	if(LSize < RSize)
	{
		std::swap(left,right);
		std::swap(LSize,RSize);		
	}
	std::string strRet;
	strRet.resize(LSize+1);  //���������߶�һλ
	strRet[0] = left[0];
	char step = 0;
	for(size_t idx = 1; idx < LSize; ++idx)
	{
		char temp = left[LSize-idx] - '0' + step;
		if(idx < RSize)
			temp += right[RSize-idx] - '0';
		step = temp/10;
		strRet[LSize+1-idx] = temp%10 + '0';
	}
	strRet[1] = step + '0';
	return strRet;
}

bool BigData::IsINT64Overflow()const  //������Ч��Χ�ڷ���true���ڷ���false
{
	std::string strTemp("+9223372036854775807");
	if(_strData[0] == '-')
		strTemp = "-9223372036854775808";
	if(_strData.size() < strTemp.size())
		return false;
	else if(_strData.size() == strTemp.size()&& _strData < strTemp)//�ַ����ıȽ�   -9223372036854775808
													                              //-9223372036854775809      
		return false;
	return true;
}

char BigData::Subloop( char *&pLeft, int &LSize, char *&pRight, int RSize)
{
	char count = '0';
	while( IsleftBig( pLeft, LSize, pRight, RSize))
	{
		for( size_t i=0; i<LSize; ++i )
		{
			char temp = pLeft[ LSize -1 -i] - '0';
			if( i<RSize )
				temp -= (pRight[ RSize -1 -i] - '0');
			//��λ
			if( temp<0 )
			{
				size_t step = 1;
				while( ( step + i + 1 < LSize ) && ( pLeft[ LSize -1 -i -step] == 0)) //���˵��м�Ϊ0������
				{
					pLeft[ LSize -1 -i -step ] = '9';
					step++;
				} 
				pLeft[ LSize - 1 -i -step ] -= 1;
				temp += 10;
			}
			pLeft[ LSize -1 -i ] = temp + '0';
		} 
		//����һ��
		count++;
		while( LSize > 0 && * pLeft == '0')
		{
			pLeft++;
			LSize--;
		}
	}
	return count;
}

bool BigData::IsleftBig(char *pLeft, int LSize, char *pRight, int RSize)
{/*
 assert(pLeft);
 assert(pRight);*/
	if( LSize >RSize || (LSize == RSize && !( strncmp( pLeft, pRight, LSize) <0) ) )
		return true;  //���������ڳ���
	return false;    //������С�ڳ���

}
