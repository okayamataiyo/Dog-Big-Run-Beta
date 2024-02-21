#include <assert.h>
#include "Fbx.h"
#include "Direct3D.h"
#include "Camera.h"
#include "Texture.h"
#include "../Global.h"

using namespace DirectX;

Fbx::Fbx():
    vertexCount_(0),polygonCount_(0),materialCount_(0),
    pVertexBuffer_(nullptr),pIndexBuffer_(nullptr),pConstantBuffer_(nullptr),
    pMaterialList_(nullptr),ppIndexData_(nullptr),pVertexData_(nullptr),animSpeed_(0)
{
}

HRESULT Fbx::Init(FbxNode* _pNode)
{
    //�m�[�h���烁�b�V���̏����擾
    FbxMesh* mesh = _pNode->GetMesh();

    //�e���̌����擾
    vertexCount_ = mesh->GetControlPointsCount();			//���_�̐�
    polygonCount_ = mesh->GetPolygonCount();				//�|���S���̐�
    polygonVertexCount_ = mesh->GetPolygonVertexCount();	//�|���S�����_�C���f�b�N�X�� 
    materialCount_ = _pNode->GetMaterialCount();     //�}�e���A���̐�


    InitVertex(mesh);		//���_�o�b�t�@����
    InitMaterial(_pNode);	//�}�e���A������
    InitIndex(mesh);		//�C���f�b�N�X�o�b�t�@����
    InitSkelton(mesh);		//���̏�������
    InitConstantBuffer();	//�R���X�^���g�o�b�t�@�i�V�F�[�_�[�ɏ��𑗂��j����

    return E_NOTIMPL;
}

HRESULT Fbx::Load(std::string _fileName)
{
    FbxManager* pFbxManager = FbxManager::Create();                        //�}�l�[�W���𐶐�
    pFbxScene_ = FbxScene::Create(pFbxManager, "fbxscene");                //�V�[���I�u�W�F�N�g��FBX�t�@�C���̏��𗬂�����
    FbxImporter* fbxImporter = FbxImporter::Create(pFbxManager, "imp");    //�C���|�[�^�[�𐶐�
    fbxImporter->Initialize(_fileName.c_str(), -1, pFbxManager->GetIOSettings());
    fbxImporter->Import(pFbxScene_);
    fbxImporter->Destroy();

    frameRate_ = pFbxScene_->GetGlobalSettings().GetTimeMode();

    //���b�V�������擾
    //���[�g�m�[�h���擾����
    FbxNode* rootNode = pFbxScene_->GetRootNode();

    //�����̎q���̐��𒲂ׂ�
    int childCount = rootNode->GetChildCount();
    FbxNode* pNode = rootNode->GetChild(0);
    FbxMesh* mesh = pNode->GetMesh();

    char defaultCurrentDir[MAX_PATH];                       //���݂̃J�����g�f�B���N�g�����擾
    GetCurrentDirectory(MAX_PATH, defaultCurrentDir);

    char dir[MAX_PATH];                                     //������fileName����f�B���N�g���������擾
    _splitpath_s(_fileName.c_str(), nullptr, 0, dir, MAX_PATH, nullptr, 0, nullptr, 0);
    SetCurrentDirectory(dir);                               //�J�����g�f�B���N�g���ύX

    for (int i = 0; childCount > i; i++)
    {
        CheckNode(rootNode->GetChild(i), &parts_);
    }

    ////�e���̌����擾
    //vertexCount_  = mesh->GetControlPointsCount();	//���_�̐�
    //polygonCount_ = mesh->GetPolygonCount();	        //�|���S���̐�
    
    //InitVertex(mesh);		//���_�o�b�t�@����
    //InitIndex(mesh);		//�C���f�b�N�X�o�b�t�@����
    //InitConstantBuffer();	//�R���X�^���g�o�b�t�@����
    //InitMaterial(pNode);  //�m�[�h����}�e���A���̏��������o��

    SetCurrentDirectory(defaultCurrentDir);                 //�J�����g�f�B���N�g�������ɖ߂�

    //�}�l�[�W�����
    //pFbxManager->Destroy();

    return S_OK;
}

