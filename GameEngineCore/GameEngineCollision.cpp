#include "PreCompile.h"
#include "GameEngineCollision.h"
#include "GameEngineCoreDebug.h"
#include "GameEngineCore.h"

bool (*GameEngineCollision::CollisionFunction[static_cast<int>(CollisionType::CT_MAX)][static_cast<int>(CollisionType::CT_MAX)])(const GameEngineTransform& _Left, const GameEngineTransform& _Right);

class GameEngineCollisionFunctionInit
{
public:
	GameEngineCollisionFunctionInit() 
	{
		memset(GameEngineCollision::CollisionFunction, 0, sizeof(GameEngineCollision::CollisionFunction));

		GameEngineCollision::CollisionFunction[static_cast<int>(CollisionType::CT_SPHERE)][static_cast<int>(CollisionType::CT_SPHERE)] = &GameEngineTransform::SphereToSphere;
		GameEngineCollision::CollisionFunction[static_cast<int>(CollisionType::CT_AABB)][static_cast<int>(CollisionType::CT_AABB)] = &GameEngineTransform::AABBToAABB;
		GameEngineCollision::CollisionFunction[static_cast<int>(CollisionType::CT_OBB)][static_cast<int>(CollisionType::CT_OBB)] = &GameEngineTransform::OBBToOBB;

		GameEngineCollision::CollisionFunction[static_cast<int>(CollisionType::CT_SPHERE2D)][static_cast<int>(CollisionType::CT_SPHERE2D)] = &GameEngineTransform::Sphere2DToSphere2D;
		GameEngineCollision::CollisionFunction[static_cast<int>(CollisionType::CT_AABB2D)][static_cast<int>(CollisionType::CT_AABB2D)] = &GameEngineTransform::AABB2DToAABB2D;
		GameEngineCollision::CollisionFunction[static_cast<int>(CollisionType::CT_OBB2D)][static_cast<int>(CollisionType::CT_OBB2D)] = &GameEngineTransform::OBB2DToOBB2D;
	}

	~GameEngineCollisionFunctionInit() 
	{

	}
};

GameEngineCollisionFunctionInit Inst;

GameEngineCollision::GameEngineCollision() 
	: DebugType(CollisionType::CT_SPHERE)
	, Color(1.0f, 0.0f, 0.0f, 0.5f)
{
}

GameEngineCollision::~GameEngineCollision() 
{
}


void GameEngineCollision::Start()
{
	DebugCameraOrder = CAMERAORDER::MAINCAMERA;

	GetActor()->GetLevel()->PushCollision(this, GetOrder());
}

void GameEngineCollision::SetUIDebugCamera()
{
	DebugCameraOrder = CAMERAORDER::UICAMERA;
}

void GameEngineCollision::ChangeOrder(int _Order)
{
	GetActor()->GetLevel()->PushCollision(this, _Order);
}

struct FRECT {
	float top, left, bottom, right;
};

bool FIntersectRect(FRECT* rcOut, FRECT* rc1, FRECT* rc2) {
	bool bVertical = false;
	bool bHorizontal = false;

	if (rc1->left < rc2->right && rc1->right > rc2->left) {
		bHorizontal = true;

		if (rcOut) {
			rcOut->left = (rc1->left > rc2->left) ? rc1->left : rc2->left;
			rcOut->right = (rc1->right > rc2->right) ? rc1->right : rc2->right;
		}
	}

	if (rc1->top < rc2->bottom && rc1->bottom > rc2->top) {
		bVertical = true;

		if (rcOut) {
			rcOut->top = (rc1->top > rc2->top) ? rc1->top : rc2->top;
			rcOut->bottom = (rc1->bottom > rc2->bottom) ? rc1->bottom : rc2->bottom;
		}
	}

	if (bHorizontal && bVertical)
		return true;

	if (rcOut)
		rcOut = nullptr;

	return false;
}

