class ActionFillFuelCB : ActionContinuousBaseCB
{
	private const float TIME_TO_REPEAT = 0.5;

	override void CreateActionComponent()
	{
		m_ActionData.m_ActionComponent = new CAContinuousFillFuel( UAQuantityConsumed.FUEL, TIME_TO_REPEAT );
	}
};

class ActionFillFuel: ActionContinuousBase
{
	const string FUEL_SELECTION_NAME = "refill";
	
	void ActionFillFuel()
	{
		m_CallbackClass = ActionFillFuelCB;
		m_CommandUID = DayZPlayerConstants.CMD_ACTIONFB_EMPTY_VESSEL;
		m_StanceMask = DayZPlayerConstants.STANCEMASK_ERECT | DayZPlayerConstants.STANCEMASK_CROUCH;
		m_FullBody = true;
		m_SpecialtyWeight = UASoftSkillsWeight.PRECISE_LOW;
		m_LockTargetOnUse = false;

		m_MessageStartFail = "";
		m_MessageStart = "";
		m_MessageSuccess = "";
		m_MessageFail = "";
		m_MessageCancel = "";

	}

	override void CreateConditionComponents()  
	{
		m_ConditionItem = new CCINonRuined;
		m_ConditionTarget = new CCTNone;
	}

	override int GetType()
	{
		return AT_FILL_FUEL;
	}

	override string GetText()
	{
		return "#refuel";
	}

	override bool ActionCondition( PlayerBase player, ActionTarget target, ItemBase item )
	{
		if( !target || !IsTransport(target) )
			return false;

		if( item.GetQuantity() <= 0 )
			return false;

		if( item.GetLiquidType() != LIQUID_GASOLINE )
			return false;

		Car car = Car.Cast( target.GetObject() );
		if( !car )
			return false;
		
		if( car.GetFluidFraction( CarFluid.FUEL ) >= 0.98 )
			return false;

		float distance = Math.AbsFloat(vector.Distance(car.GetPosition(), player.GetPosition()));

		CarScript carS = CarScript.Cast(car);
		if( distance <= carS.GetActionDistanceFuel() )
		{
			array<string> selections = new array<string>;
			target.GetObject().GetActionComponentNameList(target.GetComponentIndex(), selections);

			for (int s = 0; s < selections.Count(); s++)
			{
				if ( selections[s] == carS.GetActionCompNameFuel() )
				{
					return true;
				}
			}
		}
		return false;
	}
};