//���_�o�b�t�@����
void Fbx::InitVertex(fbxsdk::FbxMesh* _mesh)
{
    //���_��������z��
    pVertexData_ = new VERTEX[vertexCount_];

    //�S�|���S��
    for (DWORD poly = 0; poly < polygonCount_; poly++)
    {
        //3���_��
        for (int vertex = 0; vertex < 3; vertex++)
        {
            //���ׂ钸�_�̔ԍ�
            int index = _mesh->GetPolygonVertex(poly, vertex);

            //���_�̈ʒu
            FbxVector4 pos = _mesh->GetControlPointAt(index);
            pVertexData_[index].position = XMVectorSet((float)pos[0], (float)pos[1], (float)pos[2], 0.0f);

            //���_��UV
            FbxLayerElementUV* pUV = _mesh->GetLayer(0)->GetUVs();
            int uvIndex = _mesh->GetTextureUVIndex(poly, vertex, FbxLayerElement::eTextureDiffuse);
            FbxVector2 uv = pUV->GetDirectArray().GetAt(uvIndex);
            pVertexData_[index].uv = XMVectorSet((float)uv.mData[0], (float)(1.0f - uv.mData[1]), 0.0f, 0.0f);

            //���_�̖@��
            FbxVector4 Normal;
            _mesh->GetPolygonVertexNormal(poly, vertex, Normal);	    //i�Ԗڂ̃|���S���́Aj�Ԗڂ̒��_�̖@�����Q�b�g
            pVertexData_[index].normal = XMVectorSet((float)Normal[0], (float)Normal[1], (float)Normal[2], 0.0f);
        }
    }
    // ���_�o�b�t�@�쐬
    HRESULT hr;
	D3D11_BUFFER_DESC bd_vertex;
	bd_vertex.ByteWidth = sizeof(VERTEX) * vertexCount_;
	bd_vertex.Usage = D3D11_USAGE_DEFAULT;
	bd_vertex.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd_vertex.CPUAccessFlags = 0;
	bd_vertex.MiscFlags = 0;
	bd_vertex.StructureByteStride = 0;
	D3D11_SUBRESOURCE_DATA data_vertex;
	data_vertex.pSysMem = pVertexData_;
	hr = Direct3D::pDevice_->CreateBuffer(&bd_vertex, &data_vertex, &pVertexBuffer_);
	if (FAILED(hr))
	{
		MessageBox(NULL, "���_�o�b�t�@�̍쐬�Ɏ��s���܂���", "�G���[", MB_OK);
	}
}

//�C���f�b�N�X�o�b�t�@����
void Fbx::InitIndex(fbxsdk::FbxMesh* mesh)
{
//  int* index = new int[polygonCount_ * 3];
//  int count = 0;
    pIndexBuffer_ = new ID3D11Buffer * [materialCount_];
    indexCount_ = vector<int>(materialCount_);
    ppIndexData_ = new DWORD * [materialCount_];
//    indexCount_ = new int[materialCount_];

//    for (DWORD poly = 0; poly < polygonCount_; poly++)

    vector<int> index(polygonCount_ * 3);//�|���S�����@* 3 = �S���_���p��
//    int* index = new int[polygonCount_ * 3];

    for (int i = 0; i < materialCount_; i++)
    {
        //    //3���_��
        //    for (DWORD vertex = 0; vertex < 3; vertex++)
        //    {
        //        index[count] = mesh->GetPolygonVertex(poly, vertex);
        //        count++;
        //    }
        //}

        int count = 0;

        //�S�|���S��
        for (DWORD poly = 0; poly < polygonCount_; poly++)
        {
            //����}�e���A�����������|���S���̃��X�g���Ƃ��Ă��āA���_�����X�g�A�b�v
            FbxLayerElementMaterial* mtl = mesh->GetLayer(0)->GetMaterials();
            int mtlId = mtl->GetIndexArray().GetAt(poly);

            if (mtlId == i)
            {
                //3���_��
                for (DWORD vertex = 0; vertex < 3; vertex++)
                {
                    index[count] = mesh->GetPolygonVertex(poly, vertex);
                    count++;
                }
            }
        }
        indexCount_[i] = count;

        D3D11_BUFFER_DESC   bd;
        bd.Usage = D3D11_USAGE_DEFAULT;
        bd.ByteWidth = sizeof(int) * polygonCount_ * 3;
        bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
        bd.CPUAccessFlags = 0;
        bd.MiscFlags = 0;

        D3D11_SUBRESOURCE_DATA InitData;
        InitData.pSysMem = index.data();
        InitData.SysMemPitch = 0;
        InitData.SysMemSlicePitch = 0;

        HRESULT hr;
        hr = Direct3D::pDevice_->CreateBuffer(&bd, &InitData, &pIndexBuffer_[i]);
        if (FAILED(hr))
        {
            MessageBox(NULL, "�C���f�b�N�X�o�b�t�@�̍쐬�Ɏ��s���܂���", "�G���[", MB_OK);
        }
        ppIndexData_[i] = new DWORD[count];
        memcpy(ppIndexData_[i], index.data(), sizeof(DWORD) * count);
    }
}

