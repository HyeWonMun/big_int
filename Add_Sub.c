#include "bn_struct.h"

//���� ���� ���� ������ �����ϴ� �Լ�
//ĳ�� ��Ʈ(0 �Ǵ� 1)�� ��ȯ
int ADD_ABc(word* Cj, word Aj, word Bj, int cb)
{
	int c = 0; //��ȯ�� ĳ�� ��Ʈ ���� ����

	(*Cj) = (word)(Aj + Bj); //Cj = (Aj + Bj) mod word

	if (*Cj < Aj) //ĳ�� �߻�
		c = 1;
	
	(*Cj) = (word)((*Cj) + cb); //�� �� ĳ���� ����
	if (*Cj < cb) //ĳ�� �߻�
		c = 1;

	return c;
}

//��ȣ�� ���� �� ������ ���� ���� �Լ� (wordLen(A) >= wordLen(B)�� ���)
//���� �Ҵ翡 ������ ���: -1, ������ ���: 0�� ��ȯ
int ADDC(bigint** C, bigint* A, bigint* B) 
{
	int max_wordLen = (A->wordLen); //C�� ���� ���̸� ���
	bi_new(C, max_wordLen + 1); //������� ������ big_int ����ü ����

	int c = 0, j = 0; //ĳ���� 0���� �ʱ�ȭ
	word* Cp = (*C)->a; word* Bp = B->a; //�ּ� ������ ���� ������ ����

	Bp = (word*)realloc(Bp, sizeof(word) * max_wordLen); //B�� �޸� ���Ҵ�
	if (Bp == NULL) //�����Ҵ� ���н� �Լ� ����
		return -1;
	
	array_init(((B->a) + (B->wordLen)), (max_wordLen - (B->wordLen)));// Bm ~ Bn-1���� 0���� �ʱ�ȭ
	 
	for (j = 0;j < max_wordLen;j++) //���� ���� ���� ����
	{
		c = ADD_ABc(Cp + j, (A->a)[j], (B->a)[j], c); //C, c <- (A[j] + B[j] + c)
		//printf("Aj = %02x, Bj = %02x\n", (A->a)[j], (B->a)[j]); 
	}

	if (c == 1) //�ֻ��� ���� ���� ���, ĳ���� �������� ���
		Cp[max_wordLen] = 1;
	else //ĳ���� �������� ���� ��� 0�� �κ��� ������
		bi_refine(*C);

	(*C)->sign = A->sign; //C�� ��ȣ�� ����
	return 0;
}

//�� ���� A, B�� ���� ����ϴ� �Լ�
//���� ���θ� ��ȯ�� (����: 0, ����: -1)
int ADD(bigint** C, bigint* A, bigint* B)
{
	int ret = 0; 

	if (Is_Zero(A) == 1) //A�� 0�� ���, C <- B
	{
		bi_assign(C, B);
		return 0;
	}

	else if (Is_Zero(B) == 1) //B�� 0�� ���, C <- A
	{
		bi_assign(C, A);
		return 0;
	}

	else if ((A->sign == 0) & (B->sign == 1)) // A > 0, B < 0
	{
		if (CompareABS(A, B) == -1) // |A| < |B|, A + B = |A| - |B| < 0
		{
			ret = SUBC(C, B, A);
			(*C)->sign = Negative; 
			return ret;
		}
		else // |A| > |B|, A + B = |A| - |B| > 0
			return SUBC(C, A, B);
	}

	else if ((A->sign == 1) & (B->sign == 0)) // A < 0, B > 0
	{
		if (CompareABS(A, B) == 1) // |A| > |B|, A + B = |A| - |B| < 0
		{
			ret = SUBC(C, A, B);
			(*C)->sign = Negative;
			return ret;
		}
		else // |A| < |B|, A + B = |A| - |B| > 0
			return SUBC(C, B, A);
	}

	else if (A->wordLen >= B->wordLen) //A�� ������̰� �� �� ���
		return ADDC(C, A, B);

	else //B�� ������̰� �� �� ���
		return ADDC(C, B, A);
}

