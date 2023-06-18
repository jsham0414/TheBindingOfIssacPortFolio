#pragma once
#include <thread>
#include <GameEngineCore/GameEngineUpdateObject.h>

class GameEngineUpdateObject;
class GameEngineThread : public std::thread {

public:
	template <class _Fn, class... _Args, std::enable_if_t<!std::is_same_v<std::_Remove_cvref_t<_Fn>, thread>, int> = 0>
	GameEngineThread(_Fn&& _Fx, _Args&&... _Ax) : std::thread(_Fx, _Ax...) {

		/*GameEngineUpdateObject* Obj = _Ax;
		
		if (Obj == nullptr)
			return;
		
		IsDeath_ = Obj->GetDeathPtr();*/
	}

	bool IsDeath() {
		return *IsDeath_;
	}
private:
	std::shared_ptr<std::atomic<bool>> IsDeath_;
};