void Fbx::InitConstantBuffer()
{
    D3D11_BUFFER_DESC cb;
    cb.ByteWidth = sizeof(CONSTANT_BUFFER_MODEL);
    cb.Usage = D3D11_USAGE_DYNAMIC;
    cb.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    cb.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    cb.MiscFlags = 0;
    cb.StructureByteStride = 0;

    // �R���X�^���g�o�b�t�@�̍쐬
    HRESULT hr;
    hr = Direct3D::pDevice_->CreateBuffer(&cb, nullptr, &pConstantBuffer_);
    if (FAILED(hr))
    {
        MessageBox(NULL, "�R���X�^���g�o�b�t�@�̍쐬�Ɏ��s���܂���", "�G���[", MB_OK);
    }
}

void Fbx::InitMaterial(fbxsdk::FbxNode* _pNode)
{
    pMaterialList_ = new MATERIAL[materialCount_];

    for (int i = 0; i < materialCount_; i++)
    {
        //i�Ԗڂ̃}�e���A�������擾
        FbxSurfacePhong* pMaterial = (FbxSurfacePhong*)(_pNode->GetMaterial(i));
        //FbxSurfaceMaterial* pMaterial = pNode->GetMaterial(i);
        FbxSurfacePhong* pPhong = (FbxSurfacePhong*)pMaterial;
        FbxDouble3 diffuse = pMaterial->Diffuse;
        FbxDouble3 ambient = pMaterial->Ambient;    //XMFLOAT4

        pMaterialList_[i].diffuse = XMFLOAT4((float)diffuse[0], (float)diffuse[1], (float)diffuse[2], 1.0f);
        pMaterialList_[i].ambient = XMFLOAT4((float)ambient[0], (float)ambient[1], (float)ambient[2], 1.0f);
        pMaterialList_[i].specular = XMFLOAT4(0, 0, 0, 0);   //�Ƃ肠�����n�C���C�g�͍�
        pMaterialList_[i].shineness = 1;

        if (pMaterial->GetClassId().Is(FbxSurfacePhong::ClassId))
        {
            //Maya�Ŏw�肵��SpecularColor�̏��
            FbxDouble3 specular = pPhong->Specular;
            pMaterialList_[i].specular = XMFLOAT4((float)specular[0], (float)specular[1], (float)specular[2], 1.0f);

            FbxDouble shineness = pPhong->Shininess;
            pMaterialList_[i].shineness = (float)pPhong->Shininess;
        }

        pMaterialList_[i].diffuse = XMFLOAT4{ (float)diffuse[0],(float)diffuse[1] ,(float)diffuse[2] ,1.0};
        //�e�N�X�`�����
        FbxProperty  lProperty = pMaterial->FindProperty(FbxSurfaceMaterial::sDiffuse);

        //�e�N�X�`���̐���
        int fileTextureCount = lProperty.GetSrcObjectCount<FbxFileTexture>();

        //�e�N�X�`������
        if (fileTextureCount)
        {
            FbxFileTexture* textureInfo = lProperty.GetSrcObject<FbxFileTexture>(0);
            const char* textureFilePath = textureInfo->GetRelativeFileName();

            //�t�@�C����+�g�������ɂ���
            char name[_MAX_FNAME];	//�t�@�C����
            char ext[_MAX_EXT];	    //�g���q
            _splitpath_s(textureFilePath, nullptr, 0, nullptr, 0, name, _MAX_FNAME, ext, _MAX_EXT);
            wsprintf(name, "%s%s", name, ext);

            //�t�@�C������e�N�X�`���쐬
            pMaterialList_[i].pTexture = new Texture;
            HRESULT hr = pMaterialList_[i].pTexture->Load(name);
            assert(hr == S_OK);
        }

        //�e�N�X�`������
        else
        {
            pMaterialList_[i].pTexture = nullptr;
            //�}�e���A���̐F
            FbxSurfaceLambert* pMaterial = (FbxSurfaceLambert*)_pNode->GetMaterial(i);
        }


    }
}

