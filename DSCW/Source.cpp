#include <string>
#include <iostream>
#include <vector>

using namespace std;

enum state { Out_Of_Poptart, No_Credit, Has_Credit, Dispenses_Poptart };
enum stateParameter { No_Of_Poptarts, Credit, Cost_Of_Poptart };

class StateContext;

class State
{
protected:

	StateContext* CurrentContext;

public:

	State(StateContext* Context)
	{
		CurrentContext = Context;
	}
	virtual ~State(void) {}
	virtual void transition(void) {}
};

class StateContext
{
protected:

	State* CurrentState = nullptr;
	int stateIndex = 0;
	vector<State*> availableStates;
	vector<int> stateParameters;

public:
	virtual ~StateContext(void);
	virtual void setState(state newState);
	virtual int getStateIndex(void);
	virtual void setStateParam(stateParameter SP, int value);
	virtual int getStateParam(stateParameter SP);
};

StateContext::~StateContext(void)
{
	for (int i = 0; i < this->availableStates.size(); i++) delete this->availableStates[i];
	this->availableStates.clear();
	this->stateParameters.clear();
}

void StateContext::setState(state newState)
{
	this->CurrentState = availableStates[newState];
	this->stateIndex = newState;
	this->CurrentState->transition();
}

int StateContext::getStateIndex(void)
{
	return this->stateIndex;
}

void StateContext::setStateParam(stateParameter SP, int value)
{
	this->stateParameters[SP] = value;
}

int StateContext::getStateParam(stateParameter SP)
{
	return this->stateParameters[SP];
}

class Transition
{
public:
	virtual bool insertMoney(int) { cout << "Error!" << endl; return false; }
	virtual bool makeSelection(int) { cout << "Error!" << endl; return false; }
	virtual bool moneyRejected(void) { cout << "Error!" << endl; return false; }
	virtual bool addPoptart(int) { cout << "Error!" << endl; return false; }
	virtual bool dispense(void) { cout << "Error!" << endl; return false; }
};

class PoptartState : public State, public Transition
{
public:
	PoptartState(StateContext* Context) : State(Context) {}
};

class OutOfPoptart : public PoptartState
{
public:
	OutOfPoptart(StateContext* Context) : PoptartState(Context) {}
	bool insertMoney(int money);
	bool makeSelection(int option);
	bool moneyRejected(void);
	bool addPoptart(int number);
	bool dispense(void);
};

class NoCredit : public PoptartState
{
public:
	NoCredit(StateContext* Context) : PoptartState(Context) {}
	bool insertMoney(int money);
	bool makeSelection(int option);
	bool moneyRejected(void);
	bool addPoptart(int number);
	bool dispense(void);
};

class HasCredit : public PoptartState
{
public:
	HasCredit(StateContext* Context) : PoptartState(Context) {}
	bool insertMoney(int money);
	bool makeSelection(int option);
	bool moneyRejected(void);
	bool addPoptart(int number);
	bool dispense(void);
};

class DispensesPoptart : public PoptartState
{
public:
	DispensesPoptart(StateContext* Context) : PoptartState(Context) {}
	bool insertMoney(int money);
	bool makeSelection(int option);
	bool moneyRejected(void);
	bool addPoptart(int number);
	bool dispense(void);
};

class Product
{
	friend class Filling;
protected:
	string product_description;
	int itemCost = 0;
	virtual Product* ReturnNext(void);
	virtual void RemoveHighestCostItem(Product* HighestItem);
public:
	virtual void consume(void);
	virtual int cost(void);
	virtual string description(void);
	virtual Product* ReturnHighestCostItem(void);
	virtual void RemoveHighestCostItem(void);
};

class Poptart : public Product
{
public:
	Poptart(void)
	{
		this->product_description = "Poptart";
		this->itemCost = 50;
	}
};

class Poptart_Dispenser : public StateContext, public Transition
{
	friend class DispensesPoptart;
	friend class HasCredit;
private:
	PoptartState* PoptartCurrentState = nullptr;
	bool itemDispensed = false;
	//indicates whether a product is there to be retrieved
	Product* DispensedItem = nullptr;
	bool itemRetrieved = false; //indicates whether a product has been retrieved
public:
	Poptart_Dispenser(int inventory_count);
	~Poptart_Dispenser(void);
	bool insertMoney(int money);
	bool makeSelection(int option);
	bool moneyRejected(void);
	bool addPoptart(int number);
	bool dispense(void);
	Product* getProduct(void);
	virtual void setStateParam(stateParameter SP, int value);
	virtual int getStateParam(stateParameter SP);
};

Poptart_Dispenser::~Poptart_Dispenser(void)
{
	if (!this->itemRetrieved)
	{
		delete this->DispensedItem;
	}
}

bool Poptart_Dispenser::insertMoney(int money)
{
	PoptartCurrentState = (PoptartState*) this->CurrentState;
	return this->PoptartCurrentState->insertMoney(money);
}

bool Poptart_Dispenser::makeSelection(int option)
{
	PoptartCurrentState = (PoptartState*) this->CurrentState;
	return this->PoptartCurrentState->makeSelection(option);
}

bool Poptart_Dispenser::moneyRejected(void)
{
	PoptartCurrentState = (PoptartState*) this->CurrentState;
	return this->PoptartCurrentState->moneyRejected();
}