//���� ���� ���� ������ �����ϴ� �Լ�
//ĳ�� ��Ʈ(0 �Ǵ� 1)�� ��ȯ
int SUB_AbB(word* Cj, word Aj, int bb, word Bj)
{
	int b = 0; //��ȯ�� ĳ�� ��Ʈ ����

	(*Cj) = (word)(Aj - bb);

	if (Aj < bb)
		b = 1;
	
	if ((*Cj) < Bj)
		b = 1;

	(*Cj) = (word)((*Cj) - Bj);
	
	return b;
}

//�� ���� ������ ���� ������ �����ϴ� �Լ� (A >= B�� ���)
//���� �Ҵ翡 ������ ���: -1, ������ ���: 0�� ��ȯ
int SUBC(bigint** C, bigint* A, bigint* B)
{
	int max_wordLen = (A->wordLen); //C�� ���� ���̸� ���
	bi_new(C, max_wordLen); //������� ������ big_int ����ü ����

	int b = 0, j = 0, tmp = 0; //ĳ���� 0���� �ʱ�ȭ
	word* Cp = (*C)->a; word* Bp = B->a;

	Bp = (word*)realloc(Bp, sizeof(word) * max_wordLen); //B�� �޸� ���Ҵ�
	if (Bp == NULL) //�����Ҵ� ���н� �Լ� ����
		return -1;

	array_init(((B->a) + (B->wordLen)), (max_wordLen - (B->wordLen)));// Bm ~ Bn-1���� 0���� �ʱ�ȭ

	for (j = 0;j < max_wordLen;j++) //���� ���� ���� ����
	{
		b = SUB_AbB(Cp + j, (A->a)[j], b, (B->a)[j]); //C, b <- (A - B - b)
		//printf("carry = %d\n", b);
		//printf("Aj = %llx, Bj = %llx, Cj = %llx\n", (A->a)[j], (B->a)[j], *(Cp + j));
	}
	bi_refine(*C);
	return 0;
}

//�� ���� A, B�� ���� ������ �����ϴ� �Լ�
//���� ���θ� ��ȯ�� (����: 0, ����: -1)
int SUB(bigint** C, bigint* A, bigint* B)
{
	int ret = 0;

	if (Is_Zero(A) == 1) //A�� 0�� ���, C <- (-B)
	{
		bi_assign(C, B);
		(*C)->sign = (B->sign) * (-1);
		return 0;
	}
	else if (Is_Zero(B) == 1) //B�� 0�� ���, C <- A
	{
		bi_assign(C, A);
		return 0;
	}
	else if (!Compare(A, B))
	{
		bi_set_zero(C);
		return 0;
	}
	else if ((A->sign == 0) & (B->sign == 0)) //�� �� ���
	{
		if (Compare(A, B) == 1) //A > B > 0
			return SUBC(C, A, B);

		else if (Compare(A, B) == -1) //B > A > 0
		{
			ret = SUBC(C, B, A);
			(*C)->sign = Negative;
			return ret;
		}
	}
	else if ((A->sign == 1) & (B->sign == 1)) //�� �� ����
	{
		if (Compare(A, B) == 1) //0 > A > B
			return SUBC(C, B, A);

		else if (Compare(A, B) == -1) //0 > B > A
		{
			ret = SUBC(C, A, B);
			(*C)->sign = Negative;
		}
	}
	else if ((A->sign == 0) & (B->sign == 1)) //A > 0 , B < 0
	{
		if (A->wordLen > B->wordLen) // A - B = A - (-|B|) = A + |B| > 0 
			ret = ADDC(C, A, B);
		else
			ret = ADDC(C, B, A);
		(*C)->sign = Non_Negative;
		return ret;
	}

	else if ((A->sign == 1) & (B->sign == 0)) //A < 0 , B > 0
	{
		if (A->wordLen > B->wordLen) // A - B = (-|A|) - B = -(|A| + B) < 0 
			ret = ADDC(C, A, B);
		else
			ret = ADDC(C, B, A);
		(*C)->sign = Negative;
		return ret;
	}
	return 0;
}