void Fbx::InitSkelton(FbxMesh* _pMesh)
{
    FbxDeformer* pDeformer = _pMesh->GetDeformer(0);
    if (pDeformer == nullptr)
    {
        //�{�[�����Ȃ�
        return;
    }

    //�f�t�H�[�}��񂩂�X�L�����b�V�������擾
    pSkinInfo_ = (FbxSkin*)pDeformer;

    //���_����|���S�����t�������邽�߂̏����쐬����
    struct POLY_INDEX
    {
        int* polyIndex;
        int* vertexIndex;
        int  numRef;
    };

    POLY_INDEX* polyTable = new POLY_INDEX[vertexCount_];
    for (DWORD i = 0; i < vertexCount_; i++)
    {
        //�O�p�`�|���S���ɍ��킹�āA���_�ƃ|���S���̊֘A�����\�z����
        //�����_��=�|���S����*3���_
        polyTable[i].polyIndex = new int[polygonCount_ * 3];
        polyTable[i].vertexIndex = new int[polygonCount_ * 3];
        polyTable[i].numRef = 0;
        ZeroMemory(polyTable[i].polyIndex, sizeof(int)* polygonCount_ * 3);
        ZeroMemory(polyTable[i].vertexIndex, sizeof(int)* polygonCount_ * 3);

        //�|���S���Ԃŋ��L���钸�_��񋓂���
        for (DWORD k = 0; k < polygonCount_; k++)
        {
            for (int m = 0; m < 3; m++)
            {
                if (_pMesh->GetPolygonVertex(k, m) == i)
                {
                    polyTable[i].polyIndex[polyTable[i].numRef] = k;
                    polyTable[i].vertexIndex[polyTable[i].numRef] = m;
                    polyTable[i].numRef++;
                }
            }
        }
    }

    //���{�[�������擾����
    numBone_ = pSkinInfo_->GetClusterCount();
    ppCluster_ = new FbxCluster * [numBone_];
    for (int i = 0; i < numBone_; i++)
    {
        ppCluster_[i] = pSkinInfo_->GetCluster(i);
    }

    //���{�[���̐��ɍ��킹�ăE�F�C�g������������
    pWeightArray_ = new Fbx::Weight[vertexCount_];
    for (DWORD i = 0; i < vertexCount_; i++)
    {
        //pWeightArray_[i].posOrigin = pVertexData_[i].position;
        //pWeightArray_[i].normalOrigin = pVertexData_[i].normal;
        XMStoreFloat3(&pWeightArray_[i].posOrigin, pVertexData_[i].position);
        XMStoreFloat3(&pWeightArray_[i].normalOrigin, pVertexData_[i].normal);
        pWeightArray_[i].pBoneIndex = new int[numBone_];
        pWeightArray_[i].pBoneWeight = new float[numBone_];
        for (int j = 0; j < numBone_; j++)
        {
            pWeightArray_[i].pBoneIndex[j] = -1;
            pWeightArray_[i].pBoneWeight[j] = 0.0f;
        }
    }

    //�����ꂼ��̃{�[���ɉe�����󂯂钸�_�𒲂ׂ�
    //��������t�ɁA���_�x�[�X�Ń{�[���C���f�b�N�X�F�d�݂𐮓ڂ���
    for (int i = 0; i < numBone_; i++)
    {
        int numIndex = ppCluster_[i]->GetControlPointIndicesCount();    //���̃{�[���ɉe�����󂯂钸�_��
        int* piIndex = ppCluster_[i]->GetControlPointIndices();         //�{�[���E�F�C�g���̔ԍ�
        double* pdWeight = ppCluster_[i]->GetControlPointWeights();     //���_���Ƃ̃E�F�C�g���

        for (int k = 0; k < numIndex; k++)
        {
            //���_�Ɋ֘A����ꂽ�E�F�C�g��񂪃{�[��5�{�ȏ�̏ꍇ�́A�d�݂̑傫������4�{�ɍi��
            for (int m = 0; m < 4; m++)
            {
                if (m >= numBone_)
                {
                    break;
                }

                if (pdWeight[k] > pWeightArray_[piIndex[k]].pBoneWeight[m])
                {
                    for (int n = numBone_ - 1; n > m; n--)
                    {
                        pWeightArray_[piIndex[k]].pBoneIndex[n] = pWeightArray_[piIndex[k]].pBoneIndex[n - 1];
                        pWeightArray_[piIndex[k]].pBoneWeight[n] = pWeightArray_[piIndex[k]].pBoneWeight[n - 1];
                    }
                    pWeightArray_[piIndex[k]].pBoneIndex[m] = i;
                    pWeightArray_[piIndex[k]].pBoneWeight[m] = (float)pdWeight[k];
                    break;
                }
            }
        }
    }

    //���{�[�����쐬
    pBoneArray_ = new Fbx::Bone[numBone_];
    for (int i = 0; i < numBone_; i++)
    {
        //�{�[���̃f�t�H���g�ʒu���擾����
        FbxAMatrix matrix;
        ppCluster_[i]->GetTransformLinkMatrix(matrix);

        //�s��R�s�[(Fbx�`������DirectX�ւ̕ϊ�)
        XMFLOAT4X4 pose;
        for (DWORD x = 0; x < 4; x++)
        {
            for (DWORD y = 0; y < 4; y++)
            {
                pose(x, y) = (float)matrix.Get(x, y);
            }
        }
        pBoneArray_[i].bindPose = XMLoadFloat4x4(&pose);
    }

    //�ꎞ�I�ȃ������̈���������
    for (DWORD i = 0; i < vertexCount_; i++)
    {
        SAFE_DELETE_ARRAY(polyTable[i].polyIndex);
        SAFE_DELETE_ARRAY(polyTable[i].vertexIndex);
    }
    SAFE_DELETE_ARRAY(polyTable);
}