bool Poptart_Dispenser::addPoptart(int number)
{
	PoptartCurrentState = (PoptartState*) this->CurrentState;
	return this->PoptartCurrentState->addPoptart(number);
}

bool Poptart_Dispenser::dispense(void)
{
	PoptartCurrentState = (PoptartState*) this->CurrentState;
	return this->PoptartCurrentState->dispense();
}

Product* Poptart_Dispenser::getProduct(void)
{
	if (this->itemDispensed)
	{
		this->itemDispensed = false;
		this->itemRetrieved = true;
		return this->DispensedItem;
	}

	return nullptr;
}
void Poptart_Dispenser::setStateParam(stateParameter SP, int value)
{
	if (SP == Cost_Of_Poptart) return;
	this->stateParameters[SP] = value;
}

int Poptart_Dispenser::getStateParam(stateParameter SP)
{
	if (SP == Cost_Of_Poptart)
	{
		if (DispensedItem == nullptr) return 0;
		return DispensedItem->cost();
	}
	return this->stateParameters[SP];
}

bool OutOfPoptart::insertMoney(int money)
{
	cout << "Out of Poptarts!" << endl;
	return false;
}

bool OutOfPoptart::makeSelection(int option)
{
	cout << "Out of Poptarts!" << endl;
	return false;
}

bool OutOfPoptart::moneyRejected(void)
{
	cout << "Money has been returned" << endl;
	return true;
}

bool OutOfPoptart::addPoptart(int number)
{
	cout << number << " Poptarts have been added" << endl;
	this->CurrentContext->setStateParam(No_Of_Poptarts, number);
	this->CurrentContext->setState(No_Credit);
	return true;
}

bool OutOfPoptart::dispense(void)
{
	cout << "Out of Poptarts!" << endl;
	return false;
}

bool NoCredit::insertMoney(int money)
{
	cout << "You inserted: " << money;
	this->CurrentContext->setStateParam(Credit, money);
	cout << " Total: " << money << endl;
	this->CurrentContext->setState(Has_Credit);
	return true;
}

bool NoCredit::makeSelection(int option)
{
	cout << "No Money Inserted!" << endl;
	return false;
}

bool NoCredit::moneyRejected(void)
{
	cout << "No Money Inserted!" << endl;
	return false;
}

bool NoCredit::addPoptart(int number)
{
	cout << "Poptart already inserted" << endl;
	return false;
}

bool NoCredit::dispense(void)
{
	cout << "No Money Inserted" << endl;
	return false;
}


bool HasCredit::insertMoney(int money)
{
	int temp = 0;
	temp = this->CurrentContext->getStateParam(Credit);
	cout << "You inserted: " << money;
	temp = temp + money;
	this->CurrentContext->setStateParam(Credit, temp);
	cout << " Total: " << money << endl;
	this->CurrentContext->setState(Has_Credit);
	return true;
}
bool HasCredit::makeSelection(int option) {
	cout << "Poptart Selected" << endl;
	this->CurrentContext->setState(Dispenses_Poptart);
	return true;
}

bool HasCredit::moneyRejected(void) {
	cout << "Money has been returned" << endl;
	this->CurrentContext->setStateParam(Credit, 0);
	this->CurrentContext->setState(No_Credit);
	return true;
}

bool HasCredit::addPoptart(int number)
{
	cout << "Poptart already inserted" << endl;
	return false;
}

bool HasCredit::dispense(void) {
	cout << "" << endl;
	return false;
}

bool DispensesPoptart::insertMoney(int money) 
{
	cout << "Poptart Dispensing" << endl;
	return false;
}

bool DispensesPoptart::makeSelection(int option)
{
	cout << "Poptart Dispensing" << endl;
	return false;
}

bool DispensesPoptart::moneyRejected(void)
{
	cout << "Poptart Dispensing" << endl;
	return false;
}

bool DispensesPoptart::addPoptart(int number)
{
	cout << "Poptart Dispensing" << endl;
	return false;
}

bool DispensesPoptart::dispense(void) 
{
	int poptartStockCheck = this->CurrentContext->getStateParam(No_Of_Poptarts);
	int creditCheck = this->CurrentContext->getStateParam(Credit);
	if (poptartStockCheck > 0)
	{
		if (creditCheck > 0) 
		{
			cout << "Dispensing Poptart" << endl;
			this->CurrentContext->setState(Has_Credit);
			return true;
		}
		else if (creditCheck == 0)
		{
			cout << "Dispensing Poptart" << endl;
			this->CurrentContext->setState(No_Credit);
			return true;
		}
	}
	else if (poptartStockCheck == 0)
	{
		cout << "Dispensing Poptart" << endl;
		this->CurrentContext->setState(Out_Of_Poptart);
		return true;
	}

}

Poptart_Dispenser::Poptart_Dispenser(int inventory_count)
{
	this->availableStates.push_back(new OutOfPoptart(this));
	this->availableStates.push_back(new NoCredit(this));
	this->availableStates.push_back(new HasCredit(this));
	this->availableStates.push_back(new DispensesPoptart(this));
	this->availableStates.push_back(0); //Number of Poptarts
	this->availableStates.push_back(0); //Available Credit

	this->setState(Out_Of_Poptart);
	if (inventory_count > 0)
	{
		this->addPoptart(inventory_count);
	}
}