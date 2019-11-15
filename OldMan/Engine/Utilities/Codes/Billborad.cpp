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
	// TODO: 여기에 반환 구문을 삽입합니다.
	return matBillView;
}

void CBillborad::Billborad_Yagnle(const D3DXMATRIX & _matLocal, const D3DXMATRIX & _matCamera, D3DXVECTOR3 _vSize)
{
				
	matBillView._11 = _matCamera._11;		// 11,13, 31,33 행렬에 값을 넣어준다. 
	matBillView._13 = _matCamera._13;
	matBillView._31 = _matCamera._31;
	matBillView._33 = _matCamera._33;


	D3DXMatrixInverse(&matBillView, 0, &matBillView);												     // 역행렬 로 만든다.
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
// 멤버 수정
void CBillborad::Billborad_Front(const D3DXMATRIX & _matLocal, const D3DXMATRIX & _matCamera, D3DXVECTOR3 _vSize)
{

																		 //행렬 생성 
	D3DXMatrixIdentity(&matBillView);											// 초기화 

	matBillView = _matCamera;													//생성된 행렬에 카메라의 뷰행렬을 받아온다 
	D3DXMatrixInverse(&matBillView, 0, &matBillView);											    //받은 뷰행렬을 역행렬로 만든다.   -> 항상 나를 보기 위해서 

	memcpy(&matBillView._41, _matLocal.m[3], sizeof(D3DXVECTOR3));         //몬스터의 이동행렬을 받은 뷰행렬에 덧씌운다. 
	
	
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