//�e�N�X�`�������[�h
void Fbx::Draw(Transform& _transform, int _frame)
{
    //�����̏u�Ԃ̎����̎p���s��𓾂�
    FbxTime time;
    time.SetTime(0, 0, 0, _frame, 0, 0, frameRate_);

    for (int k = 0; k < parts_.size(); k++)
    {
        //�X�L���A�j���[�V����(�{�[���L��)�̏ꍇ
        if (parts_[k]->GetSkinInfo() != nullptr)
        {
            parts_[k]->DrawSkinAnime(_transform, time);
        }
        //���b�V���A�j���[�V�����̏ꍇ
        else
        {
            parts_[k]->DrawMeshAnime(_transform, time, pFbxScene_);
        }
    }
    _transform.Calclation();//�g�����X�t�H�[�����v�Z
}

void Fbx::Draw(Transform& _transform)
{
    //���_�o�b�t�@
    UINT stride = sizeof(VERTEX);
    UINT offset = 0;
    Direct3D::pContext_->IASetVertexBuffers(0, 1, &pConstantBuffer_,&stride,&offset);
    Direct3D::pContext_->VSSetConstantBuffers(0, 1, &pConstantBuffer_);
    Direct3D::pContext_->PSSetConstantBuffers(0, 1, &pConstantBuffer_);
    for (int i = 0; i < materialCount_; i++)
    {
        UINT stride = sizeof(int);
        UINT offset = 0;
        Direct3D::pContext_->IASetIndexBuffer(pIndexBuffer_[i], DXGI_FORMAT_R32_UINT, 0);

        //�R���X�^���g�o�b�t�@�ɏ���n��
        D3D11_MAPPED_SUBRESOURCE pData;
        CONSTANT_BUFFER_MODEL cb;
        cb.matWVP = XMMatrixTranspose(_transform.GetWorldMatrix() * pCamera->GetViewMatrix() * pCamera->GetProjectionMatrix());
        cb.matNormal = XMMatrixTranspose(_transform.GetNormalMatrix());
        cb.matW = XMMatrixTranspose(_transform.GetNormalMatrix());
        cb.diffuseColor = pMaterialList_[i].diffuse;
        cb.ambientColor = pMaterialList_[i].ambient;
        cb.specularColor = pMaterialList_[i].specular;
        cb.shineness = pMaterialList_[i].shineness;
        cb.isTextured = pMaterialList_[i].pTexture != nullptr;
        Direct3D::pContext_->Map(pConstantBuffer_, 0, D3D11_MAP_WRITE_DISCARD, 0, &pData);
        memcpy_s(pData.pData, pData.RowPitch, (void*)(&cb), sizeof(cb));
        Direct3D::pContext_->UpdateSubresource(pConstantBuffer_, 0, NULL, &cb, 0, 0);

        // �C���f�b�N�X�o�b�t�@�[���Z�b�g
        stride = sizeof(int);
        offset = 0;
        Direct3D::pContext_->IASetIndexBuffer(pIndexBuffer_[i], DXGI_FORMAT_R32_UINT, 0);

        //�R���X�^���g�o�b�t�@
        Direct3D::pContext_->VSSetConstantBuffers(0, 1, &pConstantBuffer_);	//���_�V�F�[�_�[�p	
        Direct3D::pContext_->PSSetConstantBuffers(0, 1, &pConstantBuffer_);	//�s�N�Z���V�F�[�_�[�p

        ////�`��
        //Direct3D::pContext_->DrawIndexed(polygonCount_ * 3, 0, 0);
        //D3D11_MAPPED_SUBRESOURCE pdata;
        //Direct3D::pContext_->Map(pConstantBuffer_, 0, D3D11_MAP_WRITE_DISCARD, 0, &pdata);  //GPU����̃f�[�^�A�N�Z�X
        //memcpy_s(pdata.pData, pdata.RowPitch, (void*)(&cb), sizeof(cb));                    //�f�[�^�𑗂�

        //Direct3D::pContext_->Unmap(pConstantBuffer_, 0);     //�ĊJ

        //���_�o�b�t�@�A�C���f�b�N�X�o�b�t�@�A�R���X�^���g�o�b�t�@���p�C�v���C���ɃZ�b�g
        /*UINT stride = sizeof(VERTEX);
        UINT offset = 0;*/

        if (pMaterialList_[i].pTexture)
        {
            ID3D11SamplerState* pSampler = pMaterialList_[i].pTexture->GetSampler();
            Direct3D::pContext_->PSSetSamplers(0, 1, &pSampler);

            ID3D11ShaderResourceView* pSRV = pMaterialList_[i].pTexture->GetSRV();
            Direct3D::pContext_->PSSetShaderResources(0, 1, &pSRV);
        }

        //ID3D11ShaderResourceView* pSRVToon = pToonTex_->GetSRV();
        //Direct3D::pContext_->PSSetShaderResources(1, 1, &pSRVToon);

        //�`��
        Direct3D::pContext_->DrawIndexed(indexCount_[i], 0, 0);
    }
}

