#pragma once
class SlotMachineCard {
public:
	SlotMachineCard();
	~SlotMachineCard();

	// delete Function
	SlotMachineCard(const SlotMachineCard& _Other) = delete;
	SlotMachineCard(SlotMachineCard&& _Other) noexcept = delete;
	SlotMachineCard& operator=(const SlotMachineCard& _Other) = delete;
	SlotMachineCard& operator=(SlotMachineCard&& _Other) noexcept = delete;

protected:
	virtual void Start();
	virtual void Update(float _DeltaTime);
	virtual void End() {}
};

