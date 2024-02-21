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
    //ノードからメッシュの情報を取得
    FbxMesh* mesh = _pNode->GetMesh();

    //各情報の個数を取得
    vertexCount_ = mesh->GetControlPointsCount();			//頂点の数
    polygonCount_ = mesh->GetPolygonCount();				//ポリゴンの数
    polygonVertexCount_ = mesh->GetPolygonVertexCount();	//ポリゴン頂点インデックス数 
    materialCount_ = _pNode->GetMaterialCount();     //マテリアルの数


    InitVertex(mesh);		//頂点バッファ準備
    InitMaterial(_pNode);	//マテリアル準備
    InitIndex(mesh);		//インデックスバッファ準備
    InitSkelton(mesh);		//骨の情報を準備
    InitConstantBuffer();	//コンスタントバッファ（シェーダーに情報を送るやつ）準備

    return E_NOTIMPL;
}

HRESULT Fbx::Load(std::string _fileName)
{
    FbxManager* pFbxManager = FbxManager::Create();                        //マネージャを生成
    pFbxScene_ = FbxScene::Create(pFbxManager, "fbxscene");                //シーンオブジェクトにFBXファイルの情報を流し込む
    FbxImporter* fbxImporter = FbxImporter::Create(pFbxManager, "imp");    //インポーターを生成
    fbxImporter->Initialize(_fileName.c_str(), -1, pFbxManager->GetIOSettings());
    fbxImporter->Import(pFbxScene_);
    fbxImporter->Destroy();

    frameRate_ = pFbxScene_->GetGlobalSettings().GetTimeMode();

    //メッシュ情報を取得
    //ルートノードを取得して
    FbxNode* rootNode = pFbxScene_->GetRootNode();

    //そいつの子供の数を調べて
    int childCount = rootNode->GetChildCount();
    FbxNode* pNode = rootNode->GetChild(0);
    FbxMesh* mesh = pNode->GetMesh();

    char defaultCurrentDir[MAX_PATH];                       //現在のカレントディレクトリを取得
    GetCurrentDirectory(MAX_PATH, defaultCurrentDir);

    char dir[MAX_PATH];                                     //引数のfileNameからディレクトリ部分を取得
    _splitpath_s(_fileName.c_str(), nullptr, 0, dir, MAX_PATH, nullptr, 0, nullptr, 0);
    SetCurrentDirectory(dir);                               //カレントディレクトリ変更

    for (int i = 0; childCount > i; i++)
    {
        CheckNode(rootNode->GetChild(i), &parts_);
    }

    ////各情報の個数を取得
    //vertexCount_  = mesh->GetControlPointsCount();	//頂点の数
    //polygonCount_ = mesh->GetPolygonCount();	        //ポリゴンの数
    
    //InitVertex(mesh);		//頂点バッファ準備
    //InitIndex(mesh);		//インデックスバッファ準備
    //InitConstantBuffer();	//コンスタントバッファ準備
    //InitMaterial(pNode);  //ノードからマテリアルの情報を引き出す

    SetCurrentDirectory(defaultCurrentDir);                 //カレントディレクトリを元に戻す

    //マネージャ解放
    //pFbxManager->Destroy();

    return S_OK;
}

//頂点バッファ準備
void Fbx::InitVertex(fbxsdk::FbxMesh* _mesh)
{
    //頂点情報を入れる配列
    pVertexData_ = new VERTEX[vertexCount_];

    //全ポリゴン
    for (DWORD poly = 0; poly < polygonCount_; poly++)
    {
        //3頂点分
        for (int vertex = 0; vertex < 3; vertex++)
        {
            //調べる頂点の番号
            int index = _mesh->GetPolygonVertex(poly, vertex);

            //頂点の位置
            FbxVector4 pos = _mesh->GetControlPointAt(index);
            pVertexData_[index].position = XMVectorSet((float)pos[0], (float)pos[1], (float)pos[2], 0.0f);

            //頂点のUV
            FbxLayerElementUV* pUV = _mesh->GetLayer(0)->GetUVs();
            int uvIndex = _mesh->GetTextureUVIndex(poly, vertex, FbxLayerElement::eTextureDiffuse);
            FbxVector2 uv = pUV->GetDirectArray().GetAt(uvIndex);
            pVertexData_[index].uv = XMVectorSet((float)uv.mData[0], (float)(1.0f - uv.mData[1]), 0.0f, 0.0f);

            //頂点の法線
            FbxVector4 Normal;
            _mesh->GetPolygonVertexNormal(poly, vertex, Normal);	    //i番目のポリゴンの、j番目の頂点の法線をゲット
            pVertexData_[index].normal = XMVectorSet((float)Normal[0], (float)Normal[1], (float)Normal[2], 0.0f);
        }
    }
    // 頂点バッファ作成
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
		MessageBox(NULL, "頂点バッファの作成に失敗しました", "エラー", MB_OK);
	}
}

