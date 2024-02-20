#include <assert.h>
#include "Fbx.h"
#include "Direct3D.h"
#include "Camera.h"
#include "Texture.h"

using namespace DirectX;

Fbx::Fbx()
    :vertexCount_(0),polygonCount_(0),materialCount_(0),
    pVertexBuffer_(nullptr),pIndexBuffer_(nullptr),pConstantBuffer_(nullptr),
    pMaterialList_(nullptr),ppIndexData_(nullptr),pVertexData_(nullptr)
{
}

HRESULT Fbx::Load(std::string fileName)
{
    //マネージャを生成
    FbxManager* pFbxManager = FbxManager::Create();

    //インポーターを生成
    FbxImporter* fbxImporter = FbxImporter::Create(pFbxManager, "imp");
    fbxImporter->Initialize(fileName.c_str(), -1, pFbxManager->GetIOSettings());

    //シーンオブジェクトにFBXファイルの情報を流し込む
    FbxScene* pFbxScene = FbxScene::Create(pFbxManager, "fbxscene");
    fbxImporter->Import(pFbxScene);
    fbxImporter->Destroy();

    //メッシュ情報を取得
    //ルートノードを取得して
    FbxNode* rootNode = pFbxScene->GetRootNode();

    //そいつの子供の数を調べて
    FbxNode* pNode = rootNode->GetChild(0);
    FbxMesh* mesh = pNode->GetMesh();

    //各情報の個数を取得
    vertexCount_  = mesh->GetControlPointsCount();	//頂点の数
    polygonCount_ = mesh->GetPolygonCount();	    //ポリゴンの数
    materialCount_ = pNode->GetMaterialCount();     //マテリアルの数


    //現在のカレントディレクトリを取得
    char defaultCurrentDir[MAX_PATH];
    GetCurrentDirectory(MAX_PATH, defaultCurrentDir);

    //引数のfileNameからディレクトリ部分を取得
    char dir[MAX_PATH];
    _splitpath_s(fileName.c_str(), nullptr, 0, dir, MAX_PATH, nullptr, 0, nullptr, 0);

    //カレントディレクトリ変更
    SetCurrentDirectory(dir);

    InitVertex(mesh);		//頂点バッファ準備
    InitIndex(mesh);		//インデックスバッファ準備
    InitConstantBuffer();	//コンスタントバッファ準備
    InitMaterial(pNode);    //ノードからマテリアルの情報を引き出す

    //カレントディレクトリを元に戻す
    SetCurrentDirectory(defaultCurrentDir);

    //マネージャ解放
    pFbxManager->Destroy();
    pToonTex_ = new Texture;
    //pToonTex_->Load("Assets\\toon2.png");

    return S_OK;
}

//頂点バッファ準備
void Fbx::InitVertex(fbxsdk::FbxMesh* mesh)
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
            int index = mesh->GetPolygonVertex(poly, vertex);

            //頂点の位置
            FbxVector4 pos = mesh->GetControlPointAt(index);
            pVertexData_[index].position = XMVectorSet((float)pos[0], (float)pos[1], (float)pos[2], 0.0f);

            //頂点のUV
            FbxLayerElementUV* pUV = mesh->GetLayer(0)->GetUVs();
            int uvIndex = mesh->GetTextureUVIndex(poly, vertex, FbxLayerElement::eTextureDiffuse);
            FbxVector2 uv = pUV->GetDirectArray().GetAt(uvIndex);
            pVertexData_[index].uv = XMVectorSet((float)uv.mData[0], (float)(1.0f - uv.mData[1]), 0.0f, 0.0f);

            //頂点の法線
            FbxVector4 Normal;
            mesh->GetPolygonVertexNormal(poly, vertex, Normal);	    //i番目のポリゴンの、j番目の頂点の法線をゲット
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
    cb.Usage = D3D11_USAGE_DEFAULT;
    cb.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    cb.CPUAccessFlags = 0;
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

void Fbx::InitMaterial(fbxsdk::FbxNode* pNode)
{
    pMaterialList_ = new MATERIAL[materialCount_];

    for (int i = 0; i < materialCount_; i++)
    {
        //i番目のマテリアル情報を取得
        FbxSurfacePhong* pMaterial = (FbxSurfacePhong*)(pNode->GetMaterial(i));
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
            FbxSurfaceLambert* pMaterial = (FbxSurfaceLambert*)pNode->GetMaterial(i);
        }


    }
}

//テクスチャをロード
void Fbx::Draw(Transform& _transform)
{

    _transform.Calclation();//トランスフォームを計算
    for (int i = 0; i < materialCount_; i++)
    {

        //コンスタントバッファに情報を渡す
        CONSTANT_BUFFER_MODEL cb;
        cb.matWVP = XMMatrixTranspose(_transform.GetWorldMatrix() * pCamera->GetViewMatrix() * pCamera->GetProjectionMatrix());
        cb.matNormal = XMMatrixTranspose(_transform.GetNormalMatrix());
        cb.matW = XMMatrixTranspose(_transform.GetNormalMatrix());

        cb.diffuseColor = pMaterialList_[i].diffuse;
        cb.ambientColor = pMaterialList_[i].ambient;
        cb.specularColor = pMaterialList_[i].specular;
        cb.shineness = pMaterialList_[i].shineness;
        cb.isTextured = pMaterialList_[i].pTexture != nullptr;

        Direct3D::pContext_->UpdateSubresource(pConstantBuffer_, 0, NULL, &cb, 0, 0);

        //頂点バッファ
        UINT stride = sizeof(VERTEX);
        UINT offset = 0;
        Direct3D::pContext_->IASetVertexBuffers(0, 1, &pVertexBuffer_, &stride, &offset);

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

        ID3D11ShaderResourceView* pSRVToon = pToonTex_->GetSRV();
        Direct3D::pContext_->PSSetShaderResources(1, 1, &pSRVToon);

        //描画
        Direct3D::pContext_->DrawIndexed(indexCount_[i], 0, 0);
    }
}

void Fbx::Release()
{

}

void Fbx::RayCast(RayCastData* data)
{
    //全てのパーツと判定
    for (int i = 0; i < parts_.size(); i++)
    {
        parts_[i]->RayCast(data);
    }
    data->hit = false;

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

            BOOL  hit = FALSE;
            float dist = 0.0f;

            hit = Direct3D::Intersect(data->start, data->dir, ver[0], ver[1], ver[2], &dist);


            if (hit && dist < data->dist)
            {
                data->hit = TRUE;
                data->dist = dist;
            }
        }
    }
}




