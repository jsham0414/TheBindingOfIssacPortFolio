#include "PreCompile.h"
#include "MapEditorWindow.h"
#include "TileMapActor.h"
#include "GameEngineContents/MapEditorLevel.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

class GameEngineCore;

MapEditorWindow::MapEditorWindow() : 
    Scale{52, 52}, 
    SelectTile(-1), 
    Selected(0) {

}

MapEditorWindow::~MapEditorWindow() {
    for (auto& iter : BatchInfos) {
        iter->Texture->Release();
        SAFE_DELETE(iter);
    }
    BatchInfos.clear();
}


void MapEditorWindow::Initialize(class GameEngineLevel* _Level) {
    // 1. 메타 파일을 읽어서 OnGUI ListBox에 읽힌 몬스터 이름으로 Index를 추출
    // 2. 격자 클릭 시 피킹으로 몬스터 배치
    // 3. 배치 된 데이터를 저장
    if (false == GameEngineInput::GetInst()->IsKey("TileSet")) {
        GameEngineInput::GetInst()->CreateKey("TileSet", VK_LBUTTON);
    }

    std::list<GameEngineActor*> temp = GameEngineCore::GetCurLevel()->GetActors(static_cast<int>(OBJECTORDER::TileMapObject));

    int index = 0;
    for (auto a : temp) {
        // 이름, 텍스쳐, 구분은 Index로
        ObjectBatchInfo* batchInfo = new ObjectBatchInfo();
        batchInfo->Index = index++;
        batchInfo->Name = a->GetNameCopy();
        batchInfo->Renderer = a->GetRenderer();

        GameEngineDirectory Dir;
        Dir.MoveParentToExitsChildDirectory("ContentsResources");
        Dir.Move("ContentsResources");
        Dir.Move("Texture");
        Dir.Move("Play");
        Dir.Move(batchInfo->Renderer->GetCurTexture()->GetNameCopy());

        ID3D11ShaderResourceView* _Texture;
        bool Result = LoadTextureFromFile(Dir.GetFullPath().c_str(), &_Texture, nullptr, nullptr);

        batchInfo->Texture = _Texture;
        
        BatchInfos.push_back(batchInfo);
    }
}

bool MapEditorWindow::LoadTextureFromFile(const char* filename, ID3D11ShaderResourceView** out_srv, int* out_width, int* out_height) {
    // Load from disk into a raw RGBA buffer
    int image_width = 0;
    int image_height = 0;
    unsigned char* image_data = stbi_load(filename, &image_width, &image_height, NULL, 4);
    if (image_data == NULL)
        return false;

    // Create texture
    D3D11_TEXTURE2D_DESC desc;
    ZeroMemory(&desc, sizeof(desc));
    desc.Width = image_width;
    desc.Height = image_height;
    desc.MipLevels = 1;
    desc.ArraySize = 1;
    desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    desc.SampleDesc.Count = 1;
    desc.Usage = D3D11_USAGE_DEFAULT;
    desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
    desc.CPUAccessFlags = 0;

    ID3D11Texture2D* pTexture = NULL;
    D3D11_SUBRESOURCE_DATA subResource;
    subResource.pSysMem = image_data;
    subResource.SysMemPitch = desc.Width * 4;
    subResource.SysMemSlicePitch = 0;
    GameEngineDevice::GetDevice()->CreateTexture2D(&desc, &subResource, &pTexture);

    // Create texture view
    D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
    ZeroMemory(&srvDesc, sizeof(srvDesc));
    srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Texture2D.MipLevels = desc.MipLevels;
    srvDesc.Texture2D.MostDetailedMip = 0;
    GameEngineDevice::GetDevice()->CreateShaderResourceView(pTexture, &srvDesc, out_srv);
    pTexture->Release();

    if (out_width != nullptr) *out_width = image_width;
    if (out_height != nullptr) *out_height = image_height;
    stbi_image_free(image_data);

    return true;
}

