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
	, Mass(10.f), Power(100.f)
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
		if (this == Collision)
			continue;

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
				ThisPos.x - GetTransform().GetWorldScale().x / 2 - 1.f,
				ThisPos.y - GetTransform().GetWorldScale().y / 2 - 1.f,
			};
			This.right = This.left + GetTransform().GetWorldScale().x + 1.f;
			This.bottom = This.top + GetTransform().GetWorldScale().y + 1.f;

			RECT Other = {
				OtherPos.x - Collision->GetTransform().GetWorldScale().x / 2 - 1.f,
				OtherPos.y - Collision->GetTransform().GetWorldScale().y / 2 - 1.f,
			};
			Other.right = Other.left + Collision->GetTransform().GetWorldScale().x + 1.f;
			Other.bottom = Other.top + Collision->GetTransform().GetWorldScale().y + 1.f;

			RECT Intersect;
			if (IntersectRect(&Intersect, &This, &Other)) {
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

				// 겹쳐있는데 어처피 나갈 방향으로 간다면 밀어내지 않는다
				//	LastMove


				float4 ImpurseNormal = (Impurse * 1.1f).NormalizeReturn();

				float _SubMass;
				if (!this->GetTransform().GetStatic()) {
					GetActor()->GetTransform().SetImpulse(float4::ZERO);
					//_SubMass = Collision->GetMass() / (this->Mass + Collision->GetMass());
					_SubMass = (this->Mass / Collision->GetMass());
					GetActor()->GetTransform().SetWorldMove(-Impurse);
				} else {
					Collision->GetActor()->GetTransform().Accel = 0.f;
				}
				if (!Collision->GetTransform().GetStatic()) {
					Collision->GetActor()->GetTransform().SetImpulse(float4::ZERO);
					_SubMass = (Collision->GetMass() / this->Mass);
					//_SubMass = this->Mass / (this->Mass + Collision->GetMass());
					Collision->GetActor()->GetTransform().SetWorldMove(Impurse);
				} else {
					GetActor()->GetTransform().Accel = 0.f;
				}

				if (!this->GetTransform().GetStatic() && !Collision->GetTransform().GetStatic()) {
					float MassScale = Collision->GetMass() / this->Mass;
					GetActor()->GetTransform().AddForce(-ImpurseNormal * MassScale * GetActor()->GetTransform().GetPower());
					MassScale = this->Mass / Collision->GetMass();
					float4 Force = ImpurseNormal * MassScale * Collision->GetActor()->GetTransform().GetPower();
					Collision->GetActor()->GetTransform().AddForce(Force);
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