void ADD_test() //���� ���� �׽�Ʈ �Լ�
{
	bigint* A = NULL;
	bigint* B = NULL;
	bigint* C = NULL;

	int ret = 0;

#if WORD_BITLEN == 8
	word arr_b[4] = { 0x78,0x56,0x34,0x12 };
	word arr_s[5] = { 0xa9,0xb8,0xc7,0xd6,0xe5 };
#elif WORD_BITLEN == 32
	word arr_b[4] = { 0x7878,0x5656,0x3434,0x1212 };
	word arr_s[4] = { 0xefef,0xcdcd,0xabab,0x0909 };
#elif WORD_BITLEN == 64
	word arr_b[4] = { 0x787878787878,0x565600005656,0x343400003434,0x121200001212 };
	word arr_s[4] = { 0xefef0000efef,0xcdcd0000cdcd,0xabab0000abab,0x090900000909 };
#endif
	printf("****Test big integer addition (wordlen = %d)****\n\n", WORD_BITLEN);
	bi_set_by_array(&A, Non_Negative, arr_b, 4);
	bi_set_by_array(&B, Non_Negative, arr_s, 5);

	//test A>0 and B>0
	printf("A = "); bi_show(A, 16);
	printf("B = "); bi_show(B, 16);

	ret = ADD(&C, A, B);
	if (ret != -1)
	{
		printf("A + B = ");
		bi_show(C, 16);
	}

	ret = ADD(&C, B, A);
	if (ret != -1)
	{
		printf("B + A = ");
		bi_show(C, 16);
	}

	ret = ADD(&C, A, A);
	if (ret != -1)
	{
		printf("A + A = ");
		bi_show(C, 16);
	}printf("\n");

	//test A<0 and B<0
	A->sign = Negative; B->sign = Negative;
	printf("A = "); bi_show(A, 16);
	printf("B = "); bi_show(B, 16);

	ret = ADD(&C, A, B);
	if (ret != -1)
	{
		printf("A + B = ");
		bi_show(C, 16);
	}

	ret = ADD(&C, B, A);
	if (ret != -1)
	{
		printf("B + A = ");
		bi_show(C, 16);
	}printf("\n");

	//test A>0 and B<0
	A->sign = Non_Negative; B->sign = Negative;
	printf("A = "); bi_show(A, 16);
	printf("B = "); bi_show(B, 16);

	ret = ADD(&C, A, B);
	if (ret != -1)
	{
		printf("A + B = ");
		bi_show(C, 16);
	}

	ret = ADD(&C, B, A);
	if (ret != -1)
	{
		printf("B + A = ");
		bi_show(C, 16);
	}printf("\n");

	//test A<0 and B>0
	A->sign = Negative; B->sign = Non_Negative;
	printf("A = "); bi_show(A, 16);
	printf("B = "); bi_show(B, 16);

	ret = ADD(&C, A, B);
	if (ret != -1)
	{
		printf("A + B = ");
		bi_show(C, 16);
	}

	ret = ADD(&C, B, A);
	if (ret != -1)
	{
		printf("B + A = ");
		bi_show(C, 16);
	}printf("\n");

	//test A = 0 or B = 0
	A->sign = Non_Negative; bi_set_zero(&B);
	printf("A = "); bi_show(A, 16);
	printf("B = "); bi_show(B, 16);
	ret = ADD(&C, A, B);
	if (ret != -1)
	{
		printf("A + B = ");
		bi_show(C, 16);
	}printf("\n");

	A->sign = Negative; bi_set_zero(&B);
	printf("A = "); bi_show(B, 16);
	printf("B = "); bi_show(A, 16);
	ret = ADD(&C, B, A);
	if (ret != -1)
	{
		printf("A + B = ");
		bi_show(C, 16);
	}printf("**********************************************\n\n");
}

