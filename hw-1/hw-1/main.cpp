#include <iostream>
#include <vector>
using namespace std;

//�ṹԪ�صı�ʾ����ά���飨����ָ�룬�У��У���ptr��row��col��
//               ԭ�㣺originX �е�index�� originY�е�index  ptr[originX][originY]����ԭ������
//�ܵ���˵�����Զ�λ�����structure element
struct Element{
	int** ptr;
	int row;
	int col;
	int originX;
	int originY;
	Element() :ptr(NULL), row(0), col(0), originX(-1), originY(-1){}
	Element(int width, int height)
	{
		ptr = new int*[width];
		row = width;
		col = height;
		for (int i = 0; i < row; i++)
			ptr[i] = new int[col];
	}
};
//ͼ��ı�ʾ��������λ���飨����ָ�룬�У��У���ptr��row��col��
//�������ǶԶ�ֵͼ��Ĳ�������
//��λ����ȡֵΪ0��1   
//0����ĵ�û��ͼ��ֻ�б���
//1����ĵ���ͼ��
class BinaryImage{
public:
	BinaryImage()
	{
		ptr = NULL;
		row = 0;
		col = 0;
	}
	BinaryImage( int width, int height)                          //���캯��
	{
		row = width;
		col = height;
		ptr = new int*[row];
		for (int i = 0; i < row; i++)
			ptr[i] = new int[col];
		for (int i = 0; i < row; i++)
		{
			for (int j = 0; j < col; j++)
				ptr[i][j] = 0;
		}
	}
	void setValue(int** p, int width, int height)                //��ֵ����������ͨ����ά���鸳ֵ
	{
		if (p == NULL || width != row || height != col)
			return;
		for (int i = 0; i < row; i++)
		{
			for (int j = 0; j < col; j++)
				ptr[i][j] = p[i][j];
		}
	}
	void setIndexValue(int i, int j, int val)                    //ͨ��ÿ������ֵ
	{
		if (i < 0 || j < 0 || i >= row || j >= col)
			return;
		ptr[i][j] = val;
	}
	int getIndexValue(int i, int j)                               //��ȡֵ
	{
		return ptr[i][j];
	}
	int getRow()                                                 //��ȡ��
	{
		return row;
	}
	int getCol()                                                //��ȡ��
	{
		return col;
	}
	BinaryImage* dilation(Element& strucEle)                    //��ֵͼ���dilation����
	{
		if (ptr == NULL || row == 0 || col == 0)
			return NULL;
		int el_col = strucEle.col - (strucEle.originY+1);       //���Ӧ����չ������
		int er_col = strucEle.originY;                          //�ұ�Ӧ����չ������
		int et_row = strucEle.row - (strucEle.originX+1);       //����Ӧ����չ������
		int ed_row = strucEle.originX;                          //����Ӧ����չ������
		int d_row = row + et_row + ed_row;
		int d_col = col + el_col + er_col;
		BinaryImage* d_BI=new BinaryImage(d_row,d_col);
		vector<int>BI_rows;                                      //ԭͼ����1��λ��
		vector<int>BI_cols;
		vector<int>SE_rows;                                      //�ṹԪ����1��λ��
		vector<int>SE_cols;
		for (int i = 0; i < row; i++)
		{
			for (int j = 0; j < col; j++)
			{
				if (ptr[i][j] == 1)
				{
					BI_rows.push_back(i);
					BI_cols.push_back(j);
				}
			}
		}
		for (int i = 0; i < strucEle.row; i++)
		{
			for (int j = 0; j < strucEle.col; j++)
			{
				if (strucEle.ptr[i][j] == 1)
				{
					SE_rows.push_back(i);
					SE_cols.push_back(j);
				}
			}
		}
		for (int i = 0; i < d_row; i++)
		{
			for (int j = 0; j < d_col; j++)
			{
				//�ж��Ƿ��н���
				if (isDilation(i, j, strucEle.originX, strucEle.originY, SE_rows, SE_cols, BI_rows, BI_cols, et_row, el_col) == true)
					d_BI->setIndexValue(i, j, 1);
			}
		}
		return d_BI;
	}
	BinaryImage* erosion(Element& strucEle)
	{
		if (ptr == NULL || row == 0 || col == 0)
			return NULL;
		int e_row = row;                         //��Ϊ��ʴͼ�񲻿��ܱ�ԭͼ������Բ���չ�к���
		int e_col = col;
		BinaryImage* e_BI = new BinaryImage(e_row, e_col);
		vector<int>BI_rows;
		vector<int>BI_cols;
		vector<int>SE_rows;
		vector<int>SE_cols;
		for (int i = 0; i < row; i++)
		{
			for (int j = 0; j < col; j++)
			{
				if (ptr[i][j] == 1)
				{
					BI_rows.push_back(i);
					BI_cols.push_back(j);
				}
			}
		}
		for (int i = 0; i < strucEle.row; i++)
		{
			for (int j = 0; j < strucEle.col; j++)
			{
				if (strucEle.ptr[i][j] == 1)
				{
					SE_rows.push_back(i);
					SE_cols.push_back(j);
				}
			}
		}
		for (int i = 0; i < e_row; i++)
		{
			for (int j = 0; j < e_col; j++)
			{
				//�жϽṹԪ���Ƿ������ԭͼ���ڲ�
				if (isErosion(i, j, strucEle.originX, strucEle.originY, SE_rows, SE_cols, BI_rows, BI_cols) == true)
					e_BI->setIndexValue(i, j, 1);
			}
		}
		return e_BI;
	}
private:
	int row;                                               //��ά�����ʾ
	int col;
	int** ptr;

