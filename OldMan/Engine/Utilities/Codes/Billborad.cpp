#include "Billborad.h"

USING(ENGINE)

CBillborad::CBillborad()
{
	D3DXMatrixIdentity(&matBillView);
}


CBillborad::~CBillborad()
{
}

const D3DXMATRIX& CBillborad::GetWorldMatrix_Billborad() const
{
	// TODO: ���⿡ ��ȯ ������ �����մϴ�.
	return matBillView;
}

void CBillborad::Billborad_Yagnle(const D3DXMATRIX & _matLocal, const D3DXMATRIX & _matCamera, D3DXVECTOR3 _vSize)
{
				
	matBillView._11 = _matCamera._11;		// 11,13, 31,33 ��Ŀ� ���� �־��ش�. 
	matBillView._13 = _matCamera._13;
	matBillView._31 = _matCamera._31;
	matBillView._33 = _matCamera._33;


	D3DXMatrixInverse(&matBillView, 0, &matBillView);												     // ����� �� �����.
	memcpy(&matBillView._41, _matLocal.m[3], sizeof(D3DXVECTOR3));
	float fScale[3];
	fScale[0] = _vSize.x;
	fScale[1] = _vSize.y;
	fScale[2] = 1.f;

	for (int i = 0; i < 3; ++i)
	{
		for (int j = 0; j < 4; ++j)
			matBillView(i, j) *= fScale[i];
	}

}
// ��� ����
void CBillborad::Billborad_Front(const D3DXMATRIX & _matLocal, const D3DXMATRIX & _matCamera, D3DXVECTOR3 _vSize)
{

																		 //��� ���� 
	D3DXMatrixIdentity(&matBillView);											// �ʱ�ȭ 

	matBillView = _matCamera;													//������ ��Ŀ� ī�޶��� ������� �޾ƿ´� 
	D3DXMatrixInverse(&matBillView, 0, &matBillView);											    //���� ������� ����ķ� �����.   -> �׻� ���� ���� ���ؼ� 

	memcpy(&matBillView._41, _matLocal.m[3], sizeof(D3DXVECTOR3));         //������ �̵������ ���� ����Ŀ� �������. 
	
	
	for (int i = 0; i < 3; ++i)
	{
		for (int j = 0; j < 4; ++j)
			matBillView(i, j) *= 1.f;
	}

}

void CBillborad::LateUpdate()
{
	//D3DXMatrixScaling(&m_matScale, m_vSize.x, m_vSize.y, m_vSize.z);



}

CBillborad * CBillborad::Create()
{
	return new CBillborad();
}