void MapEditorWindow::OnGUI(GameEngineLevel* _Level, float _DeltaTime) {
    if (BatchInfos.size() > 0) {
        //if (ImGui::BeginCombo("Select object", BatchInfos[Selected]->Name.c_str())) {
        //    for (int n = 0; n < BatchInfos.size(); n++) {
        //        const bool is_selected = (Selected == n);
        //        if (ImGui::Selectable(BatchInfos[n]->Name.c_str(), is_selected))
        //            Selected = n;

        //        // Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
        //        if (is_selected)
        //            ImGui::SetItemDefaultFocus();
        //    }
        //    ImGui::EndCombo();
        //}        

        for (int i = 0; i < BatchInfos.size(); i++) {
            auto& BatchInfo = BatchInfos[i];

            // UV coordinates are often (0.0f, 0.0f) and (1.0f, 1.0f) to display an entire textures.
            // Here are trying to display only a 32x32 pixels area of the texture, hence the UV computation.
            // Read about UV coordinates here: https://github.com/ocornut/imgui/wiki/Image-Loading-and-Displaying-Examples
            //if (i > 0)
                //ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(i - 1.0f, i - 1.0f));

            ImGui::PushID(i);
            ImVec2 size = ImVec2(32.0f, 32.0f);                         // Size of the image we want to make visible
            ImVec2 uv0 = ImVec2(0.0f, 0.0f);                            // UV coordinates for lower-left
            ImVec2 uv1 = ImVec2(
                BatchInfo->Renderer->GetCurTexture()->GetCutScale(0).x / BatchInfo->Renderer->GetCurTexture()->GetScale().x, 
                BatchInfo->Renderer->GetCurTexture()->GetCutScale(0).y / BatchInfo->Renderer->GetCurTexture()->GetScale().y
            );    // UV coordinates for (32,32) in our texture
            if (ImGui::ImageButton((void*)BatchInfo->Texture, size, uv0, uv1)) {
                Selected = i;
            }
            ImGui::SameLine();
            ImGui::PopID();
        }
        ImGui::NewLine();
    }

    
    const char* items[] = { "Start", "Room", "Boss", "Shop", "Gold" };
    static int item_current_idx = 0; // Here we store our selection data as an index.
    const char* combo_preview_value = items[item_current_idx];  // Pass in the preview value visible before opening the combo (it could be anything)
    if (ImGui::BeginCombo("combo 1", combo_preview_value)) {
        for (int n = 0; n < IM_ARRAYSIZE(items); n++) {
            const bool is_selected = (item_current_idx == n);
            if (ImGui::Selectable(items[n], is_selected))
                item_current_idx = n;

            // Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
            if (is_selected)
                ImGui::SetItemDefaultFocus();
        }
        ImGui::EndCombo();
    }

    static char buf2[64] = "0"; ImGui::InputText("Stage Number", buf2, 64, ImGuiInputTextFlags_CharsDecimal);
    //GameRoomType
    if (true == ImGui::Button("Save")) {
        auto BatchData = ObjectBatcher->GetBatchData();
        string Result = "";
        Json::Value Temp;
        Temp["RoomType"] = item_current_idx + 1;
        Temp["Stage"] = buf2;
        for (int i = 0; i < 7 * 16; i++) {
            if (BatchData->find(i) == BatchData->end())
                continue;

            Json::Value Indent;
            Indent["Type"] = BatchData->at(i)->GetType();
            Temp[to_string(i)] = Indent;
        }

        Json::StyledWriter Writer;
        Result = Writer.write(Temp);
        string RoomType = items[item_current_idx];
        string Path = "../GameEngineData\\MapForm\\" + RoomType + '\\' + GameEngineTime::GetDateTimeString(true) + ".json";
        GameEngineFile file(Path);
        file.Open(OpenMode::Write, FileMode::Binary);
        file.Write((void*)Result.c_str(), Result.length());

        // 

        //static_cast<MapEditorLevel>()

        //GameEngineFile::Write()
    }

    return;

    // ImGui::PushID
	if (true == ImGui::Button("FolderTextureLoad")) {
        GameEngineDirectory Dir;
        Dir.MoveParentToExitsChildDirectory("ContentsResources");
        Dir.Move("ContentsResources");
        Dir.Move("Texture");
        Dir.Move("Map");



        std::string Path =  GameEngineGUI::OpenFolderDlg(GameEngineString::AnsiToUTF8Return("폴더 텍스처 로드"), Dir.GetFullPath());

        if (false == Path.empty())
        {
            SelectFolderTexture = GameEnginePath::GetFileName(Path);

            GameEngineFolderTexture::Load(Path.c_str());
        }
	}

    if (false == SelectFolderTexture.empty())
    {
        ImGui::SameLine();
        ImGui::Text(SelectFolderTexture.c_str());
        ImGui::InputInt2("Scale", Scale);

        if (true == ImGui::Button("MapCreate"))
        {
            TileMap->TileRenderer->CreateIsometricTileMap(Scale[0], Scale[1], {64, 32}, SelectFolderTexture, 32);
        }


        std::string SelectIndex = "Select Index = ";
        int X, Y;
        TileMap->TileRenderer->GetTileIndex(_Level->GetMainCamera()->GetMouseWorldPosition(), X, Y);

        SelectIndex += std::to_string(X);
        SelectIndex += " " + std::to_string(Y);

        ImGui::Text(SelectIndex.c_str());
    }

    GameEngineFolderTexture* Texture = GameEngineFolderTexture::Find(SelectFolderTexture);

    if (nullptr != Texture)
    {
        ImGui::BeginChildFrame(ImGui::GetID("TileSelect"), { 90 * 5, 500 });

        for (size_t i = 0; i < Texture->GetTextureCount(); i++)
        {
            GameEngineTexture* TileImage = Texture->GetTexture(i);
            if (ImGui::ImageButton(reinterpret_cast<ImTextureID>(TileImage->CreateShaderResourceView()), { 64, 32 }))
            {
                SelectTile = i;
            }
            
            if (0 != (i + 1)% 5)
            {
                ImGui::SameLine();
            }
        }
        ImGui::EndChildFrame();
    }

    if (true == GameEngineInput::GetInst()->IsDown("TileSet")
        && nullptr != Texture
        && -1 != SelectTile
        && SelectTile < Texture->GetTextureCount())
    {

        float4 MousePos = _Level->GetMainCamera()->GetMouseWorldPosition();
        // GameEngineInput::

        TileMap->TileRenderer->SetTileIndex(MousePos, SelectTile);
        int a = 0;
    }
    // TileMap->TileRenderer->

}