	bool isDilation(int posX, int posY, int originX,int originY, vector<int>&SE_rows,
		vector<int>&SE_cols, vector<int>&BI_rows, vector<int>&BI_cols,int addX,int addY)
	{
		for (int i = 0; i < SE_rows.size(); i++)
		{
			int x=SE_rows[i] - originX + posX-addX;             //�ṹԪ����ԭͼ���ж�Ӧ��x
			int y = SE_cols[i] - originY + posY-addY;           //�ṹԪ����ԭͼ���ж�Ӧ��y
			for (int j = 0; j < BI_rows.size(); j++)
			{
				if (BI_rows[j] == x&&BI_cols[j] == y)           //�ж�ԭͼ����ṹԪ���Ƿ��ཻ
					return true;
			}

		}
		return false;
	}
	bool isErosion(int posX, int posY, int originX, int originY, vector<int>&SE_rows,
		vector<int>&SE_cols, vector<int>&BI_rows, vector<int>&BI_cols)
	{
		for (int i = 0; i < SE_rows.size(); i++)
		{
			bool found = false;
			int x = SE_rows[i] - originX + posX;             //�ṹԪ����ԭͼ���ж�Ӧ��x
			int y = SE_cols[i] - originY + posY;             //�ṹԪ����ԭͼ���ж�Ӧ��y
			for (int j = 0; j < BI_rows.size(); j++)
			{
				if (BI_rows[j] == x&&BI_cols[j] == y)        //�ж�ԭͼ����ṹԪ���Ƿ��ཻ
				{
					found = true;
					break;
				}
			}
			if (found == false)                             //ֻҪ��һ���ṹԪ�ص�1��λ����ԭͼ�����Ҳ�����������������ô�õ�
				return false;                               //������erosion��������Ϊ0
		}
		return true;
	}
};
//�򵥵Ĳ��Գ�������ok�����ok
int main()
{
	BinaryImage a(4, 5);                //�½�һ��ͼƬ
	a.setIndexValue(0, 0, 1); a.setIndexValue(0, 1, 0); a.setIndexValue(0, 2, 0); a.setIndexValue(0, 3, 1); a.setIndexValue(0, 4, 0);
	a.setIndexValue(1, 0, 1); a.setIndexValue(1, 1, 1); a.setIndexValue(1, 2, 1); a.setIndexValue(1, 3, 0); a.setIndexValue(1, 4, 0);
	a.setIndexValue(2, 0, 1); a.setIndexValue(2, 1, 0); a.setIndexValue(2, 2, 1); a.setIndexValue(2, 3, 1); a.setIndexValue(2, 4, 0);
	a.setIndexValue(3, 0, 1); a.setIndexValue(3, 1, 0); a.setIndexValue(3, 2, 0); a.setIndexValue(3, 3, 0); a.setIndexValue(3, 4, 1);
	

	Element b(2, 2);                    //�½�һ���ṹԪ��
	b.ptr[0][0] = 1; b.ptr[0][1] = 1;
	b.ptr[1][0] = 1; b.ptr[1][1] = 0;

	b.originX = 1;
	b.originY = 1;

	BinaryImage* k=a.dilation(b);        //dilation
	for (int i = 0; i < k->getRow(); i++)
	{
		for (int j = 0; j < k->getCol(); j++)
			cout << k->getIndexValue(i, j) << " ";
		cout << endl;
	}
	cout << "******************************" << endl;
	BinaryImage* l = a.erosion(b);        //erosion
	for (int i = 0; i < l->getRow(); i++)
	{
		for (int j = 0; j < l->getCol(); j++)
			cout <<l->getIndexValue(i, j) << " ";
		cout << endl;
	}
	cout << "******************************" << endl;
	system("pause");
}