void Fbx::DrawSkinAnime(Transform& _transform, FbxTime _time)
{
    //���{�[�����Ƃ̌��݂̍s����擾����
    for (int i = 0; i < numBone_; i++)
    {
        FbxAnimEvaluator* evaluator = ppCluster_[i]->GetLink()->GetScene()->GetAnimationEvaluator();
        FbxMatrix mCurrentOrentation = evaluator->GetNodeGlobalTransform(ppCluster_[i]->GetLink(), _time);

        //���s��R�s�[(Fbx�`������DirectX�ւ̕ϊ�)
        XMFLOAT4X4 pose;
        for (DWORD x = 0; x < 4; x++)
        {
            for (DWORD y = 0; y < 4; y++)
            {
                pose(x, y) = (float)mCurrentOrentation.Get(x, y);
            }
        }

        //���I�t�Z�b�g���̃|�[�Y�̍������v�Z����
        pBoneArray_[i].newPose = XMLoadFloat4x4(&pose);
        pBoneArray_[i].diffPose = XMMatrixInverse(nullptr, pBoneArray_[i].bindPose);
        pBoneArray_[i].diffPose *= pBoneArray_[i].newPose;
    }

    //���e�{�[���ɑΉ��������_�̕ό`����
    for (DWORD i = 0; i < vertexCount_; i++)
    {
        //���e���_���ƂɁA�u�e������{�[��X�E�F�C�g�l�v�𔽉f�������֐ߍs����쐬����
        XMMATRIX matrix;
        ZeroMemory(&matrix, sizeof(matrix));
        for (int m = 0; m < numBone_; m++)
        {
            if (pWeightArray_[i].pBoneIndex[m] < 0)
            {
                break;
            }
            matrix += pBoneArray_[pWeightArray_[i].pBoneIndex[m]].diffPose * pWeightArray_[i].pBoneWeight[m];

        }

        //���쐬���ꂽ�֐ߍs����g���āA���_��ό`����
        XMVECTOR Pos = XMLoadFloat3(&pWeightArray_[i].posOrigin);
        XMVECTOR Normal = XMLoadFloat3(&pWeightArray_[i].normalOrigin);
        //XMStoreFloat3(&pVertexData_[i].position, XMVector3TransformCoord(Pos, matrix));
        //XMStoreFloat3(&pVertexData_[i].normal, XMVector3TransformCoord(Normal, matrix));
        pVertexData_[i].position = XMVector3TransformCoord(Pos, matrix);
        pVertexData_[i].normal = XMVector3TransformCoord(Normal, matrix);
    }

    //�����_�o�b�t�@�����b�N���āA�ό`��������̒��_���ŏ㏑������
    D3D11_MAPPED_SUBRESOURCE msr = {};
    Direct3D::pContext_->Map(pVertexBuffer_, 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);
    if (msr.pData)
    {
        memcpy_s(msr.pData, msr.RowPitch, pVertexData_, sizeof(VERTEX) * vertexCount_);
        Direct3D::pContext_->Unmap(pVertexBuffer_, 0);
    }
    Draw(_transform);
}