//インデックスバッファ準備
void Fbx::InitIndex(fbxsdk::FbxMesh* mesh)
{
//  int* index = new int[polygonCount_ * 3];
//  int count = 0;
    pIndexBuffer_ = new ID3D11Buffer * [materialCount_];
    indexCount_ = vector<int>(materialCount_);
    ppIndexData_ = new DWORD * [materialCount_];
//    indexCount_ = new int[materialCount_];

//    for (DWORD poly = 0; poly < polygonCount_; poly++)

    vector<int> index(polygonCount_ * 3);//ポリゴン数　* 3 = 全頂点分用意
//    int* index = new int[polygonCount_ * 3];

    for (int i = 0; i < materialCount_; i++)
    {
        //    //3頂点分
        //    for (DWORD vertex = 0; vertex < 3; vertex++)
        //    {
        //        index[count] = mesh->GetPolygonVertex(poly, vertex);
        //        count++;
        //    }
        //}

        int count = 0;

        //全ポリゴン
        for (DWORD poly = 0; poly < polygonCount_; poly++)
        {
            //あるマテリアルを持ったポリゴンのリストをとってきて、頂点をリストアップ
            FbxLayerElementMaterial* mtl = mesh->GetLayer(0)->GetMaterials();
            int mtlId = mtl->GetIndexArray().GetAt(poly);

            if (mtlId == i)
            {
                //3頂点分
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
            MessageBox(NULL, "インデックスバッファの作成に失敗しました", "エラー", MB_OK);
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

    // コンスタントバッファの作成
    HRESULT hr;
    hr = Direct3D::pDevice_->CreateBuffer(&cb, nullptr, &pConstantBuffer_);
    if (FAILED(hr))
    {
        MessageBox(NULL, "コンスタントバッファの作成に失敗しました", "エラー", MB_OK);
    }
}

void Fbx::InitMaterial(fbxsdk::FbxNode* _pNode)
{
    pMaterialList_ = new MATERIAL[materialCount_];

    for (int i = 0; i < materialCount_; i++)
    {
        //i番目のマテリアル情報を取得
        FbxSurfacePhong* pMaterial = (FbxSurfacePhong*)(_pNode->GetMaterial(i));
        //FbxSurfaceMaterial* pMaterial = pNode->GetMaterial(i);
        FbxSurfacePhong* pPhong = (FbxSurfacePhong*)pMaterial;
        FbxDouble3 diffuse = pMaterial->Diffuse;
        FbxDouble3 ambient = pMaterial->Ambient;    //XMFLOAT4

        pMaterialList_[i].diffuse = XMFLOAT4((float)diffuse[0], (float)diffuse[1], (float)diffuse[2], 1.0f);
        pMaterialList_[i].ambient = XMFLOAT4((float)ambient[0], (float)ambient[1], (float)ambient[2], 1.0f);
        pMaterialList_[i].specular = XMFLOAT4(0, 0, 0, 0);   //とりあえずハイライトは黒
        pMaterialList_[i].shineness = 1;

        if (pMaterial->GetClassId().Is(FbxSurfacePhong::ClassId))
        {
            //Mayaで指定したSpecularColorの情報
            FbxDouble3 specular = pPhong->Specular;
            pMaterialList_[i].specular = XMFLOAT4((float)specular[0], (float)specular[1], (float)specular[2], 1.0f);

            FbxDouble shineness = pPhong->Shininess;
            pMaterialList_[i].shineness = (float)pPhong->Shininess;
        }

        pMaterialList_[i].diffuse = XMFLOAT4{ (float)diffuse[0],(float)diffuse[1] ,(float)diffuse[2] ,1.0};
        //テクスチャ情報
        FbxProperty  lProperty = pMaterial->FindProperty(FbxSurfaceMaterial::sDiffuse);

        //テクスチャの数数
        int fileTextureCount = lProperty.GetSrcObjectCount<FbxFileTexture>();

        //テクスチャあり
        if (fileTextureCount)
        {
            FbxFileTexture* textureInfo = lProperty.GetSrcObject<FbxFileTexture>(0);
            const char* textureFilePath = textureInfo->GetRelativeFileName();

            //ファイル名+拡張だけにする
            char name[_MAX_FNAME];	//ファイル名
            char ext[_MAX_EXT];	    //拡張子
            _splitpath_s(textureFilePath, nullptr, 0, nullptr, 0, name, _MAX_FNAME, ext, _MAX_EXT);
            wsprintf(name, "%s%s", name, ext);

            //ファイルからテクスチャ作成
            pMaterialList_[i].pTexture = new Texture;
            HRESULT hr = pMaterialList_[i].pTexture->Load(name);
            assert(hr == S_OK);
        }

        //テクスチャ無し
        else
        {
            pMaterialList_[i].pTexture = nullptr;
            //マテリアルの色
            FbxSurfaceLambert* pMaterial = (FbxSurfaceLambert*)_pNode->GetMaterial(i);
        }


    }
}

void Fbx::InitSkelton(FbxMesh* _pMesh)
{
    FbxDeformer* pDeformer = _pMesh->GetDeformer(0);
    if (pDeformer == nullptr)
    {
        //ボーン情報なし
        return;
    }

    //デフォーマ情報からスキンメッシュ情報を取得
    pSkinInfo_ = (FbxSkin*)pDeformer;

    //頂点からポリゴンを逆引きするための情報を作成する
    struct POLY_INDEX
    {
        int* polyIndex;
        int* vertexIndex;
        int  numRef;
    };

    POLY_INDEX* polyTable = new POLY_INDEX[vertexCount_];
    for (DWORD i = 0; i < vertexCount_; i++)
    {
        //三角形ポリゴンに合わせて、頂点とポリゴンの関連情報を構築する
        //総頂点数=ポリゴン数*3頂点
        polyTable[i].polyIndex = new int[polygonCount_ * 3];
        polyTable[i].vertexIndex = new int[polygonCount_ * 3];
        polyTable[i].numRef = 0;
        ZeroMemory(polyTable[i].polyIndex, sizeof(int)* polygonCount_ * 3);
        ZeroMemory(polyTable[i].vertexIndex, sizeof(int)* polygonCount_ * 3);

        //ポリゴン間で共有する頂点を列挙する
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

    //▼ボーン情報を取得する
    numBone_ = pSkinInfo_->GetClusterCount();
    ppCluster_ = new FbxCluster * [numBone_];
    for (int i = 0; i < numBone_; i++)
    {
        ppCluster_[i] = pSkinInfo_->GetCluster(i);
    }

    //▼ボーンの数に合わせてウェイト情報を準備する
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

    //▼それぞれのボーンに影響を受ける頂点を調べる
    //そこから逆に、頂点ベースでボーンインデックス：重みを整頓する
    for (int i = 0; i < numBone_; i++)
    {
        int numIndex = ppCluster_[i]->GetControlPointIndicesCount();    //このボーンに影響を受ける頂点数
        int* piIndex = ppCluster_[i]->GetControlPointIndices();         //ボーンウェイト情報の番号
        double* pdWeight = ppCluster_[i]->GetControlPointWeights();     //頂点ごとのウェイト情報

        for (int k = 0; k < numIndex; k++)
        {
            //頂点に関連つけられたウェイト情報がボーン5本以上の場合は、重みの大きい順に4本に絞る
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

    //▼ボーンを作成
    pBoneArray_ = new Fbx::Bone[numBone_];
    for (int i = 0; i < numBone_; i++)
    {
        //ボーンのデフォルト位置を取得する
        FbxAMatrix matrix;
        ppCluster_[i]->GetTransformLinkMatrix(matrix);

        //行列コピー(Fbx形式からDirectXへの変換)
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

    //一時的なメモリ領域を解放する
    for (DWORD i = 0; i < vertexCount_; i++)
    {
        SAFE_DELETE_ARRAY(polyTable[i].polyIndex);
        SAFE_DELETE_ARRAY(polyTable[i].vertexIndex);
    }
    SAFE_DELETE_ARRAY(polyTable);
}

//テクスチャをロード
void Fbx::Draw(Transform& _transform, int _frame)
{
    //▼その瞬間の自分の姿勢行列を得る
    FbxTime time;
    time.SetTime(0, 0, 0, _frame, 0, 0, frameRate_);

    for (int k = 0; k < parts_.size(); k++)
    {
        //スキンアニメーション(ボーン有り)の場合
        if (parts_[k]->GetSkinInfo() != nullptr)
        {
            parts_[k]->DrawSkinAnime(_transform, time);
        }
        //メッシュアニメーションの場合
        else
        {
            parts_[k]->DrawMeshAnime(_transform, time, pFbxScene_);
        }
    }
    _transform.Calclation();//トランスフォームを計算
}

void Fbx::Draw(Transform& _transform)
{
    //頂点バッファ
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

        //コンスタントバッファに情報を渡す
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

        // インデックスバッファーをセット
        stride = sizeof(int);
        offset = 0;
        Direct3D::pContext_->IASetIndexBuffer(pIndexBuffer_[i], DXGI_FORMAT_R32_UINT, 0);

        //コンスタントバッファ
        Direct3D::pContext_->VSSetConstantBuffers(0, 1, &pConstantBuffer_);	//頂点シェーダー用	
        Direct3D::pContext_->PSSetConstantBuffers(0, 1, &pConstantBuffer_);	//ピクセルシェーダー用

        ////描画
        //Direct3D::pContext_->DrawIndexed(polygonCount_ * 3, 0, 0);
        //D3D11_MAPPED_SUBRESOURCE pdata;
        //Direct3D::pContext_->Map(pConstantBuffer_, 0, D3D11_MAP_WRITE_DISCARD, 0, &pdata);  //GPUからのデータアクセス
        //memcpy_s(pdata.pData, pdata.RowPitch, (void*)(&cb), sizeof(cb));                    //データを送る

        //Direct3D::pContext_->Unmap(pConstantBuffer_, 0);     //再開

        //頂点バッファ、インデックスバッファ、コンスタントバッファをパイプラインにセット
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

        //描画
        Direct3D::pContext_->DrawIndexed(indexCount_[i], 0, 0);
    }
}

void Fbx::DrawSkinAnime(Transform& _transform, FbxTime _time)
{
    //▼ボーンごとの現在の行列を取得する
    for (int i = 0; i < numBone_; i++)
    {
        FbxAnimEvaluator* evaluator = ppCluster_[i]->GetLink()->GetScene()->GetAnimationEvaluator();
        FbxMatrix mCurrentOrentation = evaluator->GetNodeGlobalTransform(ppCluster_[i]->GetLink(), _time);

        //▼行列コピー(Fbx形式からDirectXへの変換)
        XMFLOAT4X4 pose;
        for (DWORD x = 0; x < 4; x++)
        {
            for (DWORD y = 0; y < 4; y++)
            {
                pose(x, y) = (float)mCurrentOrentation.Get(x, y);
            }
        }

        //▼オフセット時のポーズの差分を計算する
        pBoneArray_[i].newPose = XMLoadFloat4x4(&pose);
        pBoneArray_[i].diffPose = XMMatrixInverse(nullptr, pBoneArray_[i].bindPose);
        pBoneArray_[i].diffPose *= pBoneArray_[i].newPose;
    }

    //▼各ボーンに対応した頂点の変形制御
    for (DWORD i = 0; i < vertexCount_; i++)
    {
        //▼各頂点ごとに、「影響するボーンXウェイト値」を反映させた関節行列を作成する
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

        //▼作成された関節行列を使って、頂点を変形する
        XMVECTOR Pos = XMLoadFloat3(&pWeightArray_[i].posOrigin);
        XMVECTOR Normal = XMLoadFloat3(&pWeightArray_[i].normalOrigin);
        //XMStoreFloat3(&pVertexData_[i].position, XMVector3TransformCoord(Pos, matrix));
        //XMStoreFloat3(&pVertexData_[i].normal, XMVector3TransformCoord(Normal, matrix));
        pVertexData_[i].position = XMVector3TransformCoord(Pos, matrix);
        pVertexData_[i].normal = XMVector3TransformCoord(Normal, matrix);
    }

    //▼頂点バッファをロックすて、変形させた後の頂点情報で上書きする
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
    //そのノードにはメッシュ情報が入っているだろうか？
    FbxNodeAttribute* attr = _pNode->GetNodeAttribute();
    if (attr != nullptr && attr->GetAttributeType() == FbxNodeAttribute::eMesh)
    {
        Fbx* pParts = new Fbx;
        pParts->Init(_pNode);

        _pPartsList->push_back(pParts);
    }

    //子ノードにもデータがあるかも
    //子供の数を調べて
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
    //全てのパーツと判定
    for (int i = 0; i < parts_.size(); i++)
    {
        parts_[i]->RayCast(_pData);
    }
    _pData->hit = false;

    //マテリアル毎
    for (DWORD i = 0; i < materialCount_; i++)
    {
        //そのマテリアルのポリゴン毎
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