bool GameEngineCollision::IsCollision(CollisionType _ThisType, int _GroupOrder
	, CollisionType _OtherType
	, std::function<bool(GameEngineCollision* _This, GameEngineCollision* _Other)> _Function /*= nullptr*/)
{
	if (false == IsUpdate())
	{
		return false;
	}

	int ThisType = static_cast<int>(_ThisType);
	int OtherType = static_cast<int>(_OtherType);

	if (nullptr == GameEngineCollision::CollisionFunction[ThisType][OtherType])
	{
		MsgBoxAssert("아직 만들어지지 않은 콜리전 처리입니다");
	}

	std::map<int, std::list<GameEngineCollision*>>& AllCollisions = GetActor()->GetLevel()->AllCollisions;

	std::list<GameEngineCollision*>& Collisions = AllCollisions[_GroupOrder];

	for (GameEngineCollision* Collision : Collisions)
	{
		if (false == Collision->IsUpdate())
		{
			continue;
		}

		if (true == GameEngineCollision::CollisionFunction[ThisType][OtherType](GetTransform(), Collision->GetTransform()))
		{

			if (nullptr != _Function)
			{
				if (true == _Function(this, Collision))
				{

					return true;
				}
			}
			else {
				return true;
			}
		}
	}

	return false;
}

void GameEngineCollision::Rotate(float* nx, float* ny, float tx, float ty, float cx, float cy, float q) {
	float cosq = cos(q), sinq = sin(q);

	// 회전중심점 C가 원점  O와 일치하도록 회전할 점 T를 함께 평행이동
	tx -= cx, ty -= cy;

	// 원점 O에 대하여 회전할 점 T를 q라디안 만큼 회전
	*nx = tx * cosq - ty * sinq;
	*ny = ty * cosq + tx * sinq;

	// 원점 O가 원래의 회전 중심점 C와 일치하도록 회전된 점 N과 함께 이동
	*nx += cx, * ny += cy;
}