void Fbx::DrawMeshAnime(Transform& _transform, FbxTime _time, FbxScene* _scene)
{
    Draw(_transform);
}

XMFLOAT3 Fbx::GetBonePosition(string _boneName)
{
    XMFLOAT3 position = XMFLOAT3(0, 0, 0);
    for (int i = 0; i < parts_.size(); i++)
    {
        if (parts_[i]->GetBonePosition(_boneName, &position))
        {
            break;
        }
    }
    return position;
}

bool Fbx::GetBonePosition(string _boneName, XMFLOAT3* _position)
{
    for (int i = 0; i < numBone_; i++)
    {
        if (_boneName == ppCluster_[i]->GetLink()->GetName())
        {
            FbxAMatrix matrix;
            ppCluster_[i]->GetTransformLinkMatrix(matrix);

            _position->x = (float)matrix[3][0];
            _position->y = (float)matrix[3][1];
            _position->z = (float)matrix[3][2];

            return true;
        }
    }
}

void Fbx::CheckNode(FbxNode* _pNode, vector<Fbx*>* _pPartsList)
{
    //���̃m�[�h�ɂ̓��b�V����񂪓����Ă��邾�낤���H
    FbxNodeAttribute* attr = _pNode->GetNodeAttribute();
    if (attr != nullptr && attr->GetAttributeType() == FbxNodeAttribute::eMesh)
    {
        Fbx* pParts = new Fbx;
        pParts->Init(_pNode);

        _pPartsList->push_back(pParts);
    }

    //�q�m�[�h�ɂ��f�[�^�����邩��
    //�q���̐��𒲂ׂ�
    int childCount = _pNode->GetChildCount();

    for (int i = 0; i < childCount; i++)
    {
        CheckNode(_pNode->GetChild(i), _pPartsList);
    }
}

void Fbx::Release()
{

}

void Fbx::RayCast(RayCastData* _pData)
{
    //�S�Ẵp�[�c�Ɣ���
    for (int i = 0; i < parts_.size(); i++)
    {
        parts_[i]->RayCast(_pData);
    }
    _pData->hit = false;

    //�}�e���A����
    for (DWORD i = 0; i < materialCount_; i++)
    {
        //���̃}�e���A���̃|���S����
        for (DWORD j = 0; j < polygonCount_; j++)
        {
            XMFLOAT3 ver[3];
            XMStoreFloat3(&ver[0], pVertexData_[ppIndexData_[i][j * 3 + 0]].position);
            XMStoreFloat3(&ver[1], pVertexData_[ppIndexData_[i][j * 3 + 1]].position);
            XMStoreFloat3(&ver[2], pVertexData_[ppIndexData_[i][j * 3 + 2]].position);

            BOOL  hit = false;
            float dist = 0.0f;

            hit = Direct3D::Intersect(_pData->start, _pData->dir, ver[0], ver[1], ver[2], &dist);


            if (hit && dist < _pData->dist)
            {
                _pData->hit = true;
                _pData->dist = dist;
            }
        }
    }
}