void SUB_test() //���� ���� �׽�Ʈ �Լ�
{
	bigint* A = NULL;
	bigint* B = NULL;
	bigint* C = NULL;
	
	int ret = 0;

#if WORD_BITLEN == 8
	word arr_b[4] = { 0x78,0x56,0x34,0x12 };
	word arr_s[5] = { 0xef,0xcd,0xab,0x09, 0x11 };
#elif WORD_BITLEN == 32
	word arr_b[4] = { 0x7878,0x5656,0x3434,0x1212 };
	word arr_s[4] = { 0xefef,0xcdcd,0xabab,0x0909 };
#elif WORD_BITLEN == 64
	word arr_b[4] = { 0x787878787878,0x565600005656,0x343400003434,0x121200001212 };
	word arr_s[4] = { 0xefef0000efef,0xcdcd0000cdcd,0xabab0000abab,0x090900000909 };
#endif
	printf("****Test big integer subtraction (wordlen = %d)****\n\n", WORD_BITLEN);
	bi_set_by_array(&A, Non_Negative, arr_b, 4);
	bi_set_by_array(&B, Non_Negative, arr_s, 5);

	//test A>0 and B>0
	printf("A = "); bi_show(A, 16);
	printf("B = "); bi_show(B, 16);
	
	ret = SUB(&C, A, B);
	if (ret != -1)
	{
		printf("A - B = ");
		bi_show(C, 16);
	}

	ret = SUB(&C, B, A);
	if (ret != -1)
	{
		printf("B - A = ");
		bi_show(C, 16);
	}
	ret = SUB(&C, A, A);
	if (ret != -1)
	{
		printf("A - A = ");
		bi_show(C, 16);
	}printf("\n");

	//test A<0 and B<0
	A->sign = Negative; B->sign = Negative;
	printf("A = "); bi_show(A, 16);
	printf("B = "); bi_show(B, 16);

	ret = SUB(&C, A, B);
	if (ret != -1)
	{
		printf("A - B = ");
		bi_show(C, 16);
	}
	ret = SUB(&C, B, A);
	if (ret != -1)
	{
		printf("B - A = ");
		bi_show(C, 16);
	}printf("\n");

	//test A>0 and B<0
	A->sign = Non_Negative; B->sign = Negative;
	printf("A = "); bi_show(A, 16);
	printf("B = "); bi_show(B, 16);

	ret = SUB(&C, A, B);
	if (ret != -1)
	{
		printf("A - B = ");
		bi_show(C, 16);
	}
	ret = SUB(&C, B, A);
	if (ret != -1)
	{
		printf("B - A = ");
		bi_show(C, 16);
	}printf("\n");

	//test A<0 and B>0
	A->sign = Negative; B->sign = Non_Negative;
	printf("A = "); bi_show(A, 16);
	printf("B = "); bi_show(B, 16);

	ret = SUB(&C, A, B);
	if (ret != -1)
	{
		printf("A - B = ");
		bi_show(C, 16);
	}
	ret = SUB(&C, B, A);
	if (ret != -1)
	{
		printf("B - A = ");
		bi_show(C, 16);
	}printf("\n");

	//test A = 0 or B = 0
	A->sign = Negative; bi_set_zero(&B);
	printf("A = "); bi_show(A, 16);
	printf("B = "); bi_show(B, 16);
	ret = SUB(&C, A, B);
	if (ret != -1)
	{
		printf("A - B = ");
		bi_show(C, 16);
	}printf("\n");

	A->sign = Negative; bi_set_zero(&B);
	printf("A = "); bi_show(B, 16);
	printf("B = "); bi_show(A, 16);
	ret = SUB(&C, B, A);
	if (ret != -1)
	{
		printf("A - B = ");
		bi_show(C, 16);
	}printf("***********************************************\n\n");
}