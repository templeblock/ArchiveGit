#if false
CMoneyObject
	CString GetClassName
	CFunction* GetObjectCreateWizard
	CFunction* GetObjectEditWizard
	CIndividual* GetOwner
	bool ActualOrProjected
	bool SignOfValue (+ or - value)
	bool NeedsCalcUpdate (this object or some child has changed)
	CMoneyObject* ProjectedFromObject
	CFunction* GetProjectionFunction
	long DateToStart			DateAcquired	DateAccrued		DateBorn
	long DateToEnd				DateSold		DatePaid		DateDeathProjected
	long ValueToStart			Principal		InitialInvestment
	long ValueToEnd
	long GetCurrentValue(time)
	Type: OneTime, Recurring
	long GetRateOfChange		InterestRate	RateOfReturn
	long GetFrequencyOfChange	PaymentFrequency
	CFunction* GetPerformanceFunction
	long TaxStatus (Charity=+, SalesTax=-)
	CString* RequiredObjectNames
	CMoneyObject* RequiredObjects	CIndividual* List if family
	CMoneyObject* ParentObject	FamilyPointer(if an Individual)

CIncome
	"CIncome"
	CIndividual* BackPtr
	bIsReal
	DateToStart
	DateToEnd
	Type: OneTime, Recurring
	Frequency
	AmountEarned
	ProjectedPerformance
	TaxStatus
	FutureProjectionPercentOfNationalAverage
	bEligibleForOvertime
	CObligatedChild* List
	CObligatedParent* List

CExpenditure
	"CExpenditure"
	CIndividual* BackPtr
	bIsReal
	DateAcquired
	DateSold
	Type: OneTime, Recurring
	Frequency
	AmountSpent
	ProjectedPerformance
	TaxStatus (Charity=+, SalesTax=-)
	FutureProjectionPercentOfNationalAverage
	SaleableValue
	ReplacementValue
	ProductID
	Category
	ProductName
	PaymentTerms
	CObligatedChild* List
	CObligatedParent* List

CLoan
	"CLoan"
	CIndividual* BackPtr
	bIsReal
	DateStarted
	DateEnded
	Type: Fixed, Variable, Balloon
	Payment
	PaymentFrequency
	Principal
	InterestRate
	OriginationFee
	PrepaidInterest
	FinanceChanges
	Fees
	CObligatedChild* List
	CObligatedParent* List

CLease
	"CLease"
	CIndividual* BackPtr
	bIsReal
	DateToStart
	DateToEnd
	Payment
	PaymentFrequency
	Principal
	CObligatedChild* List
	CObligatedParent* List

CRent
	"CRent"
	CIndividual* BackPtr
	bIsReal
	DateToStart
	DateToEnd
	Payment
	PaymentFrequency
	CObligatedChild* List
	CObligatedParent* List

CInvestment
	"CInvestment"
	CIndividual* BackPtr
	bIsReal
	DateToStart
	DateToEnd
	InitialInvestment
	CurrentValue
	FeesAndPenalties
	RateOfReturn
	CObligatedChild* List
	CObligatedParent* List

CTax
	"CTax"
	CIndividual* BackPtr
	bIsReal
	DateAccrued
	DatePaid
	CObligatedChild* List
	CObligatedParent* List

CIndividual
	"CIndividual"
	CFamily* BackPtr
	bIsReal
	DateBorn
	DateDeathProjected
	Sex
	MaritalStatus
	JobSchoolStatus
	StateResiding
	CLifeStyle* List
	CHabits* List
	CCharacteristics* List
	CIncome* List
	CExpenditure* List
	CLoan* List
	CLease* List
	CRent* List
	CInvestment* List
	CGain* List
	CTax* List

CFamily
	"CFamily"
	CIndividual* List
	CEvent* List

CClimate
	"CClimate"
	StateOfResidence
	InflationPerformance
	CostOfLivingPerformance
	DividendPerformance
	InterestRatePerformance
	EquityPerformance
	BondPerformance

CEvent
	CMoneyObject
	eEvent Type
		Family member goes/quits to college
		Family member gets/loses a job
		Family member retires from work
		Family member buys/sells a car, boat, RV, etc.
		Family member rents an apartment
		Family member buys/sells a house/condo
		Family member buys/sells appliances or furniture
		Family member buys/sells jewelry
		Family member gets married/divorced
		Family member has a child
		Family member dies
		Family buys a vacation home
		Family renovates the home
		Family member starts a business
		Family members change personal habits; smoking, etc.
		Family members open/close financial accounts
		Family members have medical problems
		Family members take a vacation
	Financial impact

CLifeStyle
	"CLifeStyle"
	ObjectCreateWizard
	ObjectEditWizard
	CExpenditure* PriorityList
	CEvent* List

---Database-----------------------------

CProduct
	"CProduct"
	ProductID
	CategoryPath or Heirarchy
	ProductName
	Cost
	PaymentOptions (Cash,Loan,Credit,Lease)
	GoodBetterBest
	Durability

CProductPreferences
	"CProductPreferences"
	ObjectCreateWizard
	ObjectEditWizard
	CIndividual* BackPtr
	ProductID
	CategoryPath or Heirarchy
	ProductRoleInIndividualLifeStyle
	Priority
		I need this product
		I don't like this product
		I will never need this product
		I will always need this product
		Use you best judgement
		If you think I need it
		Buy as soon as possible
		Buy when I can afford it
		I consume this product ___ than the average person
		This product's priority is always higher/lower than Product Z

#endif