bool GameEngineCollision::IsCollisionRigidBody(CollisionType _ThisType, int _GroupOrder, CollisionType _OtherType, std::function<bool(GameEngineCollision* _This, GameEngineCollision* _Other)> _Function) {
	if (false == IsUpdate()) {
		return false;
	}

	int ThisType = static_cast<int>(_ThisType);
	int OtherType = static_cast<int>(_OtherType);

	if (nullptr == GameEngineCollision::CollisionFunction[ThisType][OtherType]) {
		MsgBoxAssert("아직 만들어지지 않은 콜리전 처리입니다");
	}

	std::map<int, std::list<GameEngineCollision*>>& AllCollisions = GetActor()->GetLevel()->AllCollisions;

	std::list<GameEngineCollision*>& Collisions = AllCollisions[_GroupOrder];

	for (GameEngineCollision* Collision : Collisions) {
		if (false == Collision->IsUpdate()) {
			continue;
		}

		if (true == GameEngineCollision::CollisionFunction[ThisType][OtherType](GetTransform(), Collision->GetTransform())) {
			auto& ac = GetActor()->GetTransform();
			float4 ThisPos = GetActor()->GetTransform().GetWorldPosition();
			ThisPos += GetTransform().GetLocalPosition();

			Rotate(&ThisPos.x, &ThisPos.y, ThisPos.x, ThisPos.y,
				GetActor()->GetTransform().GetWorldPosition().x, GetActor()->GetTransform().GetWorldPosition().y,
				GetActor()->GetTransformData().WorldRotation.z * GameEngineMath::DegreeToRadian);


			float4 OtherPos = Collision->GetActor()->GetTransform().GetWorldPosition();
			OtherPos += Collision->GetTransform().GetLocalPosition();

			Rotate(&OtherPos.x, &OtherPos.y, OtherPos.x, OtherPos.y,
				Collision->GetActor()->GetTransform().GetWorldPosition().x, Collision->GetActor()->GetTransform().GetWorldPosition().y,
				Collision->GetActor()->GetTransformData().WorldRotation.z * GameEngineMath::DegreeToRadian);


			RECT This = {
				ThisPos.x - GetTransform().GetWorldScale().x / 2,
				ThisPos.y - GetTransform().GetWorldScale().y / 2,
			};
			This.right = This.left + GetTransform().GetWorldScale().x;
			This.bottom = This.top + GetTransform().GetWorldScale().y;

			RECT Other = {
				OtherPos.x - Collision->GetTransform().GetWorldScale().x / 2,
				OtherPos.y - Collision->GetTransform().GetWorldScale().y / 2,
			};
			Other.right = Other.left + Collision->GetTransform().GetWorldScale().x;
			Other.bottom = Other.top + Collision->GetTransform().GetWorldScale().y;

			GameEngineTransform T, TT, TTT;
			TT.SetWorldPosition(float4(This.left + (This.right - This.left) / 2, This.top + (This.bottom - This.top) / 2, -3));
			TT.SetWorldScale(float4(This.right - This.left, This.bottom - This.top));
			//GameEngineDebug::DrawBox(TT, float4::GREEN);

			TTT.SetWorldPosition(float4(Other.left + (Other.right - Other.left) / 2, Other.top + (Other.bottom - Other.top) / 2, -3));
			TTT.SetWorldScale(float4(Other.right - Other.left, Other.bottom - Other.top));
			//GameEngineDebug::DrawBox(TTT, float4::GREEN);

			RECT Intersect;
			if (IntersectRect(&Intersect, &This, &Other)) {
				T.SetWorldPosition(float4(Intersect.left + (Intersect.right - Intersect.left) / 2, Intersect.top + (Intersect.bottom - Intersect.top) / 2, -3));
				T.SetWorldScale(float4(Intersect.right - Intersect.left, Intersect.bottom - Intersect.top));

				//GameEngineDebug::DrawBox(T, float4::GREEN);
				float InterWidth = Intersect.right - Intersect.left;
				float InterHeight = Intersect.bottom - Intersect.top;

				float4 Impurse = float4::ZERO;

				if (InterWidth > InterHeight) {
					if (Intersect.top == Other.top) {
						Impurse.y = +InterHeight;
					}
					if (Intersect.bottom == Other.bottom) {
						Impurse.y = -InterHeight;
					}
				} else {
					if (Intersect.left == Other.left) {
						Impurse.x = +InterWidth;
					}
					if (Intersect.right == Other.right) {
						Impurse.x = -InterWidth;
					}
				}

				if (!this->GetTransform().GetStatic()) {
					GetActor()->GetTransform().SetPower(float4::ZERO);
					GetActor()->GetTransform().SetWorldMove(-Impurse);
				}
				if (!Collision->GetTransform().GetStatic()) {
					Collision->GetActor()->GetTransform().SetPower(float4::ZERO);
					Collision->GetActor()->GetTransform().SetWorldMove(Impurse);
				}
			}

			if (nullptr != _Function) {

				if (true == _Function(this, Collision)) {

					return true;
				}
			} else {
				return true;
			}
		}
	}

	return false;
}

void GameEngineCollision::DebugRender()
{
	GameEngineCamera* DebugRenderCamera = GetActor()->GetLevel()->Cameras[static_cast<UINT>(DebugCameraOrder)];

	switch (DebugType)
	{
	case CollisionType::CT_POINT2D:
		break;
	case CollisionType::CT_SPHERE2D:
		GameEngineDebug::DrawSphere(GetTransform(), DebugRenderCamera, Color);
		break;
	case CollisionType::CT_AABB2D:
		GameEngineDebug::DrawBox(GetTransform(), DebugRenderCamera, Color);
		break;
	case CollisionType::CT_OBB2D:
		GameEngineDebug::DrawBox(GetTransform(), DebugRenderCamera, Color);
		break;
	case CollisionType::CT_POINT:
		break;
	case CollisionType::CT_SPHERE:
		GameEngineDebug::DrawSphere(GetTransform(), DebugRenderCamera, Color);
		break;
	case CollisionType::CT_AABB:
		GameEngineDebug::DrawBox(GetTransform(), DebugRenderCamera, Color);
		break;
	case CollisionType::CT_OBB:
		GameEngineDebug::DrawBox(GetTransform(), DebugRenderCamera, Color);
		break;
	case CollisionType::CT_MAX:
		break;
	default:
		break;
	}

}